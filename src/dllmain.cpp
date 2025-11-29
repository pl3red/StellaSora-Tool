#include "pch.h"
#include "main.h"
#include <process.h>   // for _beginthreadex

// Wrapper thread for safety
unsigned __stdcall ThreadEntry(void* param)
{
    HMODULE hModule = (HMODULE)param;

    // Start your mod
    Run(hModule);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        
        // Use _beginthreadex (SAFE inside DllMain)
        {
            uintptr_t thread = _beginthreadex(
                nullptr,        // default security
                0,              // default stack
                ThreadEntry,    // entry function
                hModule,        // pass REAL module handle
                0,
                nullptr
            );

            if (thread)
                CloseHandle((HANDLE)thread);
        }
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        // should never arrive because of DisableThreadLibraryCalls
        break;

    case DLL_PROCESS_DETACH:
        break;
    }

    return TRUE;
}
