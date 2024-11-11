#include "../../../ibos/include/interrupt.h"
#include "../../../ibos/include/memory.h"
#include "../../../ibos/include/task.h"
#include "../include/uart.h"

extern const u8 __HeapBase;
extern const u8 __HeapLimit;

int main(void) {
    IBOS_task_enter_critical();
    usize __HeapSize = &__HeapLimit - &__HeapBase;
    IBOS_memory_block_t memory = {(void *) &__HeapBase, __HeapSize};
    IBOS_memory_initialize(memory);
    IBOS_interrupt_initialize();
    DEMO_uart_initialize();
    IBOS_task_exit_critical();

    while (1);
}
