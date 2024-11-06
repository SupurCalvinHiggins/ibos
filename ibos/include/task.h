#pragma once

#include "../include/event_queue.h"
#include "../port/include/interrupt.h"
#include "int.h"

#define IBOS_TASK_LEN IBOS_INTERRUPT_LEN

typedef enum {
  IBOS_TASK_0 = IBOS_INTERRUPT_0,
  IBOS_TASK_1 = IBOS_INTERRUPT_1,
  IBOS_TASK_2 = IBOS_INTERRUPT_2,
  IBOS_TASK_3 = IBOS_INTERRUPT_3,
} IBOS_task_id_t;

typedef struct {
  IBOS_task_id_t task_id;
  IBOS_event_t event;
} IBOS_task_event_t;

void IBOS_task_enter_critical(void);
void IBOS_task_exit_critical(void);

void IBOS_task_init(IBOS_task_id_t task_id, void (*task)(void),
                    usize max_events);
void IBOS_task_dispatch(IBOS_task_event_t *events, usize events_len);
