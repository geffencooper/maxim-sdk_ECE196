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
#include "IR_temp_sensor.h"

// Macros
#define DISPLAY_BB 1 // option to display bounding box to LCD
#define DISPLAY_FACE_STATUS 1 // option to display text to LCD ("Face Detected")
#define FACE_PRESENT 0
#define NO_FACE_PRESENT 1
#define LCD_TEXT_BUFF_SIZE 32
#define STATE_TEXT_X 90
#define STATE_TEXT_Y 10
#define STATE_TEXT_W 200
#define STATE_TEXT_H 20
#define CENTERING_THRESHOLD 10


// This struct defines the scanner state machine
// The only two members are the state and a function pointer to
// get the amount of time until the state expires
typedef struct 
{
    scan_state_t current_state;
    const uint8_t (*time_left) (void); // should not be changed
} scanner_state_machine_t;

typedef enum
{
    MOVING_LEFT = 0,
    MOVING_RIGHT,
    MOVING_DOWN,
    MOVING_UP
} positioning_state_t;


// Global variables
volatile scanner_state_machine_t ssm = {IDLE, get_state_time_left};
char lcd_text_buff[LCD_TEXT_BUFF_SIZE]; // buffer to hold the text to display to the LCD
area_t clear_state_text = {STATE_TEXT_X, STATE_TEXT_Y, STATE_TEXT_W, STATE_TEXT_H}; // a rectangle used to clear state text
area_t clear_info_text = {0, 240, 240, 20}; // a rectangle used to clear info text

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

    // display state text to LCD, we only want to update the state text once
    // when entering the state so we display it here
    TFT_Print(lcd_text_buff, 10, STATE_TEXT_Y, (int)&SansSerif19x19[0], sprintf(lcd_text_buff, "STATE:"));
    switch (state)
    {
        case SEARCH:
        {  
            MXC_TFT_FillRect(&clear_state_text, 4);
            TFT_Print(lcd_text_buff, STATE_TEXT_X, STATE_TEXT_Y, (int)&SansSerif16x16[0], sprintf(lcd_text_buff, "SEARCH"));
            break;
        }
        case POSITIONING:
        {
            MXC_TFT_FillRect(&clear_state_text, 4);
            TFT_Print(lcd_text_buff, STATE_TEXT_X, STATE_TEXT_Y, (int)&SansSerif16x16[0], sprintf(lcd_text_buff, "POSITIONING"));
            break;
        }
        case MEASUREMENT:
        {
            MXC_TFT_FillRect(&clear_state_text, 4);
            TFT_Print(lcd_text_buff, STATE_TEXT_X, STATE_TEXT_Y, (int)&SansSerif16x16[0], sprintf(lcd_text_buff, "MEASUREMENT"));
            break;
        }
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


// This is the ssm_action_fn for the IR sensor
// MEASUREMENT is the last state so it should reset after
static void measurement_complete()
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
    ret = init_state_timer(70, state_expired);
    if(ret < 0)
    {
        return -1;
    }
    init_IR_temp_sensor();

    printf("Scanner Initialized Successfully\n");

    set_state(IDLE);
    return 0;
}


