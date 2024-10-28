#include "../include/assert.h"
#include "../include/int.h"
#include "../include/memory.h"
#include "../include/ptr.h"
#include <stdalign.h>

#define IBOS_HELPER_MIN(a, b) ((a) < (b) ? (a) : (b))

usize IBOS_helper_overlap(IBOS_memory_block_t block1, IBOS_memory_block_t block2) {
  if (block1.ptr > block2.ptr) {
    void *ptr = block1.ptr;
    usize size = block1.size;
    block1.ptr = block2.ptr;
    block1.size = block2.size;
    block2.ptr = ptr;
    block2.size = size;
  }

  if (block1.ptr + block1.size <= block2.ptr) {
    return 0;
  }

  return IBOS_HELPER_MIN(block1.ptr + block1.size - block2.ptr, block2.size);
}

bool IBOS_helper_aligned(IBOS_memory_block_t block, usize alignment) {
  return IBOS_ptr_aligned((uptr)block.ptr, alignment);
}

void IBOS_test_memory_static_allocate(void) {
  alignas(256) u8 buffer[256];
  IBOS_memory_block_t memory = {buffer, 256};
  IBOS_memory_initialize(memory);

  IBOS_memory_block_t block = IBOS_memory_static_allocate(1, 0);
  IBOS_assert(block.size == 1);
  IBOS_assert(IBOS_helper_aligned(block, 0));
  IBOS_assert(IBOS_helper_overlap(block, memory) == 1);
}

void IBOS_test_memory_static_allocate_aligned(void) {
  alignas(256) u8 buffer[256];
  IBOS_memory_block_t memory = {buffer + 1, 255};
  IBOS_memory_initialize(memory);

  IBOS_memory_block_t block = IBOS_memory_static_allocate(4, 2);
  IBOS_assert(block.size == 4);
  IBOS_assert(IBOS_helper_aligned(block, 2));
  IBOS_assert(IBOS_helper_overlap(block, memory) == 4);
}

void IBOS_test_memory_static_allocate_multiple(void) {
  alignas(256) u8 buffer[256];
  IBOS_memory_block_t memory = {buffer, 256};
  IBOS_memory_initialize(memory);

  IBOS_memory_block_t block1 = IBOS_memory_static_allocate(1, 0);
  IBOS_assert(block1.size == 1);
  IBOS_assert(IBOS_helper_aligned(block1, 0));
  IBOS_assert(IBOS_helper_overlap(block1, memory) == 1);

  IBOS_memory_block_t block2 = IBOS_memory_static_allocate(4, 2);
  IBOS_assert(block2.size == 4);
  IBOS_assert(IBOS_helper_aligned(block2, 2));
  IBOS_assert(IBOS_helper_overlap(block2, memory) == 4);
  IBOS_assert(IBOS_helper_overlap(block2, block1) == 0);

  IBOS_memory_block_t block3 = IBOS_memory_static_allocate(3, 5);
  IBOS_assert(block3.size == 3);
  IBOS_assert(IBOS_helper_aligned(block3, 5));
  IBOS_assert(IBOS_helper_overlap(block3, memory) == 3);
  IBOS_assert(IBOS_helper_overlap(block3, block1) == 0);
  IBOS_assert(IBOS_helper_overlap(block3, block2) == 0);

  IBOS_memory_block_t block4 = IBOS_memory_static_allocate(220, 1);
  IBOS_assert(block4.size == 220);
  IBOS_assert(IBOS_helper_aligned(block4, 1));
  IBOS_assert(IBOS_helper_overlap(block4, memory) == 220);
  IBOS_assert(IBOS_helper_overlap(block4, block1) == 0);
  IBOS_assert(IBOS_helper_overlap(block4, block2) == 0);
  IBOS_assert(IBOS_helper_overlap(block4, block3) == 0);
}

void IBOS_test_memory(void) {
  IBOS_test_memory_static_allocate();
  IBOS_test_memory_static_allocate_aligned();
  IBOS_test_memory_static_allocate_multiple();
}

int main(void) { IBOS_test_memory(); }
