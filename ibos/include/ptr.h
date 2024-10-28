#pragma once

#include "int.h"

#define IBOS_PTR_MAX_ALIGNMENT 8

bool IBOS_ptr_aligned(uptr ptr, usize alignment);
usize IBOS_ptr_get_aligned(uptr ptr, usize alignment);
