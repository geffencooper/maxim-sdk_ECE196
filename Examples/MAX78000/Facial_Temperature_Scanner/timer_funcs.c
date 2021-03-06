/***** Includes *****/
#include <stdint.h>
#include "mxc.h"
#include "mxc.h"
#include "mxc_device.h"
#include "mxc_sys.h"
#include "nvic_table.h"
#include "gcr_regs.h"
#include "timer_funcs.h"
#include "ssm_action.h"

// Global variables
mxc_tmr_cfg_t tmr;
ssm_action_fn expiration_action;
volatile int timer_period = 0;
volatile int current_seconds_count = 0;

void expiration_handler()
{
    // Clear interrupt
    MXC_TMR_ClearFlags(MXC_TMR1);
    
    // increments a second
    current_seconds_count += 1;

    // check if expired
    if(current_seconds_count >= timer_period)
    {
        current_seconds_count = 0;
        stop_state_timer();
        expiration_action();
    }
}

int init_state_timer(int expiration_period, ssm_action_fn ssm_action)
{
    // init timer variables
    timer_period = expiration_period;
    expiration_action = ssm_action;

    // setup the interrupt for timer 0
    NVIC_SetVector(TMR1_IRQn, expiration_handler);
    NVIC_EnableIRQ(TMR1_IRQn);

    // init timer 0 to interrupt every 1s (32KHz clock with prescaler 32 and count compare 1024)
    MXC_TMR_Shutdown(MXC_TMR1);
    tmr.pres = TMR_PRES_32;
    tmr.mode = TMR_MODE_CONTINUOUS;
    tmr.bitMode = TMR_BIT_MODE_32;
    tmr.clock = MXC_TMR_32K_CLK;
    tmr.cmp_cnt = 1024;
    tmr.pol = 0;
    
    // init the timer
    if (MXC_TMR_Init(MXC_TMR1, &tmr, true) != E_NO_ERROR) 
    {
        printf("Failed one-shot timer Initialization.\n");
        return -1;
    }
    
    // enable the interrupt
    MXC_TMR_EnableInt(MXC_TMR1);

    printf("State timer initialized.\n\n");
    return 0;
}

int get_state_time_left()
{
    return (timer_period - current_seconds_count);
}

void reset_state_timer()
{
    MXC_TMR_Stop(MXC_TMR1);
    current_seconds_count = 0;
    MXC_TMR1->cnt = 1; // this is the reset value for the timer count
}

void start_state_timer()
{
    printf("timer started\n");
    MXC_TMR_Start(MXC_TMR1);
}

void stop_state_timer()
{
    MXC_TMR_Stop(MXC_TMR1);
}
