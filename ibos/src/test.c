#include "../include/assert.h"
#include "../include/memory.h"

typedef struct {
  u32 *slab_meta;
  usize slab_meta_len;
  u32 *block_metas;
  usize block_metas_len;
  u32 *slabs;
  usize slabs_len;
} IBOS_memory_t;

IBOS_memory_t IBOS_memory;

#define SMALL_BLOCK_SIZE 16
#define LARGE_BLOCK_SIZE (SMALL_BLOCK_SIZE * 32)

#define SMALL_BLOCK_LEN (SMALL_BLOCK_SIZE / 4)
#define LARGE_BLOCK_LEN (LARGE_BLOCK_SIZE / 4)

#define MASK_SIZE_BIT 0x55555555UL
#define MASK_FULL_BIT 0xAAAAAAAAUL

#define FILTER_LARGE_SLAB(x)                                                   \
  (((x) & MASK_SIZE_BIT) & ((~(x) & MASK_FULL_BIT) >> 1UL))
#define FILTER_SMALL_SLAB(x)                                                   \
  ((~(x) & MASK_SIZE_BIT) & ((~(x) & MASK_FULL_BIT) >> 1UL))

static u32 IBOS_memory_get_set(u32 x) { return -x & x; }
static u32 IBOS_memory_get_unset(u32 x) { return IBOS_memory_get_set(~x); }

static usize IBOS_memory_get_set_index(u32 x) {
  IBOS_require(x != 0);
  x = IBOS_memory_get_set(x);
  return (((x & 0xFFFF0000) != 0) << 4) | (((x & 0xFF00FF00) != 0) << 3) |
         (((x & 0xF0F0F0F0) != 0) << 2) | (((x & 0x88888888) != 0) << 1) |
         ((x & 0xAAAAAAAA) != 0);
}

static usize IBOS_memory_get_unset_index(u32 x) {
  IBOS_require(x != 0);
  x = IBOS_memory_get_unset(x);
  return (((x & 0xFFFF0000) != 0) << 4) | (((x & 0xFF00FF00) != 0) << 3) |
         (((x & 0xF0F0F0F0) != 0) << 2) | (((x & 0x88888888) != 0) << 1) |
         ((x & 0xAAAAAAAA) != 0);
}

#define SLAB_META_SIZE 0x1
#define SLAB_META_FULL 0x2
#define SLAB_META_MASK (SLAB_META_FULL | SLAB_META_SIZE)

#define SLAB_META_SMALL 0x0
#define SLAB_META_LARGE (SLAB_META_SIZE)
#define SLAB_META_FULL_SMALL (SLAB_META_FULL)
#define SLAB_META_FULL_LARGE (SLAB_META_FULL | SLAB_META_SIZE)

static u32 IBOS_memory_slab_meta_get(usize slab) {
  IBOS_require(slab < IBOS_memory.slabs_len);
  usize shift = 2 * (slab % 32);
  return IBOS_memory.slab_meta[slab / 32] & (SLAB_META_MASK << shift);
}

static void IBOS_memory_slab_meta_set(usize slab, u32 type) {
  IBOS_require(slab < IBOS_memory.slabs_len);
  IBOS_require((type & SLAB_META_MASK) == type);
  usize shift = 2 * (slab % 32);
  IBOS_memory.slab_meta[slab / 32] &= ~(SLAB_META_MASK << shift);
  IBOS_memory.slab_meta[slab / 32] |= type << shift;
  IBOS_ensure(IBOS_memory_slab_meta_get(slab) == type);
}

#define BLOCK_META_MASK 0x1

#define BLOCK_META_DEALLOCATED 0x0
#define BLOCK_META_ALLOCATED 0x1

#define BLOCK_META_EMPTY 0x0
#define BLOCK_META_FULL 0xFFFFFFFF

static u32 IBOS_memory_block_meta_get(usize slab, usize block) {
  IBOS_require(slab < IBOS_memory.slabs_len);
  IBOS_require(block < 32);
  return IBOS_memory.block_metas[slab] & (BLOCK_META_MASK << block);
}

static void IBOS_memory_block_meta_set(usize slab, usize block, u32 type) {
  IBOS_require(slab < IBOS_memory.slabs_len);
  IBOS_require(block < 32);
  IBOS_require((type & BLOCK_META_MASK) == type);
  IBOS_memory.block_metas[slab] &= ~(BLOCK_META_MASK << block);
  IBOS_memory.block_metas[slab] |= type << block;
  IBOS_ensure(IBOS_memory_block_meta_get(slab, block) == type);
}

