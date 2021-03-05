#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "mxc_delay.h"
#include "mxc_device.h"
#include "mxc_sys.h"
#include "bbfc_regs.h"
#include "fcr_regs.h"
#include "icc.h"
#include "pb.h"
#include "cnn.h"
#include "tft_fthr.h"

#include "camera_funcs.h"
#include "scanner_state_machine.h"
#include "HiLetgo_ILI9341.h"
#include "PIR_sensor.h"
#include "cnn_helper_funcs.h"

#define DISPLAY_BB 1
#define DISPLAY_FACE_STATUS 1
#define FACE_PRESENT 0
#define NO_FACE_PRESENT 1

// this struct defines the state machine
typedef struct 
{
    scan_state_t current_state;
    uint8_t time_left;
} scanner_state_machine_t;

volatile uint8_t state = 0;
volatile scanner_state_machine_t ssm;

static void set_state(scan_state_t state)
{
    ssm.current_state = state;
    // start the timer
}

static void motion_sensor_trigger()
{
    //NVIC_DisableIRQ(GPIO2_IRQn);
    MXC_GPIO_DisableInt(MXC_GPIO2, MXC_GPIO_PIN_7);
    set_state(SEARCH);
    startup_cnn();
}

static void init_system()
{
  // Enable cache
  MXC_ICC_Enable(MXC_ICC0);

  // Switch to 100 MHz clock
  MXC_SYS_Clock_Select(MXC_SYS_CLOCK_IPO);
  SystemCoreClockUpdate();
}

int init_ssm()
{
    // microcontroller initialization
    init_system();

    // initialize the peripherals
    int ret = init_camera_sensor(160,160);
    if(ret < 0)
    {
        return -1;
    }
    init_ILI_LCD();
    init_PIR_sensor(motion_sensor_trigger);
    // init temp sensor

    //printf("Scanner Initialized Successfully\n");
    //printf("Scanner State: idle\n");

    set_state(IDLE);
    return 0;
}

void execute_ssm()
{
    cnn_output_t* cnn_out;
    while(1)
    {
        // printf("state: %i\n", ssm.current_state);
        switch (ssm.current_state)
        {
            case IDLE:
            {
                // do nothing
                break;
            }
            case SEARCH:
            {
                printf("STATE: search\n");
                cnn_out = run_cnn(DISPLAY_FACE_STATUS, NULL);
                if(cnn_out->face_status == FACE_PRESENT)
                {
                    set_state(POSITIONING);
                }
                break;
            }
            case POSITIONING:
            {
                printf("STATE: positioning\n");
                cnn_out = run_cnn(DISPLAY_FACE_STATUS, DISPLAY_BB);  
                if(cnn_out->face_status == NO_FACE_PRESENT)
                {
                    set_state(SEARCH);
                } 
                break;
            }
            case MEASUREMENT:
            {
                break;
            }
            default:
            {
                break;
            }
        }
    }
}

static void reset_ssm()
{

}


