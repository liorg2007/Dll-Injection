#include "mydll.h"
#include <Windows.h>
#include <stdio.h>
#include <tlhelp32.h>
#include <string>
#include <iostream>

HANDLE GetProcessByName(const std::wstring& processName);

#define PROCESS_NAME L"notepad.exe"

int main(int argc, char* argv[])
{
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <DLL_PATH>" << std::endl;
    return 1;
  }

  std::string dllPath = argv[1];
  HANDLE notepadH = NULL;

  // Wait until the process is alive, then open it
  std::cout << "Waiting for the notepad to open";
  do {
    notepadH = GetProcessByName(PROCESS_NAME);
    Sleep(1000);
    std::cout << ". ";
  } while (notepadH == NULL);

  // Get the address of LoadLibrary
  LPVOID LoadLibrary = (LPVOID)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA");

  // Allocate memory to load the DLL path to the remote thread
  LPVOID memAlloc = (LPVOID)VirtualAllocEx(notepadH, NULL, dllPath.length() + 1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

  // Load the DLL path to the remote process
  WriteProcessMemory(notepadH, (LPVOID)memAlloc, dllPath.c_str(), dllPath.length() + 1, NULL);

  // Create the remote thread
  HANDLE RemoteThread = CreateRemoteThread(notepadH, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibrary, (LPVOID)memAlloc, NULL, NULL);

  Sleep(2000);

  CloseHandle(notepadH);
  // Free the memory
  VirtualFreeEx(notepadH, (LPVOID)memAlloc, 0, MEM_RELEASE);

  return 0;
}

HANDLE GetProcessByName(const std::wstring& processName)
{
  HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (hSnapshot == INVALID_HANDLE_VALUE) {
    return NULL;
  }

  PROCESSENTRY32 pe32;
  pe32.dwSize = sizeof(PROCESSENTRY32);

  if (!Process32First(hSnapshot, &pe32)) {
    CloseHandle(hSnapshot);
    return NULL;
  }

  do {
    if (std::wstring(pe32.szExeFile) == processName) {
      CloseHandle(hSnapshot);
      return OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
    }
  } while (Process32Next(hSnapshot, &pe32));

  CloseHandle(hSnapshot);
  return NULL;
}
