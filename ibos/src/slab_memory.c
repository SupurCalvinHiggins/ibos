#include "../include/assert.h"
#include "../include/memory.h"
#include "../include/port.h"
#include "../include/ptr.h"

#define IBOS_MEMORY_WORD_SIZE_SHIFT 2UL
#define IBOS_MEMORY_WORD_SIZE (1 << IBOS_MEMORY_WORD_SIZE_SHIFT)

#define IBOS_MEMORY_SLAB_SIZE_SHIFT 10UL
#define IBOS_MEMORY_SLAB_SIZE (1UL << IBOS_MEMORY_SLAB_SIZE_SHIFT)
#define IBOS_MEMORY_SLAB_WORD_SIZE_SHIFT                                       \
  (IBOS_MEMORY_SLAB_SIZE_SHIFT - IBOS_MEMORY_WORD_SIZE_SHIFT)
#define IBOS_MEMORY_SLAB_WORD_SIZE (1UL << IBOS_MEMORY_SLAB_WORD_SIZE_SHIFT)

IBOS_static_assert(IBOS_MEMORY_SLAB_SIZE_SHIFT >= IBOS_MEMORY_WORD_SIZE_SHIFT);

#define IBOS_MEMORY_SLAB_SMALL_SIZE_SHIFT 2UL
#define IBOS_MEMORY_SLAB_SMALL_SIZE (1UL << IBOS_MEMORY_SLAB_SMALL_SIZE_SHIFT)
#define IBOS_MEMORY_SLAB_SMALL_WORD_SIZE_SHIFT                                 \
  (IBOS_MEMORY_SLAB_SMALL_SIZE_SHIFT - IBOS_MEMORY_WORD_SIZE_SHIFT)
#define IBOS_MEMORY_SLAB_SMALL_WORD_SIZE                                       \
  (1UL << IBOS_MEMORY_SLAB_SMALL_WORD_SIZE_SHIFT)

IBOS_static_assert(IBOS_MEMORY_SLAB_SMALL_SIZE_SHIFT >=
                   IBOS_MEMORY_WORD_SIZE_SHIFT);
IBOS_static_assert(IBOS_MEMORY_SLAB_SMALL_SIZE_SHIFT <=
                   IBOS_MEMORY_SLAB_SIZE_SHIFT);
/* Slab index must take at least one word. */
IBOS_static_assert(IBOS_MEMORY_SLAB_SIZE_SHIFT + IBOS_MEMORY_WORD_SIZE_SHIFT >=
                   IBOS_MEMORY_SLAB_SMALL_SIZE_SHIFT + 3UL);

#define IBOS_MEMORY_SLAB_LARGE_SIZE_SHIFT 5UL
#define IBOS_MEMORY_SLAB_LARGE_SIZE (1UL << IBOS_MEMORY_SLAB_LARGE_SIZE_SHIFT)
#define IBOS_MEMORY_SLAB_LARGE_WORD_SIZE_SHIFT                                 \
  (IBOS_MEMORY_SLAB_LARGE_SIZE_SHIFT - IBOS_MEMORY_WORD_SIZE_SHIFT)
#define IBOS_MEMORY_SMAL_LARGE_WORD_SIZE                                       \
  (1UL << IBOS_MEMORY_SLAB_LARGE_WORD_SIZE_SHIFT)

IBOS_static_assert(IBOS_MEMORY_SLAB_LARGE_SIZE_SHIFT >=
                   IBOS_MEMORY_WORD_SIZE_SHIFT);
IBOS_static_assert(IBOS_MEMORY_SLAB_LARGE_SIZE_SHIFT <=
                   IBOS_MEMORY_SLAB_SIZE_SHIFT);
IBOS_static_assert(IBOS_MEMORY_SLAB_LARGE_SIZE_SHIFT >=
                   IBOS_MEMORY_SLAB_SMALL_SIZE_SHIFT);

#define IBOS_MEMORY_SLAB_INFO_SIZE_SHIFT                                       \
  (IBOS_MEMORY_SLAB_SIZE_SHIFT - IBOS_MEMORY_SLAB_SMALL_SIZE_SHIFT - 3UL)
#define IBOS_MEMORY_SLAB_INFO_SIZE (1UL << IBOS_MEMORY_SLAB_INFO_SIZE_SHIFT)
#define IBOS_MEMORY_SLAB_INFO_WORD_SIZE_SHIFT                                  \
  (IBOS_MEMORY_SLAB_INFO_SIZE_SHIFT - IBOS_MEMORY_WORD_SIZE_SHIFT)
#define IBOS_MEMORY_SLAB_INFO_WORD_SIZE                                        \
  (1UL << IBOS_MEMORY_SLAB_INFO_WORD_SIZE_SHIFT)

typedef struct {
  u32 info[IBOS_MEMORY_SLAB_INFO_WORD_SIZE];
} IBOS_memory_slab_info_t;

typedef struct {
  // Slab index. Controls the number of
  usize slab_index_len;
  u32 *slab_index;
  usize slab_infos_len;
  IBOS_memory_slab_info_t *slab_infos;
  /* slabs */
  usize slabs_len;
  u32 *slabs;
} IBOS_memory_t;

IBOS_memory_t IBOS_memory = {0, NULL, 0, NULL, 0, NULL};

