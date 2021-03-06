/*
 * File:   main.c
 * Author: francesco testa
 *         luca covizi
 *
 * Created on 16 gennaio 2021, 15.14
 */


#include <p30F4011.h>

#include "xc.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
// personal libraries
#include "timerFunc.h"
#include "myADC.h"
#include "parser.h"
#include "myScheduler.h"
#include "myUART.h"
#include "myPWM.h"
#include "mySPI_LCD.h"
#include "global_&_define.h"  // file.h which contains all global variables 
#include "myBuffer.h"
#include "buttons.h"
#include "tasks.h"  // contains all scheduled task 

// The following are variables necessarily global since they are also used in some interrupt handler functions,
//and in other project files.
// Microcontroller state 
int uC_state;
// Flag for button S6
int S6status;
// UART circular buffer
circularBuffer UARTbuf;
// Scheduler
heart_beat schedInfo[MAX_TASK];
// Motors data struct (defined in the global & define file)
motorsData motor_data; 



int main(void) {
    //////////////////////////////   Peripherals   /////////////////////////////////////////
    // ADC init
    adc_config(AN3); // to configure two channel of the adc 
    // PWM init 
    PWM_config();
    // UART init
    UART_config(UART_2); 
    // SPI init 
    spi_config();

    //////////////////////////////   Initialization Data   ///////////////////////////////////////////////
    // LED init 
    TRISBbits.TRISB0 = 0; // D3
    TRISBbits.TRISB1 = 0; // D4

    // motor data init
    motor_data.leftRPM = 0;
    motor_data.rightRPM = 0;
    motor_data.maxRPM = MAX_PROPELLER;
    motor_data.minRPM = MIN_PROPELLER;

    // temperature buffer 
    temperatureBuffer tempBuf;

    // initialization of the two buffers (see myBuffer.c file)
    initBuf(&UARTbuf, &tempBuf, UART_BUFF_DIM);
    // init the button S6 status which represent the mode of the lcd 
    S6status = S6_NOT_PRESSED;
    // init microcontroller mode 
    uC_state = CONTROLLED_MODE;
    // Parser state variable
    parser_state pstate;

    // Init timer for TIMEOUT mode  (see the timerFunc.c file)
    tmr_setup_period(TIMER3, 5000);
    IFS0bits.T3IF = 1; // enable timer 3 interrupt 

    // all the following tasks are defined into the task.c file 
    schedInfo[0].task = &task_PWM_controller;
    schedInfo[1].task = &task_temperature_acquisition;
    schedInfo[2].task = &task_send_temperature;
    schedInfo[3].task = &task_feedback_ack;
    schedInfo[4].task = &task_LED_blink;
    schedInfo[5].task = &task_LCD;
    schedInfo[6].task = &task_receiver;

    schedInfo[0].params = NULL;
    schedInfo[1].params = &tempBuf;
    schedInfo[2].params = &tempBuf;
    schedInfo[3].params = NULL;
    schedInfo[4].params = &tempBuf;
    schedInfo[5].params = &tempBuf;
    schedInfo[6].params = &pstate;

    // initialize n
    schedInfo[0].n = 0;
    schedInfo[1].n = 0;
    schedInfo[2].n = 0;
    schedInfo[3].n = 0;
    schedInfo[4].n = 0;
    schedInfo[5].n = 0;
    schedInfo[6].n = 0;

    // N[i] =  heartbeat / period of task 
    schedInfo[0].N = 1; // PWM controller 50 Hz
    schedInfo[1].N = 5; // Temperature Acquisition 10 Hz
    schedInfo[2].N = 50; // Send temperature 1 Hz
    schedInfo[3].N = 10; // Feedback 5 Hz
    schedInfo[4].N = 25; // LED blink 1 Hz (500ms on /500ms off = 2 Hz)
    schedInfo[5].N = 5; // LCD 10 Hz
    schedInfo[6].N = 1; // Task Reciver 50 Hz 
    // Since the bound rate is 9600 bps (to avoid UART buffer overflow in TX), therfore the circular buffer
    //will be written quite fast. So we need a reciver which is not too slow to read (50 Hz), otherwise 
    // some data might be ovrewritten before being processing. 



    // heartbeat time
    tmr_setup_period(TIMER1, 50); // 10 ms =  100 Hz 
    while (1) {
        scheduler(MAX_TASK, &schedInfo); // ora non devo pi� passare &si allo scheduler 
        tmr_wait_period(TIMER1);
    }
    return 0;
}
