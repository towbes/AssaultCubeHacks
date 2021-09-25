// FirstExternalTrainer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "stdafx.h"
#include "InternalTrainer.h"

void setAmmo(uintptr_t moduleBase, int value)
{
    //resolve ammo pointer chain using cheat engine offsets
    uintptr_t ammoAddr = mem::findDMAAddy(moduleBase + 0x10f4f4, { 0x374, 0x14, 0x0 });

    int* ammo = (int*)ammoAddr;

    if (ammo) {
        *ammo = value;
    }

    

}

void toggleAmmo(uintptr_t moduleBase, bool toggle)
{

    //If toggle is true, enable infinite ammo
    if (toggle) {
        mem::Patch((BYTE*)moduleBase + 0x637e9, (BYTE*)"\xFF\x06", 2);
    }
    //otherwise put the memory back
    else {
        mem::Patch((BYTE*)moduleBase + 0x637e9, (BYTE*)"\xFF\x0E", 2);
    }
    

}

void setHealth(uintptr_t moduleBase, int value)
{
    uintptr_t* localPlayerPtr = (uintptr_t*)(moduleBase + 0x10f4f4);

    if (localPlayerPtr) {
        *(int*)(*localPlayerPtr + 0xf8) = value;
    }


}

void toggleRecoil(uintptr_t moduleBase, bool toggle)
{
    //If toggle is true, enable infinite ammo
    if (toggle) {
        mem::Nop((BYTE*)moduleBase + 0x63786, 10);
    }
    //otherwise put the memory back
    else {
        mem::Patch((BYTE*)moduleBase + 0x63786, (BYTE*)"\x50\x8D\x4C\x24\x1C\x51\x8B\xCE\xFF\xD2", 10);
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
