#pragma once
#include <Minhook.h>

void __mat_dash_add_hook(void* addr, void* detour, void** trampoline) {
    static bool initialized = false;
    if (!initialized) {
        MH_Initialize();
        initialized = true;
    }
    MH_CreateHook(addr, detour, trampoline);
    MH_EnableHook(addr);
}