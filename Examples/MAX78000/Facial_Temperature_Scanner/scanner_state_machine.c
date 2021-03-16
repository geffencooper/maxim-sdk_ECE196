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
#define STATE_TEXT_X 0
#define STATE_TEXT_Y 25
#define STATE_TEXT_W 240
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
    MOVING_UP,
    CENTERED
} positioning_state_t;


// Global variables
volatile scanner_state_machine_t ssm = {IDLE, get_state_time_left};
char lcd_text_buff[LCD_TEXT_BUFF_SIZE]; // buffer to hold the text to display to the LCD
area_t clear_state_text = {STATE_TEXT_X, STATE_TEXT_Y+10, STATE_TEXT_W, STATE_TEXT_H}; // a rectangle used to clear state text
area_t clear_state_text2 = {STATE_TEXT_X, STATE_TEXT_Y-10, STATE_TEXT_W, STATE_TEXT_H}; // a rectangle used to clear state text
area_t clear_info_text = {0, 240, 240, 20}; // a rectangle used to clear info text
area_t timer_bar = {0,0,240,2};
area_t clear_idle_text = {0,0,35,20};
area_t clear_pos_text = {60,240, 50, 160};
mxc_gpio_cfg_t pwr_switch_gpio;

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
        timer_bar.x = 0;
        timer_bar.w = 240;
        MXC_TFT_FillRect(&timer_bar, GREEN);
    }

    // display state text to LCD, we only want to update the state text once
    // when entering the state so we display it here
    switch (state)
    {
        case SEARCH:
        {  
            MXC_TFT_FillRect(&clear_state_text, 4);
            MXC_TFT_FillRect(&clear_state_text2, 4);
            MXC_TFT_SetForeGroundColor(YELLOW);
            TFT_Print(lcd_text_buff, STATE_TEXT_X+47, STATE_TEXT_Y-10, (int)&Arial12x12[0], sprintf(lcd_text_buff, "MOTION DETECTED!"));
            MXC_TFT_SetForeGroundColor(WHITE);
            TFT_Print(lcd_text_buff, STATE_TEXT_X+28, STATE_TEXT_Y+10, (int)&Arial12x12[0], sprintf(lcd_text_buff, "SEARCHING FOR A FACE"));
            set_expiration_period(5);
            break;
        }
        case POSITIONING:
        {
            MXC_TFT_FillRect(&clear_state_text, 4);
            MXC_TFT_FillRect(&clear_state_text2, 4);
            MXC_TFT_SetForeGroundColor(YELLOW);
            TFT_Print(lcd_text_buff, STATE_TEXT_X+55, STATE_TEXT_Y-10, (int)&Arial12x12[0], sprintf(lcd_text_buff, "FACE DETECTED!"));
            MXC_TFT_SetForeGroundColor(WHITE);
            TFT_Print(lcd_text_buff, STATE_TEXT_X+45, STATE_TEXT_Y+10, (int)&Arial12x12[0], sprintf(lcd_text_buff, "CENTER YOUR FACE"));
            set_expiration_period(10);
            break;
        }
        case MEASUREMENT:
        {
            MXC_TFT_FillRect(&clear_state_text, 4);
            MXC_TFT_FillRect(&clear_state_text2, 4);
            MXC_TFT_SetForeGroundColor(YELLOW);
            TFT_Print(lcd_text_buff, STATE_TEXT_X+55, STATE_TEXT_Y-10, (int)&Arial12x12[0], sprintf(lcd_text_buff, "FACE CENTERED!"));
            MXC_TFT_SetForeGroundColor(WHITE);
            TFT_Print(lcd_text_buff, STATE_TEXT_X+25, STATE_TEXT_Y+10, (int)&Arial12x12[0], sprintf(lcd_text_buff, "MEASURING TEMPERATURE"));
            set_expiration_period(10);
            break;
        }
        default :
            break;
    }
}


// This is the ssm_action_fn for the motion sensor.
// It disables the motion interrupt, transitions states, and starts the CNN
static void motion_sensor_trigger()
{
    MXC_TFT_ClearScreen();
    MXC_GPIO_DisableInt(MXC_GPIO2, MXC_GPIO_PIN_7);
    MXC_Delay(100000);
    MXC_GPIO_OutSet(MXC_GPIO2, MXC_GPIO_PIN_3);
    set_state(SEARCH);
    startup_cnn();
}

