#pragma once

#include "definitions.h"

namespace memory
{
	void* get_system_module_base(const char* module_name);
	void* get_system_module_export(const char* module_name, const char* function_name);
	bool write_to_read_only_memory(void* address, void* buffer, size_t size);
	ULONG get_module_base(PEPROCESS process, LPCWSTR module_name);
	NTSTATUS write_memory(PEPROCESS target_process, void* source, void* target, size_t size);
	NTSTATUS read_memory(PEPROCESS target_process, void* source, void* target, size_t size);
}