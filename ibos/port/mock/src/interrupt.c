#include "../include/interrupt.h"

IBOS_interrupt_id_t IBOS_interrupt_unused_ids[4] = {0, 1, 2, 3};

bool IBOS_interrupt_enable_all_state = true;
bool IBOS_interrupt_enable_state[256] = {false};
bool IBOS_interrupt_pending_state[256] = {false};
IBOS_priority_t IBOS_interrupt_priority[256] = {IBOS_PRIORITY_LOW};
void (*IBOS_interrupt_handler[256])(void) = {NULL};

void IBOS_interrupt_enable_all() { IBOS_interrupt_enable_all_state = true; }
void IBOS_interrupt_disable_all() { IBOS_interrupt_enable_all_state = false; }
bool IBOS_interrupt_get_enable_all() { return IBOS_interrupt_enable_all_state; }

void IBOS_interrupt_enable(IBOS_interrupt_id_t id) {
  IBOS_interrupt_enable_state[id] = true;
}
void IBOS_interrupt_disable(IBOS_interrupt_id_t id) {
  IBOS_interrupt_enable_state[id] = false;
}
bool IBOS_interrupt_get_enable(IBOS_interrupt_id_t id) {
  return IBOS_interrupt_enable_state[id];
}

void IBOS_interrupt_schedule(IBOS_interrupt_id_t id) {
  IBOS_interrupt_pending_state[id] = true;
}
void IBOS_interrupt_deschedule(IBOS_interrupt_id_t id) {
  IBOS_interrupt_pending_state[id] = false;
}
bool IBOS_interrupt_get_schedule(IBOS_interrupt_id_t id) {
  return IBOS_interrupt_pending_state[id];
}

void IBOS_interrupt_set_handler(IBOS_interrupt_id_t id, void (*handler)(void)) {
  IBOS_interrupt_handler[id] = handler;
}
void (*IBOS_interrupt_get_handler(IBOS_interrupt_id_t id))(void) {
  return IBOS_interrupt_handler[id];
}

void IBOS_interrupt_set_priority(IBOS_interrupt_id_t id,
                                 IBOS_priority_t priority) {
  IBOS_interrupt_priority[id] = priority;
}
IBOS_priority_t IBOS_interrupt_get_priority(IBOS_interrupt_id_t id) {
  return IBOS_interrupt_priority[id];
}
