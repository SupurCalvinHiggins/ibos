#include "../include/ptr.h"
#include "../include/assert.h"

bool IBOS_ptr_is_alignment(usize alignment) {
  return (alignment <= IBOS_PORT_MAX_ALIGNMENT) && (alignment != 0) &&
         ((alignment & (alignment - 1)) == 0);
}

bool IBOS_ptr_is_aligned(uptr ptr, usize alignment) {
  IBOS_assert(IBOS_ptr_is_alignment(alignment));
  uptr mask = alignment - 1;
  return (ptr & mask) == 0;
}

usize IBOS_ptr_get_aligned(uptr ptr, usize alignment) {
  IBOS_assert(IBOS_ptr_is_alignment(alignment));
  uptr mask = alignment - 1;
  uptr masked_ptr = ptr & mask;
  uptr aligned_ptr = masked_ptr == 0 ? ptr : ptr + alignment - masked_ptr;
  IBOS_assert(IBOS_ptr_is_aligned(aligned_ptr, alignment));
  IBOS_assert(ptr <= aligned_ptr);
  return aligned_ptr;
}
