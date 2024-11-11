#pragma once

#include "../../../ibos/include/int.h"
#include "../../../ibos/include/priority.h"
#include "../../../ibos/port/stm32l476rg/src/lib/stm32l476rg/include/stm32l476xx.h"

#define DEMO_UART_ID UART4_IRQn
#define DEMO_RECEIVE_ID 0
#define DEMO_TRANSMIT_ID 1
#define DEMO_FAST_ID 2
#define DEMO_SLOW_ID 3

#define DEMO_UART_PRIORITY IBOS_PRIORITY_HIGHEST
#define DEMO_RECEIVE_PRIORITY IBOS_PRIORITY_HIGH
#define DEMO_TRANSMIT_PRIORITY IBOS_PRIORITY_HIGH
#define DEMO_FAST_PRIORITY IBOS_PRIORITY_MEDIUM
#define DEMO_SLOW_PRIORITY IBOS_PRIORITY_LOW

#define DEMO_MAX_EVENTS 16

#define DEMO_FAST_PACKET_KIND 0
#define DEMO_SLOW_PACKET_KIND 1

typedef struct {
  u32 packet_id;
  u32 type;
  u32 data;
} DEMO_packet_t;
