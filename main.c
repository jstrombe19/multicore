#include "main.h"


// Global shared telemetry data
volatile uint32_t telemetry_data[2] = {0};

// Function to run on Core 1
void core1_task() {
    while (1) {
        telemetry_data[1]++;
        printf("Core 1: Telemetry Data 0: %d\n", telemetry_data[0]);
        printf("Core 1: Telemetry Data 1: %d\n", telemetry_data[1]);
        sleep_ms(2000);
    }
}

int main() {
    stdio_init_all();
    printf("Hello world!");

    // Start Core 1
    multicore_launch_core1(core1_task);

    int adc_status = adc_quick_init(4);
    printf("ADC status: %d\n", adc_status);

    while(1) {
        telemetry_data[0] += 2;
        printf("Core 0: Telemetry Data 0: %d\n", telemetry_data[0]);
        printf("Core 0: Telemetry Data 1: %d\n", telemetry_data[1]);

        int raw_adc_value = adc_read_raw_temp();
        printf("ADC raw temp: %d\n", raw_adc_value);

        float adc_voltage_converted = convert_raw_to_voltage(raw_adc_value);
        printf("ADC voltage: %f\n", adc_voltage_converted);

        float current_adc_temp = convert_voltage_to_temp(adc_voltage_converted);
        printf("ADC temperature: %f\n", current_adc_temp);

        sleep_ms(2000);
    }

    return 0;

}