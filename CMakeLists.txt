cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico-stepper-counter C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

add_executable(
    stepper-counter
    stepper-counter.c
)

# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(
    stepper-counter
    pico_stdlib
)

# enable usb output, disable uart output
pico_enable_stdio_usb(stepper-counter 1)
pico_enable_stdio_uart(stepper-counter 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(stepper-counter)

# add url via pico_set_program_url
# example_auto_set_url(stepper-counter)
