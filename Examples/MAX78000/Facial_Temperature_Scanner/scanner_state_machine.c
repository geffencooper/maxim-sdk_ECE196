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
#include "scanner_state_machine_constants.h"


// ========================================================================================= //
// ================================== TYPE DEFINITIONS ===================================== //
// ========================================================================================= //


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
    MOVING_UP,
    CENTERED
} positioning_state_t;


// ========================================================================================= //
// ================================== GLOBAL VARIABLES ===================================== //
// ========================================================================================= //

// the single instance of the scanner state machine
volatile scanner_state_machine_t ssm = {IDLE, get_state_time_left};
char lcd_text_buff[LCD_TEXT_BUFF_SIZE]; // buffer to hold the text to display to the LCD

// rectangle structs for clearing text from the screen
area_t clear_state_text = {STATE_TEXT_X, STATE_TEXT_Y, STATE_TEXT_W, STATE_TEXT_H}; // displays state
area_t clear_state_text_desc = {STATE_TEXT_DESC_X, STATE_TEXT_DESC_Y, STATE_TEXT_DESC_W, STATE_TEXT_DESC_H}; // displays state description
area_t clear_info_text = {0, LCD_W, LCD_W, INFO_TEXT_H}; // a rectangle used to clear info text
area_t clear_idle_text = {0,0,IDLE_TEXT_W,IDLE_TEXT_H};
area_t clear_pos_text = {POS_TEXT_X,POS_TEXT_Y, POS_TEXT_W, POS_TEXT_H};

// time bar at the top of the screen the counts down as a state expires
area_t timer_bar = {0,0,LCD_W,TIME_BAR_H};

mxc_gpio_cfg_t pwr_switch_gpio;

// return value of CNN (bounding box values and face state)
cnn_output_t* cnn_out;

// rectangle the user needs to center their face in
area_t ideal_top = {IDEAL_X, IDEAL_Y, IDEAL_W, IDEAL_BB_LINE_W};
area_t ideal_left = {IDEAL_X, IDEAL_Y, IDEAL_BB_LINE_W, IDEAL_H};
area_t ideal_bottom = {IDEAL_X, IDEAL_Y+IDEAL_H, IDEAL_W, IDEAL_BB_LINE_W};
area_t ideal_right = {IDEAL_X+IDEAL_W, IDEAL_Y, IDEAL_BB_LINE_W, IDEAL_H};

// reference point for centering the bounding box   
int ideal_center_x = IDEAL_X + IDEAL_W/2;
int ideal_center_y = IDEAL_Y + IDEAL_H/2;
int diff_x = 0;
int diff_y = 0;

uint8_t is_centered = 0;

// to filter out noisy bounding box data, the user must be
// out of the center for multiple frames if they were centered
uint8_t unstable_frame_count = 0;
uint8_t unstable_x_count = 0;
uint8_t unstable_y_count = 0;

// keeps track of which position to stablize
positioning_state_t position;

// 'Look up table' for interpolating the temperature scale factor
float boxes[10] = {40, 50, 60, 70, 75, 85, 95, 100, 105, 120}; 
float temps[10] = {1.3798,1.3243, 1.3066, 1.2695, 1.2505, 1.2098, 1.1264,1.1136,1.0652,1.0432};
static uint8_t temp_taken = 0;


// ========================================================================================= //
// ================================ FUNCTION DEFINITIONS =================================== //
// ========================================================================================= //

