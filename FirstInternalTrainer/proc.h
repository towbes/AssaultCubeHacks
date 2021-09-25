#pragma once


DWORD GetProcId(const wchar_t* procName);

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName);

uintptr_t findDMAAddyProc(HANDLE hProc, uintptr_t baseptr, std::vector<unsigned int> offsets);