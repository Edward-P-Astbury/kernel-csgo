#include "hook.h"

void hook::call_hook(void* info_struct)
{
	void* hooked_function = GetProcAddress(GetModuleHandleA("win32u.dll"), "NtQueryCompositionSurfaceStatistics"); // pass in the function that we hooked

	auto function = static_cast<void(_stdcall*)(void*)>(hooked_function);

	function(info_struct);
}
