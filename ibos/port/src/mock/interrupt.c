#include "../../include/interrupt.h"

IBOS_interrupt_enable_t IBOS_interrupt_enable_all = IBOS_INTERRUPT_ENABLE;
IBOS_interrupt_enable_t IBOS_interrupt_enable[IBOS_INTERRUPT_LEN] = {
    IBOS_INTERRUPT_DISABLE};
IBOS_interrupt_pending_t IBOS_interrupt_pending[IBOS_INTERRUPT_LEN] = {
    IBOS_INTERRUPT_NOT_PENDING};
IBOS_interrupt_priority_t IBOS_interrupt_priority[IBOS_INTERRUPT_LEN] = {
    IBOS_INTERRUPT_LOW};

void IBOS_interrupt_set_handler(IBOS_interrupt_id_t interrupt_id,
                                void (*handler)(void)) {}

void IBOS_interrupt_set_enable_all(IBOS_interrupt_enable_t enable) {
  IBOS_interrupt_enable_all = enable;
}
IBOS_interrupt_enable_t IBOS_interrupt_get_enable_all(void) {
  return IBOS_interrupt_enable_all;
}

void IBOS_interrupt_set_enable(IBOS_interrupt_id_t interrupt_id,
                               IBOS_interrupt_enable_t enable) {
  IBOS_interrupt_enable[interrupt_id] = enable;
}
IBOS_interrupt_enable_t
IBOS_interrupt_get_enable(IBOS_interrupt_id_t interrupt_id) {
  return IBOS_interrupt_enable[interrupt_id];
}

void IBOS_interrupt_set_pending(IBOS_interrupt_id_t interrupt_id,
                                IBOS_interrupt_pending_t pending) {
  IBOS_interrupt_pending[interrupt_id] = pending;
}
IBOS_interrupt_pending_t
IBOS_interrupt_get_pending(IBOS_interrupt_id_t interrupt_id) {
  return IBOS_interrupt_pending[interrupt_id];
}

void IBOS_interrupt_set_priority(IBOS_interrupt_id_t interrupt_id,
                                 IBOS_interrupt_priority_t priority) {
  IBOS_interrupt_priority[interrupt_id] = priority;
}
IBOS_interrupt_priority_t
IBOS_interrupt_get_priority(IBOS_interrupt_id_t interrupt_id) {
  return IBOS_interrupt_priority[interrupt_id];
}
