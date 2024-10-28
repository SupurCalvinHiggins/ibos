#include "../include/ptr.h"
#include "../include/assert.h"

bool IBOS_ptr_aligned(uptr ptr, usize alignment) {
  IBOS_assert(alignment <= IBOS_PTR_MAX_ALIGNMENT);
  uptr mask = (1 << alignment) - 1;
  return (ptr & mask) == 0;
}

usize IBOS_ptr_get_aligned(uptr ptr, usize alignment) {
  IBOS_assert(alignment <= IBOS_PTR_MAX_ALIGNMENT);
  uptr mask = (1 << alignment) - 1;
  uptr masked_ptr = ptr & mask;
  uptr aligned_ptr = masked_ptr == 0 ? ptr : ptr + mask - masked_ptr + 1;
  IBOS_assert(IBOS_ptr_aligned(aligned_ptr, alignment));
  IBOS_assert(ptr <= aligned_ptr);
  return aligned_ptr;
}
