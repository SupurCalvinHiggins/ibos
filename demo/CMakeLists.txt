cmake_minimum_required(VERSION 3.0)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/cmake/toolchain-arm-gcc.cmake")

project(main C ASM)

#set(CPU_FLAGS )
set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/linker.ld")

file(GLOB IBOS_SRCS "${CMAKE_SOURCE_DIR}/../ibos/src/*.*")
file(GLOB IBOS_PORT_SRCS "${CMAKE_SOURCE_DIR}/../ibos/port/stm32l476rg/src/*.*")
add_executable(${PROJECT_NAME} demo/src/main.c demo/src/fast.c demo/src/receive.c demo/src/slow.c demo/src/startup.S demo/src/transmit.c demo/src/uart.c ${IBOS_SRCS} ${IBOS_PORT_SRCS})

target_compile_options(${PROJECT_NAME} PRIVATE
        -mcpu=cortex-m4 -mthumb -Wall -Wextra -g -O0
)
# -flto -ffunction-sections -fdata-sections

target_link_options(${PROJECT_NAME} PRIVATE
        -T${LINKER_SCRIPT}
        -mcpu=cortex-m4 -mthumb -g
)
# -Wl,--gc-sections,-Map=${PROJECT_NAME}.map

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/demo/include" "${CMAKE_SOURCE_DIR}/../ibos/include")

target_compile_definitions(${PROJECT_NAME} PRIVATE __STARTUP_CLEAR_BSS __START=main __STACK_SIZE=4096 __HEAP_SIZE=4096 __NO_SYSTEM_INIT)
