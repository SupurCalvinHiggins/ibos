#include "int.h"

typedef struct {
  void *ptr;
  usize size;
} IBOS_memory_block_t;

void IBOS_memory_initialize(IBOS_memory_block_t memory);
IBOS_memory_block_t IBOS_memory_static_allocate(usize size, usize alignment);
IBOS_memory_block_t IBOS_memory_allocate(usize size, usize alignment);
void IBOS_memory_deallocate(IBOS_memory_block_t block);
