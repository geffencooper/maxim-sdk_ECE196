// This file defines the initialization function for the PIR motion sensor

#ifndef PIR_SENSOR_H
#define PIR_SENSOR_H

#include "ssm_action.h"
/*
    Description: This function sets up the GPIO for the PIR motion sensor

    Parameters: The input is a function pointer that gets called when
                a motion sensor interrupt is triggered. This function pointer
                should be of type ssm_action_fn as shown in ssm_action.h. This
                function will cause a transition in the state machine and
                prevents the motion sensor from directly having access to
                the state machine's data

    Return: none
*/
void init_PIR_sensor(ssm_action_fn ssm_action);

#endif