void IBOS_memory_initialize(IBOS_memory_block_t block) {
  // 0. Extract the memory region.
  uptr ptr = (uptr)block.ptr;
  usize size = block.size;

  // 1. Align the memory region.
  uptr aligned_ptr = IBOS_ptr_get_aligned(ptr, IBOS_PORT_STRUCT_ALIGNMENT);
  IBOS_assert(aligned_ptr < ptr + size);
  usize aligned_size = size - (aligned_ptr - ptr);

  // 2. Approximate slab count.
  usize approx_slabs_len = aligned_size >> IBOS_MEMORY_SLAB_SIZE_SHIFT;

  // 3. Approximate index and info size.
  usize approx_slab_infos_size = approx_slabs_len
                                 << IBOS_MEMORY_SLAB_INFO_SIZE_SHIFT;
  usize approx_index_size = approx_slabs_len >> (IBOS_MEMORY_WORD_SIZE_SHIFT +
                                                 2UL); // Four bits per entry.

  // Compute the actual metadata size.
  usize slab_infos_size = approx_slab_infos_size >> IBOS_MEMORY_WORD_SIZE_SHIFT;
  usize slab_infos_size = slab_infos_word_size << IBOS_MEMORY_WORD_SIZE_SHIFT;
  usize index_len = approx_slab_info_size >> IBOS_MEMORY_WORD_SIZE_SHIFT;
  usize index_size = index_len << IBOS_MEMORY_WORD_SIZE_SHIFT;
  IBOS_assert(slab_infos_size + index_size <= aligned_size);

  // Allocate the metadata.
  uptr index = aligned_ptr;
  usize slab_infos = aligned_ptr + index_size;

  // Extract the pointer to the slab region.
  uptr slabs = slab_infos + slab_infos_size;
  usize slabs_size = aligned_size - slab_infos_size - index_size;

  // Align the slabs pointer so we can allocate slabs.
  uptr aligned_slabs_ptr = IBOS_ptr_get_aligned(slabs, IBOS_MEMORY_SLAB_SIZE);
  IBOS_assert(aligned_slabs_ptr < slabs + slabs_size);
  usize aligned_slabs_size = slabs_size - (aligned_slabs_ptr - slabs);
  usize slabs_len = aligned_slabs_size >> IBOS_MEMORY_SLAB_SIZE_SHIFT;
  IBOS_assert(slabs_len > 0);

  // Configure the memory.
  IBOS_memory.index = (u32 *)index;
  IBOS_memory.index_len = index_len;
  IBOS_memory.slab_infos = (IBOS_memory_slab_info_t *)slab_infos;
  IBOS_memory.slab_infos_len = slab_infos_len;
  IBOS_memory.slabs = (u32 *)slabs;
  IBOS_memory.slabs_len = slabs_len;

  // Initialize the memory.
  for (usize i = 0; i < (slab_count >> 2UL); ++i) {
    IBOS_memory.index[i] = 0;
  }
}

usize IBOS_memory_promote_size(usize size) {
  if (size <= IBOS_MEMORY_SLAB_SMALL_SIZE) {
    return IBOS_MEMORY_SLAB_SMALL_SIZE;
  } else if (size <= IBOS_MEMORY_SLAB_LARGE_SIZE) {
    return IBOS_MEMORY_SLAB_LARGE_SIZE;
  }
  IBOS_assert(false);
}

usize IBOS_memory_get_open_slab();
usize IBOS_memory_get_free_slab();

typedef enum {
  IBOS_MEMORY_SLAB_SMALL,
  IBOS_MEMORY_SLAB_LARGE,
  IBOS_MEMORY_SLAB_HUGE,
} IBOS_memory_slab_kind_t;

u32 IBOS_memory_slab_index_is_alloc_bit_0(u32 index) {
  return (~index & 0x88888888) >> 3;
}

u32 IBOS_memory_slab_index_is_alloc_bit_1(u32 index) {
  return (index & 0x88888888) >> 3;
}

u32 IBOS_memory_slab_index_is_full_bit_0(u32 index) {
  return (~index & 0x44444444) >> 2;
}

u32 IBOS_memory_slab_index_is_full_bit_1(u32 index) {
  return (index & 0x44444444) >> 2;
}

u32 IBOS_memory_slab_index_is_size_bit_0(u32 index) {
  return (~index & 0x22222222) >> 1;
}

u32 IBOS_memory_slab_index_is_size_bit_1(u32 index) {
  return (index & 0x22222222) >> 1;
}

u32 IBOS_memory_slab_index_(u32 index) {}

u32 IBOS_memory_slab_index_open_small(u32 index) {
  return (((index & 0x88888888) >> 1) & (~index & 0x44444444)) >> 2;
}

bool IBOS_memory_slab_index_has_open_large(u32 index) {
  return (((index & 0x88888888) >> 1) & (~index & 0x44444444)) != 0;
}

usize IBOS_memory_slab_allocate()

    IBOS_memory_block_t IBOS_memory_allocate(usize size, usize alignment) {
  IBOS_memory_slab_kind_t slab_kind;
  if (size <= IBOS_MEMORY_SLAB_SMALL_SIZE) {
  }
  if (size <= IBOS_MEMORY_SLAB_LARGE_SIZE) {
  }
  if (size <= IBOS_MEMORY_SLAB_HUGE_SIZE) {
  }

  // search in the index for slab with correct size and free space (1 bit for
  // full/not-full, 1 bit for large-not-large, 1 bit for alloc/not-alloc, 1 bit
  // reserved)
  // while (current u32 does not indicate OK slab) advance
  // if not found: call allocate

  // if one is found, use it.
  // if one is not found, search for a free slab and allocate it.
  // while the current chunk does not contain a free slab, advance
  // extract the free slab chunk index

  // allocate block within the non-full slab
  // mark the slab as full if it is full
}

void IBOS_memory_deallocate(IBOS_memory_block_t block) {
  // find the slab in the index
  // mark the block as deallocated.
  // mark the slab as empty if it is empty
}
