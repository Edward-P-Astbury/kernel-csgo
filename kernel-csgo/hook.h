#pragma once

#include "memory.h"

namespace hook
{
	bool call_kernel_function(void* kernel_function_address);
	void handler(PVOID called_param);
}