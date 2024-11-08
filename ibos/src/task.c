#include "../include/task.h"
#include "../include/assert.h"
#include "../include/interrupt.h"

u8 IBOS_critical_count = 0;

void IBOS_task_enter_critical(void) {
  IBOS_interrupt_disable_all();
  ++IBOS_critical_count;
}

void IBOS_task_exit_critical(void) {
  IBOS_assert(IBOS_critical_count > 0,
              "exited critical section more times than entered");
  --IBOS_critical_count;
  if (IBOS_critical_count == 0) {
    IBOS_interrupt_disable_all();
  }
}

IBOS_event_queue_t IBOS_event_queues[IBOS_TASK_MAX];

void IBOS_task_initialize(IBOS_task_id_t id, void (*task)(void),
                          IBOS_priority_t priority, usize max_events) {
  IBOS_require(id < IBOS_TASK_MAX);
  IBOS_task_enter_critical();
  IBOS_event_queues[id] = IBOS_event_queue_allocate(max_events);
  IBOS_interrupt_id_t interrupt_id = IBOS_interrupt_unused_ids[id];
  IBOS_interrupt_set_handler(interrupt_id, task);
  IBOS_interrupt_set_priority(interrupt_id, priority);
  IBOS_interrupt_enable(interrupt_id);
  IBOS_task_exit_critical();
}

void IBOS_task_send_event(IBOS_task_id_t id, IBOS_event_t event) {
  IBOS_require(id < IBOS_TASK_MAX);
  IBOS_task_enter_critical();
  IBOS_event_queue_push(&IBOS_event_queues[id], event);
  IBOS_interrupt_id_t interrupt_id = IBOS_interrupt_unused_ids[id];
  IBOS_interrupt_schedule(interrupt_id);
  IBOS_task_exit_critical();
}

IBOS_event_t IBOS_task_receive_event(IBOS_task_id_t id) {
  IBOS_require(id < IBOS_TASK_MAX);
  IBOS_task_enter_critical();
  IBOS_event_t event = IBOS_event_queue_peek(IBOS_event_queues[id]);
  IBOS_event_queue_pop(&IBOS_event_queues[id]);
  IBOS_task_exit_critical();
  return event;
}

bool IBOS_task_can_receive_event(IBOS_task_id_t id) {
  IBOS_require(id < IBOS_TASK_MAX);
  IBOS_task_enter_critical();
  usize size = IBOS_event_queue_size(IBOS_event_queues[id]);
  IBOS_task_exit_critical();
  return size != 0;
}