static usize IBOS_memory_find_large() {
  usize slab_i = 0;
  usize filter = FILTER_LARGE_SLAB(IBOS_memory.slab_meta[slab_i]);
  while (slab_i < IBOS_memory.slab_meta_len && filter == 0) {
    ++slab_i;
    filter = FILTER_LARGE_SLAB(IBOS_memory.slab_meta[slab_i]);
  }

  IBOS_assert(slab_i < IBOS_memory.slab_meta_len,
              "allocation failed: out of memory");

  usize slab_j = IBOS_memory_get_set_index(filter);
  return (slab_i * 16) + slab_j;
}

static usize IBOS_memory_find_small() {
  usize slab_i = 0;
  usize filter = FILTER_SMALL_SLAB(IBOS_memory.slab_meta[slab_i]);
  while (slab_i < IBOS_memory.slab_meta_len && filter == 0) {
    ++slab_i;
    filter = FILTER_SMALL_SLAB(IBOS_memory.slab_meta[slab_i]);
  }

  if (slab_i == IBOS_memory.slab_meta_len) {
    return IBOS_memory_find_large();
  }

  usize slab_j = IBOS_memory_get_set_index(filter);
  return (slab_i * 16) + slab_j;
}

static IBOS_memory_block_t IBOS_memory_allocate_small() {
  usize slab = IBOS_memory_find_small();
  // TODO: find small block.
  usize block = IBOS_memory_get_unset_index(IBOS_memory.block_metas[slab]);

  if (IBOS_memory.block_metas[slab] == BLOCK_META_EMPTY) {
    IBOS_memory_slab_meta_set(slab, SLAB_META_SMALL);
  }

  IBOS_memory_block_meta_set(slab, block, BLOCK_META_ALLOCATED);

  if (IBOS_memory.block_metas[slab] == BLOCK_META_FULL) {
    IBOS_memory_slab_meta_set(slab, SLAB_META_FULL_SMALL);
  }

  return (IBOS_memory_block_t){
      &IBOS_memory.slabs[slab * LARGE_BLOCK_LEN + block * SMALL_BLOCK_LEN],
      SMALL_BLOCK_SIZE};
}

static IBOS_memory_block_t IBOS_memory_allocate_large() {
  usize slab = IBOS_memory_find_large();
  IBOS_memory_slab_meta_set(slab, SLAB_META_FULL_LARGE);
  return (IBOS_memory_block_t){&IBOS_memory.slabs[slab * LARGE_BLOCK_LEN],
                               LARGE_BLOCK_SIZE};
}

IBOS_memory_block_t IBOS_memory_allocate(usize size) {
  IBOS_require(size <= LARGE_BLOCK_SIZE,
               "allocation failed: size exceeded maximum allocation");
  return size <= SMALL_BLOCK_SIZE ? IBOS_memory_allocate_small()
                                  : IBOS_memory_allocate_large();
}

static void IBOS_memory_deallocate_small(uptr ptr) {
  usize slab = ((uptr)IBOS_memory.slabs - ptr) / LARGE_BLOCK_SIZE;
  usize block = ((uptr)IBOS_memory.slabs - ptr) / SMALL_BLOCK_SIZE;

  if (IBOS_memory.block_metas[slab] == BLOCK_META_FULL) {
    IBOS_memory_slab_meta_set(slab, SLAB_META_SMALL);
  }

  IBOS_memory_block_meta_set(slab, block, BLOCK_META_DEALLOCATED);

  if (IBOS_memory.block_metas[slab] == BLOCK_META_EMPTY) {
    IBOS_memory_slab_meta_set(slab, SLAB_META_LARGE);
  }
}

static void IBOS_memory_deallocate_large(uptr ptr) {
  usize slab = ((uptr)IBOS_memory.slabs - ptr) / LARGE_BLOCK_SIZE;
  IBOS_memory_slab_meta_set(slab, SLAB_META_LARGE);
}

void IBOS_memory_deallocate(IBOS_memory_block_t block) {
  IBOS_require(block.size <= LARGE_BLOCK_SIZE,
               "deallocation failed: size exceeded maximum allocation");
  block.size <= SMALL_BLOCK_SIZE
      ? IBOS_memory_deallocate_small((uptr)block.ptr)
      : IBOS_memory_deallocate_large((uptr)block.ptr);
}

void IBOS_memory_initialize(IBOS_memory_block_t block) {
  // align the block.
  // estimate the slabs
  // estimate the metadata size
  // get actual slabs
  // get actual metadata size
  // allocate the slabs
  // allocate the metadata
}