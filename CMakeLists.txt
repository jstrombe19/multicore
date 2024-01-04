cmake_minimum_required(VERSION 3.13)

set(PICO_SDK_PATH "/home/jstromberg/Development/raspberry_pi/pico-sdk")

include(${PICO_SDK_PATH}/pico_sdk_init.cmake)

project(multicore)

# Initialize the Pico SDK
pico_sdk_init()

add_executable(multicore main.c)

# Add Pico SDK as a subdirectory
add_subdirectory(${PICO_SDK_PATH} pico_sdk)

# Add pico multicore (required for multicore functionality)
target_link_libraries(multicore pico_stdlib pico_multicore)

# Enable multicore support
pico_enable_stdio_uart(multicore 1)
pico_enable_stdio_usb(multicore 0)

# Add pico SDK dependency
target_include_directories(multicore PRIVATE ${PICO_SDK_PATH}/src/rp2_common/hardware_gpio)

# Create map/bin/hex file etc.
pico_add_extra_outputs(multicore)