#include "../../include/assert.h"
#include "../../include/int.h"
#include "../../include/memory.h"
#include "../../include/port.h"
#include "../../include/ptr.h"
#include "../include/test.h"
#include <stdalign.h>

#define IBOS_HELPER_MIN(a, b) ((a) < (b) ? (a) : (b))

usize IBOS_helper_overlap(IBOS_memory_block_t block1,
                          IBOS_memory_block_t block2) {
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

void IBOS_test_memory_allocate_tiny(void) {
  IBOS_TEST_START();

  alignas(512) u8 buffer[512 * 4];
  IBOS_memory_block_t memory = {buffer, 512 * 4};
  IBOS_memory_initialize(memory);

  IBOS_memory_block_t block = IBOS_memory_allocate(1);
  IBOS_assert(block.size >= 1);
  IBOS_assert(IBOS_helper_overlap(block, memory) == block.size);
  IBOS_assert(IBOS_ptr_is_aligned((uptr)block.ptr, IBOS_PORT_STRUCT_ALIGNMENT));

  IBOS_TEST_END();
}

void IBOS_test_memory_allocate_small(void) {
  IBOS_TEST_START();

  alignas(512) u8 buffer[512 * 4];
  IBOS_memory_block_t memory = {buffer, 512 * 4};
  IBOS_memory_initialize(memory);

  IBOS_memory_block_t block = IBOS_memory_allocate(16);
  IBOS_assert(block.size >= 16);
  IBOS_assert(IBOS_helper_overlap(block, memory) == block.size);
  IBOS_assert(IBOS_ptr_is_aligned((uptr)block.ptr, IBOS_PORT_STRUCT_ALIGNMENT));

  IBOS_TEST_END();
}

void IBOS_test_memory_allocate_medium(void) {
  IBOS_TEST_START();

  alignas(512) u8 buffer[512 * 4];
  IBOS_memory_block_t memory = {buffer, 512 * 4};
  IBOS_memory_initialize(memory);

  IBOS_memory_block_t block = IBOS_memory_allocate(103);
  IBOS_assert(block.size >= 103);
  IBOS_assert(IBOS_helper_overlap(block, memory) == block.size);
  IBOS_assert(IBOS_ptr_is_aligned((uptr)block.ptr, IBOS_PORT_STRUCT_ALIGNMENT));

  IBOS_TEST_END();
}

void IBOS_test_memory_allocate_large(void) {
  IBOS_TEST_START();

  alignas(512) u8 buffer[512 * 4];
  IBOS_memory_block_t memory = {buffer, 512 * 4};
  IBOS_memory_initialize(memory);

  IBOS_memory_block_t block = IBOS_memory_allocate(512);
  IBOS_assert(block.size >= 512);
  IBOS_assert(IBOS_helper_overlap(block, memory) == block.size);
  IBOS_assert(IBOS_ptr_is_aligned((uptr)block.ptr, IBOS_PORT_STRUCT_ALIGNMENT));

  IBOS_TEST_END();
}

void IBOS_test_memory_allocate_all(void) {
  IBOS_TEST_START();

  alignas(512) u8 buffer[512 * 4];
  IBOS_memory_block_t memory = {buffer, 512 * 4};
  IBOS_memory_initialize(memory);

  IBOS_memory_block_t block1 = IBOS_memory_allocate(3);
  IBOS_assert(block1.size >= 3);
  IBOS_assert(IBOS_helper_overlap(block1, memory) == block1.size);
  IBOS_assert(
      IBOS_ptr_is_aligned((uptr)block1.ptr, IBOS_PORT_STRUCT_ALIGNMENT));

  IBOS_memory_block_t block2 = IBOS_memory_allocate(16);
  IBOS_assert(block2.size >= 16);
  IBOS_assert(IBOS_helper_overlap(block2, memory) == block2.size);
  IBOS_assert(
      IBOS_ptr_is_aligned((uptr)block2.ptr, IBOS_PORT_STRUCT_ALIGNMENT));
  IBOS_assert(IBOS_helper_overlap(block2, block1) == 0);

  IBOS_memory_block_t block3 = IBOS_memory_allocate(17);
  IBOS_assert(block3.size >= 17);
  IBOS_assert(IBOS_helper_overlap(block3, memory) == block3.size);
  IBOS_assert(
      IBOS_ptr_is_aligned((uptr)block3.ptr, IBOS_PORT_STRUCT_ALIGNMENT));
  IBOS_assert(IBOS_helper_overlap(block3, block1) == 0);
  IBOS_assert(IBOS_helper_overlap(block3, block2) == 0);

  IBOS_memory_block_t block4 = IBOS_memory_allocate(512);
  IBOS_assert(block4.size >= 512);
  IBOS_assert(IBOS_helper_overlap(block4, memory) == block4.size);
  IBOS_assert(
      IBOS_ptr_is_aligned((uptr)block4.ptr, IBOS_PORT_STRUCT_ALIGNMENT));
  IBOS_assert(IBOS_helper_overlap(block4, block1) == 0);
  IBOS_assert(IBOS_helper_overlap(block4, block2) == 0);
  IBOS_assert(IBOS_helper_overlap(block4, block3) == 0);

  IBOS_TEST_END();
}

void IBOS_test_memory_allocate_full_small(void) {
  IBOS_TEST_START();

  alignas(512) u8 buffer[512 * 4];
  IBOS_memory_block_t memory = {buffer, 512 * 4};
  IBOS_memory_initialize(memory);

  IBOS_memory_block_t blocks[16 * 3];
  usize blocks_len = 0;
  for (usize i = 0; i < 16 * 3; ++i) {
    IBOS_memory_block_t block = IBOS_memory_allocate(16);
    IBOS_assert(block.size >= 16);
    IBOS_assert(IBOS_helper_overlap(block, memory) == block.size);
    IBOS_assert(
        IBOS_ptr_is_aligned((uptr)block.ptr, IBOS_PORT_STRUCT_ALIGNMENT));
    for (usize j = 0; j < blocks_len; ++j) {
      IBOS_assert(IBOS_helper_overlap(block, blocks[j]) == 0);
    }
    blocks[blocks_len] = block;
    ++blocks_len;
  }

  IBOS_TEST_END();
}

void IBOS_test_memory_allocate_full_large(void) {
  IBOS_TEST_START();

  alignas(512) u8 buffer[512 * 4];
  IBOS_memory_block_t memory = {buffer, 512 * 4};
  IBOS_memory_initialize(memory);

  IBOS_memory_block_t blocks[3];
  usize blocks_len = 0;
  for (usize i = 0; i < 3; ++i) {
    IBOS_memory_block_t block = IBOS_memory_allocate(512);
    IBOS_assert(block.size >= 512);
    IBOS_assert(IBOS_helper_overlap(block, memory) == block.size);
    IBOS_assert(
        IBOS_ptr_is_aligned((uptr)block.ptr, IBOS_PORT_STRUCT_ALIGNMENT));
    for (usize j = 0; j < blocks_len; ++j) {
      IBOS_assert(IBOS_helper_overlap(block, blocks[j]) == 0);
    }
    blocks[blocks_len] = block;
    ++blocks_len;
  }

  IBOS_TEST_END();
}

void IBOS_test_memory_allocate_full_all(void) {
  IBOS_TEST_START();

  alignas(512) u8 buffer[512 * 4];
  IBOS_memory_block_t memory = {buffer, 512 * 4};
  IBOS_memory_initialize(memory);

  IBOS_memory_block_t blocks[2 + 16];
  usize blocks_len = 0;

  {
    IBOS_memory_block_t block = IBOS_memory_allocate(512);
    IBOS_assert(block.size >= 512);
    IBOS_assert(IBOS_helper_overlap(block, memory) == block.size);
    IBOS_assert(
        IBOS_ptr_is_aligned((uptr)block.ptr, IBOS_PORT_STRUCT_ALIGNMENT));
    blocks[blocks_len] = block;
    ++blocks_len;
  }

  for (usize i = 0; i < 8; ++i) {
    IBOS_memory_block_t block = IBOS_memory_allocate(16);
    IBOS_assert(block.size >= 16);
    IBOS_assert(IBOS_helper_overlap(block, memory) == block.size);
    IBOS_assert(
        IBOS_ptr_is_aligned((uptr)block.ptr, IBOS_PORT_STRUCT_ALIGNMENT));
    for (usize j = 0; j < blocks_len; ++j) {
      IBOS_assert(IBOS_helper_overlap(block, blocks[j]) == 0);
    }
    blocks[blocks_len] = block;
    ++blocks_len;
  }

  {
    IBOS_memory_block_t block = IBOS_memory_allocate(512);
    IBOS_assert(block.size >= 512);
    IBOS_assert(IBOS_helper_overlap(block, memory) == block.size);
    IBOS_assert(
        IBOS_ptr_is_aligned((uptr)block.ptr, IBOS_PORT_STRUCT_ALIGNMENT));
    for (usize j = 0; j < blocks_len; ++j) {
      IBOS_assert(IBOS_helper_overlap(block, blocks[j]) == 0);
    }
    blocks[blocks_len] = block;
    ++blocks_len;
  }

  for (usize i = 0; i < 8; ++i) {
    IBOS_memory_block_t block = IBOS_memory_allocate(16);
    IBOS_assert(block.size >= 16);
    IBOS_assert(IBOS_helper_overlap(block, memory) == block.size);
    IBOS_assert(
        IBOS_ptr_is_aligned((uptr)block.ptr, IBOS_PORT_STRUCT_ALIGNMENT));
    for (usize j = 0; j < blocks_len; ++j) {
      IBOS_assert(IBOS_helper_overlap(block, blocks[j]) == 0);
    }
    blocks[blocks_len] = block;
    ++blocks_len;
  }

  IBOS_TEST_END();
}

void IBOS_test_memory_deallocate_tiny(void) {
  IBOS_TEST_START();

  alignas(512) u8 buffer[512 * 4];
  IBOS_memory_block_t memory = {buffer, 512 * 4};
  IBOS_memory_initialize(memory);

  IBOS_memory_block_t block = IBOS_memory_allocate(1);
  IBOS_memory_deallocate(block);

  IBOS_TEST_END();
}

void IBOS_test_memory_deallocate_small(void) {
  IBOS_TEST_START();

  alignas(512) u8 buffer[512 * 4];
  IBOS_memory_block_t memory = {buffer, 512 * 4};
  IBOS_memory_initialize(memory);

  IBOS_memory_block_t block = IBOS_memory_allocate(16);
  IBOS_memory_deallocate(block);

  IBOS_TEST_END();
}

void IBOS_test_memory_deallocate_medium(void) {
  IBOS_TEST_START();

  alignas(512) u8 buffer[512 * 4];
  IBOS_memory_block_t memory = {buffer, 512 * 4};
  IBOS_memory_initialize(memory);

  IBOS_memory_block_t block = IBOS_memory_allocate(103);
  IBOS_memory_deallocate(block);

  IBOS_TEST_END();
}

void IBOS_test_memory_deallocate_large(void) {
  IBOS_TEST_START();

  alignas(512) u8 buffer[512 * 4];
  IBOS_memory_block_t memory = {buffer, 512 * 4};
  IBOS_memory_initialize(memory);

  IBOS_memory_block_t block = IBOS_memory_allocate(512);
  IBOS_memory_deallocate(block);

  IBOS_TEST_END();
}

void IBOS_test_memory_deallocate_all(void) {
  IBOS_TEST_START();

  alignas(512) u8 buffer[512 * 4];
  IBOS_memory_block_t memory = {buffer, 512 * 4};
  IBOS_memory_initialize(memory);

  IBOS_memory_block_t block1 = IBOS_memory_allocate(3);
  IBOS_memory_block_t block2 = IBOS_memory_allocate(16);
  IBOS_memory_block_t block3 = IBOS_memory_allocate(17);
  IBOS_memory_block_t block4 = IBOS_memory_allocate(512);
  IBOS_memory_deallocate(block1);
  IBOS_memory_deallocate(block2);
  IBOS_memory_deallocate(block3);
  IBOS_memory_deallocate(block4);

  IBOS_TEST_END();
}

void IBOS_test_memory_deallocate_full_small(void) {
  IBOS_TEST_START();

  alignas(512) u8 buffer[512 * 4];
  IBOS_memory_block_t memory = {buffer, 512 * 4};
  IBOS_memory_initialize(memory);

  IBOS_memory_block_t blocks[16 * 3];
  usize blocks_len = 0;
  for (usize i = 0; i < 16 * 3; ++i) {
    IBOS_memory_block_t block = IBOS_memory_allocate(16);
    blocks[blocks_len] = block;
    ++blocks_len;
  }

  for (usize i = 0; i < blocks_len; ++i) {
    IBOS_memory_deallocate(blocks[i]);
  }

  IBOS_TEST_END();
}

void IBOS_test_memory_deallocate_full_large(void) {
  IBOS_TEST_START();

  alignas(512) u8 buffer[512 * 4];
  IBOS_memory_block_t memory = {buffer, 512 * 4};
  IBOS_memory_initialize(memory);

  IBOS_memory_block_t blocks[3];
  usize blocks_len = 0;
  for (usize i = 0; i < 3; ++i) {
    IBOS_memory_block_t block = IBOS_memory_allocate(512);
    blocks[blocks_len] = block;
    ++blocks_len;
  }

  for (usize i = 0; i < blocks_len; ++i) {
    IBOS_memory_deallocate(blocks[i]);
  }

  IBOS_TEST_END();
}

void IBOS_test_memory_deallocate_full_all(void) {
  IBOS_TEST_START();

  alignas(512) u8 buffer[512 * 4];
  IBOS_memory_block_t memory = {buffer, 512 * 4};
  IBOS_memory_initialize(memory);

  IBOS_memory_block_t blocks[2 + 16];
  usize blocks_len = 0;

  {
    IBOS_memory_block_t block = IBOS_memory_allocate(512);
    blocks[blocks_len] = block;
    ++blocks_len;
  }

  for (usize i = 0; i < 8; ++i) {
    IBOS_memory_block_t block = IBOS_memory_allocate(16);
    blocks[blocks_len] = block;
    ++blocks_len;
  }

  {
    IBOS_memory_block_t block = IBOS_memory_allocate(512);
    blocks[blocks_len] = block;
    ++blocks_len;
  }

  for (usize i = 0; i < 8; ++i) {
    IBOS_memory_block_t block = IBOS_memory_allocate(16);
    blocks[blocks_len] = block;
    ++blocks_len;
  }

  for (usize i = 0; i < blocks_len; ++i) {
    IBOS_memory_deallocate(blocks[i]);
  }

  IBOS_TEST_END();
}

void IBOS_test_memory_reallocate_full_all(void) {
  IBOS_TEST_START();

  alignas(512) u8 buffer[512 * 4];
  IBOS_memory_block_t memory = {buffer, 512 * 4};
  IBOS_memory_initialize(memory);

  IBOS_memory_block_t blocks[2 + 16];
  usize blocks_len = 0;

  {
    IBOS_memory_block_t block = IBOS_memory_allocate(512);
    IBOS_assert(block.size >= 512);
    IBOS_assert(IBOS_helper_overlap(block, memory) == block.size);
    IBOS_assert(
        IBOS_ptr_is_aligned((uptr)block.ptr, IBOS_PORT_STRUCT_ALIGNMENT));
    blocks[blocks_len] = block;
    ++blocks_len;
  }

  for (usize i = 0; i < 8; ++i) {
    IBOS_memory_block_t block = IBOS_memory_allocate(16);
    IBOS_assert(block.size >= 16);
    IBOS_assert(IBOS_helper_overlap(block, memory) == block.size);
    IBOS_assert(
        IBOS_ptr_is_aligned((uptr)block.ptr, IBOS_PORT_STRUCT_ALIGNMENT));
    for (usize j = 0; j < blocks_len; ++j) {
      IBOS_assert(IBOS_helper_overlap(block, blocks[j]) == 0);
    }
    blocks[blocks_len] = block;
    ++blocks_len;
  }

  for (usize i = 0; i < 4; ++i) {
    IBOS_memory_deallocate(blocks[blocks_len - i - 1]);
  }
  blocks_len -= 4;

  {
    IBOS_memory_block_t block = IBOS_memory_allocate(512);
    IBOS_assert(block.size >= 512);
    IBOS_assert(IBOS_helper_overlap(block, memory) == block.size);
    IBOS_assert(
        IBOS_ptr_is_aligned((uptr)block.ptr, IBOS_PORT_STRUCT_ALIGNMENT));
    for (usize j = 0; j < blocks_len; ++j) {
      IBOS_assert(IBOS_helper_overlap(block, blocks[j]) == 0);
    }
    blocks[blocks_len] = block;
    ++blocks_len;
  }

  for (usize i = 0; i < 12; ++i) {
    IBOS_memory_block_t block = IBOS_memory_allocate(16);
    IBOS_assert(block.size >= 16);
    IBOS_assert(IBOS_helper_overlap(block, memory) == block.size);
    IBOS_assert(
        IBOS_ptr_is_aligned((uptr)block.ptr, IBOS_PORT_STRUCT_ALIGNMENT));
    for (usize j = 0; j < blocks_len; ++j) {
      IBOS_assert(IBOS_helper_overlap(block, blocks[j]) == 0);
    }
    blocks[blocks_len] = block;
    ++blocks_len;
  }

  for (usize i = 0; i < blocks_len; ++i) {
    IBOS_memory_deallocate(blocks[i]);
  }
  blocks_len = 0;

  {
    IBOS_memory_block_t block = IBOS_memory_allocate(512);
    IBOS_assert(block.size >= 512);
    IBOS_assert(IBOS_helper_overlap(block, memory) == block.size);
    IBOS_assert(
        IBOS_ptr_is_aligned((uptr)block.ptr, IBOS_PORT_STRUCT_ALIGNMENT));
    blocks[blocks_len] = block;
    ++blocks_len;
  }

  for (usize i = 0; i < 8; ++i) {
    IBOS_memory_block_t block = IBOS_memory_allocate(16);
    IBOS_assert(block.size >= 16);
    IBOS_assert(IBOS_helper_overlap(block, memory) == block.size);
    IBOS_assert(
        IBOS_ptr_is_aligned((uptr)block.ptr, IBOS_PORT_STRUCT_ALIGNMENT));
    for (usize j = 0; j < blocks_len; ++j) {
      IBOS_assert(IBOS_helper_overlap(block, blocks[j]) == 0);
    }
    blocks[blocks_len] = block;
    ++blocks_len;
  }

  for (usize i = 0; i < 4; ++i) {
    IBOS_memory_deallocate(blocks[blocks_len - i - 1]);
  }
  blocks_len -= 4;

  {
    IBOS_memory_block_t block = IBOS_memory_allocate(512);
    IBOS_assert(block.size >= 512);
    IBOS_assert(IBOS_helper_overlap(block, memory) == block.size);
    IBOS_assert(
        IBOS_ptr_is_aligned((uptr)block.ptr, IBOS_PORT_STRUCT_ALIGNMENT));
    for (usize j = 0; j < blocks_len; ++j) {
      IBOS_assert(IBOS_helper_overlap(block, blocks[j]) == 0);
    }
    blocks[blocks_len] = block;
    ++blocks_len;
  }

  for (usize i = 0; i < 12; ++i) {
    IBOS_memory_block_t block = IBOS_memory_allocate(16);
    IBOS_assert(block.size >= 16);
    IBOS_assert(IBOS_helper_overlap(block, memory) == block.size);
    IBOS_assert(
        IBOS_ptr_is_aligned((uptr)block.ptr, IBOS_PORT_STRUCT_ALIGNMENT));
    for (usize j = 0; j < blocks_len; ++j) {
      IBOS_assert(IBOS_helper_overlap(block, blocks[j]) == 0);
    }
    blocks[blocks_len] = block;
    ++blocks_len;
  }

  for (usize i = 0; i < blocks_len; ++i) {
    IBOS_memory_deallocate(blocks[i]);
  }
  blocks_len = 0;

  IBOS_TEST_END();
}

void IBOS_test_memory(void) {
  IBOS_test_memory_allocate_tiny();
  IBOS_test_memory_allocate_small();
  IBOS_test_memory_allocate_medium();
  IBOS_test_memory_allocate_large();
  IBOS_test_memory_allocate_all();
  IBOS_test_memory_allocate_full_small();
  IBOS_test_memory_allocate_full_large();
  IBOS_test_memory_allocate_full_all();

  IBOS_test_memory_deallocate_tiny();
  IBOS_test_memory_deallocate_small();
  IBOS_test_memory_deallocate_medium();
  IBOS_test_memory_deallocate_large();
  IBOS_test_memory_deallocate_all();
  IBOS_test_memory_deallocate_full_small();
  IBOS_test_memory_deallocate_full_large();
  IBOS_test_memory_deallocate_full_all();

  IBOS_test_memory_reallocate_full_all();
}

int main(void) { IBOS_test_memory(); }
