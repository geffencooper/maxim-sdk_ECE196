// standard C header files
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// Maxim Integrated Drivers
#include "mxc_delay.h"
#include "mxc_device.h"
#include "mxc_sys.h"
#include "bbfc_regs.h"
#include "fcr_regs.h"
#include "icc.h"
#include "pb.h"
#include "cnn.h"
#include "tft_fthr.h"

// Geffen's Header Files
#include "camera_funcs.h"
#include "scanner_state_machine.h"
#include "HiLetgo_ILI9341.h"
#include "PIR_sensor.h"
#include "cnn_helper_funcs.h"

// Macros
#define DISPLAY_BB 1 // option to display bounding box to LCD
#define DISPLAY_FACE_STATUS 1 // option to display text to LCD ("Face Detected")
#define FACE_PRESENT 0
#define NO_FACE_PRESENT 1


// This struct defines the scanner state machine
// The only two members are the state and a variable to
// store the amount of time until the state expires
typedef struct 
{
    scan_state_t current_state;
    uint8_t time_left;
} scanner_state_machine_t;


// Global variables
volatile scanner_state_machine_t ssm;


// A private functio  for setting the state machine's state
// It also starts the timer for a given state.
static void set_state(scan_state_t state)
{
    ssm.current_state = state;
    // start the timer
}


// This is the ssm_action_fn for the motion sensor.
// It disables the interrupt, transitions states, and starts the CNN
static void motion_sensor_trigger()
{
    MXC_GPIO_DisableInt(MXC_GPIO2, MXC_GPIO_PIN_7);
    set_state(SEARCH);
    startup_cnn();
}

// This function initializes system parameters
static void init_system()
{
  // Enable cache
  MXC_ICC_Enable(MXC_ICC0);

  // Switch to 100 MHz clock
  MXC_SYS_Clock_Select(MXC_SYS_CLOCK_IPO);
  SystemCoreClockUpdate();
}


// this function initializes peripherals used by the state machine
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

    printf("Scanner Initialized Successfully\n");

    set_state(IDLE);
    return 0;
}


// This is the main control loop
void execute_ssm()
{
    cnn_output_t* cnn_out;
    while(1)
    {
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


