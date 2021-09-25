#pragma once

#include "proc.h"
#include "mem.h"

void setAmmo(DWORD procId, uintptr_t moduleBase, HANDLE hProcess, uintptr_t dynamicPtrBaseAddr, int value);
void setHealth(DWORD procId, uintptr_t moduleBase, HANDLE hProcess, uintptr_t dynamicPtrBaseAddr, int value);
void toggleAmmo(DWORD procId, uintptr_t moduleBase, HANDLE hProcess, uintptr_t dynamicPtrBaseAddr, bool toggle);
void toggleRecoil(DWORD procId, uintptr_t moduleBase, HANDLE hProcess, uintptr_t dynamicPtrBaseAddr, bool toggle);