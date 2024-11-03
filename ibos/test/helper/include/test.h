#pragma once
#include <stdio.h>

#define IBOS_TEST_START()                                                      \
  {                                                                            \
    fprintf(stderr, "%-48s... \t", __func__);                                  \
    fflush(stderr);                                                            \
  }
#define IBOS_TEST_END()                                                        \
  {                                                                            \
    fprintf(stderr, "OK\n");                                                   \
    fflush(stderr);                                                            \
  }
