#pragma once
#include <assert.h>

#define IBOS_assert(...) IBOS_assert_impl(__VA_ARGS__, "")
#define IBOS_assert_impl(x, msg, ...) assert((x) && (msg));

#define IBOS_require(...) IBOS_assert(__VA_ARGS__)
#define IBOS_ensure(...) IBOS_assert(__VA_ARGS__)

#define IBOS_static_assert(...) IBOS_static_assert_impl(__VA_ARGS__, "")
#define IBOS_static_assert_impl(x, msg) _Static_assert((x), (msg))
