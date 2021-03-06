// This file defines helper functions for
// interacting with the timer used by the state machine

#ifndef TIMER_FUNCS_H
#define TIMER_FUNCS_H

#include "ssm_action.h"

/*
    Description: This function sets up a continuous timer using the Maxim
                 integrated drivers. When the timer expires (every 1 sec) an
                 interrupt will be generated. This interrupt can
                 interact with the state machine by calling an
                 "scanner state machine action function" which
                 is passed in as a function pointer. Once the timer expires, 
                 the start function needs to be called again to reenable it.

    Parameters: The expiration period in seconds (ex: 5 means an interrupt
                is generated after 5 seconds). A function pointer to a state
                machine action

    Return: -1 if fails, 0 if successful
*/
int init_state_timer(int expiration_period, ssm_action_fn ssm_action);


/*
    Description: This function can be used to get the amount of time
                 left in seconds before the state expires.

    Parameters: none

    Return: time in seconds until the state expires
*/
int get_state_time_left();


/*
    Description: This function resets the count to 0 and disables th timer

    Parameters: none

    Return: none
*/
void reset_state_timer();


/*
    Description: This function starts the timer and should be called
                 after init or after reset.

    Parameters: none

    Return: none
*/
void start_state_timer();


/*
    Description: This function disables (pauses) the timer but does not reset the count.

    Parameters: none

    Return: none
*/
void stop_state_timer();

#endif