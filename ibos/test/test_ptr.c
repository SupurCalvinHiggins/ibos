#include "../include/assert.h"
#include "../include/ptr.h"
#include "helper/include/test.h"

void IBOS_test_ptr_is_alignment(void) {
  IBOS_TEST_START();

  IBOS_assert(!IBOS_ptr_is_alignment(0));
  IBOS_assert(IBOS_ptr_is_alignment(1));
  IBOS_assert(IBOS_ptr_is_alignment(2));
  IBOS_assert(!IBOS_ptr_is_alignment(3));
  IBOS_assert(IBOS_ptr_is_alignment(4));

  IBOS_TEST_END();
}

void IBOS_test_ptr_is_aligned(void) {
  IBOS_TEST_START();

  IBOS_assert(IBOS_ptr_is_aligned(0x10, 1));
  IBOS_assert(IBOS_ptr_is_aligned(0x10, 2));
  IBOS_assert(IBOS_ptr_is_aligned(0x10, 4));

  IBOS_assert(IBOS_ptr_is_aligned(0x11, 1));
  IBOS_assert(!IBOS_ptr_is_aligned(0x11, 2));
  IBOS_assert(!IBOS_ptr_is_aligned(0x11, 4));

  IBOS_assert(IBOS_ptr_is_aligned(0x12, 1));
  IBOS_assert(IBOS_ptr_is_aligned(0x12, 2));
  IBOS_assert(!IBOS_ptr_is_aligned(0x12, 4));

  IBOS_assert(IBOS_ptr_is_aligned(0x13, 1));
  IBOS_assert(!IBOS_ptr_is_aligned(0x13, 2));
  IBOS_assert(!IBOS_ptr_is_aligned(0x13, 4));

  IBOS_assert(IBOS_ptr_is_aligned(0x14, 1));
  IBOS_assert(IBOS_ptr_is_aligned(0x14, 2));
  IBOS_assert(IBOS_ptr_is_aligned(0x14, 4));

  IBOS_TEST_END();
}

void IBOS_test_ptr_get_aligned(void) {
  IBOS_TEST_START();

  IBOS_assert(IBOS_ptr_get_aligned(0x10, 1) == 0x10);
  IBOS_assert(IBOS_ptr_get_aligned(0x10, 2) == 0x10);
  IBOS_assert(IBOS_ptr_get_aligned(0x10, 4) == 0x10);

  IBOS_assert(IBOS_ptr_get_aligned(0x11, 1) == 0x11);
  IBOS_assert(IBOS_ptr_get_aligned(0x11, 2) == 0x12);
  IBOS_assert(IBOS_ptr_get_aligned(0x11, 4) == 0x14);

  IBOS_assert(IBOS_ptr_get_aligned(0x12, 1) == 0x12);
  IBOS_assert(IBOS_ptr_get_aligned(0x12, 2) == 0x12);
  IBOS_assert(IBOS_ptr_get_aligned(0x12, 4) == 0x14);

  IBOS_assert(IBOS_ptr_get_aligned(0x13, 1) == 0x13);
  IBOS_assert(IBOS_ptr_get_aligned(0x13, 2) == 0x14);
  IBOS_assert(IBOS_ptr_get_aligned(0x13, 4) == 0x14);

  IBOS_assert(IBOS_ptr_get_aligned(0x14, 1) == 0x14);
  IBOS_assert(IBOS_ptr_get_aligned(0x14, 2) == 0x14);
  IBOS_assert(IBOS_ptr_get_aligned(0x14, 4) == 0x14);

  IBOS_TEST_END();
}

void IBOS_test_ptr(void) {
  IBOS_test_ptr_is_alignment();
  IBOS_test_ptr_is_aligned();
  IBOS_test_ptr_get_aligned();
}

int main(void) { IBOS_test_ptr(); }
