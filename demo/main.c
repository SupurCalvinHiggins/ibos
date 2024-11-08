#include "../ibos/include/interrupt.h"
#include "../ibos/include/task.h"
#include "../ibos/port/stm32l476rg/src/lib/stm32l476rg/include/stm32l476xx.h"

#define RECEIVE_TASK 0
#define TRANSMIT_TASK 1
#define TSP_TASK 2
#define MAT_TASK 3

typedef struct {
  u8 version;
  u8 content_kind;
  u16 content_size;
  u8 content[506];
  u16 checksum;
} packet_t;

IBOS_memory_block_t receive_block;
usize receive_size;

IBOS_memory_block_t transmit_block;
usize transmit_size;

void UART4_IRQHandler(void) {
  if (UART4->ISR & USART_ISR_RXNE) {
    ((u8 *)receive_block.ptr)[receive_size] = UART4->RDR;
    ++receive_size;
    if (receive_size == 512) {
      IBOS_task_push(RECEIVE_TASK, (IBOS_event_t){0, receive_block});
      receive_block = IBOS_memory_allocate(512);
      receive_size = 0;
    }
  }

  if (UART4->ISR & USART_ISR_TXE) {
    UART4->TDR = ((u8 *)(transmit_block.ptr))[transmit_size];
    ++transmit_size;
    if (transmit_size == 512) {
      UART4->CR1 &= ~USART_CR1_TXEIE;
      IBOS_memory_deallocate(transmit_block);
      transmit_size = 0;
    }
  }
}

void receive_task(void) {
  // TODO: need to be able to iterate while there are events
  IBOS_event_t event = IBOS_task_pop(RECEIVE_TASK);
  packet_t *packet = (packet_t *)event.event_data.ptr;
  switch (packet->content_kind) {}
}
void transmit_task(void) {
  // if ready to transmit, transmit
  // if not ready to transmit, put event back in queue? no should be signaled by
  // UART4
}
void tsp_task(void) {}
void mat_task(void) {}

int main(void) {
  IBOS_task_enter_critical();

  IBOS_memory_block_t memory;
  IBOS_memory_initialize(memory);

  IBOS_task_initialize(RECEIVE_TASK, receive_task, IBOS_PRIORITY_HIGH, 16);
  IBOS_task_initialize(TRANSMIT_TASK, transmit_task, IBOS_PRIORITY_HIGH, 16);
  IBOS_task_initialize(TSP_TASK, tsp_task, IBOS_PRIORITY_MEDIUM, 16);
  IBOS_task_initialize(MAT_TASK, mat_task, IBOS_PRIORITY_LOW, 16);

  IBOS_interrupt_set_handler(UART4_IRQn, UART4_IRQHandler);
  IBOS_interrupt_set_priority(UART4_IRQn, IBOS_PRIORITY_HIGHEST);
  IBOS_interrupt_enable(UART4_IRQn);

  IBOS_task_exit_critical();

  while (1)
    ;
}
