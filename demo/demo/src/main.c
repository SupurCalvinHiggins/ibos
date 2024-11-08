#include "../../../ibos/include/interrupt.h"
#include "../include/fast.h"
#include "../include/receive.h"
#include "../include/slow.h"
#include "../include/transmit.h"
#include "../include/uart.h"

int main(void) {
  IBOS_interrupt_disable_all();
  DEMO_receive_initialize();
  DEMO_transmit_initialize();
  DEMO_fast_initialize();
  DEMO_slow_initialize();
  DEMO_uart_initialize();
  IBOS_interrupt_enable_all();

  while (1)
    ;
}
