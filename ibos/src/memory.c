#include "../include/memory.h"
#include "../include/assert.h"
#include "../include/port.h"
#include "../include/ptr.h"

/* memory */

#define SMALL_BLOCK_SIZE 16
#define LARGE_BLOCK_SIZE (SMALL_BLOCK_SIZE * 32)

#define SMALL_BLOCK_LEN (SMALL_BLOCK_SIZE / 4)
#define LARGE_BLOCK_LEN (LARGE_BLOCK_SIZE / 4)

typedef struct {
  u32 *slab_metas;
  u32 *block_metas;
  u32 *slabs;
  usize slabs_len; /* length in slabs */
} IBOS_memory_t;

IBOS_memory_t IBOS_memory = {NULL, NULL, NULL, 0};

/* helpers */

static u32 IBOS_memory_get_set(u32 x) { return -x & x; }
static u32 IBOS_memory_get_unset(u32 x) { return IBOS_memory_get_set(~x); }

static usize IBOS_memory_find_set(u32 x) {
  IBOS_require(x != 0);
  x = IBOS_memory_get_set(x);
  usize idx = (((x & 0xFFFF0000) != 0) << 4) | (((x & 0xFF00FF00) != 0) << 3) |
              (((x & 0xF0F0F0F0) != 0) << 2) | (((x & 0xCCCCCCCC) != 0) << 1) |
              ((x & 0xAAAAAAAA) != 0);
  IBOS_ensure(x == (1U << idx));
  return idx;
}

static usize IBOS_memory_find_unset(u32 x) {
  IBOS_require(x != 0xFFFFFFFF);
  x = IBOS_memory_get_unset(x);
  usize idx = (((x & 0xFFFF0000) != 0) << 4) | (((x & 0xFF00FF00) != 0) << 3) |
              (((x & 0xF0F0F0F0) != 0) << 2) | (((x & 0xCCCCCCCC) != 0) << 1) |
              ((x & 0xAAAAAAAA) != 0);
  IBOS_ensure(x == (1U << idx));
  return idx;
}

/* slab meta */

#define SLAB_META_SIZE 0x1
#define SLAB_META_FULL 0x2
#define SLAB_META_MASK (SLAB_META_FULL | SLAB_META_SIZE)

#define SLAB_META_SMALL 0x0
#define SLAB_META_LARGE (SLAB_META_SIZE)
#define SLAB_META_FULL_SMALL (SLAB_META_FULL)
#define SLAB_META_FULL_LARGE (SLAB_META_FULL | SLAB_META_SIZE)

static u32 IBOS_memory_slab_metas_get(usize slab) {
  IBOS_require(slab < IBOS_memory.slabs_len);
  usize shift = 2 * (slab % 16);
  return (IBOS_memory.slab_metas[slab / 16] & (SLAB_META_MASK << shift)) >>
         shift;
}

static void IBOS_memory_slab_metas_set(usize slab, u32 type) {
  IBOS_require(slab < IBOS_memory.slabs_len);
  IBOS_require((type & SLAB_META_MASK) == type);
  usize shift = 2 * (slab % 16);
  IBOS_memory.slab_metas[slab / 16] &= ~(SLAB_META_MASK << shift);
  IBOS_memory.slab_metas[slab / 16] |= type << shift;
  IBOS_ensure(IBOS_memory_slab_metas_get(slab) == type);
}

#define SLAB_META_SIZE_FILTER 0x55555555
#define SLAB_META_FULL_FILTER 0xAAAAAAAA

#define SLAB_META_LARGE_FILTER(x)                                              \
  (((x) & SLAB_META_SIZE_FILTER) & ((~(x) & SLAB_META_FULL_FILTER) >> 1))
#define SLAB_META_SMALL_FILTER(x)                                              \
  ((~(x) & SLAB_META_SIZE_FILTER) & ((~(x) & SLAB_META_FULL_FILTER) >> 1))

static usize IBOS_memory_slab_metas_find_large(void) {
  usize slab_i = 0;
  usize filter = SLAB_META_LARGE_FILTER(IBOS_memory.slab_metas[slab_i]);
  // Handle all the full metas.
  while (slab_i < (IBOS_memory.slabs_len / 16) && filter == 0) {
    ++slab_i;
    filter = SLAB_META_LARGE_FILTER(IBOS_memory.slab_metas[slab_i]);
  }

  // Handle the partial meta if it exists.
  // TODO: wrong?
  if (slab_i == IBOS_memory.slabs_len / 16 &&
      (IBOS_memory.slabs_len & 0xF) != 0) {
    if ((filter & ((1 << ((IBOS_memory.slabs_len & 0xF) * 2)) - 1)) == 0) {
      ++slab_i;
    }
  }

  IBOS_assert(slab_i < (IBOS_memory.slabs_len / 16) +
                           ((IBOS_memory.slabs_len & 0xF) != 0),
              "allocation failed: out of memory");

  usize slab_j = IBOS_memory_find_set(filter) / 2;
  usize slab = 16 * slab_i + slab_j;
  IBOS_ensure(IBOS_memory_slab_metas_get(slab) == SLAB_META_LARGE);
  return slab;
}