// A private function for setting the state machine's state
// It also starts the timer for a given state.
static void set_state(scan_state_t state)
{
    // set the state
    ssm.current_state = state;

    // start the timer for non-idle states
    if(ssm.current_state != IDLE && ssm.current_state != RESET_STATE)
    {
        reset_state_timer();
        start_state_timer();

        // reset the countdown timer bar to full
        timer_bar.x = 0;
        timer_bar.w = LCD_W;
        MXC_TFT_FillRect(&timer_bar, WHITE);
    }

    // display state text to LCD, we only want to update the state text once
    // when entering the state so we display it here
    switch (state)
    {
        case SEARCH:
        {  
            // clear the state text
            MXC_TFT_FillRect(&clear_state_text, BLACK);
            MXC_TFT_FillRect(&clear_state_text_desc, BLACK);

            // display state description
            MXC_TFT_SetForeGroundColor(YELLOW);
            TFT_Print(lcd_text_buff, STATE_TEXT_DESC_X+47, STATE_TEXT_DESC_Y, (int)&Arial12x12[0], sprintf(lcd_text_buff, "MOTION DETECTED!"));
            
            // display state 
            MXC_TFT_SetForeGroundColor(WHITE);
            TFT_Print(lcd_text_buff, STATE_TEXT_X+28, STATE_TEXT_Y, (int)&Arial12x12[0], sprintf(lcd_text_buff, "SEARCHING FOR A FACE"));
            
            set_expiration_period(SEARCH_PERIOD);
            break;
        }
        case POSITIONING:
        {
            // clear the text
            MXC_TFT_FillRect(&clear_state_text, BLACK);
            MXC_TFT_FillRect(&clear_state_text_desc, BLACK);
           
            // display state description
            MXC_TFT_SetForeGroundColor(ORANGE);
            TFT_Print(lcd_text_buff, STATE_TEXT_DESC_X+55, STATE_TEXT_DESC_Y, (int)&Arial12x12[0], sprintf(lcd_text_buff, "FACE DETECTED!"));
           
            // display the state
            MXC_TFT_SetForeGroundColor(WHITE);
            TFT_Print(lcd_text_buff, STATE_TEXT_X+45, STATE_TEXT_Y, (int)&Arial12x12[0], sprintf(lcd_text_buff, "CENTER YOUR FACE"));
            
            set_expiration_period(POSITIONING_PERIOD);
            break;
        }
        case MEASUREMENT:
        {
            // clear the text
            MXC_TFT_FillRect(&clear_state_text, BLACK);
            MXC_TFT_FillRect(&clear_state_text_desc, BLACK);
           
            // display state description
            MXC_TFT_SetForeGroundColor(GREEN);
            TFT_Print(lcd_text_buff, STATE_TEXT_DESC_X+55, STATE_TEXT_DESC_Y, (int)&Arial12x12[0], sprintf(lcd_text_buff, "FACE CENTERED!"));
           
            // display the state
            MXC_TFT_SetForeGroundColor(WHITE);
            TFT_Print(lcd_text_buff, STATE_TEXT_X+25, STATE_TEXT_Y, (int)&Arial12x12[0], sprintf(lcd_text_buff, "MEASURING TEMPERATURE"));
            
            set_expiration_period(MEASUREMENT_PERIOD);
            break;
        }
        default :
            break;
    }
}


// ========================================================================================= //


// This is the ssm_action_fn for the motion sensor.
// It disables the motion interrupt, transitions states, and starts the CNN
static void motion_sensor_trigger()
{
    MXC_GPIO_DisableInt(MXC_GPIO2, MXC_GPIO_PIN_7);
    MXC_Delay(100000); // time for LED to flash before depowering sensor
    MXC_GPIO_OutSet(MXC_GPIO2, MXC_GPIO_PIN_3);
    set_state(SEARCH);
    startup_cnn();
}


// ========================================================================================= //


// This function is called after one 'cycle' of the state machine
// completes or a state expires
static void reset_ssm()
{
    reset_state_timer();

    // power on the motion sensor and its interrupt
    MXC_GPIO_OutClr(MXC_GPIO2, MXC_GPIO_PIN_3);
    MXC_Delay(100000);
    MXC_GPIO_EnableInt(MXC_GPIO2, MXC_GPIO_PIN_7);
    MXC_TFT_ClearScreen();
}


// ========================================================================================= //


// This is the ssm_action_fn for the timer expiration
// It simply activates the reset state
static void state_expired()
{
    set_state(RESET_STATE);
}


// ========================================================================================= //


// This is the ssm_action_fn for the IR sensor
// MEASUREMENT is the last state so it should reset after
static void measurement_complete()
{
    set_state(RESET_STATE);
}


// ========================================================================================= //


// This function initializes system parameters
static void init_system()
{
  // Enable cache
  MXC_ICC_Enable(MXC_ICC0);

  // Switch to 100 MHz clock
  MXC_SYS_Clock_Select(MXC_SYS_CLOCK_IPO);
  SystemCoreClockUpdate();
}


// ========================================================================================= //


// initializes a gpio which controls power to motion sensor
static void init_pwr_switch_gpio()
{
    pwr_switch_gpio.port = MXC_GPIO2;
    pwr_switch_gpio.mask = MXC_GPIO_PIN_3;
    pwr_switch_gpio.func = MXC_GPIO_FUNC_OUT;

    MXC_GPIO_Config(&pwr_switch_gpio);
    MXC_GPIO_OutClr(MXC_GPIO2, MXC_GPIO_PIN_3);
}


