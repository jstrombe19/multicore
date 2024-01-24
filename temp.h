#pragma once 
#ifndef TEMP_H
#define TEMP_H

#include "hardware/adc.h"

static const float conversion = 3.3f / (1 << 12);
static const float twenty_seven_degrees = 0.706;
static const float per_degree_slope = -1.721;



int adc_quick_init(int input);
int adc_read_raw_temp(void);
float convert_raw_to_voltage(int raw);
float convert_voltage_to_temp(float voltage);


#endif