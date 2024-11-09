#include "../../../include/interrupt.h"
#include "lib/cmsis/include/cmsis_gcc.h"
#include "lib/stm32l476rg/include/stm32l476xx.h"

IBOS_interrupt_id_t IBOS_interrupt_unused_ids[] = {
        TSC_IRQn,
        LCD_IRQn,
        RNG_IRQn,
        FPU_IRQn,
};

extern u32 __isr_vector[256];
u32 IBOS_interrupt_ivt[256] __attribute__(( aligned(0x100)));

// TOOD: must be called in protected context
void IBOS_interrupt_initialize(void) {
    for (usize i = 0; i < 256; ++i) {
        IBOS_interrupt_ivt[i] = __isr_vector[i];
    }

    SCB->VTOR = (u32) IBOS_interrupt_ivt;
    __DSB();
}

void IBOS_interrupt_enable_all(void) { __enable_irq(); }

void IBOS_interrupt_disable_all(void) { __disable_irq(); }

bool IBOS_interrupt_get_enable_all(void) {
    u32 primask;
    asm volatile("MRS %0, primask" : "=r"(primask));
    return !primask;
}

void IBOS_interrupt_enable(IBOS_interrupt_id_t id) {
    NVIC_EnableIRQ(id);
}

void IBOS_interrupt_disable(IBOS_interrupt_id_t id) { NVIC_DisableIRQ(id); }

bool IBOS_interrupt_get_enable(IBOS_interrupt_id_t id) {
    return NVIC_GetEnableIRQ(id);
}

void IBOS_interrupt_schedule(IBOS_interrupt_id_t id) { NVIC_SetPendingIRQ(id); }

void IBOS_interrupt_deschedule(IBOS_interrupt_id_t id) {
    NVIC_ClearPendingIRQ(id);
}

bool IBOS_interrupt_get_schedule(IBOS_interrupt_id_t id) {
    return NVIC_GetPendingIRQ(id);
}

void IBOS_interrupt_set_handler(IBOS_interrupt_id_t id, void (*handler)(void)) {
    NVIC_SetVector(id, (uptr) handler);
}

void (*IBOS_interrupt_get_handler(IBOS_interrupt_id_t id))(void) {
    return NVIC_GetVector(id);
}

void IBOS_interrupt_set_priority(IBOS_interrupt_id_t id,
                                 IBOS_priority_t priority) {
    NVIC_SetPriority(id, priority);
}

IBOS_priority_t IBOS_interrupt_get_priority(IBOS_interrupt_id_t id) {
    return NVIC_GetPriority(id);
}
