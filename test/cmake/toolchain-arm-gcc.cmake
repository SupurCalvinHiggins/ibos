set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER "arm-none-eabi-gcc")
set(CMAKE_ASM_COMPILER "arm-none-eabi-gcc")
set(CMAKE_LINKER "arm-none-eabi-ld")
set(CMAKE_OBJCOPY "arm-none-eabi-objcopy")
set(CMAKE_SIZE "arm-none-eabi-size")

set(CMAKE_C_FLAGS_INIT "")
set(CMAKE_EXE_LINKER_FLAGS_INIT "--specs=nano.specs --specs=nosys.specs")

set(CMAKE_EXECUTABLE_SUFFIX_C ".elf")
