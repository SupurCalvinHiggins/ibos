#include "../include/event_queue.h"
#include "../include/assert.h"
#include "../include/ptr.h"

IBOS_event_queue_t IBOS_event_queue_static_allocate(usize capacity) {
  IBOS_assert(IBOS_ptr_is_alignment(capacity));
  usize allocation_size = sizeof(IBOS_event_t) * capacity;
  IBOS_memory_block_t block =
      IBOS_memory_static_allocate(allocation_size, IBOS_EVENT_QUEUE_ALIGNMENT);
  return (IBOS_event_queue_t){block.ptr, capacity, 0, capacity};
}

IBOS_event_queue_t IBOS_event_queue_allocate(usize capacity) {
  IBOS_assert(IBOS_ptr_is_alignment(capacity));
  usize allocation_size = sizeof(IBOS_event_t) * capacity;
  IBOS_memory_block_t block =
      IBOS_memory_allocate(allocation_size, IBOS_EVENT_QUEUE_ALIGNMENT);
  return (IBOS_event_queue_t){block.ptr, capacity, 0, capacity};
}

void IBOS_event_queue_deallocate(IBOS_event_queue_t *queue) {
  IBOS_memory_block_t block = {queue->base,
                               sizeof(IBOS_event_t) * queue->capacity};
  IBOS_memory_deallocate(block);
}

void IBOS_event_queue_push(IBOS_event_queue_t *queue, IBOS_event_t event) {
  IBOS_assert(IBOS_event_queue_size(*queue) <
              IBOS_event_queue_capacity(*queue));
  queue->base[queue->end] = event;
  if (queue->start == queue->capacity) {
    queue->start = queue->end;
  }
  queue->end = (queue->end + 1) & (queue->capacity - 1);
}

void IBOS_event_queue_pop(IBOS_event_queue_t *queue) {
  IBOS_assert(IBOS_event_queue_size(*queue) > 0);
  queue->start = (queue->start + 1) & (queue->capacity - 1);
  if (queue->start == queue->end) {
    queue->start = queue->capacity;
  }
}

IBOS_event_t IBOS_event_queue_peek(IBOS_event_queue_t queue) {
  IBOS_assert(IBOS_event_queue_size(queue) > 0);
  return queue.base[queue.start];
}

usize IBOS_event_queue_size(IBOS_event_queue_t queue) {
  if (queue.start == queue.capacity) {
    return 0;
  }
  return queue.start < queue.end ? queue.end - queue.start
                                 : queue.capacity - queue.start + queue.end;
}

usize IBOS_event_queue_capacity(IBOS_event_queue_t queue) {
  return queue.capacity;
}
