#include "../include/transmit.h"
#include "../../../ibos/include/event_queue.h"
#include "../../../ibos/include/interrupt.h"
#include "../../../ibos/include/task.h"
#include "../include/config.h"
#include "../include/uart.h"
#include <assert.h>

typedef enum {
  DEMO_TRANSMIT_STATE_READY,
  DEMO_TRANSMIT_STATE_WAITING,
} DEMO_transmit_task_state_t;

DEMO_transmit_task_state_t DEMO_transmit_state;
IBOS_event_queue_t DEMO_transmit_queue;
usize transmit_count = 0;

void DEMO_transmit_task(void) {
  while (IBOS_task_can_receive_event(DEMO_TRANSMIT_ID)) {
    IBOS_event_t event = IBOS_task_receive_event(DEMO_TRANSMIT_ID);
    switch (event.id) {
    case DEMO_TRANSMIT_EVENT_PACKET:
      IBOS_event_queue_push(&DEMO_transmit_queue, event);
      transmit_count += 1;
      break;
    case DEMO_TRANSMIT_EVENT_READY:
      assert(DEMO_transmit_state == DEMO_TRANSMIT_STATE_WAITING);
      DEMO_transmit_state = DEMO_TRANSMIT_STATE_READY;
      break;
    default:
      assert(false);
    }

    if (DEMO_transmit_state == DEMO_TRANSMIT_STATE_READY &&
        IBOS_event_queue_size(DEMO_transmit_queue) != 0) {
      IBOS_event_t event = IBOS_event_queue_peek(DEMO_transmit_queue);
      IBOS_event_queue_pop(&DEMO_transmit_queue);
      DEMO_transmit_state = DEMO_TRANSMIT_STATE_WAITING;
      DEMO_packet_t *packet = (DEMO_packet_t *)(event.data.ptr);
      assert(packet->type == 1 || packet->type == 0);
      DEMO_uart_transmit(*packet);
      IBOS_memory_deallocate(event.data);
    }
  }
}

void DEMO_transmit_initialize(void) {
  assert(!IBOS_interrupt_get_enable_all());
  IBOS_task_initialize(DEMO_TRANSMIT_ID, DEMO_transmit_task,
                       DEMO_TRANSMIT_PRIORITY, DEMO_MAX_EVENTS);
  DEMO_transmit_state = DEMO_TRANSMIT_STATE_READY;
  DEMO_transmit_queue = IBOS_event_queue_allocate(DEMO_MAX_EVENTS);
}
