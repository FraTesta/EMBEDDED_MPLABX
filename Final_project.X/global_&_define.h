#ifndef XC_HEADER_GLOBAL_H
#define	XC_HEADER_GLOBAL_H

#include <xc.h>
#include "myBuffer.h"
#include "parser.h"
#include "myScheduler.h"
//Hardware specifications
#define MAX_PROPELLER 8000
#define MIN_PROPELLER -8000
#define MAX_MOTOR_RPM 10000
#define MIN_MOTOR_RPM -10000

// states of the uC
#define CONTROLLED_MODE 0
#define TIMEOUT_MODE 1
#define SAFE_MODE 2

// buttons
#define S6_NOT_PRESSED 0 // S6 not pressed
#define S6_PRESSED 1 // S6 pressed 
// buffers 
#define UART_BUFF_DIM 100
#define TEMP_BUFF_DIM 10

// frequencies
#define FCY 1843200
#define F_PWM 1000

// scheduler 
#define MAX_TASK 7

typedef struct{
    int maxRPM;
    int minRPM;
    int leftRPM;
    int rightRPM;
}motorsData;

extern heart_beat schedInfo[MAX_TASK];
extern int uC_state;
extern int S6status;
extern circularBuffer UARTbuf;
extern temperatureBuffer tempBuf;
extern motorsData motor_data;




#endif	/* XC_HEADER_GLOBAL_H */

