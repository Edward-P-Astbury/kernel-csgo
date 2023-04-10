#include "memory.h"

#pragma warning (disable: 6011)
#pragma warning (disable: 4996)

void* memory::get_system_module_base(const char* module_name)
{
    void* module_base = 0;
    ULONG bytes = 0;
    NTSTATUS status = ZwQuerySystemInformation(SystemModuleInformation, NULL, bytes, &bytes);

    if (!bytes)
        return NULL;

    PRTL_PROCESS_MODULES modules = (PRTL_PROCESS_MODULES)ExAllocatePoolWithTag(NonPagedPool, bytes, 0x4744554b); // GDUK

    status = ZwQuerySystemInformation(SystemModuleInformation, modules, bytes, &bytes);

    if (!NT_SUCCESS(status))
        return NULL;

    PRTL_PROCESS_MODULE_INFORMATION module = modules->Modules;

    for (ULONG i = 0; i < modules->NumberOfModules; i++)
    {
        if (strcmp((char*)module[i].FullPathName, module_name) == 0)
        {
            module_base = module[i].ImageBase;
            break;
        }
    }

    if (modules)
        ExFreePoolWithTag(modules, 0x4744554b); // GDUK

    if (module_base <= 0)
        return NULL;

    return module_base;
}

void* memory::get_system_module_export(const char* module_name, const char* function_name)
{
    void* module = get_system_module_base(module_name);

    if (!module)
        return NULL;

    return RtlFindExportedRoutineByName(module, function_name);
}

bool memory::write_to_read_only_memory(void* address, void* buffer, size_t size) // wpm
{
    PMDL mdl = IoAllocateMdl(address, (ULONG)size, FALSE, FALSE, NULL);

    if (!mdl)
        return false;

    MmProbeAndLockPages(mdl, KernelMode, IoReadAccess);
    PVOID map = MmMapLockedPagesSpecifyCache(mdl, KernelMode, MmNonCached, NULL, FALSE, NormalPagePriority);
    MmProtectMdlSystemAddress(mdl, PAGE_READWRITE);

    memcpy(map, buffer, size);

    MmUnmapLockedPages(map, mdl);
    MmUnlockPages(mdl);
    IoFreeMdl(mdl);

    return true;
}

ULONG memory::get_module_base(PEPROCESS process, LPCWSTR module_name)
{
    if (!process)
        return NULL;

    __try
    {
        PPEB32 peb32 = (PPEB32)PsGetProcessWow64Process(process);

        if (!peb32 || !peb32->Ldr)
            return NULL;

        for (PLIST_ENTRY32 plist_entry = (PLIST_ENTRY32)((PPEB_LDR_DATA32)peb32->Ldr)->InLoadOrderModuleList.Flink;
            plist_entry != &((PPEB_LDR_DATA32)peb32->Ldr)->InLoadOrderModuleList;
            plist_entry = (PLIST_ENTRY32)plist_entry->Flink)
        {
            PLDR_DATA_TABLE_ENTRY32 pentry = CONTAINING_RECORD(plist_entry, LDR_DATA_TABLE_ENTRY32, InLoadOrderLinks);

            if (wcscmp((PWCH)pentry->BaseDllName.Buffer, module_name) == NULL)
            {
                return pentry->DllBase;
            }
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {

    }

    return NULL;
}

NTSTATUS memory::write_memory(PEPROCESS target_process, void* source, void* target, size_t size)
{
    if (!target_process)
        return STATUS_INVALID_PARAMETER;

    size_t bytes = 0;
    NTSTATUS status = MmCopyVirtualMemory(IoGetCurrentProcess(), source, target_process, target, size, KernelMode, &bytes);
    if (!NT_SUCCESS(status) || !bytes)
    {
        DbgPrintEx(0, 0, "Process lookup failed (write)");
        return STATUS_INVALID_ADDRESS;
    }
    return status;
}

NTSTATUS memory::read_memory(PEPROCESS target_process, void* source, void* target, size_t size)
{
    if (!target_process)
        return STATUS_INVALID_PARAMETER;

    size_t bytes = 0;
    NTSTATUS status = MmCopyVirtualMemory(target_process, source, IoGetCurrentProcess(), target, size, KernelMode, &bytes);
    if (!NT_SUCCESS(status) || !bytes)
    {
        DbgPrintEx(0, 0, "Process lookup failed (read)");
        return STATUS_INVALID_ADDRESS;
    }
    return status;
}
