#include "memory.h"
#include<TlHelp32.h>

Memory::Memory(const char* processName)
{
	//here we loop through the processes and find required process;
	// we need handles because read and write can only be accesed by handles;
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	const auto snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	while (Process32Next(snapShot, &entry))
	{
		if (!strcmp(processName, entry.szExeFile))
		{
			this->id = entry.th32ProcessID;
			this->process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, this->id);
			break;
		}
	}

	if (snapShot) {
		CloseHandle(snapShot);
	}
}
Memory::~Memory()
{
	if (this->process)
		CloseHandle(this->process);
}

DWORD Memory::GetProcesID()
{
	return this->id;
}

HANDLE Memory::GetProcessHandle()
{
	return this->process;
}

uintptr_t Memory::GetModuleAddress(const char* moduleName)
{
	MODULEENTRY32 entry;
	entry.dwSize = sizeof(MODULEENTRY32);

	const auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, this->id);

	uintptr_t result = 0;

	while (Module32Next(snapshot, &entry))
	{
		if (!strcmp(moduleName, entry.szModule))
		{
			result = reinterpret_cast<uintptr_t>(entry.modBaseAddr);
			break;
		}
	}
	if (snapshot) {
		CloseHandle(snapshot);
	}

	return result;
}