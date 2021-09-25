#pragma once

#include "proc.h"
#include "mem.h"

void setAmmo(uintptr_t moduleBase, int value);
void setHealth(uintptr_t moduleBase, int value);
void toggleAmmo(uintptr_t moduleBase, bool toggle);
void toggleRecoil(uintptr_t moduleBase, bool toggle);

//Infinite grenades?

