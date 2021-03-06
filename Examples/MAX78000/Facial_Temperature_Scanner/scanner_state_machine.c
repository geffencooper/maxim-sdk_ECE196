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
#include "timer_funcs.h"

// Macros
#define DISPLAY_BB 1 // option to display bounding box to LCD
#define DISPLAY_FACE_STATUS 1 // option to display text to LCD ("Face Detected")
#define FACE_PRESENT 0
#define NO_FACE_PRESENT 1
#define CENTER_X 80
#define CENTER_Y 80


// This struct defines the scanner state machine
// The only two members are the state and a function pointer to
// get the amount of time until the state expires
typedef struct 
{
    scan_state_t current_state;
    const uint8_t (*time_left) (void); // should not be changed
} scanner_state_machine_t;


// Global variables
volatile scanner_state_machine_t ssm = {IDLE, get_state_time_left};


// A private function for setting the state machine's state
// It also starts the timer for a given state.
static void set_state(scan_state_t state)
{
    // set the state
    ssm.current_state = state;

    // start the timer for non-idle states
    if(state != IDLE && state != RESET_STATE)
    {
        reset_state_timer();
        start_state_timer();
    }
}


// This is the ssm_action_fn for the motion sensor.
// It disables the motion interrupt, transitions states, and starts the CNN
static void motion_sensor_trigger()
{
    MXC_GPIO_DisableInt(MXC_GPIO2, MXC_GPIO_PIN_7);
    set_state(SEARCH);
    startup_cnn();
}

// This function is called after one 'cycle' of the state machine
// completes or a state expires
static void reset_ssm()
{
    reset_state_timer();
    MXC_GPIO_EnableInt(MXC_GPIO2, MXC_GPIO_PIN_7);
    MXC_TFT_ClearScreen();
}


// This is the ssm_action_fn for the timer expiration
// It simply activates the reset state
static void state_expired()
{
    set_state(RESET_STATE);
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
    ret = init_state_timer(7, state_expired);
    if(ret < 0)
    {
        return -1;
    }
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
                break;
            }
            case SEARCH:
            {
                //printf("STATE: search\ntime left: %i\n",ssm.time_left());
                cnn_out = run_cnn(DISPLAY_FACE_STATUS, NULL);
                if(cnn_out->face_status == FACE_PRESENT)
                {
                    set_state(POSITIONING);
                }
                break;
            }
            case POSITIONING:
            {
                //printf("STATE: positioning\ntime left: %i\n",ssm.time_left());
                cnn_out = run_cnn(DISPLAY_FACE_STATUS, DISPLAY_BB);  
                if(cnn_out->face_status == NO_FACE_PRESENT)
                {
                    set_state(SEARCH);
                } 
                static area_t ideal_top = {80, 230, 80, 1};
                static area_t ideal_left = {160, 130, 1, 100};
                static area_t ideal_bottom = {80, 130, 80, 1};
                static area_t ideal_right = {80, 130, 1, 100};

                int diff_x = cnn_out->x - ideal_top.x;
                int diff_y = cnn_out->y - ideal_top.y;
                uint8_t is_good = 0;

                printf("x: %i\t", diff_x);
                if(diff_x < 15 && diff_x > -15)
                {
                    printf("GOOD     \n");
                    is_good = 1;
                }
                else
                {
                    is_good = 0;
                    if(diff_x > 15)
                    {
                        printf("MOVE RIGHT\n");
                    }
                    else
                    {
                        printf("MOVE LEFT\n");
                    }
                }
                printf("y: %i\t", diff_y);
                if(diff_y < 15 && diff_y > -15)
                {
                    printf("GOOD      \n");
                    is_good &= 1;
                }
                else
                {
                    is_good = 0;
                    if(diff_y > 15)
                    {
                        printf("MOVE DOWN\n");
                    }
                    else
                    {
                        printf("MOVE UP\n");
                    }
                }
                // printf("w: %i\n",cnn_out->w - ideal_bottom.w);
                // printf("h: %i\n",cnn_out->h - ideal_left.h);
                printf("\033[0;0f");

                MXC_TFT_FillRect(&ideal_top, BLACK);
                MXC_TFT_FillRect(&ideal_bottom, BLACK);
                MXC_TFT_FillRect(&ideal_left, BLACK);
                MXC_TFT_FillRect(&ideal_right, BLACK);
                if(is_good)
                {
                    MXC_TFT_FillCircle(120,180,3,GREEN);
                }
                else
                {
                    MXC_TFT_FillCircle(120,180,3,BLACK);
                }
                break;
            }
            case MEASUREMENT:
            {
                break;
            }
            case RESET_STATE:
            {
                reset_ssm();
                set_state(IDLE);
                break;
            }
            default:
            {
                break;
            }
        }
    }
}


