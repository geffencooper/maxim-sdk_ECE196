#include "cnn_helper_funcs.h"
#include "mxc_delay.h"
#include "mxc_device.h"
#include "mxc_sys.h"
#include "bbfc_regs.h"
#include "fcr_regs.h"
#include "cnn.h"
#include "camera_funcs.h"
#include "HiLetgo_ILI9341.h"
#include "tft_fthr.h"
#include "bitmap.h"

#define SCREEN_W 160
#define SCREEN_H 160
#define SCREEN_X 40
#define SCREEN_Y 100
#define BB_COLOR YELLOW
#define BB_W 2
#define TFT_BUFF_SIZE   50    // TFT buffer size

uint32_t cnn_buffer[1600];
volatile uint32_t cnn_time; // Stopwatch
cnn_output_t output;
area_t clear_word = {0, 0, 200, 20};
// buffer for touch screen text
char buff[TFT_BUFF_SIZE];

void load_input(void)
{
  memcpy32((uint32_t *) 0x50400000, cnn_buffer, 1600);
}

// Classification layer:
static int32_t ml_data[2];
static q15_t ml_softmax[2];

void softmax_layer(void)
{
  ((uint32_t *) ml_data)[0] = (*((volatile uint32_t *) 0x50404000));
  ((uint32_t *) ml_data)[1] = (*((volatile uint32_t *) 0x50404004));
  softmax_q17p14_q15((const q31_t *) ml_data, 2, ml_softmax);
}

uint32_t* get_cnn_buffer()
{
    return cnn_buffer;
}

cnn_output_t* run_cnn(int display_txt, int display_bb)
{
    int i;
    int digs, tens;
    int last_state = 0;
    int max = 0;
    int max_i = 0;

    capture_camera_img();
    display_grayscale_img(40,100,get_cnn_buffer());
    
    // Enable CNN clock
    MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_CNN);

    cnn_init(); // Bring state machine into consistent state
    cnn_configure(); // Configure state machine

    load_input();
    cnn_start();
        
    while (cnn_time == 0)
    __WFI(); // Wait for CNN

    softmax_layer();
    // int face = (*((volatile uint32_t *) 0x50404000));
    // int no_face = (*((volatile uint32_t *) 0x50404004));
    output.face_status = max_i;
    output.x = (*((volatile uint32_t *) 0x50404008))/480;
    output.y = (*((volatile uint32_t *) 0x5040400C))/500;
    output.w = (*((volatile uint32_t *) 0x50404010))/500;
    output.h = (*((volatile uint32_t *) 0x50404014))/480;

    cnn_stop();
    // Disable CNN clock to save power
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_CNN);

#ifdef CNN_INFERENCE_TIMER
    printf("Approximate inference time: %u us\n\n", cnn_time);
#endif
    
    printf("Classification results:\n");
    
    for (i = 0; i < 2; i++) 
    {
        digs = (1000 * ml_softmax[i] + 0x4000) >> 15;
        tens = digs % 10;
        digs = digs / 10;
        if(digs > max)
        {
            max = digs;
            max_i = i;
        }
        printf("[%7d] -> Class %d: %d.%d%%\n", ml_data[i], i, digs, tens);
    }
    output.face_status = max_i;
    if(max_i == 0 && display_txt)
    {
        printf("DETECTED A FACE: %i\n", max);
        //memset(buff,32,TFT_BUFF_SIZE);
        if(max_i != last_state)
        {
            MXC_TFT_FillRect(&clear_word, 4);
            TFT_Print(buff, 0, 0, 3, sprintf(buff, "DETECTED A FACE"));
        }
    }
    else if(max_i != 0 && display_txt)
    {
        printf("NO FACE DETECTED: %i\n", max);
        //memset(buff,32,TFT_BUFF_SIZE);
        if(max_i != last_state)
        {
            MXC_TFT_FillRect(&clear_word, 4);
            TFT_Print(buff, 0, 0, 3, sprintf(buff, "NO FACE DETECTED"));
        }
    }
    last_state = max_i;
    
    if(max_i == 0 && display_bb)
    {
        printf("positioing state so display bb\n");
        
        //printf("face: %i\n",face);
        //printf("no face: %i\n",no_face);
        printf("x: %i\n",output.x); // 0,0,0 
        printf("y: %i\n",output.y); // 0,0,1
        printf("w: %i\n",output.w); // 0,0,2
        printf("h: %i\n",output.h); // 0,0,3

        area_t top = {100, 150, 4, 4};
        area_t left = {100, 150, 4, 4};
        area_t bottom = {100, 150, 4, 4};
        area_t right = {100, 150, 4, 4};

        top.x = (output.x >= 0 && output.x < SCREEN_W) ? output.x : 0;
        top.y = (output.y >= 0 && output.y < SCREEN_H) ? output.y : 0;
        top.w = (top.x+output.w) <=SCREEN_W ? output.w : SCREEN_W-top.x;
        top.h = 2;

        bottom.x = top.x;
        bottom.y = (output.y+output.h-BB_W) <= SCREEN_H-BB_W ? output.y+output.h-BB_W : SCREEN_H-BB_W;
        bottom.w = top.w;
        bottom.h = BB_W;

        left.x = top.x;
        left.y = top.y;
        left.w = BB_W;
        left.h = (left.y+output.h) <= SCREEN_H ? output.h : SCREEN_H-left.y;

        right.x = (output.x+output.w) <= SCREEN_W-BB_W ? (output.x+output.w): SCREEN_W-BB_W;
        right.y = top.y;
        right.w = BB_W;
        right.h = left.h;

        top.x += SCREEN_X;
        bottom.x += SCREEN_X;
        left.x += SCREEN_X;
        right.x += SCREEN_X;

        top.y = (SCREEN_H-BB_W-top.y)+SCREEN_Y;
        bottom.y = (SCREEN_H-BB_W-bottom.y)+SCREEN_Y;
        left.y = (SCREEN_H-left.y-left.h)+SCREEN_Y;
        right.y = (SCREEN_H-right.y-right.h)+SCREEN_Y;


        MXC_TFT_FillRect(&top, BB_COLOR);
        MXC_TFT_FillRect(&bottom, BB_COLOR);
        MXC_TFT_FillRect(&left, BB_COLOR);
        MXC_TFT_FillRect(&right, BB_COLOR);
    }
    printf("\033[0;0f");
    return &output;
}

void startup_cnn()
{
    // Enable peripheral, enable CNN interrupt, turn on CNN clock
    // CNN clock: 50 MHz div 1
    cnn_enable(MXC_S_GCR_PCLKDIV_CNNCLKSEL_PCLK, MXC_S_GCR_PCLKDIV_CNNCLKDIV_DIV1);

    cnn_init(); // Bring state machine into consistent state
    cnn_load_weights(); // Load kernels
    cnn_load_bias();
    cnn_configure(); // Configure state machine
}