// this file defines the scanner state machine
// 'ssm' means 'scanner state machine'

#ifndef SCANNER_STATE_MACHINE
#define SCANNER_STATE_MACHINE

#include "stdint.h"

// this enum defines the states in the state machine
typedef enum
{
    IDLE = 0,
    SEARCH,
    POSITIONING,
    MEASUREMENT
} scan_state_t;

int init_ssm();
void execute_ssm();
scan_state_t get_state();
uint8_t get_time_left();



#endif