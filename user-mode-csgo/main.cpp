#include "cheat.h"

int main()
{
	printf("[user-mode-csgo]: Loading necessary modules...\n");
	LoadLibraryA("user32.dll");
	LoadLibraryA("win32u.dll");
	if (GetModuleHandleA("win32u.dll"))
	{
		printf("[user - mode - csgo]: Modules loaded");
		cheat::run();
	}
	else
	{
		printf("[kernel-csgo]: Modules failed to load, quitting\n");
	}

	Sleep(5000);
	return 0;
}