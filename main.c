#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"


// Global shared telemetry data
volatile uint32_t telemetry_data[2] = {0};

// Function to run on Core 1
void core1_task() {
    while (1) {
        telemetry_data[1]++;
        printf("Core 1: Telemetry Data 1: %d", telemetry_data[1]);
        sleep_ms(1000);
    }
}

int main() {
    stdio_init_all();
    printf("Hello world!");

    // Start Core 1
    multicore_launch_core1(core1_task);

    while(1) {
        telemetry_data[0]++;
        printf("Core 0: Telemetry Data 0: %d", telemetry_data[0]);
        sleep_ms(1000);
    }

    return 0;

}