// This is the main control loop
void execute_ssm()
{
    cnn_output_t* cnn_out;
    area_t ideal_top = {80, 90, 80, 1};
    area_t ideal_left = {80, 90, 1, 100};
    area_t ideal_bottom = {80, 190, 80, 1};
    area_t ideal_right = {160, 90, 1, 100};
    int diff_x = 0;
    int diff_y = 0;
    uint8_t is_centered = 0;
    static uint8_t unstable_frame_count = 0;
    static uint8_t unstable_x_count = 0;
    static uint8_t unstable_y_count = 0;
    positioning_state_t position;
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
               // printf("\033[0;0f");
                break;
            }
            case POSITIONING:
            {
                // variables to determine if position stable in x and y directions
                // first stabilize x then y
                static uint8_t x_stable = 0;
                static uint8_t y_stable = 0;
                
                //printf("STATE: positioning\ntime left: %i\n",ssm.time_left());
                // do a forward pass through the CNN and confirm a face is in the frame
                cnn_out = run_cnn(DISPLAY_FACE_STATUS, DISPLAY_BB);  
                if(cnn_out->face_status == NO_FACE_PRESENT)
                {
                    set_state(SEARCH);
                } 
                
                // if a face is present determine how far away the face is from the center
                diff_x = cnn_out->x - ideal_right.x;
                diff_y = cnn_out->y - ideal_top.y;
                is_centered = 0;

                //printf("x: %i\t", diff_x);
                // first check if x position stable
                if(diff_x < CENTERING_THRESHOLD && diff_x > -CENTERING_THRESHOLD)
                {
                   // printf("GOOD     \n");
                    // set x is stable
                    is_centered = 1;
                    x_stable = 1;
                    unstable_x_count = 0;
                }
                else
                {
                    // if x is unstable, increment the count
                    // only set to unstable if uncentered for multiple frames
                    // this filters out the noise of x measurements
                    unstable_x_count++;
                    if(unstable_x_count >= 2)
                    {
                        // reset the stabilization variables
                        is_centered = 0;
                        x_stable = 0;
                        // try to restabilize by first moving left
                        // only redisplay the text if not already moving left
                        if(diff_x > CENTERING_THRESHOLD && position != MOVING_LEFT)
                        {
                            position = MOVING_LEFT;
                            MXC_TFT_FillRect(&clear_info_text,BLACK);
                            TFT_Print(lcd_text_buff, 10, 240, (int)&SansSerif16x16[0], sprintf(lcd_text_buff, "MOVE LEFT"));
                        }
                        // try to restabilize by moving right
                        // only redisplay the text if not already moving right
                        else if(diff_x < -CENTERING_THRESHOLD && position != MOVING_RIGHT)
                        {
                            position = MOVING_RIGHT;
                            MXC_TFT_FillRect(&clear_info_text,BLACK);
                            TFT_Print(lcd_text_buff, 10, 240, (int)&SansSerif16x16[0], sprintf(lcd_text_buff, "MOVE RIGHT"));
                        }
                    }
                }
                //printf("y: %i\t", diff_y);
                // once x is stable then stabilize y
                if(x_stable)
                {
                    if(diff_y < CENTERING_THRESHOLD && diff_y > -CENTERING_THRESHOLD && x_stable)
                    {
                        //printf("GOOD      \n");
                        is_centered &= 1;
                        y_stable = 1;
                        unstable_y_count = 0;
                    }
                    else
                    {
                        unstable_y_count++;
                        if(unstable_y_count >= 2)
                        {
                            is_centered = 0;
                            y_stable = 0;
                            if(diff_y > CENTERING_THRESHOLD && position != MOVING_UP)
                            {
                                position = MOVING_UP;
                                MXC_TFT_FillRect(&clear_info_text,BLACK);
                                TFT_Print(lcd_text_buff, 10, 240, (int)&SansSerif16x16[0], sprintf(lcd_text_buff, "MOVE UP"));
                            }
                            else if(diff_y < -CENTERING_THRESHOLD && position != MOVING_DOWN)
                            {
                                position = MOVING_DOWN;
                                MXC_TFT_FillRect(&clear_info_text,BLACK);
                                TFT_Print(lcd_text_buff, 10, 240, (int)&SansSerif16x16[0], sprintf(lcd_text_buff, "MOVE DOWN"));
                            }
                        }
                    }
                }
                // printf("w: %i\n",cnn_out->w - ideal_bottom.w);
                // printf("h: %i\n",cnn_out->h - ideal_left.h);
                //printf("\033[0;0f");

                MXC_TFT_FillRect(&ideal_top, BLACK);
                MXC_TFT_FillRect(&ideal_bottom, BLACK);
                MXC_TFT_FillRect(&ideal_left, BLACK);
                MXC_TFT_FillRect(&ideal_right, BLACK);
                if(is_centered)
                {
                    MXC_TFT_FillCircle(120,140,3,GREEN);
                }
                else
                {
                    MXC_TFT_FillCircle(120,140,3,BLACK);
                }
                if(is_centered)
                {
                    set_state(MEASUREMENT);
                }
                break;
            }
            case MEASUREMENT:
            {
               // printf("STATE: measurement\ntime left: %i\n",ssm.time_left());
                cnn_out = run_cnn(DISPLAY_FACE_STATUS, DISPLAY_BB);  
                if(cnn_out->face_status == NO_FACE_PRESENT)
                {
                    set_state(SEARCH);
                } 
                
                diff_x = cnn_out->x - ideal_right.x;
                diff_y = cnn_out->y - ideal_top.y;

                //printf("x: %i\t", diff_x);
                if(diff_x < CENTERING_THRESHOLD && diff_x > -CENTERING_THRESHOLD)
                {
                    //printf("GOOD     \n");
                    is_centered = 1;
                }
                else
                {
                    is_centered = 0;
                }
               // printf("y: %i\t", diff_y);
                if(diff_y < CENTERING_THRESHOLD && diff_y > -CENTERING_THRESHOLD)
                {
                    //printf("GOOD      \n");
                    is_centered &= 1;
                }
                else
                {
                    is_centered = 0;
                }
                // printf("w: %i\n",cnn_out->w - ideal_bottom.w);
                // printf("h: %i\n",cnn_out->h - ideal_left.h);

                MXC_TFT_FillRect(&ideal_top, BLACK);
                MXC_TFT_FillRect(&ideal_bottom, BLACK);
                MXC_TFT_FillRect(&ideal_left, BLACK);
                MXC_TFT_FillRect(&ideal_right, BLACK);
                if(is_centered)
                {
                    unstable_frame_count = 0;
                    MXC_TFT_FillCircle(120,140,3,GREEN);
                    TFT_Print(lcd_text_buff, 0, 240, (int)&SansSerif16x16[0], sprintf(lcd_text_buff, "Temperature: %.1f", get_temp()));
                }
                else
                {
                    unstable_frame_count++;
                    if(unstable_frame_count >= 3)
                    {
                        set_state(POSITIONING);
                       // printf("\033[0;0f");
                        unstable_frame_count = 0;
                    }
                }
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


