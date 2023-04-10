#pragma once

#include <Windows.h>
#include <iostream>

#define CLIENT_MODULE_REQUEST 0x1
#define CODE_READ_MEMORY 0x2
#define CODE_WRITE_MEMORY 0x3
#define ENGINE_MODULE_REQUEST 0x4 

typedef struct _INFO_STRUCT
{
	ULONG code;
	ULONG process_id;
	ULONG client_base;
	ULONG engine_base;
	ULONG address;
	void* buffer;
	ULONG size;
} INFO_STRUCT, * PINFO_STRUCT;