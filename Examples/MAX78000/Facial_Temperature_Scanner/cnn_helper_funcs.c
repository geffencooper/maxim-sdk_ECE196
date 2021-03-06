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

// macros
#define SCREEN_W 160 // image output width
#define SCREEN_H 160 // image output height
#define SCREEN_X 40 // image output top left corner
#define SCREEN_Y 100 // image output top left corner
#define BB_COLOR YELLOW // the bounding box color
#define BB_W 2 // the bounding box width in pixels
#define TFT_BUFF_SIZE   50 // TFT text buffer size
#define NUM_CLASSES 2 // number of output classes

// Global variables
uint32_t cnn_buffer[1600]; // the input image data into the CNN
char buff[TFT_BUFF_SIZE]; // buffer for touch screen text
volatile uint32_t cnn_time; // Stopwatch
cnn_output_t output; // the output data of the CNN
area_t clear_word = {0, 0, 200, 20}; // a rectangle used to clear text
static int32_t ml_data[NUM_CLASSES]; // classification output data
static q15_t ml_softmax[NUM_CLASSES]; // softmax output data

// this function loads the image data into the input layer's data memory instance
void load_input(void)
{
  memcpy32((uint32_t *) 0x50400000, cnn_buffer, 1600);
}

// this function gets the classification data (face or no face) from the output layer
// and passes it to the auto-generated softmax function
void softmax_layer(void)
{
  ((uint32_t *) ml_data)[0] = (*((volatile uint32_t *) 0x50404000));
  ((uint32_t *) ml_data)[1] = (*((volatile uint32_t *) 0x50404004));
  softmax_q17p14_q15((const q31_t *) ml_data, NUM_CLASSES, ml_softmax);
}

// simple getter function for the CNN input data buffer
uint32_t* get_cnn_buffer()
{
    return cnn_buffer;
}

// this function does a forward pass through the CNN
cnn_output_t* run_cnn(int display_txt, int display_bb)
{
    int digs, tens; // format probability
    int last_state = 0; // only clear text if state changed
    int max = 0; // the current highest class probability
    int max_i = 0; // the class with the highest probability (0 = face, 1 = no face)

    // first get an image from the camera and load it into the CNN buffer
    capture_camera_img();
    load_grayscale_img(40,100,get_cnn_buffer()); // this also displays the image to the screen at (40,100)
    
    // Enable CNN clock
    MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_CNN);

    cnn_init(); // Bring state machine into consistent state
    cnn_configure(); // Configure state machine

    load_input();
    cnn_start();
        
    while (cnn_time == 0)
    __WFI(); // Wait for CNN

    // classify the output
    softmax_layer();


    // get the output data from the CNN output layer's data memory instance

    // int face = (*((volatile uint32_t *) 0x50404000));
    // int no_face = (*((volatile uint32_t *) 0x50404004));
    output.x = (*((volatile uint32_t *) 0x50404008))/480;
    output.y = (*((volatile uint32_t *) 0x5040400C))/500;
    output.w = (*((volatile uint32_t *) 0x50404010))/500;
    output.h = (*((volatile uint32_t *) 0x50404014))/480;

    cnn_stop();
    // Disable CNN clock to save power
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_CNN);

#ifdef CNN_INFERENCE_TIMER
    //printf("Approximate inference time: %u us\n\n", cnn_time);
#endif
    
   // printf("Classification results:\n");
    for (int i = 0; i < NUM_CLASSES; i++) 
    {
        digs = (1000 * ml_softmax[i] + 0x4000) >> 15;
        tens = digs % 10;
        digs = digs / 10;
        if(digs > max)
        {
            max = digs;
            max_i = i;
        }
        //printf("[%7d] -> Class %d: %d.%d%%\n", ml_data[i], i, digs, tens);
    }

    // store the output data state
    output.face_status = max_i;

    // a face was detected
    if(max_i == 0 && display_txt)
    {
        //printf("DETECTED A FACE: %i\n", max);
        //memset(buff,32,TFT_BUFF_SIZE);
        if(max_i != last_state)
        {
           // MXC_TFT_FillRect(&clear_word, 4);
           // TFT_Print(buff, 0, 0, 3, sprintf(buff, "DETECTED A FACE"));
        }
    }
    // no face was detected
    else if(max_i != 0 && display_txt)
    {
        //printf("NO FACE DETECTED: %i\n", max);
        //memset(buff,32,TFT_BUFF_SIZE);
        if(max_i != last_state)
        {
           // MXC_TFT_FillRect(&clear_word, 4);
           // TFT_Print(buff, 0, 0, 3, sprintf(buff, "NO FACE DETECTED"));
        }
    }
    last_state = max_i;
    
    // if there is a face then display the bounding box
    if(max_i == 0 && display_bb)
    {
        //printf("face: %i\n",face);
        //printf("no face: %i\n",no_face);
        // printf("x: %i\n",output.x);
        // printf("y: %i\n",output.y);
        // printf("w: %i\n",output.w);
        // printf("h: %i\n",output.h);

        // the sides of the bounding box
        static area_t top = {100, 150, 4, 4};
        static area_t left = {100, 150, 4, 4};
        static area_t bottom = {100, 150, 4, 4};
        static area_t right = {100, 150, 4, 4};

        // the top should not go off of the screen
        top.x = (output.x >= 0 && output.x < SCREEN_W) ? output.x : 0;
        top.y = (output.y >= 0 && output.y < SCREEN_H) ? output.y : 0;
        top.w = (top.x+output.w) < SCREEN_W ? output.w : SCREEN_W-top.x-1;
        top.h = 2;

        // the bottom should not go off of the screen
        bottom.x = top.x;
        bottom.y = (output.y+output.h-BB_W) < SCREEN_H-BB_W ? output.y+output.h-BB_W : SCREEN_H-BB_W-1;
        bottom.w = top.w;
        bottom.h = BB_W;

        // the left should not go off of the screen
        left.x = top.x;
        left.y = top.y;
        left.w = BB_W;
        left.h = (left.y+output.h) < SCREEN_H ? output.h : SCREEN_H-left.y-1;

        // the right should not go off of the screen
        right.x = (output.x+output.w) < SCREEN_W-BB_W ? (output.x+output.w): SCREEN_W-BB_W-1;
        right.y = top.y;
        right.w = BB_W;
        right.h = left.h;

        // shift the box to the screen X location
        top.x += SCREEN_X;
        bottom.x += SCREEN_X;
        left.x += SCREEN_X;
        right.x += SCREEN_X;

        // shift the box to the screen Y location
        // also flip the box because the screen is upsidedown
        top.y = (SCREEN_H-BB_W-top.y-1)+SCREEN_Y;
        bottom.y = (SCREEN_H-BB_W-bottom.y-1)+SCREEN_Y;
        left.y = (SCREEN_H-left.y-left.h-1)+SCREEN_Y;
        right.y = (SCREEN_H-right.y-right.h-1)+SCREEN_Y;

        // draw the box
        MXC_TFT_FillRect(&top, BB_COLOR);
        MXC_TFT_FillRect(&bottom, BB_COLOR);
        MXC_TFT_FillRect(&left, BB_COLOR);
        MXC_TFT_FillRect(&right, BB_COLOR);

        output.x = top.x;
        output.y = top.y;
        output.w = top.w;
        output.h = left.h;
    }
    // this moves the cursor to the top left corner, allows for nicer printf output
    // that is easier to read
    // printf("\033[0;0f");
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