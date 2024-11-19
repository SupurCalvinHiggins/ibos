#include "../include/slow.h"
#include "../../../ibos/include/task.h"
#include "../include/config.h"
#include <assert.h>

void DEMO_slow_task(void) {
  while (IBOS_task_can_receive_event(DEMO_SLOW_ID)) {
    IBOS_event_t event = IBOS_task_receive_event(DEMO_SLOW_ID);
    DEMO_packet_t *packet = (DEMO_packet_t *)event.data.ptr;
    volatile u32 hash = packet->data;
    for (usize i = 0; i < packet->data; ++i) {
      for (usize j = 0; j < packet->data; ++j) {
        hash *= 0xCC9E2D51;
        hash = (hash << 15) | ((hash & 0xFFFE) >> 17);
        hash *= 0x1B873593;
        hash = (hash << 13) | ((hash & 0xFFF8) >> 19);
        hash *= 5;
        hash += 0xE6546B64;
      }
    }
    packet->data = hash;
    IBOS_task_send_event(DEMO_TRANSMIT_ID, event);
  }
}

void DEMO_slow_initialize(void) {
  assert(!IBOS_interrupt_get_enable_all());
  IBOS_task_initialize(DEMO_SLOW_ID, DEMO_slow_task, DEMO_SLOW_PRIORITY,
                       DEMO_MAX_EVENTS);
}
