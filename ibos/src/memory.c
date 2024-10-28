#include "../include/memory.h"
#include "../include/assert.h"
#include "../include/ptr.h"

typedef enum {
  UNINITIALIZED,
  STATIC_ALLOCATE,
  DYNAMIC_ALLOCATE,
} IBOS_memory_state_t;

typedef struct {
  IBOS_memory_state_t state;
  uptr static_low_ptr;
  uptr static_high_ptr;
  uptr low_ptr;
  uptr high_ptr;
} IBOS_memory_t;

IBOS_memory_t IBOS_memory = {UNINITIALIZED, 0, 0, 0, 0};

void IBOS_memory_initialize(IBOS_memory_block_t memory) {
  IBOS_memory.state = STATIC_ALLOCATE;
  IBOS_memory.static_low_ptr = (uptr)memory.ptr;
  IBOS_memory.static_high_ptr = (uptr)(memory.ptr + memory.size);
  IBOS_memory.low_ptr = IBOS_memory.static_low_ptr;
  IBOS_memory.high_ptr = IBOS_memory.static_high_ptr;
}

IBOS_memory_block_t IBOS_memory_static_allocate(usize size, usize alignment) {
  IBOS_assert(IBOS_memory.state == STATIC_ALLOCATE);
  IBOS_memory.static_low_ptr =
      IBOS_ptr_get_aligned(IBOS_memory.static_low_ptr, alignment);
  IBOS_assert(size <= IBOS_memory.static_high_ptr - IBOS_memory.static_low_ptr);
  IBOS_memory_block_t block = {(void *)IBOS_memory.static_low_ptr, size};
  IBOS_memory.static_low_ptr += size;
  return block;
}

IBOS_memory_block_t IBOS_memory_allocate(usize size, usize alignment) {
  if (IBOS_memory.state == STATIC_ALLOCATE) {
    IBOS_memory.state = DYNAMIC_ALLOCATE;
    IBOS_memory.static_high_ptr = IBOS_memory.static_low_ptr;
    IBOS_memory.static_low_ptr = IBOS_memory.low_ptr;
    IBOS_memory.low_ptr = IBOS_memory.static_high_ptr;
    IBOS_assert(IBOS_memory.low_ptr <= IBOS_memory.high_ptr);
  }
  IBOS_assert(IBOS_memory.state == DYNAMIC_ALLOCATE);
  // TODO.
}

void IBOS_memory_deallocate(IBOS_memory_block_t block) {
  IBOS_assert(IBOS_memory.state == DYNAMIC_ALLOCATE);
  // TODO.
}
