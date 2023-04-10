#include "hook.h"

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT driver_object, PUNICODE_STRING reg_path)
{
	UNREFERENCED_PARAMETER(driver_object);
	UNREFERENCED_PARAMETER(reg_path);

	if (hook::call_kernel_function(&hook::handler))
	{
		DbgPrintEx(0, 0, "Driver loaded\n");
		DbgPrintEx(0, 0, "NtQueryCompositionSurfaceStatistics function hooked\n");
		return STATUS_SUCCESS;
	}
	else
	{
		DbgPrintEx(0, 0, "Function hook failed\n");
		return STATUS_FAILED_DRIVER_ENTRY;
	}
}