// This function is called after one 'cycle' of the state machine
// completes or a state expires
static void reset_ssm()
{
    reset_state_timer();
    MXC_GPIO_OutClr(MXC_GPIO2, MXC_GPIO_PIN_3);
    MXC_Delay(100000);
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


// initializes a gpio which controls power to motion sensor
static void init_pwr_switch_gpio()
{
    pwr_switch_gpio.port = MXC_GPIO2;
    pwr_switch_gpio.mask = MXC_GPIO_PIN_3;
    pwr_switch_gpio.func = MXC_GPIO_FUNC_OUT;

    MXC_GPIO_Config(&pwr_switch_gpio);
    MXC_GPIO_OutClr(MXC_GPIO2, MXC_GPIO_PIN_3);
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
    init_pwr_switch_gpio();
    MXC_GPIO_OutClr(MXC_GPIO2, MXC_GPIO_PIN_3);
    init_PIR_sensor(motion_sensor_trigger);
    ret = init_state_timer(7, state_expired);
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
    int ideal_center_x = 120;
    int ideal_center_y = 140;
    int diff_x = 0;
    int diff_y = 0;
    uint8_t is_centered = 0;
    static uint8_t unstable_frame_count = 0;
    static uint8_t unstable_x_count = 0;
    static uint8_t unstable_y_count = 0;
    positioning_state_t position;
    float boxes[5] = {80, 100, 110, 120, 130}; 
    float temps[5] = {1.2564, 1.2099, 1.1264, 1.0652, 1.0316};
    while(1)
    {
        switch (ssm.current_state)
        {
            case IDLE: 
            {
                static uint32_t count = 0;
                if(count == 100000)
                {
                    count = 0;
                    static int x_dir = 1;
                    static int y_dir = 1;
                    static int idle_x = 0;
                    static int idle_y = 0;
                    MXC_TFT_FillRect(&clear_idle_text, BLACK);
                    TFT_Print(lcd_text_buff, idle_x, idle_y, (int)&Arial12x12[0], sprintf(lcd_text_buff, "IDLE"));
                    clear_idle_text.x = idle_x;
                    clear_idle_text.y = idle_y;
                    idle_x+=x_dir;
                    idle_y+=y_dir;
                    if(idle_x+40 > 240)
                    {
                        x_dir = -1;
                        idle_x+=x_dir;
                        MXC_TFT_SetForeGroundColor(YELLOW);
                    }
                    if(idle_y + 20 > 320)
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
            case SEARCH:
            {
                MXC_TFT_FillRect(&clear_pos_text, BLACK);
                printf("STATE: search\ntime left: %i\n",ssm.time_left());
                timer_bar.x = ((240/get_expiration_period())*ssm.time_left());
                timer_bar.w = (240/get_expiration_period());
                MXC_TFT_FillRect(&timer_bar, BLACK);
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
                timer_bar.x = ((240/get_expiration_period())*ssm.time_left());
                timer_bar.w = (240/get_expiration_period());
                MXC_TFT_FillRect(&timer_bar, BLACK);
                // variables to determine if position stable in x and y directions
                // first stabilize x then y
                static uint8_t x_stable = 0;
                static uint8_t y_stable = 0;
                
                printf("STATE: positioning\ntime left: %i\n",ssm.time_left());
                // do a forward pass through the CNN and confirm a face is in the frame
                cnn_out = run_cnn(DISPLAY_FACE_STATUS, DISPLAY_BB);  
                if(cnn_out->face_status == NO_FACE_PRESENT)
                {
                    set_state(SEARCH);
                } 
                
                // if a face is present determine how far away the face is from the center
                // diff_x = cnn_out->x - ideal_right.x;
                // diff_y = cnn_out->y - ideal_top.y;
                diff_x = cnn_out->x-(cnn_out->w)/2 - ideal_center_x;
                diff_y = cnn_out->y+(cnn_out->h)/2 - ideal_center_y;
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
                position = CENTERED;
                printf("STATE: measurement\ntime left: %i\n",ssm.time_left());
                cnn_out = run_cnn(DISPLAY_FACE_STATUS, DISPLAY_BB);  
                if(cnn_out->face_status == NO_FACE_PRESENT)
                {
                    set_state(SEARCH);
                } 
                
                // diff_x = cnn_out->x - ideal_right.x;
                // diff_y = cnn_out->y - ideal_top.y;
                diff_x = cnn_out->x-(cnn_out->w)/2 - ideal_center_x;
                diff_y = cnn_out->y+(cnn_out->h)/2 - ideal_center_y;

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
                    static int frame_count = 0;
                    static int last_area = 0;
                    static int current_area = 0;
                    static int running_avg_iir = 0;
                    current_area = (cnn_out->w * cnn_out->h)/100;
                    if(last_area > 0)
                    {
                        running_avg_iir = current_area/10 + (9*last_area)/10; // y[n] = alpha*x[n] + (1-alpha)*y[n-1]
                        last_area = running_avg_iir;
                    }
                    else
                    {
                        last_area = current_area;
                    }
                    frame_count++;
                    unstable_frame_count = 0;
                    MXC_TFT_FillCircle(120,140,3,GREEN);
                    if(frame_count == 5)
                    {
                        int i;
                        for(i = 0; i < 5; i++)
                        {
                            if(boxes[i] > running_avg_iir)
                            {
                                break;
                            }
                        }
                        float factor = (
                            temps[i]*(boxes[i]-(float)running_avg_iir) \
                            + temps[i-1]*((float)running_avg_iir-boxes[i-1])) \
                            /(boxes[i]-boxes[i-1]);
                        TFT_Print(lcd_text_buff, 0, 240, (int)&SansSerif16x16[0], sprintf(lcd_text_buff, "Temp: %.1f-->%i  ", get_temp()/**factor*/, running_avg_iir));
                        frame_count = 0;
                        //temp_result(temp_value);
                    }
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


