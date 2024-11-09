#pragma once

#include "int.h"
#include "priority.h"

typedef u8 IBOS_interrupt_id_t;

void IBOS_interrupt_initialize(void);

void IBOS_interrupt_enable_all(void);
void IBOS_interrupt_disable_all(void);
bool IBOS_interrupt_get_enable_all(void);

void IBOS_interrupt_enable(IBOS_interrupt_id_t id);
void IBOS_interrupt_disable(IBOS_interrupt_id_t id);
bool IBOS_interrupt_get_enable(IBOS_interrupt_id_t id);

void IBOS_interrupt_schedule(IBOS_interrupt_id_t id);
void IBOS_interrupt_deschedule(IBOS_interrupt_id_t id);
bool IBOS_interrupt_get_schedule(IBOS_interrupt_id_t id);

void IBOS_interrupt_set_handler(IBOS_interrupt_id_t id, void (*handler)(void));
void (*IBOS_interrupt_get_handler(IBOS_interrupt_id_t id))(void);

void IBOS_interrupt_set_priority(IBOS_interrupt_id_t id,
                                 IBOS_priority_t priority);
IBOS_priority_t IBOS_interrupt_get_priority(IBOS_interrupt_id_t id);

extern IBOS_interrupt_id_t IBOS_interrupt_unused_ids[];