// ========================================================================================= //


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

    init_pwr_switch_gpio();
    MXC_GPIO_OutClr(MXC_GPIO2, MXC_GPIO_PIN_3);

    init_PIR_sensor(motion_sensor_trigger);

    ret = init_state_timer(SEARCH_PERIOD, state_expired);
    if(ret < 0)
    {
        return -1;
    }

    init_IR_temp_sensor();

    printf("Scanner Initialized Successfully\n");

    set_state(IDLE);
    return 0;
}


// ========================================================================================= //


// displays a new screen when temperature recorded
static void display_temperature(float temp)
{
    MXC_TFT_SetForeGroundColor(BLACK);
    if(temp < TEMP_LIMIT)
    {
        MXC_TFT_SetBackGroundColor(GREEN);
        TFT_Print(lcd_text_buff, 60, 150, (int)&Arial28x28[0], sprintf(lcd_text_buff, "%3.2f F",temp));
        TFT_Print(lcd_text_buff, 142, 150, (int)&Arial12x12[0], sprintf(lcd_text_buff, "o"));
        TFT_Print(lcd_text_buff, 75, 180, (int)&Arial28x28[0], sprintf(lcd_text_buff, "SAFE!"));
    }
    else
    {
        MXC_TFT_SetBackGroundColor(RED);
        TFT_Print(lcd_text_buff, 60, 150, (int)&Arial28x28[0], sprintf(lcd_text_buff, "%3.2f F",temp));
        TFT_Print(lcd_text_buff, 158, 150, (int)&Arial12x12[0], sprintf(lcd_text_buff, "o"));
        TFT_Print(lcd_text_buff, 40, 180, (int)&Arial28x28[0], sprintf(lcd_text_buff, "NOT SAFE!"));
    }
}


// ========================================================================================= //


