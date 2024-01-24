#include "temp.h"



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
    float voltage = raw * conversion;
    return voltage;
}


float convert_voltage_to_temp(float voltage) {
    float temp = 27 - (voltage - twenty_seven_degrees) / per_degree_slope;
    return temp;
}
