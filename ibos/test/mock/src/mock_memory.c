#include "../include/mock_memory.h"
#include "../../../include/assert.h"
#include <string.h>

IBOS_memory_block_t IBOS_memory_static_allocate(usize size, usize alignment) {
  // TODO: implement.
  IBOS_assert(false);
}

IBOS_memory_block_t IBOS_memory_allocate(usize size, usize alignment) {
  u8 *ptr = aligned_alloc(alignment, size);
  IBOS_assert(ptr != NULL);
  return (IBOS_memory_block_t){ptr, size};
}

void IBOS_memory_deallocate(IBOS_memory_block_t block) { free(block.ptr); }
