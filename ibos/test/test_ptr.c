#include "../include/assert.h"
#include "../include/ptr.h"

void IBOS_test_ptr_aligned(void) {
  IBOS_assert(IBOS_ptr_aligned(0x10, 0));
  IBOS_assert(IBOS_ptr_aligned(0x10, 1));
  IBOS_assert(IBOS_ptr_aligned(0x10, 2));

  IBOS_assert(IBOS_ptr_aligned(0x11, 0));
  IBOS_assert(!IBOS_ptr_aligned(0x11, 1));
  IBOS_assert(!IBOS_ptr_aligned(0x11, 2));

  IBOS_assert(IBOS_ptr_aligned(0x12, 0));
  IBOS_assert(IBOS_ptr_aligned(0x12, 1));
  IBOS_assert(!IBOS_ptr_aligned(0x12, 2));

  IBOS_assert(IBOS_ptr_aligned(0x13, 0));
  IBOS_assert(!IBOS_ptr_aligned(0x13, 1));
  IBOS_assert(!IBOS_ptr_aligned(0x13, 2));

  IBOS_assert(IBOS_ptr_aligned(0x14, 0));
  IBOS_assert(IBOS_ptr_aligned(0x14, 1));
  IBOS_assert(IBOS_ptr_aligned(0x14, 2));
}

void IBOS_test_ptr_get_aligned(void) {
  IBOS_assert(IBOS_ptr_get_aligned(0x10, 0) == 0x10);
  IBOS_assert(IBOS_ptr_get_aligned(0x10, 1) == 0x10);
  IBOS_assert(IBOS_ptr_get_aligned(0x10, 2) == 0x10);

  IBOS_assert(IBOS_ptr_get_aligned(0x11, 0) == 0x11);
  IBOS_assert(IBOS_ptr_get_aligned(0x11, 1) == 0x12);
  IBOS_assert(IBOS_ptr_get_aligned(0x11, 2) == 0x14);

  IBOS_assert(IBOS_ptr_get_aligned(0x12, 0) == 0x12);
  IBOS_assert(IBOS_ptr_get_aligned(0x12, 1) == 0x12);
  IBOS_assert(IBOS_ptr_get_aligned(0x12, 2) == 0x14);

  IBOS_assert(IBOS_ptr_get_aligned(0x13, 0) == 0x13);
  IBOS_assert(IBOS_ptr_get_aligned(0x13, 1) == 0x14);
  IBOS_assert(IBOS_ptr_get_aligned(0x13, 2) == 0x14);

  IBOS_assert(IBOS_ptr_get_aligned(0x14, 0) == 0x14);
  IBOS_assert(IBOS_ptr_get_aligned(0x14, 1) == 0x14);
  IBOS_assert(IBOS_ptr_get_aligned(0x14, 2) == 0x14);
}

void IBOS_test_ptr(void) {
  IBOS_test_ptr_aligned();
  IBOS_test_ptr_get_aligned();
}

int main(void) { IBOS_test_ptr(); }
