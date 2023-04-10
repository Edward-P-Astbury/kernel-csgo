#pragma once

#include "hook.h"
#include <TlHelp32.h>

namespace memory
{
	int get_process_id(const char* process_name);
}