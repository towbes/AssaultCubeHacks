// FirstExternalTrainer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "stdafx.h"
#include "FirstExternalTrainer.h"

void setAmmo(DWORD procId, uintptr_t moduleBase, HANDLE hProcess, uintptr_t dynamicPtrBaseAddr, int value)
{
    //resolve ammo pointer chain using cheat engine offsets
    std::vector<unsigned int> ammoOffsets = { 0x374, 0x14, 0x0 };
    uintptr_t ammoAddr = findDMAAddy(hProcess, dynamicPtrBaseAddr, ammoOffsets);

    //read ammo value
    int ammoValue = 0;

    ReadProcessMemory(hProcess, (BYTE*)ammoAddr, &ammoValue, sizeof(ammoValue), nullptr);

    //write to it
    int newAmmo = value;
    if (newAmmo > 0) {
        WriteProcessMemory(hProcess, (BYTE*)ammoAddr, &newAmmo, sizeof(newAmmo), nullptr);
    }

}

void toggleAmmo(DWORD procId, uintptr_t moduleBase, HANDLE hProcess, uintptr_t dynamicPtrBaseAddr, bool toggle)
{
    //If toggle is true, enable infinite ammo
    if (toggle) {
        mem::PatchEx((BYTE*)moduleBase + 0x637e9, (BYTE*)"\xFF\x06", 2, hProcess);
    }
    //otherwise put the memory back
    else {
        mem::PatchEx((BYTE*)moduleBase + 0x637e9, (BYTE*)"\xFF\x0E", 2, hProcess);
    }
    

}

void setHealth(DWORD procId, uintptr_t moduleBase, HANDLE hProcess, uintptr_t dynamicPtrBaseAddr, int value)
{
    //resolve ammo pointer chain using cheat engine offsets
    std::vector<unsigned int> healthOffsets = { 0xF8 };
    uintptr_t healthAddr = findDMAAddy(hProcess, dynamicPtrBaseAddr, healthOffsets);

    //read ammo value
    int healthValue = 0;

    ReadProcessMemory(hProcess, (BYTE*)healthAddr, &healthValue, sizeof(healthValue), nullptr);

    //write to it
    int newHealth = value;
    if (newHealth > 0) {
        WriteProcessMemory(hProcess, (BYTE*)healthAddr, &newHealth, sizeof(newHealth), nullptr);
    }

}

void toggleRecoil(DWORD procId, uintptr_t moduleBase, HANDLE hProcess, uintptr_t dynamicPtrBaseAddr, bool toggle)
{
    //If toggle is true, enable infinite ammo
    if (toggle) {
        mem::NopEx((BYTE*)moduleBase + 0x63786, 10, hProcess);
    }
    //otherwise put the memory back
    else {
        mem::PatchEx((BYTE*)moduleBase + 0x63786, (BYTE*)"\x50\x8D\x4C\x24\x1C\x51\x8B\xCE\xFF\xD2", 10, hProcess);
    }


}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
