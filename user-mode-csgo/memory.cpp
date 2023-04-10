#include "memory.h"

int memory::get_process_id(const char* process_name)
{
	ULONG process_id = 0;
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (snapshot)
	{
		PROCESSENTRY32 process_entry;
		process_entry.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(snapshot, &process_entry))
		{
			do
			{
				if (!strcmp(process_entry.szExeFile, process_name))
				{
					process_id = process_entry.th32ProcessID;
					break;
				}
			} while (Process32Next(snapshot, &process_entry));
		}
		CloseHandle(snapshot);
	}
	return process_id;
}
