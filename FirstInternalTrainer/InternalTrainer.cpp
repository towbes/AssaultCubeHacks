// FirstExternalTrainer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "stdafx.h"
#include "InternalTrainer.h"

//Autopadding https://guidedhacking.com/threads/auto-padding-for-class-recreation.13478/
#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}


// Created with ReClass.NET 1.2 by KN4CK3R

struct Vector3 { float x, y, z; };

class weapon
{
public:
    union
    {
        //              Type     Name    Offset
        DEFINE_MEMBER_N(int*, ammoptr, 0x0014);
        DEFINE_MEMBER_N(Vector3, bodyPos, 0x0034);
        DEFINE_MEMBER_N(Vector3, angles, 0x0040);
        DEFINE_MEMBER_N(int32_t, health, 0x00F8);
        DEFINE_MEMBER_N(weapon*, currentWpn, 0x0374);

    };
}; 



class entity
{
public:

    // be sure to put all the offsets you pad this way in a union
    union
    {
        //              Type     Name    Offset
        DEFINE_MEMBER_N(Vector3, headPos, 0x0004);
        DEFINE_MEMBER_N(Vector3, bodyPos, 0x0034);
        DEFINE_MEMBER_N(Vector3, angles, 0x0040);
        DEFINE_MEMBER_N(int32_t, health, 0x00F8);
        DEFINE_MEMBER_N(weapon*, currentWpn, 0x0374);

    };


};

//original detour function https://guidedhacking.com/threads/c-detour-hooking-function-tutorial.7930/
bool Hook(void* toHook, void* ourFunc, int len) {
    if (len < 5) {
        return false;
    }

    DWORD curProtection;
    //Set page to executee / readwrite
    VirtualProtect(toHook, len, PAGE_EXECUTE_READWRITE, &curProtection);

    //set the instructions to nop to prevent stray bytes
    memset(toHook, 0x90, len);

    //Offset from jump address to the function
    DWORD relativeAddress = ((DWORD)ourFunc - (DWORD)toHook) - 5;

    //place jump instruction
    *(BYTE*)toHook = 0xE9;
    //write the jump address
    *(DWORD*)((DWORD)toHook + 1) = relativeAddress;

    DWORD temp;
    VirtualProtect(toHook, len, curProtection, &temp);

    return true;
}


DWORD jmpBackAddy;
void __declspec(naked) recoilFunc() {
    __asm {
        inc esi
        push edi
        mov edi,[esp+0x14]
        jmp [jmpBackAddy]
    }
}

//Trampoline hook from guidedhacking https://www.youtube.com/watch?v=HLh_9qOkzy0


void setAmmo(uintptr_t moduleBase, int value)
{
    //resolve ammo pointer chain using cheat engine offsets
    //uintptr_t ammoAddr = mem::findDMAAddy(moduleBase + 0x10f4f4, { 0x374, 0x14, 0x0 });
    //int* ammo = (int*)ammoAddr;

    //use entity structure
    entity* localPlayer = *(entity**)(moduleBase + 0x10f4f4);

    if (localPlayer) {
        *localPlayer->currentWpn->ammoptr = value;
    }

    

}

void toggleAmmo(uintptr_t moduleBase, bool toggle)
{
    int hookLength = 7;
    DWORD hookAddress = moduleBase + 0x637e9;
    jmpBackAddy = hookAddress + hookLength;

    //If toggle is true, enable infinite ammo
    if (toggle) {

        //hooking method
        Hook((void*)hookAddress, recoilFunc, hookLength);

        //memory patch method
        //mem::Patch((BYTE*)moduleBase + 0x637e9, (BYTE*)"\xFF\x06", 2);
    }
    //otherwise put the memory back
    else {
        //mem::Patch((BYTE*)moduleBase + 0x637e9, (BYTE*)"\xFF\x0E", 2);
    }
    

}

void setHealth(uintptr_t moduleBase, int value)
{
    entity* localPlayer = *(entity**)(moduleBase + 0x10f4f4);
    //uintptr_t* localPlayerPtr = (uintptr_t*)(moduleBase + 0x10f4f4);

    if (localPlayer) {
        //*(int*)(*localPlayerPtr + 0xf8) = value;
        localPlayer->health = value;
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
