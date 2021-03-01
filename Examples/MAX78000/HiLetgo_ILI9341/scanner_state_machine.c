
#include "mxc_delay.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


// helper libraries for peripherals
#include "mxc_device.h"
#include "mxc_sys.h"
#include "bbfc_regs.h"
#include "fcr_regs.h"
#include "icc.h"
#include "pb.h"
#include "cnn.h"
#include "tft_fthr.h"


#define TFT_BUFF_SIZE   50    // TFT buffer size
uint32_t cnn_buffer[1600];
volatile uint32_t cnn_time; // Stopwatch

void load_input(void)
{
  // This function loads the sample data input -- replace with actual data

  //memcpy32((uint32_t *) 0x50400000, input_0, 1600);
  memcpy32((uint32_t *) 0x50400000, cnn_buffer, 1600);
}

// Classification layer:
static int32_t ml_data[CNN_NUM_OUTPUTS];
static q15_t ml_softmax[CNN_NUM_OUTPUTS];

void softmax_layer(void)
{
  cnn_unload((uint32_t *) ml_data);
  softmax_q17p14_q15((const q31_t *) ml_data, CNN_NUM_OUTPUTS, ml_softmax);
}

#include "camera_funcs.h"
#include "scanner_state_machine.h"
#include "HiLetgo_ILI9341.h"
#include "PIR_sensor.h"

// this struct defines the state machine
typedef struct 
{
    scan_state_t current_state;
    uint8_t time_left;
} scanner_state_machine_t;

// buffer for touch screen text
char buff[TFT_BUFF_SIZE];
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

    // Enable peripheral, enable CNN interrupt, turn on CNN clock
    // CNN clock: 50 MHz div 1
    // cnn_enable(MXC_S_GCR_PCLKDIV_CNNCLKSEL_PCLK, MXC_S_GCR_PCLKDIV_CNNCLKDIV_DIV1);

    // printf("\n*** CNN Inference Test ***\n");

    // cnn_init(); // Bring state machine into consistent state
    // cnn_load_weights(); // Load kernels
    // cnn_load_bias();
    // cnn_configure(); // Configure state machine
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
            //     int i;
            //     int digs, tens;
            //     area_t clear_word = {10, 100, 200, 20};
            //     int last_state = 0;
            //     capture_camera_img();
            //     display_grayscale_img(100,150,cnn_buffer);
                
            //     // Enable CNN clock
            //     MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_CNN);

            //     cnn_init(); // Bring state machine into consistent state
            //     cnn_configure(); // Configure state machine

            //     load_input();
            //     cnn_start();
                    
            //     while (cnn_time == 0)
            //     __WFI(); // Wait for CNN

            //     softmax_layer();

            //     cnn_stop();
            //     // Disable CNN clock to save power
            //     MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_CNN);

            // #ifdef CNN_INFERENCE_TIMER
            //     printf("Approximate inference time: %u us\n\n", cnn_time);
            // #endif

            //     printf("Classification results:\n");
            //     int max = 0;
            //     int max_i = 0;
            //     for (i = 0; i < CNN_NUM_OUTPUTS; i++) {
            //     digs = (1000 * ml_softmax[i] + 0x4000) >> 15;
            //     tens = digs % 10;
            //     digs = digs / 10;
            //     if(digs > max)
            //     {
            //         max = digs;
            //         max_i = i;
            //     }
            //     printf("[%7d] -> Class %d: %d.%d%%\n", ml_data[i], i, digs, tens);
            //     }
            //     if(max_i == 0)
            //     {
            //     printf("DETECTED A FACE: %i\n", max);
            //     memset(buff,32,TFT_BUFF_SIZE);
            //     if(max_i != last_state)
            //     {
            //         MXC_TFT_FillRect(&clear_word, 4);
            //         TFT_Print(buff, 10, 100, 1, sprintf(buff, "DETECTED A FACE"));
            //     }
            //     }
            //     else
            //     {
            //     printf("NO FACE DETECTED: %i\n", max);
            //     memset(buff,32,TFT_BUFF_SIZE);
            //     if(max_i != last_state)
            //     {
            //         MXC_TFT_FillRect(&clear_word, 4);
            //         TFT_Print(buff, 10, 100, 1, sprintf(buff, "NO FACE DETECTED"));
            //     }
            //     }
            //     last_state = max_i;
            //     printf("\033[0;0f");

                capture_camera_img();
                display_RGB565_img(40,50);
                //printf("Scanner State: Search\n");
                // start the CNN and search for a face
                break;
            }
            case POSITIONING:
            {
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

