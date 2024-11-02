#include <stdalign.h>
#include <stdio.h>
#include <time.h>

#define IBOS_BENCH_MOCK_MEMORY 1

#if IBOS_BENCH_MOCK_MEMORY != 0
#include "../test/mock/include/mock_memory.h"
#else
#include "../include/memory.h"
#endif

static void escape(void *p) { asm volatile("" : : "g"(p) : "memory"); }

static void clobber() { asm volatile("" : : : "memory"); }

void bench_memory(usize size, usize allocations, usize samples) {
  alignas(512) u8 buffer[512 * (2 * allocations)];
  IBOS_memory_block_t memory = {buffer, sizeof(buffer)};
  IBOS_memory_initialize(memory);

  IBOS_memory_block_t blocks[allocations];
  printf("allocating %zu bytes %zu times ... ", size, allocations);

  clock_t begin = clock();
  for (usize i = 0; i < samples; ++i) {
    for (usize j = 0; j < allocations; ++j) {
      blocks[j] = IBOS_memory_allocate(size);
      escape(blocks[j].ptr);
    }

    for (usize j = 0; j < allocations; ++j) {
      IBOS_memory_deallocate(blocks[j]);
    }
  }
  clobber();
  clock_t end = clock();
  double secs = (double)(end - begin) / (CLOCKS_PER_SEC * (double)samples);
  double nanos = secs * (double)1000000000;
  printf("%f nanoseconds per sample\n", nanos);
}

int main(void) {
  usize sizes[] = {1, 4, 16, 64, 256, 512};
  usize allocations[] = {1, 4, 16, 64, 256, 512};
  usize samples = 1000000;

  for (usize i = 0; i < sizeof(sizes) / sizeof(usize); ++i) {
    for (usize j = 0; j < sizeof(allocations) / sizeof(usize); ++j) {
      bench_memory(sizes[i], allocations[j], samples);
    }
  }
}
