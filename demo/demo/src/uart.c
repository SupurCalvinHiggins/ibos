#include "../include/uart.h"
#include "../../../ibos/include/interrupt.h"
#include "../../../ibos/include/task.h"
#include "../include/config.h"
#include "../include/receive.h"
#include "../include/transmit.h"
#include "../include/uart.h"
#include <assert.h>

IBOS_memory_block_t receive_block;
usize receive_size;

DEMO_packet_t transmit_packet;
usize transmit_size;

void DEMO_uart_transmit(DEMO_packet_t packet) {
  assert(transmit_size == sizeof(DEMO_packet_t));
  transmit_size = 1;
  transmit_packet = transmit_packet;
  UART4->CR1 |= USART_CR1_TXEIE;
  UART4->TDR = ((u8 *)&transmit_packet)[0];
}

void DEMO_uart_handler(void) {
  // Receive
  if (UART4->ISR & USART_ISR_RXNE) {
    ((u8 *)receive_block.ptr)[receive_size] = UART4->RDR;
    ++receive_size;
    if (receive_size == sizeof(DEMO_packet_t)) {
      IBOS_event_t event = {DEMO_RECEIVE_EVENT_PACKET, receive_block};
      IBOS_task_send_event(DEMO_RECEIVE_ID, event);
      receive_block = IBOS_memory_allocate(sizeof(DEMO_packet_t));
      receive_size = 0;
    }
  }

  // Transmit
  if (UART4->ISR & USART_ISR_TXE) {
    assert(transmit_size <= sizeof(DEMO_packet_t));
    UART4->TDR = ((u8 *)&transmit_packet)[transmit_size];
    ++transmit_size;
    if (transmit_size == sizeof(DEMO_packet_t)) {
      UART4->CR1 &= ~USART_CR1_TXEIE;
      IBOS_event_t event = {DEMO_TRANSMIT_EVENT_READY, {NULL, 0}};
      IBOS_task_send_event(DEMO_TRANSMIT_ID, event);
    }
  }
}

#define TX_PIN 0
#define RX_PIN 1

void DEMO_uart_initialize(void) {
  assert(!IBOS_interrupt_get_enable_all());

  // Enable GPIO port A clock.
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

  // Set mode to alternate function.
  GPIOA->MODER &= ~(0x3 << (2 * TX_PIN));
  GPIOA->MODER |= 0x2 << (2 * TX_PIN);
  GPIOA->MODER &= ~(0x3 << (2 * RX_PIN));
  GPIOA->MODER |= 0x2 << (2 * RX_PIN);

  // Set alternate function to UART.
  GPIOA->AFR[0] &= ~(0xF << (4 * TX_PIN));
  GPIOA->AFR[0] |= 0x8 << (4 * TX_PIN);
  GPIOA->AFR[0] &= ~(0xF << (4 * RX_PIN));
  GPIOA->AFR[0] |= 0x8 << (4 * RX_PIN);

  // Set speed to high speed.
  GPIOA->OSPEEDR &= ~(0x3 << (2 * TX_PIN));
  GPIOA->OSPEEDR |= 0x3 << (2 * TX_PIN);
  GPIOA->OSPEEDR &= ~(0x3 << (2 * RX_PIN));
  GPIOA->OSPEEDR |= 0x3 << (2 * RX_PIN);

  // Enable pull-up.
  GPIOA->PUPDR &= ~(0x3 << (2 * TX_PIN));
  GPIOA->PUPDR |= 0x1 << (2 * TX_PIN);
  GPIOA->PUPDR &= ~(0x3 << (2 * RX_PIN));
  GPIOA->PUPDR |= 0x1 << (2 * RX_PIN);

  // Set to push-pull output.
  GPIOA->OTYPER &= ~(0x1 << TX_PIN);
  GPIOA->OTYPER &= ~(0x1 << RX_PIN);

  // Enable UART peripheral clock.
  RCC->APB1ENR1 |= RCC_APB1ENR1_UART4EN;

  // Set UART peripheral clock to the system clock.
  RCC->CCIPR &= ~RCC_CCIPR_UART4SEL;
  RCC->CCIPR |= RCC_CCIPR_UART4SEL_0;

  // Disable UART.
  UART4->CR1 &= ~USART_CR1_UE;

  // Set data length to 8 bits.
  UART4->CR1 &= ~USART_CR1_M;

  // Set single stop bit.
  UART4->CR2 &= ~USART_CR2_STOP;

  // Set parity to no parity.
  UART4->CR1 &= ~USART_CR1_PCE;

  // Set oversampling to 16.
  UART4->CR1 &= ~USART_CR1_OVER8;

  // Set baud rate to 9600 (4MHz clock).
  UART4->BRR = 0x1A1;

  // Enable transmit and receive.
  UART4->CR1 |= (USART_CR1_TE | USART_CR1_RE);

  // Enable UART.
  UART4->CR1 |= USART_CR1_UE;
  while ((UART4->ISR & USART_ISR_TEACK) == 0)
    ;
  while ((UART4->ISR & USART_ISR_REACK) == 0)
    ;

  // Enable receive interrupt.
  UART4->CR1 |= USART_CR1_RXNEIE;

  // Disable transmit interrupt.
  UART4->CR1 &= ~USART_CR1_TXEIE;

  IBOS_interrupt_set_handler(DEMO_UART_ID, DEMO_uart_handler);
  IBOS_interrupt_set_priority(DEMO_UART_ID, DEMO_UART_PRIORITY);
  IBOS_interrupt_enable(DEMO_UART_ID);
}
