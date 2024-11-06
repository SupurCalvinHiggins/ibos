#include "../../include/interrupt.h"
#include "lib/cmsis/include/cmsis_gcc.h"
#include "lib/stm32l476rg/include/stm32l476xx.h"

u32 IBOS_interrupt_id_to_cmsis_id[IBOS_INTERRUPT_LEN] = {
    TSC_IRQn,
    LCD_IRQn,
    RNG_IRQn,
    FPU_IRQn,
};

void IBOS_interrupt_set_handler(IBOS_interrupt_id_t interrupt_id,
                                void (*handler)(void)) {
  u32 cmsis_id = IBOS_interrupt_id_to_cmsis_id[interrupt_id];
  NVIC_SetVector(cmsis_id, (u32)handler);
}

void IBOS_interrupt_set_enable_all(IBOS_interrupt_enable_t enable) {
  enable == IBOS_INTERRUPT_ENABLE ? __enable_irq() : __disable_irq();
}
IBOS_interrupt_enable_t IBOS_interrupt_get_enable_all(void) {
  u32 primask;
  asm volatile("MRS %0, primask" : "=r"(primask));
  return primask;
}

void IBOS_interrupt_set_enable(IBOS_interrupt_id_t interrupt_id,
                               IBOS_interrupt_enable_t enable) {
  u32 cmsis_id = IBOS_interrupt_id_to_cmsis_id[interrupt_id];
  enable == IBOS_INTERRUPT_ENABLE ? NVIC_EnableIRQ(cmsis_id)
                                  : NVIC_DisableIRQ(cmsis_id);
}
IBOS_interrupt_enable_t
IBOS_interrupt_get_enable(IBOS_interrupt_id_t interrupt_id) {
  u32 cmsis_id = IBOS_interrupt_id_to_cmsis_id[interrupt_id];
  return NVIC_GetEnableIRQ(cmsis_id);
}

void IBOS_interrupt_set_pending(IBOS_interrupt_id_t interrupt_id,
                                IBOS_interrupt_pending_t pending);
IBOS_interrupt_pending_t
IBOS_interrupt_get_pending(IBOS_interrupt_id_t interrupt_id);

void IBOS_interrupt_set_priority(IBOS_interrupt_id_t interrupt_id,
                                 IBOS_interrupt_priority_t priority);
IBOS_interrupt_priority_t
IBOS_interrupt_get_priority(IBOS_interrupt_id_t interrupt_id);
