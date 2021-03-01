/*******************************************************************************
* Copyright (C) Maxim Integrated Products, Inc., All rights Reserved.
*
* This software is protected by copyright laws of the United States and
* of foreign countries. This material may also be protected by patent laws
* and technology transfer regulations of the United States and of foreign
* countries. This software is furnished under a license agreement and/or a
* nondisclosure agreement and may only be used or reproduced in accordance
* with the terms of those agreements. Dissemination of this information to
* any party or parties not specified in the license agreement and/or
* nondisclosure agreement is expressly prohibited.
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
* OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
* Except as contained in this notice, the name of Maxim Integrated
* Products, Inc. shall not be used except as stated in the Maxim Integrated
* Products, Inc. Branding Policy.
*
* The mere transfer of this software does not imply any licenses
* of trade secrets, proprietary technology, copyrights, patents,
* trademarks, maskwork rights, or any other form of intellectual
* property whatsoever. Maxim Integrated Products, Inc. retains all
* ownership rights.
*******************************************************************************/

// mini_vgg_face_classifier
// Created using ./ai8xize.py --verbose --log --test-dir sdk/Examples/MAX78000/CNN --prefix mini_vgg_face_classifier --checkpoint-file trained/mini_vgg_net_q.pth.tar --config-file networks/mini_vgg_net.yaml --device MAX78000 --softmax --compact-data --mexpress --timer 0 --display-checkpoint

/***** Includes *****/
// standard C libraries
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

// helper libraries for peripherals
#include "mxc_device.h"
#include "mxc_sys.h"
#include "bbfc_regs.h"
#include "fcr_regs.h"
#include "icc.h"
#include "led.h"
#include "tft.h"
#include "pb.h"
#include "mxc_delay.h"
#include "camera.h"
#include "bitmap.h"
#include "camera_tft_funcs.h"
#include "dma.h"
#include "cnn.h"
#include "mxc.h"
#include "sampledata.h"

volatile uint32_t cnn_time; // Stopwatch
/***** Definitions *****/
#define TFT_BUFF_SIZE   50    // TFT buffer size
#define CAMERA_FREQ   (10 * 1000 * 1000)

/***** Globals *****/
uint32_t cnn_buffer[1600];

static const uint32_t input_0[] = SAMPLE_INPUT_0;

// buffer for touch screen text
char buff[TFT_BUFF_SIZE];

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

int main(void)
{
  int i;
  int digs, tens;

  MXC_ICC_Enable(MXC_ICC0); // Enable cache

  // Switch to 100 MHz clock
  MXC_SYS_Clock_Select(MXC_SYS_CLOCK_IPO);
  SystemCoreClockUpdate();

  // Initialize DMA for camera interface
  MXC_DMA_Init();
  int dma_channel = MXC_DMA_AcquireChannel();

  // Initialize TFT display.
  printf("Init LCD.\n");
  init_LCD();
  MXC_TFT_ClearScreen();
  MXC_TFT_ShowImage(0, 0, img_1_bmp);

  // Initialize camera.
  printf("Init Camera.\n");
  camera_init(CAMERA_FREQ);
  set_image_dimensions(80, 80);

  /* Set the screen rotation because camera flipped*/
  MXC_TFT_SetRotation(SCREEN_FLIP);

  // Setup the camera image dimensions, pixel format and data acquiring details.
  // four bytes because each pixel is 2 bytes, can get 2 pixels at a time
  int ret = camera_setup(get_image_x(), get_image_y(), PIXFORMAT_YUV422, FIFO_FOUR_BYTE, USE_DMA, dma_channel);
  if (ret != STATUS_OK) 
  {
	printf("Error returned from setting up camera. Error %d\n", ret);
	return -1;
  }
  MXC_Delay(1000000);
  MXC_TFT_SetPalette(logo_white_bg_darkgrey_bmp);
  MXC_TFT_SetBackGroundColor(4);
  capture_camera_img();
  display_grayscale_img(100,150,cnn_buffer);
  

  printf("Waiting...\n");

  // DO NOT DELETE THIS LINE:
  MXC_Delay(SEC(2)); // Let debugger interrupt if needed

  // Enable peripheral, enable CNN interrupt, turn on CNN clock
  // CNN clock: 50 MHz div 1
  cnn_enable(MXC_S_GCR_PCLKDIV_CNNCLKSEL_PCLK, MXC_S_GCR_PCLKDIV_CNNCLKDIV_DIV1);

  printf("\n*** CNN Inference Test ***\n");

  cnn_init(); // Bring state machine into consistent state
  cnn_load_weights(); // Load kernels
  cnn_load_bias();
  cnn_configure(); // Configure state machine

  area_t clear_word = {10, 100, 200, 20};
  int last_state = 0;

  while(true)
  {
    capture_camera_img();
    display_grayscale_img(100,150,cnn_buffer);
    
    // Enable CNN clock
    MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_CNN);

    cnn_init(); // Bring state machine into consistent state
    cnn_configure(); // Configure state machine

    load_input();
    cnn_start();
        
    while (cnn_time == 0)
      __WFI(); // Wait for CNN

    softmax_layer();

    cnn_stop();
    // Disable CNN clock to save power
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_CNN);

  #ifdef CNN_INFERENCE_TIMER
    printf("Approximate inference time: %u us\n\n", cnn_time);
  #endif

    printf("Classification results:\n");
    int max = 0;
    int max_i = 0;
    for (i = 0; i < CNN_NUM_OUTPUTS; i++) {
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
    if(max_i == 0)
    {
      printf("DETECTED A FACE: %i\n", max);
      memset(buff,32,TFT_BUFF_SIZE);
      if(max_i != last_state)
      {
        MXC_TFT_FillRect(&clear_word, 4);
        TFT_Print(buff, 10, 100, 1, sprintf(buff, "DETECTED A FACE"));
      }
    }
    else
    {
      printf("NO FACE DETECTED: %i\n", max);
      memset(buff,32,TFT_BUFF_SIZE);
      if(max_i != last_state)
      {
        MXC_TFT_FillRect(&clear_word, 4);
        TFT_Print(buff, 10, 100, 1, sprintf(buff, "NO FACE DETECTED"));
      }
    }
    last_state = max_i;
    printf("\033[0;0f");
  }

  return 0;
}

/*
  SUMMARY OF OPS
  Hardware: 25,135,316 ops (24,833,684 macc; 301,632 comp; 0 add; 0 mul; 0 bitwise)

  RESOURCE USAGE
  Weight memory: 152,732 bytes out of 442,368 bytes total (35%)
  Bias memory:   12 bytes out of 2,048 bytes total (1%)
*/
