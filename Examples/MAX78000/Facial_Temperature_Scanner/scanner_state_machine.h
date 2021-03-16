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
    MEASUREMENT,
    RESET_STATE // this resets the state machine before going to idle
} scan_state_t;


/*
    Description: This function initializes all the peripherals
                 used by the state machine.

    Parameters: none

    Return: -1 if initialization failed
*/
int init_ssm();


/*
    Description: This function starts the state machine which
                 is an infinite loop over a switch statement

    Parameters: none

    Return: none
*/
void execute_ssm();


/*
    Description: This function returns the current state 
                 of the state machine

    Parameters: none

    Return: scan_state_t enum

    ** Note that there is no public set_state() function. This
       prevents external files and functions from directly
       changing the state machine data.
       
*/
scan_state_t get_state();


/*
    Description: This function returns the amount of time
                 in seconds left before the current state
                 expires. Once the state expires a timer
                 interrupt handler will execute and reset
                 the state machine

    Parameters: none

    Return: the amount of time left in seconds
*/
uint8_t get_time_left();

#endif