static usize IBOS_memory_slab_metas_find_small(void) {
  usize slab_i = 0;
  usize filter = SLAB_META_SMALL_FILTER(IBOS_memory.slab_metas[slab_i]);
  while (slab_i < (IBOS_memory.slabs_len / 16) && filter == 0) {
    ++slab_i;
    filter = SLAB_META_SMALL_FILTER(IBOS_memory.slab_metas[slab_i]);
  }

  if (slab_i == IBOS_memory.slabs_len / 16 &&
      (IBOS_memory.slabs_len & 0xF) != 0) {
    if ((filter & ((1 << (IBOS_memory.slabs_len & 0xF) * 2) - 1)) == 0) {
      ++slab_i;
    }
  }

  if (slab_i ==
      (IBOS_memory.slabs_len / 16) + ((IBOS_memory.slabs_len & 0xF) != 0)) {
    return IBOS_memory_slab_metas_find_large();
  }

  usize slab_j = IBOS_memory_find_set(filter) / 2;
  usize slab = 16 * slab_i + slab_j;
  IBOS_ensure(IBOS_memory_slab_metas_get(slab) == SLAB_META_SMALL);
  return slab;
}

/* block meta */

#define BLOCK_META_MASK 0x1

#define BLOCK_META_DEALLOCATED 0x0
#define BLOCK_META_ALLOCATED 0x1

#define BLOCK_META_EMPTY 0x0
#define BLOCK_META_FULL 0xFFFFFFFF

static bool IBOS_memory_block_meta_slab_full(usize slab, usize block) {
  IBOS_require(slab < IBOS_memory.slabs_len);
  IBOS_require(block < 32);
  return IBOS_memory.block_metas[block] == BLOCK_META_FULL;
}

static bool IBOS_memory_block_meta_slab_empty(usize slab, usize block) {
  IBOS_require(slab < IBOS_memory.slabs_len);
  IBOS_require(block < 32);
  return IBOS_memory.block_metas[block] == BLOCK_META_EMPTY;
}

static u32 IBOS_memory_block_meta_get(usize slab, usize block) {
  IBOS_require(slab < IBOS_memory.slabs_len);
  IBOS_require(block < 32);
  return (IBOS_memory.block_metas[slab] & (BLOCK_META_MASK << block)) >> block;
}

static void IBOS_memory_block_meta_set(usize slab, usize block, u32 type) {
  IBOS_require(slab < IBOS_memory.slabs_len);
  IBOS_require(block < 32);
  IBOS_require((type & BLOCK_META_MASK) == type);
  IBOS_memory.block_metas[slab] &= ~(BLOCK_META_MASK << block);
  IBOS_memory.block_metas[slab] |= type << block;
  IBOS_ensure(IBOS_memory_block_meta_get(slab, block) == type);
}

/* allocation/deallocation */

static IBOS_memory_block_t IBOS_memory_allocate_small() {
  usize slab = IBOS_memory_slab_metas_find_small();
  usize block = IBOS_memory_find_unset(IBOS_memory.block_metas[slab]);

  if (IBOS_memory.block_metas[slab] == BLOCK_META_EMPTY) {
    IBOS_memory_slab_metas_set(slab, SLAB_META_SMALL);
  }

  IBOS_memory_block_meta_set(slab, block, BLOCK_META_ALLOCATED);

  if (IBOS_memory.block_metas[slab] == BLOCK_META_FULL) {
    IBOS_memory_slab_metas_set(slab, SLAB_META_FULL_SMALL);
  }

  return (IBOS_memory_block_t){
      &IBOS_memory.slabs[slab * LARGE_BLOCK_LEN + block * SMALL_BLOCK_LEN],
      SMALL_BLOCK_SIZE};
}

static IBOS_memory_block_t IBOS_memory_allocate_large() {
  usize slab = IBOS_memory_slab_metas_find_large();
  IBOS_memory_slab_metas_set(slab, SLAB_META_FULL_LARGE);
  return (IBOS_memory_block_t){&IBOS_memory.slabs[slab * LARGE_BLOCK_LEN],
                               LARGE_BLOCK_SIZE};
}

