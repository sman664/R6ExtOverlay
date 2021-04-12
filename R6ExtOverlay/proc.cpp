#include "proc.h"

DWORD GetProcId(const wchar_t* procName)
{
	DWORD procId = 0;
	HANDLE  hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32W procEntry;
		procEntry.dwSize = sizeof(procEntry);

		if (Process32FirstW(hSnap, &procEntry))
		{

			do
			{
				if (!_wcsicmp(procEntry.szExeFile, procName))
				{
					procId = procEntry.th32ProcessID;
					break;
				}

			} while (Process32NextW(hSnap, &procEntry));
		}
	}
	CloseHandle(hSnap);
	return procId;
}

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName)
{
	uintptr_t modBaseAddr = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32W modEntry;
		modEntry.dwSize = sizeof(modEntry);
		if (Module32FirstW(hSnap, &modEntry))
		{
			do
			{
				if (!_wcsicmp(modEntry.szModule, modName))
				{
					modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
				}
			} while (Module32NextW(hSnap, &modEntry));
		}
	}
	CloseHandle(hSnap);
	return modBaseAddr;
}

uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets)
{
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); ++i)
	{
		ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), 0);
		addr += offsets[i];
	}
	return addr;
}