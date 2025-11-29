#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <iostream>

bool InjectDLL(DWORD processId, const std::string& dllPath)
{
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    if (!hProcess) {
        std::cout << "Failed to open target process. Error: " << GetLastError() << "\n";
        return false;
    }

    LPVOID allocMem = VirtualAllocEx(
        hProcess,
        NULL,
        dllPath.size() + 1,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE
    );

    if (!allocMem) {
        std::cout << "Failed to allocate memory in target process.\n";
        CloseHandle(hProcess);
        return false;
    }

    if (!WriteProcessMemory(hProcess, allocMem, dllPath.c_str(), dllPath.size() + 1, NULL)) {
        std::cout << "WriteProcessMemory failed.\n";
        VirtualFreeEx(hProcess, allocMem, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    HANDLE hThread = CreateRemoteThread(
        hProcess,
        NULL,
        0,
        (LPTHREAD_START_ROUTINE)LoadLibraryA,
        allocMem,
        0,
        NULL
    );

    if (!hThread) {
        std::cout << "CreateRemoteThread failed.\n";
        VirtualFreeEx(hProcess, allocMem, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    WaitForSingleObject(hThread, INFINITE);

    VirtualFreeEx(hProcess, allocMem, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProcess);

    return true;
}

DWORD GetProcessIdByName(const std::string& processName)
{
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (Process32First(snapshot, &entry)) {
        do {
            if (!_stricmp(entry.szExeFile, processName.c_str())) {
                CloseHandle(snapshot);
                return entry.th32ProcessID;
            }
        } while (Process32Next(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return 0;
}

int main()
{
    std::string processName;
    std::string dllName = "StellaSora-Tool.dll";

    std::cout << "Target process name (example: Game.exe): ";
    std::getline(std::cin, processName);

    DWORD pid = GetProcessIdByName(processName);

    if (pid == 0) {
        std::cout << "Process not found.\n";
        return 1;
    }

    char fullDllPath[MAX_PATH];
    GetFullPathNameA(dllName.c_str(), MAX_PATH, fullDllPath, nullptr);

    std::cout << "Injecting: " << fullDllPath << "\n";

    if (InjectDLL(pid, fullDllPath)) {
        std::cout << "Injection successful.\n";
    } else {
        std::cout << "Injection failed.\n";
    }

    system("pause");
    return 0;
}