IBOS_memory_block_t IBOS_memory_allocate(usize size) {
  IBOS_require(IBOS_memory.slabs != 0,
               "allocation failed: memory not initialized");
  IBOS_require(size <= LARGE_BLOCK_SIZE,
               "allocation failed: size exceeded maximum allocation");
  return size <= SMALL_BLOCK_SIZE ? IBOS_memory_allocate_small()
                                  : IBOS_memory_allocate_large();
}

static void IBOS_memory_deallocate_small(uptr ptr) {
  usize slab = (ptr - (uptr)IBOS_memory.slabs) / LARGE_BLOCK_SIZE;
  usize block = (ptr - (uptr)IBOS_memory.slabs - (slab * LARGE_BLOCK_SIZE)) /
                SMALL_BLOCK_SIZE;
  IBOS_require(slab < IBOS_memory.slabs_len);
  IBOS_require(block < 32);
  IBOS_require(IBOS_memory_slab_metas_get(slab) == SLAB_META_SMALL ||
                   IBOS_memory_slab_metas_get(slab) == SLAB_META_FULL_SMALL,
               "deallocation failed: slab is not small");
  IBOS_require(IBOS_memory_block_meta_get(slab, block) == BLOCK_META_ALLOCATED,
               "deallocation failed: small block already free");

  if (IBOS_memory.block_metas[slab] == BLOCK_META_FULL) {
    IBOS_memory_slab_metas_set(slab, SLAB_META_SMALL);
  }

  IBOS_memory_block_meta_set(slab, block, BLOCK_META_DEALLOCATED);

  if (IBOS_memory.block_metas[slab] == BLOCK_META_EMPTY) {
    IBOS_memory_slab_metas_set(slab, SLAB_META_LARGE);
  }
}

static void IBOS_memory_deallocate_large(uptr ptr) {
  usize slab = (ptr - (uptr)IBOS_memory.slabs) / LARGE_BLOCK_SIZE;
  IBOS_require(slab < IBOS_memory.slabs_len);
  IBOS_require(IBOS_memory_slab_metas_get(slab) == SLAB_META_FULL_LARGE,
               "deallocation failed: large block already free");
  IBOS_memory_slab_metas_set(slab, SLAB_META_LARGE);
}

void IBOS_memory_deallocate(IBOS_memory_block_t block) {
  IBOS_require(IBOS_memory.slabs != 0,
               "deallocation failed: memory not initialized");
  IBOS_require(block.size <= LARGE_BLOCK_SIZE,
               "deallocation failed: size exceeded maximum allocation");
  IBOS_require((uptr)(block.ptr + block.size) <=
                   (uptr)IBOS_memory.slabs +
                       IBOS_memory.slabs_len * LARGE_BLOCK_SIZE,
               "deallocation failed: block out of bounds");
  IBOS_require((uptr)block.ptr >= (uptr)IBOS_memory.slabs,
               "deallocation failed: block out of bounds");
  block.size <= SMALL_BLOCK_SIZE
      ? IBOS_memory_deallocate_small((uptr)block.ptr)
      : IBOS_memory_deallocate_large((uptr)block.ptr);
}

void IBOS_memory_initialize(IBOS_memory_block_t block) {
  uptr ptr = (uptr)block.ptr;
  usize size = block.size;

  uptr aligned_ptr = IBOS_ptr_get_aligned(ptr, IBOS_PORT_STRUCT_ALIGNMENT);
  usize aligned_size = size - (aligned_ptr - ptr);
  IBOS_assert(aligned_size <= size);

  usize approx_slabs_len = aligned_size / LARGE_BLOCK_SIZE;
  usize slabs_len =
      ((approx_slabs_len * LARGE_BLOCK_SIZE) -
       4 * ((approx_slabs_len / 16) + (approx_slabs_len & 0xF) != 0) -
       4 * approx_slabs_len) /
      LARGE_BLOCK_SIZE;
  IBOS_memory.slabs_len = slabs_len;

  usize slab_metas_size =
      4 * ((IBOS_memory.slabs_len / 16) + ((IBOS_memory.slabs_len & 0xF) != 0));
  usize block_metas_size = 4 * IBOS_memory.slabs_len;

  IBOS_memory.slab_metas = (u32 *)aligned_ptr;
  IBOS_memory.block_metas = IBOS_memory.slab_metas + slab_metas_size;
  IBOS_memory.slabs = (u32 *)IBOS_ptr_get_aligned((uptr)IBOS_memory.block_metas,
                                                  LARGE_BLOCK_SIZE);

  for (usize i = 0; i < slab_metas_size / 4; ++i) {
    IBOS_memory.slab_metas[i] = SLAB_META_SIZE_FILTER;
  }

  for (usize i = 0; i < block_metas_size / 4; ++i) {
    IBOS_memory.block_metas[i] = 0;
  }
}
