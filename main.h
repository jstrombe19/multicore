#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <math.h>
#include "temp.h"
#include "pico/rand.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/spi.h"
#include "hardware/sync.h"


// === fixed point macros ===
static const float f14 = 2 << 14;
typedef signed int fix15;
#define multfix15(a,b)  ((fix15)((((signed long long)(a))*((signed long long)(b)))>>15))
#define float2fix15(a)  ((fix15)((a)*f14))
#define fix2float15(a)  ((float)(a)/f14)
#define absfix15(a)     abs(a)
#define int2fix15(a)    ((fix15)(a << 15))
#define fix2int15(a)    ((int)(a >> 15))
#define char2fix15(a)   (fix15)(((fix15)(a)) << 15)
#define divfix(a,b)     (fix15)((((signed long long)(a)) << 15) / (b))

// === DDS parameters ===
#define two32 (double)2 << 31
#define Fs 40000
// DDS units - core 1
volatile unsigned int phase_accum_main_1;
volatile unsigned int phase_incre_main_1 = (800.0*two32) / Fs;
// DDS units - core 2
volatile unsigned int phase_accum_main_2;
volatile unsigned int phase_incre_main_2 = (400.0*two32) / Fs;

// DDS sine table 
#define sine_table_size 256
fix15 sin_table[sine_table_size];



#endif