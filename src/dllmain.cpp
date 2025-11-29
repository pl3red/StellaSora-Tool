#include "pch.h"
#include "main.h"
#include <process.h>   // for _beginthreadex

// Thread start procedure
unsigned __stdcall ThreadEntry(void* param)
{
    HMODULE* mod = reinterpret_cast<HMODULE*>(param);

    Run(mod);

    delete mod; // free allocated pointer
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID)
{
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);

        // Allocate a pointer because Run(HMODULE*) requires it
        {
            HMODULE* modPtr = new HMODULE(hModule);

            uintptr_t thread = _beginthreadex(
                nullptr,
                0,
                ThreadEntry,      // Start routine
                modPtr,           // Pass pointer version of hModule
                0,
                nullptr
            );

            if (thread)
                CloseHandle((HANDLE)thread);
            else
                delete modPtr;  // prevent memory leak if thread fails
        }
        break;

    case DLL_PROCESS_DETACH:
        break;
    }

    return TRUE;
}
