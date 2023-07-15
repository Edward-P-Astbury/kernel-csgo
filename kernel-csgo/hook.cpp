#include "hook.h"

bool hook::call_kernel_function(void* kernel_function_address)
{
    if (!kernel_function_address)
        return false;

    void** hook_function = reinterpret_cast<PVOID*>(memory::get_system_module_export("\\SystemRoot\\System32\\drivers\\dxgkrnl.sys", "NtFlipObjectReadNextMessageToProducer")); // (alternative: NtQueryCompositionSurfaceStatistics)

    if (!hook_function)
        return false;

    BYTE original_function[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    BYTE shell_code_start[] = { 0x48, 0xB8 }; // mov rax
    BYTE shell_code_end[] = { 0xFF, 0xE0 }; // jmp rax

    RtlSecureZeroMemory(&original_function, sizeof(original_function));
    memcpy((PVOID)((ULONG_PTR)original_function), &shell_code_start, sizeof(shell_code_start));

    uintptr_t hook_address = reinterpret_cast<uintptr_t>(kernel_function_address);

    memcpy((PVOID)((ULONG_PTR)original_function + sizeof(shell_code_start)), &hook_address, sizeof(void*));
    memcpy((PVOID)((ULONG_PTR)original_function + sizeof(shell_code_start) + sizeof(void*)), &shell_code_end, sizeof(shell_code_end));

    memory::write_to_read_only_memory(hook_function, &original_function, sizeof(original_function));

    return true;
}

void hook::handler(PVOID called_param)
{
	PINFO_STRUCT info = (PINFO_STRUCT)called_param;

	if (info->code == CLIENT_MODULE_REQUEST) // request the base 
	{
		PEPROCESS target_process = NULL;
		if (NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)info->process_id, &target_process)))
		{
			KAPC_STATE apc;
			KeStackAttachProcess(target_process, &apc);

			ULONG base = memory::get_module_base(target_process, L"client.dll"); // CSGO specific dll
			KeUnstackDetachProcess(&apc);
			if (base)
				info->client_base = base;
		}
	}
	else if (info->code == CODE_READ_MEMORY) // read
	{
		PEPROCESS target_process = NULL;
		if (NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)info->process_id, &target_process)))
		{
			memory::read_memory(target_process, (void*)info->address, &info->buffer, info->size);
		}
	}
	else if (info->code == CODE_WRITE_MEMORY) // write
	{
		PEPROCESS target_process = NULL;
		if (NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)info->process_id, &target_process)))
		{
			memory::write_memory(target_process, &info->buffer, (void*)info->address, info->size);
		}
	}
	else if (info->code == ENGINE_MODULE_REQUEST)
	{
		PEPROCESS target_process = NULL;
		if (NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)info->process_id, &target_process)))
		{
			KAPC_STATE apc;
			KeStackAttachProcess(target_process, &apc);

			ULONG base = memory::get_module_base(target_process, L"engine.dll");
			KeUnstackDetachProcess(&apc);
			if (base)
				info->engine_base = base;
		}
	}
}
