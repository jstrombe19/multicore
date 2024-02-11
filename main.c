#include "main.h"

// Arrays in which raw measurements will be stored
fix15 acceleration[3], gyro[3];

// character array
char screentext[40];

// draw speed
int threshold = 10 ;

// Some macros for max/min/abs
#define min(a,b)    ((a<b) ? a:b)
#define max(a,b)    ((a<b) ? b:a)
#define abs(a)      ((a>0) ? a:-a)

#define LED 25

// semaphore
static struct pt_sem vga_semaphore ;

// Some paramters for PWM
#define WRAPVAL 5000
#define CLKDIV  25.0
uint slice_num ;

// Interrupt service routine
void on_pwm_wrap() {

    // Clear the interrupt flag that brought us here
    pwm_clear_irq(pwm_gpio_to_slice_num(5));

    // Read the IMU
    // NOTE! This is in 15.16 fixed point. Accel in g's, gyro in deg/s
    // If you want these values in floating point, call fix2float15() on
    // the raw measurements.
    mpu6050_read_raw(acceleration, gyro);

    // Signal VGA to draw
    PT_SEM_SIGNAL(pt, &vga_semaphore);
}

// // Thread that draws to VGA display
// static PT_THREAD (protothread_vga(struct pt *pt))
// {
//     // Indicate start of thread
//     PT_BEGIN(pt) ;

//     // We will start drawing at column 81
//     static int xcoord = 81 ;
    
//     // Rescale the measurements for display
//     static float OldRange = 500. ; // (+/- 250)
//     static float NewRange = 150. ; // (looks nice on VGA)
//     static float OldMin = -250. ;
//     static float OldMax = 250. ;

//     // Control rate of drawing
//     static int throttle ;

//     // Draw the static aspects of the display
//     setTextSize(1) ;
//     setTextColor(WHITE);

//     // Draw bottom plot
//     drawHLine(75, 430, 5, CYAN) ;
//     drawHLine(75, 355, 5, CYAN) ;
//     drawHLine(75, 280, 5, CYAN) ;
//     drawVLine(80, 280, 150, CYAN) ;
//     sprintf(screentext, "0") ;
//     setCursor(50, 350) ;
//     writeString(screentext) ;
//     sprintf(screentext, "+2") ;
//     setCursor(50, 280) ;
//     writeString(screentext) ;
//     sprintf(screentext, "-2") ;
//     setCursor(50, 425) ;
//     writeString(screentext) ;

//     // Draw top plot
//     drawHLine(75, 230, 5, CYAN) ;
//     drawHLine(75, 155, 5, CYAN) ;
//     drawHLine(75, 80, 5, CYAN) ;
//     drawVLine(80, 80, 150, CYAN) ;
//     sprintf(screentext, "0") ;
//     setCursor(50, 150) ;
//     writeString(screentext) ;
//     sprintf(screentext, "+250") ;
//     setCursor(45, 75) ;
//     writeString(screentext) ;
//     sprintf(screentext, "-250") ;
//     setCursor(45, 225) ;
//     writeString(screentext) ;
    

//     while (true) {
//         // Wait on semaphore
//         PT_SEM_WAIT(pt, &vga_semaphore);
//         // Increment drawspeed controller
//         throttle += 1 ;
//         // If the controller has exceeded a threshold, draw
//         if (throttle >= threshold) { 
//             // Zero drawspeed controller
//             throttle = 0 ;

//             // Erase a column
//             drawVLine(xcoord, 0, 480, BLACK) ;

//             // Draw bottom plot (multiply by 120 to scale from +/-2 to +/-250)
//             drawPixel(xcoord, 430 - (int)(NewRange*((float)((fix2float15(acceleration[0])*120.0)-OldMin)/OldRange)), WHITE) ;
//             drawPixel(xcoord, 430 - (int)(NewRange*((float)((fix2float15(acceleration[1])*120.0)-OldMin)/OldRange)), RED) ;
//             drawPixel(xcoord, 430 - (int)(NewRange*((float)((fix2float15(acceleration[2])*120.0)-OldMin)/OldRange)), GREEN) ;

//             // Draw top plot
//             drawPixel(xcoord, 230 - (int)(NewRange*((float)((fix2float15(gyro[0]))-OldMin)/OldRange)), WHITE) ;
//             drawPixel(xcoord, 230 - (int)(NewRange*((float)((fix2float15(gyro[1]))-OldMin)/OldRange)), RED) ;
//             drawPixel(xcoord, 230 - (int)(NewRange*((float)((fix2float15(gyro[2]))-OldMin)/OldRange)), GREEN) ;

//             // Update horizontal cursor
//             if (xcoord < 609) {
//                 xcoord += 1 ;
//             }
//             else {
//                 xcoord = 81 ;
//             }
//         }
//     }
//     // Indicate end of thread
//     PT_END(pt);
// }

// User input thread. User can change draw speed
static PT_THREAD (protothread_serial(struct pt *pt)) {
    PT_BEGIN(pt) ;
    static char classifier ;
    static int test_in ;
    static float float_in ;
    while(1) {
        sprintf(pt_serial_out_buffer, "input a command: ");
        serial_write ;
        // spawn a thread to do the non-blocking serial read
        serial_read ;
        // convert input string to number
        sscanf(pt_serial_in_buffer,"%c", &classifier) ;

        // num_independents = test_in ;
        if (classifier=='t') {
            sprintf(pt_serial_out_buffer, "timestep: ");
            serial_write ;
            serial_read ;
            // convert input string to number
            sscanf(pt_serial_in_buffer,"%d", &test_in) ;
            if (test_in > 0) {
                threshold = test_in ;
            }
        }
    }
    PT_END(pt) ;
}

