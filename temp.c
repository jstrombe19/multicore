#include "temp.h"
#include <stdint.h>



int adc_quick_init(int input) {
    int outcome = 0;

    // configure ADC
    adc_init();
    // enable temp sensor
    adc_set_temp_sensor_enabled(true);
    // set input to 5th
    adc_select_input(input);

    return outcome;
}


int adc_read_raw_temp(void) {
    uint16_t raw = adc_read();
}

float convert_raw_to_voltage(int raw) {
    uint64_t voltage_start_time, voltage_stop_time;
    voltage_start_time = time_us_64();
    printf("Voltage start time: %llu\n", voltage_start_time);
    float voltage = raw * conversion;
    voltage_stop_time = time_us_64();
    printf("Voltage stop time: %llu\n", voltage_stop_time);
    uint64_t voltage_time_elapsed = voltage_stop_time - voltage_start_time;
    printf("Elapsed time for voltage calculation: %llu\n", voltage_time_elapsed);
    return voltage;
}


float convert_voltage_to_temp(float voltage) {
    uint64_t temp_time_start = time_us_64();
    float temp = 27 - (voltage - twenty_seven_degrees) / per_degree_slope;
    uint64_t temp_time_stop = time_us_64();
    uint64_t temp_time_elapsed = temp_time_stop - temp_time_start;
    printf("Elapsed time for temperature calculation: %llu\n", temp_time_elapsed);
    return temp;
}
