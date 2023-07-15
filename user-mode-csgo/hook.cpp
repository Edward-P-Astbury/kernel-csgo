#include "hook.h"

void hook::call_hook(void* info_struct)
{
	void* hooked_function = GetProcAddress(GetModuleHandleA("win32u.dll"), "NtFlipObjectReadNextMessageToProducer"); // pass in the function that we hooked (alternative: NtQueryCompositionSurfaceStatistics)

	auto function = static_cast<void(_stdcall*)(void*)>(hooked_function);

	function(info_struct);
}