// LED thread to demonstrate running multiple threads on a single core
static PT_THREAD (protothread_led(struct pt *pt)) {
    PT_BEGIN(pt);
    while(1) {
        // toggle the LED state: low -> high | high -> low
        gpio_put(LED, !(gpio_get(LED)));
        PT_YIELD_usec(250000);
    }
    PT_END(pt);
}

// Entry point for core 1
void core1_entry() {
    pt_add_thread(protothread_vga) ;
    pt_add_thread(protothread_led);
    pt_schedule_start ;
}

int main() {

    // Initialize stdio
    stdio_init_all();

    // // Initialize VGA
    // initVGA() ;

    ////////////////////////////////////////////////////////////////////////
    ///////////////////////// I2C CONFIGURATION ////////////////////////////
    i2c_init(I2C_CHAN, I2C_BAUD_RATE) ;
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C) ;
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C) ;
    gpio_pull_up(SDA_PIN) ;
    gpio_pull_up(SCL_PIN) ;

    // MPU6050 initialization
    mpu6050_reset();
    mpu6050_read_raw(acceleration, gyro);

    ////////////////////////////////////////////////////////////////////////
    ///////////////////////// PWM CONFIGURATION ////////////////////////////
    ////////////////////////////////////////////////////////////////////////
    // Tell GPIO's 4,5 that they are allocated to the PWM
    gpio_set_function(5, GPIO_FUNC_PWM);
    gpio_set_function(4, GPIO_FUNC_PWM);

    // Find out which PWM slice is connected to GPIO 5 (it's slice 2, same for 4)
    slice_num = pwm_gpio_to_slice_num(5);

    // Mask our slice's IRQ output into the PWM block's single interrupt line,
    // and register our interrupt handler
    pwm_clear_irq(slice_num);
    pwm_set_irq_enabled(slice_num, true);
    irq_set_exclusive_handler(PWM_IRQ_WRAP, on_pwm_wrap);
    irq_set_enabled(PWM_IRQ_WRAP, true);

    // This section configures the period of the PWM signals
    pwm_set_wrap(slice_num, WRAPVAL) ;
    pwm_set_clkdiv(slice_num, CLKDIV) ;

    // This sets duty cycle
    pwm_set_chan_level(slice_num, PWM_CHAN_B, 0);
    pwm_set_chan_level(slice_num, PWM_CHAN_A, 0);

    // Start the channel
    pwm_set_mask_enabled((1u << slice_num));


    ////////////////////////////////////////////////////////////////////////
    ///////////////////////////// ROCK AND ROLL ////////////////////////////
    ////////////////////////////////////////////////////////////////////////
    // start core 1 
    multicore_reset_core1();
    multicore_launch_core1(core1_entry);

    // start core 0
    pt_add_thread(protothread_serial) ;
    pt_schedule_start ;

}


// Global shared telemetry data
volatile uint32_t telemetry_data[2] = {0};

// Function to increase taxation of the processor to plot the effects on chip temperature
void dormamu_ive_come_to_bargain(void) {
    for (size_t i = 0; i < 25; i++)
    {
        uint32_t random_number = get_rand_32();
        printf("Random number %d: %lu\n", i, random_number);
    }
    
}


// Thread that runs on core 1
static PT_THREAD (protothread_vga(struct pt *pt))
{
    // Indicate start of thread
    PT_BEGIN(pt) ;

    while (true) {
        // Wait on semaphore
        PT_SEM_WAIT(pt, &vga_semaphore);

        printf("acceleration[0]: %lf\n",((float)((fix2float15(acceleration[0])*120.0))));
        printf("acceleration[1]: %lf\n",((float)((fix2float15(acceleration[1])*120.0))));
        printf("acceleration[2]: %lf\n",((float)((fix2float15(acceleration[2])*120.0))));

        // Draw top plot
        printf("gyro[0]: %lf\n",((float)((fix2float15(gyro[0])))));
        printf("gyro[1]: %lf\n",((float)((fix2float15(gyro[1])))));
        printf("gyro[2]: %lf\n",((float)((fix2float15(gyro[2])))));  
        
        telemetry_data[1]++;
        printf("Core 1: Telemetry Data 0: %d\n", telemetry_data[0]);
        printf("Core 1: Telemetry Data 1: %d\n", telemetry_data[1]);
        printf("\n");
        // dormamu_ive_come_to_bargain();
        printf("\n");
        sleep_ms(200);
    }
    // Indicate end of thread
    PT_END(pt);
}


// Function to run on Core 1
void core1_task() {
    while (1) {
        telemetry_data[1]++;
        printf("Core 1: Telemetry Data 0: %d\n", telemetry_data[0]);
        printf("Core 1: Telemetry Data 1: %d\n", telemetry_data[1]);
        printf("\n");
        dormamu_ive_come_to_bargain();
        printf("\n");
        sleep_ms(2000);
    }
}

// int main() {
//     stdio_init_all();
//     printf("Hello world!");

//     // Start Core 1
//     multicore_launch_core1(core1_task);

//     int adc_status = adc_quick_init(4);
//     printf("ADC status: %d\n", adc_status);

//     while(1) {
//         telemetry_data[0] += 2;
//         printf("Core 0: Telemetry Data 0: %d\n", telemetry_data[0]);
//         printf("Core 0: Telemetry Data 1: %d\n", telemetry_data[1]);

//         int raw_adc_value = adc_read_raw_temp();
//         printf("ADC raw temp: %d\n", raw_adc_value);

//         float adc_voltage_converted = convert_raw_to_voltage(raw_adc_value);
//         printf("ADC voltage: %f\n", adc_voltage_converted);

//         float current_adc_temp = convert_voltage_to_temp(adc_voltage_converted);
//         printf("ADC temperature: %f\n", current_adc_temp);

//         sleep_ms(1000);
//     }

//     return 0;

// }