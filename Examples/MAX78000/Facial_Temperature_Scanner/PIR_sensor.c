#include "PIR_sensor.h"
#include "mxc_device.h"
#include <stdlib.h>
#include "ssm_action.h"
#include <stdio.h>

// ========================================================================================= //
// =================================== GLOBAL VARIABLES ==================================== //
// ========================================================================================= //
mxc_gpio_cfg_t motion_gpio;


// ========================================================================================= //
// ================================ FUNCTION DEFINITIONS =================================== //
// ========================================================================================= //

// This is the callback function (interrupt handler)
// that gets called when the motion sensor is triggered.
// This interrupt will be disabled when the state machine
// leaves the IDLE state and will only be reenabled when 
// it returns to an IDLE state. As mentioned in the header file,
// this callback function receives a function pointer which is a
// "state machine action". This action triggers a transition in
// the state machine.
void motion_isr(void* ssm_action)
{
    // cast the callback data to the state machine action
    ssm_action_fn action = ssm_action;
    action();
}

// GPIO port 2 pin 7 is available on the board
void init_PIR_sensor(ssm_action_fn ssm_action)
{
    motion_gpio.port = MXC_GPIO2;
    motion_gpio.mask = MXC_GPIO_PIN_7;
    motion_gpio.pad = MXC_GPIO_PAD_PULL_DOWN; // GND by default
    motion_gpio.func = MXC_GPIO_FUNC_IN;

    MXC_GPIO_Config(&motion_gpio);
    MXC_GPIO_RegisterCallback(&motion_gpio, motion_isr, (void*)ssm_action);
    MXC_GPIO_IntConfig(&motion_gpio, MXC_GPIO_INT_RISING);
    MXC_GPIO_EnableInt(MXC_GPIO2, MXC_GPIO_PIN_7);
    NVIC_EnableIRQ(GPIO2_IRQn);
}
