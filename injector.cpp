#include <iostream>
#include <windows.h>
#include <TlHelp32.h>



DWORD GetProcId(const char * procName) {

    PROCESSENTRY32 procEntry;

    HANDLE hSnap;
    HANDLE hProcess;

    DWORD procId = 0;

    hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnap == INVALID_HANDLE_VALUE)
    {
        std::cout << "Couldnt create a snapshot";
        return(FALSE);
    }

    procEntry.dwSize = sizeof(procEntry);

    if (Process32First(hSnap, &procEntry)) {

        while (Process32Next(hSnap, &procEntry))
        {

            if (strstr(reinterpret_cast<const char*>(procEntry.szExeFile), procName)) {

                 procId = procEntry.th32ProcessID;

                std::cout << "CoreKeeper procId: " << procId << std::endl;

            }
        }
    }

    return procId;

    CloseHandle(hSnap);

}

BOOL InjectDLL(DWORD procID, const char* dllPath)
{
    BOOL WPM = 0;

    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, procID);
    if (hProc == INVALID_HANDLE_VALUE)
    {
        return -1;
    }

    void* loc = VirtualAllocEx(hProc, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    WPM = WriteProcessMemory(hProc, loc, dllPath, strlen(dllPath) + 1, 0);
    if (!WPM)
    {
        CloseHandle(hProc);
        return -1;
    }
    std::cout << "Dll Injected" << std::endl;

    HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, loc, 0, 0);
    if (!hThread)
    {
        VirtualFree(loc, strlen(dllPath) + 1, MEM_RELEASE);
        CloseHandle(hProc);
        return -1;
    }

    CloseHandle(hProc);

    VirtualFree(loc, strlen(dllPath) + 1, MEM_RELEASE);

    CloseHandle(hThread);

    return 0;
}

int main()
{

    const char * procName = "CoreKeeper.exe";

    const char* dllName = "Core.dll";

    char dllPath[MAX_PATH];

    GetFullPathName(dllName, MAX_PATH, dllPath, nullptr);

    DWORD procId = GetProcId(procName);

    InjectDLL(procId, reinterpret_cast<const char *>(dllPath));

    system("PAUSE");
}

