#include "../include/receive.h"
#include "../../../ibos/include/interrupt.h"
#include "../../../ibos/include/task.h"
#include "../include/config.h"
#include <assert.h>

usize receive_count = 0;

void DEMO_receive_task(void) {
  while (IBOS_task_can_receive_event(DEMO_RECEIVE_ID)) {
    IBOS_event_t event = IBOS_task_receive_event(DEMO_RECEIVE_ID);
    assert(event.id == DEMO_RECEIVE_EVENT_PACKET);
    receive_count += 1;

    DEMO_packet_t *packet = (DEMO_packet_t *)event.data.ptr;
    switch (packet->type) {
    case DEMO_SLOW_PACKET_KIND:
      IBOS_task_send_event(DEMO_SLOW_ID, event);
      break;
    case DEMO_FAST_PACKET_KIND:
      IBOS_task_send_event(DEMO_FAST_ID, event);
      break;
    default:
      assert(false);
    }
  }
}

void DEMO_receive_initialize(void) {
  assert(!IBOS_interrupt_get_enable_all());
  IBOS_task_initialize(DEMO_RECEIVE_ID, DEMO_receive_task,
                       DEMO_RECEIVE_PRIORITY, DEMO_MAX_EVENTS);
}
