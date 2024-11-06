#include "../include/task.h"
#include "../include/assert.h"
#include "../port/include/interrupt.h"

u8 IBOS_critical_count = 0;

void IBOS_task_enter_critical(void) {
  IBOS_interrupt_set_enable_all(IBOS_INTERRUPT_DISABLE);
  ++IBOS_critical_count;
}

void IBOS_task_exit_critical(void) {
  IBOS_assert(IBOS_critical_count > 0,
              "exited critical section more times than entered");
  --IBOS_critical_count;
  if (IBOS_critical_count == 0) {
    IBOS_interrupt_set_enable_all(IBOS_INTERRUPT_ENABLE);
  }
}

IBOS_event_queue_t IBOS_event_queues[IBOS_TASK_LEN] = {0};

void IBOS_task_init(IBOS_task_id_t task_id, void (*task)(void),
                    usize max_events) {
  IBOS_task_enter_critical();
  IBOS_event_queues[task_id] = IBOS_event_queue_allocate(max_events);
  IBOS_interrupt_id_t interrupt_id = (IBOS_interrupt_id_t)task_id;
  IBOS_interrupt_set_handler(interrupt_id, task);
  IBOS_interrupt_set_enable(interrupt_id, IBOS_INTERRUPT_ENABLE);
  IBOS_task_exit_critical();
}

void IBOS_task_dispatch(IBOS_task_event_t *events, usize events_len) {
  IBOS_task_enter_critical();
  for (usize i = 0; i < events_len; ++i) {
    IBOS_event_queue_push(&IBOS_event_queues[events[i].task_id],
                          events[i].event);
    IBOS_interrupt_set_pending((IBOS_interrupt_id_t)events[i].task_id,
                               IBOS_INTERRUPT_PENDING);
  }
  IBOS_task_exit_critical();
}
