#pragma once
#include "stdafx.h"

namespace mem
{
	void PatchEx(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess);
	void NopEx(BYTE* dst, unsigned int size, HANDLE hProcess);
	uintptr_t findDMAAddyEx(HANDLE hProc, uintptr_t baseptr, std::vector<unsigned int> offsets);

	void Patch(BYTE* dst, BYTE* src, unsigned int size);
	void Nop(BYTE* dst, unsigned int size);
	uintptr_t findDMAAddy(uintptr_t baseptr, std::vector<unsigned int> offsets);
}
