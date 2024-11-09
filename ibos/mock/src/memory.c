#include "../../include/memory.h"
#include "../../include/assert.h"
#include "../../include/port.h"
#include <string.h>

void IBOS_memory_initialize(IBOS_memory_block_t block) { (void)block; }
IBOS_memory_block_t IBOS_memory_allocate(usize size) {
  u8 *ptr = aligned_alloc(IBOS_PORT_STRUCT_ALIGNMENT, size);
  IBOS_assert(ptr != NULL);
  return (IBOS_memory_block_t){ptr, size};
}

void IBOS_memory_deallocate(IBOS_memory_block_t block) { free(block.ptr); }
