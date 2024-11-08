#pragma once

#include "../include/event_queue.h"
#include "int.h"
#include "interrupt.h"

#define IBOS_TASK_MAX 4

typedef u8 IBOS_task_id_t;

void IBOS_task_enter_critical(void);
void IBOS_task_exit_critical(void);

void IBOS_task_initialize(IBOS_task_id_t task_id, void (*task)(void),
                          IBOS_priority_t priority, usize max_events);
void IBOS_task_send_event(IBOS_task_id_t id, IBOS_event_t event);
IBOS_event_t IBOS_task_receive_event(IBOS_task_id_t id);
bool IBOS_task_can_receive_event(IBOS_task_id_t id);