// This is the main control loop
void execute_ssm()
{
    static uint8_t was_idle = 1;
    while(1)
    {
        switch (ssm.current_state)
        {
            // the idle state displays bouncing text to the screen
            case IDLE: 
            {
                static uint32_t count = 0;
                was_idle = 1;
                if(count == IDLE_TEXT_PERIOD)
                {
                    count = 0;
                    // text direction and position
                    static int x_dir = 1;
                    static int y_dir = 1;
                    static int idle_x = 0;
                    static int idle_y = 0;

                    // clear the text last position, display the next position
                    MXC_TFT_FillRect(&clear_idle_text, BLACK);
                    TFT_Print(lcd_text_buff, idle_x, idle_y, (int)&Arial12x12[0], sprintf(lcd_text_buff, "IDLE"));
                    
                    // update the position to clear
                    clear_idle_text.x = idle_x;
                    clear_idle_text.y = idle_y;

                    // increment the position
                    idle_x+=x_dir;
                    idle_y+=y_dir;

                    // check LCD boundaries, then switch directions
                    if(idle_x+40 > LCD_W)
                    {
                        x_dir = -1;
                        idle_x+=x_dir;
                        MXC_TFT_SetForeGroundColor(YELLOW);
                    }
                    if(idle_y + 20 > LCD_H)
                    {
                        y_dir = -1;
                        idle_y+=y_dir;
                        MXC_TFT_SetForeGroundColor(RED);
                    }
                    if(idle_x < 0)
                    {
                        x_dir = 1;
                        idle_x+=x_dir;
                        MXC_TFT_SetForeGroundColor(GREEN);
                    }
                    if(idle_y < 0)
                    {
                        y_dir = 1;
                        idle_y+=y_dir;
                        MXC_TFT_SetForeGroundColor(WHITE);
                    }
                }
                count++;
                break;
            }
            // the search state looks for a face
            case SEARCH:
            {
                if(was_idle)
                {
                    // clear the idle text
                    MXC_TFT_FillRect(&clear_idle_text, BLACK);
                    MXC_TFT_FillRect(&clear_pos_text, BLACK);
                    was_idle = 0;
                }
                MXC_TFT_FillRect(&clear_info_text,BLACK);

                // update the timer countdown bar
                timer_bar.x = ((LCD_W/get_expiration_period())*ssm.time_left());
                timer_bar.w = (LCD_W/get_expiration_period());
                MXC_TFT_FillRect(&timer_bar, BLACK);

                // do a forward pass through the CNN
                cnn_out = run_cnn(DISPLAY_FACE_STATUS, NULL);
                if(cnn_out->face_status == FACE_PRESENT)
                {
                    set_state(POSITIONING);
                }
                break;
            }
            // the positioning state helps the user center their face
            case POSITIONING:
            {
                // update the timer countdown bar
                timer_bar.x = ((240/get_expiration_period())*ssm.time_left());
                timer_bar.w = (240/get_expiration_period());
                MXC_TFT_FillRect(&timer_bar, BLACK);

                // variables to determine if face position stable in x and y directions
                // first stabilize x then y
                static uint8_t x_stable = 0;
                static uint8_t y_stable = 0;
                
                // do a forward pass through the CNN and confirm a face is in the frame
                cnn_out = run_cnn(DISPLAY_FACE_STATUS, DISPLAY_BB);  
                if(cnn_out->face_status == NO_FACE_PRESENT)
                {
                    set_state(SEARCH);
                } 
                
                // if a face is present determine how far away the face is from the center
                diff_x = cnn_out->x-(cnn_out->w)/2 - ideal_center_x;
                diff_y = cnn_out->y+(cnn_out->h)/2 - ideal_center_y;
                is_centered = 0;

                // first check if x position stable
                if(diff_x < CENTERING_THRESHOLD && diff_x > -CENTERING_THRESHOLD)
                {
                    // set x to stable
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
                            TFT_Print(lcd_text_buff, 60, 240, (int)&SansSerif16x16[0], sprintf(lcd_text_buff, "MOVE LEFT"));
                        }
                        // try to restabilize by moving right
                        // only redisplay the text if not already moving right
                        else if(diff_x < -CENTERING_THRESHOLD && position != MOVING_RIGHT)
                        {
                            position = MOVING_RIGHT;
                            MXC_TFT_FillRect(&clear_info_text,BLACK);
                            TFT_Print(lcd_text_buff, 60, 240, (int)&SansSerif16x16[0], sprintf(lcd_text_buff, "MOVE RIGHT"));
                        }
                    }
                }
                // once x is stable then stabilize y
                if(x_stable)
                {
                    // check if y is stable
                    if(diff_y < CENTERING_THRESHOLD && diff_y > -CENTERING_THRESHOLD)
                    {
                        // set y to stable
                        is_centered &= 1; // doesn't do anything but both x and y should be stable to be centered
                        y_stable = 1;
                        unstable_y_count = 0;
                    }
                    else
                    {
                        // if y is unstable, increment the count
                        // only set to unstable if uncentered for multiple frames
                        // this filters out the noise of y measurements
                        unstable_y_count++;
                        if(unstable_y_count >= 2)
                        {
                            is_centered = 0;
                            y_stable = 0;
                            if(diff_y > CENTERING_THRESHOLD && position != MOVING_UP)
                            {
                                position = MOVING_UP;
                                MXC_TFT_FillRect(&clear_info_text,BLACK);
                                TFT_Print(lcd_text_buff, 60, 240, (int)&SansSerif16x16[0], sprintf(lcd_text_buff, "MOVE UP"));
                            }
                            else if(diff_y < -CENTERING_THRESHOLD && position != MOVING_DOWN)
                            {
                                position = MOVING_DOWN;
                                MXC_TFT_FillRect(&clear_info_text,BLACK);
                                TFT_Print(lcd_text_buff, 60, 240, (int)&SansSerif16x16[0], sprintf(lcd_text_buff, "MOVE DOWN"));
                            }
                        }
                    }
                }

                // now display the ideal rectangle the user needs to center their face in
                MXC_TFT_FillRect(&ideal_top, BLACK);
                MXC_TFT_FillRect(&ideal_bottom, BLACK);
                MXC_TFT_FillRect(&ideal_left, BLACK);
                MXC_TFT_FillRect(&ideal_right, BLACK);
                if(is_centered)
                {
                    // make the center circle green to notify the user that they are centered
                    MXC_TFT_FillCircle(120,140,3,GREEN);
                    set_state(MEASUREMENT);
                }
                else
                {
                    MXC_TFT_FillCircle(120,140,3,BLACK);
                }
                break;
            }
            // the measurement state finds a running average of the box area
            // uses interpolation to determine a factor to multiply the temperature by
            // We do this because the temperature sensor has a wide field of view and we 
            // want to generate a rough estimate regardless of how far the user is standing
            // This also prevents a person from purposely standing far to 'trick' the system
            case MEASUREMENT:
            {
                // once a temperature is taken we don't need to go through the following process
                if(!temp_taken)
                {
                    // update the timer countdown bar
                    timer_bar.x = ((240/3)*(ssm.time_left()-5));
                    timer_bar.w = (240/3);
                    MXC_TFT_FillRect(&timer_bar, BLACK);

                    // update the position to centered
                    position = CENTERED;

                    // do a forward pass through the network
                    cnn_out = run_cnn(DISPLAY_FACE_STATUS, DISPLAY_BB);  
                    if(cnn_out->face_status == NO_FACE_PRESENT)
                    {
                        set_state(SEARCH);
                    } 
                    
                    // find the distance to the center
                    diff_x = cnn_out->x-(cnn_out->w)/2 - ideal_center_x;
                    diff_y = cnn_out->y+(cnn_out->h)/2 - ideal_center_y;

                    // make sure we are centered
                    if(diff_x < CENTERING_THRESHOLD && diff_x > -CENTERING_THRESHOLD)
                    {
                        is_centered = 1;
                    }
                    else
                    {
                        is_centered = 0;
                    }
                    if(diff_y < CENTERING_THRESHOLD && diff_y > -CENTERING_THRESHOLD)
                    {
                        is_centered &= 1;
                    }
                    else
                    {
                        is_centered = 0;
                    }

                    // display the ideal bounding box the user needs to center their face in
                    MXC_TFT_FillRect(&ideal_top, BLACK);
                    MXC_TFT_FillRect(&ideal_bottom, BLACK);
                    MXC_TFT_FillRect(&ideal_left, BLACK);
                    MXC_TFT_FillRect(&ideal_right, BLACK);

                    // if we are still centered then we can move on
                    if(is_centered)
                    {
                        static int frame_count = 0;
                        static int last_area = 0;
                        static int current_area = 0;
                        static int running_avg_iir = 0;

                        // get the current area and divide by 100 for conveniance
                        current_area = (cnn_out->w * cnn_out->h)/100;

                        // only start the running average after the first area is captured
                        if(last_area > 0)
                        {
                            // y[n] = alpha*x[n] + (1-alpha)*y[n-1]
                            running_avg_iir = current_area/6 + (5*last_area)/6; 
                            last_area = running_avg_iir;
                        }
                        else
                        {
                            last_area = current_area;
                        }

                        // count the number of frames for the running average
                        frame_count++;
                        unstable_frame_count = 0; // we are stable so reset this variable

                        // continue to display center dot
                        MXC_TFT_FillCircle(120,140,3,GREEN);

                        // the user is centered so tell them to keep still so can get a good reading
                        MXC_TFT_FillRect(&clear_pos_text, BLACK);
                        TFT_Print(lcd_text_buff, 60, 240, (int)&SansSerif16x16[0], sprintf(lcd_text_buff, "HOLD STILL! %i", ssm.time_left()-5));
                        
                        // after 5 frames the running average is good enough to continue
                        if(frame_count == 5)
                        {
                            // find the closest box area in the 'look up table'
                            int i;
                            for(i = 0; i < 10; i++)
                            {
                                if(boxes[i] > running_avg_iir)
                                {
                                    break;
                                }
                            }

                            // derive the scaling factor using interpolation
                            float factor = (
                                temps[i]*(boxes[i]-(float)running_avg_iir) \
                                + temps[i-1]*((float)running_avg_iir-boxes[i-1])) \
                                /(boxes[i]-boxes[i-1]);

                            // display the estimated temperature
                            //TFT_Print(lcd_text_buff, 0, 270, (int)&SansSerif16x16[0], sprintf(lcd_text_buff, "Temp: %.1f-->%i  ", get_temp()*factor, running_avg_iir));
                            frame_count = 0; // reset the frame count

                            // a stable temperature has been recorded so move on to the display page
                            if(ssm.time_left() < 6)
                            {
                                temp_taken = 1;
                                display_temperature(get_temp()*factor);
                            }
                        }
                    }
                    // if the user is not centered for multiple frames, move back to the positioning state
                    // This filters out the noise of the bounding box measurements because otherwise we
                    // jump between states too easily when we get one bad bounding box measurement
                    else
                    {
                        unstable_frame_count++;
                        if(unstable_frame_count >= 3)
                        {
                            set_state(POSITIONING);
                            unstable_frame_count = 0;
                        }
                    }
                }
                break;
            }
            case RESET_STATE:
            {
                reset_ssm();
                set_state(IDLE);
                temp_taken = 0;
                MXC_TFT_SetBackGroundColor(BLACK);
                break;
            }
            default:
            {
                break;
            }
        }
    }
}


