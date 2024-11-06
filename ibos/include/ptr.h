#pragma once

#include "../port/include/port.h"
#include "int.h"

bool IBOS_ptr_is_alignment(usize alignment);
bool IBOS_ptr_is_aligned(uptr ptr, usize alignment);
usize IBOS_ptr_get_aligned(uptr ptr, usize alignment);
