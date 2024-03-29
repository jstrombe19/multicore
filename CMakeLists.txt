cmake_minimum_required(VERSION 3.13)

set(PICO_SDK_PATH "/home/jstromberg/Development/raspberry_pi/pico-sdk")

include(${PICO_SDK_PATH}/pico_sdk_init.cmake)

project(multicore)

# Initialize the Pico SDK
pico_sdk_init()

add_executable(multicore main.c temp.c mpu6050.c)

# Add Pico SDK as a subdirectory
add_subdirectory(${PICO_SDK_PATH} pico_sdk)

# Add pico multicore (required for multicore functionality)
target_link_libraries(multicore 
    pico_float
    pico_multicore
    pico_rand
    pico_stdlib 
    pico_time
    hardware_adc
    hardware_dma
    hardware_gpio
    hardware_i2c
    hardware_irq
    hardware_pio
    hardware_pwm
    hardware_spi
    hardware_sync
    hardware_timer
    )

# Enable multicore support
pico_enable_stdio_uart(multicore 0)
pico_enable_stdio_usb(multicore 1)

# Add pico SDK dependency
target_include_directories(multicore PRIVATE 
    ${PICO_SDK_PATH}/src/rp2_common/hardware_adc
    ${PICO_SDK_PATH}/src/rp2_common/hardware_dma
    ${PICO_SDK_PATH}/src/rp2_common/hardware_gpio
    ${PICO_SDK_PATH}/src/rp2_common/hardware_irq
    ${PICO_SDK_PATH}/src/rp2_common/hardware_i2c
    ${PICO_SDK_PATH}/src/rp2_common/hardware_pio
    ${PICO_SDK_PATH}/src/rp2_common/hardware_pwm 
    ${PICO_SDK_PATH}/src/rp2_common/hardware_spi
    ${PICO_SDK_PATH}/src/rp2_common/hardware_sync
    ${PICO_SDK_PATH}/src/rp2_common/hardware_timer
)

# Create map/bin/hex file etc.
pico_add_extra_outputs(multicore)