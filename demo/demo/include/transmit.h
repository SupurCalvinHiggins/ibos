#pragma once

#include "../../../ibos/include/int.h"

typedef enum {
  DEMO_TRANSMIT_EVENT_PACKET,
  DEMO_TRANSMIT_EVENT_READY,
} DEMO_transmit_event_id_t;

void DEMO_transmit_initialize(void);
