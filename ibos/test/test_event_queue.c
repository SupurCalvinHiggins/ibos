#include "../include/assert.h"
#include "../include/event_queue.h"
#include "helper/include/test.h"
#include "mock/include/mock_memory.h"
#include <string.h>

void IBOS_helper_memory_initialize(void) {
  u8 buffer[4096];
  IBOS_memory_block_t memory = {buffer, sizeof(buffer)};
  IBOS_memory_initialize(memory);
}

bool IBOS_helper_event_equal(IBOS_event_t event1, IBOS_event_t event2) {
  return memcmp(&event1, &event2, sizeof(event1)) == 0;
}

void IBOS_test_event_queue_allocate(void) {
  IBOS_TEST_START();

  IBOS_helper_memory_initialize();
  IBOS_event_queue_t queue = IBOS_event_queue_allocate(4);
  IBOS_assert(IBOS_event_queue_size(queue) == 0);
  IBOS_assert(IBOS_event_queue_capacity(queue) == 4);
  IBOS_event_queue_deallocate(&queue);

  IBOS_TEST_END();
}

void IBOS_test_event_queue_push(void) {
  IBOS_TEST_START();

  IBOS_helper_memory_initialize();
  IBOS_event_queue_t queue = IBOS_event_queue_allocate(4);
  IBOS_assert(IBOS_event_queue_size(queue) == 0);
  IBOS_assert(IBOS_event_queue_capacity(queue) == 4);

  IBOS_event_t event1 = {1, {0, 0}};
  IBOS_event_t event2 = {2, {0, 0}};
  IBOS_event_t event3 = {3, {0, 0}};
  IBOS_event_t event4 = {4, {0, 0}};

  IBOS_event_queue_push(&queue, event1);
  IBOS_assert(IBOS_event_queue_size(queue) == 1);
  IBOS_assert(IBOS_event_queue_capacity(queue) == 4);
  IBOS_assert(IBOS_helper_event_equal(IBOS_event_queue_peek(queue), event1));

  IBOS_event_queue_push(&queue, event2);
  IBOS_assert(IBOS_event_queue_size(queue) == 2);
  IBOS_assert(IBOS_event_queue_capacity(queue) == 4);
  IBOS_assert(IBOS_helper_event_equal(IBOS_event_queue_peek(queue), event1));

  IBOS_event_queue_push(&queue, event3);
  IBOS_assert(IBOS_event_queue_size(queue) == 3);
  IBOS_assert(IBOS_event_queue_capacity(queue) == 4);
  IBOS_assert(IBOS_helper_event_equal(IBOS_event_queue_peek(queue), event1));

  IBOS_event_queue_push(&queue, event4);
  IBOS_assert(IBOS_event_queue_size(queue) == 4);
  IBOS_assert(IBOS_event_queue_capacity(queue) == 4);
  IBOS_assert(IBOS_helper_event_equal(IBOS_event_queue_peek(queue), event1));

  IBOS_event_queue_deallocate(&queue);

  IBOS_TEST_END();
}

void IBOS_test_event_queue_pop(void) {
  IBOS_TEST_START();

  IBOS_helper_memory_initialize();
  IBOS_event_queue_t queue = IBOS_event_queue_allocate(4);
  IBOS_assert(IBOS_event_queue_size(queue) == 0);
  IBOS_assert(IBOS_event_queue_capacity(queue) == 4);

  IBOS_event_t event1 = {1, {0, 0}};
  IBOS_event_t event2 = {2, {0, 0}};
  IBOS_event_t event3 = {3, {0, 0}};
  IBOS_event_t event4 = {4, {0, 0}};

  for (usize i = 0; i < 2; ++i) {
    IBOS_event_queue_push(&queue, event1);
    IBOS_assert(IBOS_event_queue_size(queue) == 1);
    IBOS_assert(IBOS_event_queue_capacity(queue) == 4);
    IBOS_assert(IBOS_helper_event_equal(IBOS_event_queue_peek(queue), event1));

    IBOS_event_queue_push(&queue, event2);
    IBOS_assert(IBOS_event_queue_size(queue) == 2);
    IBOS_assert(IBOS_event_queue_capacity(queue) == 4);
    IBOS_assert(IBOS_helper_event_equal(IBOS_event_queue_peek(queue), event1));

    IBOS_event_queue_push(&queue, event3);
    IBOS_assert(IBOS_event_queue_size(queue) == 3);
    IBOS_assert(IBOS_event_queue_capacity(queue) == 4);
    IBOS_assert(IBOS_helper_event_equal(IBOS_event_queue_peek(queue), event1));

    IBOS_event_queue_push(&queue, event4);
    IBOS_assert(IBOS_event_queue_size(queue) == 4);
    IBOS_assert(IBOS_event_queue_capacity(queue) == 4);
    IBOS_assert(IBOS_helper_event_equal(IBOS_event_queue_peek(queue), event1));

    IBOS_event_queue_pop(&queue);
    IBOS_assert(IBOS_event_queue_size(queue) == 3);
    IBOS_assert(IBOS_event_queue_capacity(queue) == 4);
    IBOS_assert(IBOS_helper_event_equal(IBOS_event_queue_peek(queue), event2));

    IBOS_event_queue_pop(&queue);
    IBOS_assert(IBOS_event_queue_size(queue) == 2);
    IBOS_assert(IBOS_event_queue_capacity(queue) == 4);
    IBOS_assert(IBOS_helper_event_equal(IBOS_event_queue_peek(queue), event3));

    IBOS_event_queue_pop(&queue);
    IBOS_assert(IBOS_event_queue_size(queue) == 1);
    IBOS_assert(IBOS_event_queue_capacity(queue) == 4);
    IBOS_assert(IBOS_helper_event_equal(IBOS_event_queue_peek(queue), event4));

    IBOS_event_queue_pop(&queue);
    IBOS_assert(IBOS_event_queue_size(queue) == 0);
    IBOS_assert(IBOS_event_queue_capacity(queue) == 4);
  }

  IBOS_event_queue_deallocate(&queue);

  IBOS_TEST_END();
}

void IBOS_test_event_queue(void) {
  IBOS_test_event_queue_allocate();
  IBOS_test_event_queue_push();
  IBOS_test_event_queue_pop();
}

int main(void) { IBOS_test_event_queue(); }
