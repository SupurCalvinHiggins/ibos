#include "../../../ibos/include/interrupt.h"
#include "../../../ibos/include/memory.h"
#include "../../../ibos/include/task.h"
#include "../include/fast.h"
#include "../include/receive.h"
#include "../include/slow.h"
#include "../include/transmit.h"
#include "../include/uart.h"

extern const uptr __HeapBase;
extern const uptr __HeapLimit;

int main(void) {
    IBOS_task_enter_critical();
    IBOS_memory_block_t memory = {(void *) 0x200001a8, __HEAP_SIZE};
    IBOS_memory_initialize(memory);
    IBOS_interrupt_initialize();
    DEMO_receive_initialize();
    DEMO_transmit_initialize();
    DEMO_fast_initialize();
    DEMO_slow_initialize();
    DEMO_uart_initialize();
    IBOS_task_exit_critical();

    while (1);
}
