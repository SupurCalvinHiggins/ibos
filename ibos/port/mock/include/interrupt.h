#pragma once

#include "../../include/int.h"

#define IBOS_INTERRUPT_LEN 4

typedef enum {
  IBOS_INTERRUPT_0 = 0,
  IBOS_INTERRUPT_1 = 1,
  IBOS_INTERRUPT_2 = 2,
  IBOS_INTERRUPT_3 = 3,
} IBOS_interrupt_id_t;

typedef enum {
  IBOS_INTERRUPT_VERY_HIGH,
  IBOS_INTERRUPT_HIGH,
  IBOS_INTERRUPT_MEDIUM,
  IBOS_INTERRUPT_LOW,
} IBOS_interrupt_priority_t;

typedef enum {
  IBOS_INTERRUPT_DISABLE = 0,
  IBOS_INTERRUPT_ENABLE = 1,
} IBOS_interrupt_enable_t;

typedef enum {
  IBOS_INTERRUPT_PENDING = 1,
  IBOS_INTERRUPT_NOT_PENDING = 0,
} IBOS_interrupt_pending_t;

void IBOS_interrupt_set_handler(IBOS_interrupt_id_t interrupt_id,
                                void (*handler)(void));

void IBOS_interrupt_set_enable_all(IBOS_interrupt_enable_t enable);
IBOS_interrupt_enable_t IBOS_interrupt_get_enable_all(void);

void IBOS_interrupt_set_enable(IBOS_interrupt_id_t interrupt_id,
                               IBOS_interrupt_enable_t enable);
IBOS_interrupt_enable_t
IBOS_interrupt_get_enable(IBOS_interrupt_id_t interrupt_id);

void IBOS_interrupt_set_pending(IBOS_interrupt_id_t interrupt_id,
                                IBOS_interrupt_pending_t pending);
IBOS_interrupt_pending_t
IBOS_interrupt_get_pending(IBOS_interrupt_id_t interrupt_id);

void IBOS_interrupt_set_priority(IBOS_interrupt_id_t interrupt_id,
                                 IBOS_interrupt_priority_t priority);
IBOS_interrupt_priority_t
IBOS_interrupt_get_priority(IBOS_interrupt_id_t interrupt_id);
