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

// geffen_regression
// Created using ./ai8xize.py --verbose --log --test-dir sdk/Examples/MAX78000/CNN --prefix geffen_regression --checkpoint-file trained/regression_q.pth.tar --config-file networks/geffen_face_classifier.yaml --device MAX78000 --compact-data --mexpress --timer 0 --display-checkpoint

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
#define TFT_BUFF_SIZE   50    // TFT buffer size
#define CAMERA_FREQ   (10 * 1000 * 1000)

/***** Globals *****/
// mnist data is 28 x 28 px = 784 px --> 8 bits each --> 784 bytes. 784/4 = 196 4 byte chunks
uint32_t cnn_buffer[1024];
void fail(void)
{
  printf("\n*** FAIL ***\n\n");
  while (1);
}

// 1-channel 64x64 data input (4096 bytes / 1024 32-bit words):
// CHW 64x64, channel 0
static const uint32_t input_0[] = SAMPLE_INPUT_0;

void load_input(void)
{
  // This function loads the sample data input -- replace with actual data

  memcpy32((uint32_t *) 0x50400000, cnn_buffer, 1024);
}

// Expected output of layer 5 for geffen_regression given the sample input
int check_output(void)
{
  if ((*((volatile uint32_t *) 0x50401000)) != 0x00002671) return CNN_FAIL; // 0,0,0
  if ((*((volatile uint32_t *) 0x50401004)) != 0x00001615) return CNN_FAIL; // 0,0,1
  if ((*((volatile uint32_t *) 0x50401008)) != 0x0000125b) return CNN_FAIL; // 0,0,2
  if ((*((volatile uint32_t *) 0x5040100c)) != 0x00001693) return CNN_FAIL; // 0,0,3
  if ((*((volatile uint32_t *) 0x50409000)) != 0x00001615) return CNN_FAIL; // 0,0,4
  if ((*((volatile uint32_t *) 0x50409004)) != 0x00002574) return CNN_FAIL; // 0,0,5
  if ((*((volatile uint32_t *) 0x50409008)) != 0x00001713) return CNN_FAIL; // 0,0,6
  if ((*((volatile uint32_t *) 0x5040900c)) != 0x000024f8) return CNN_FAIL; // 0,0,7

  return CNN_OK;
}

static int32_t ml_data[CNN_NUM_OUTPUTS];

int main(void)
{
  
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
  set_image_dimensions(64, 64); // gets decimated to 28x28

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
  //load_input(); // Load data input
  //cnn_start(); // Start CNN processing
area_t left_eye = {100, 150, 4, 4};
area_t right_eye = {100, 150, 4, 4};
area_t nose = {100, 150, 4, 4};
area_t chin = {100, 150, 4, 4};
area_t face = {100,150,0,0};
while(1)
{
  capture_camera_img();
  display_grayscale_img(100,150,cnn_buffer);
  cnn_init(); // Bring state machine into consistent state
  cnn_configure(); // Configure state machine

  load_input();
  cnn_start();
  while (cnn_time == 0)
    __WFI(); // Wait for CNN

  //if (check_output() != CNN_OK) fail();
  cnn_unload((uint32_t *) ml_data);

  printf("\n*** PASS ***\n\n");
  int x = (*((volatile uint32_t *) 0x50401000))/8294;
  int y = (*((volatile uint32_t *) 0x50401004))/16540;
  int w = (*((volatile uint32_t *) 0x50401008))/6947;
  int h = (*((volatile uint32_t *) 0x5040100c))/6261;
  printf("%i\n",x); // 0,0,0
  printf("%i\n",y); // 0,0,1
  printf("%i\n",w); // 0,0,2
  printf("%i\n",h); // 0,0,3
  face.x = 100+x;
  face.y = 150+y;
  face.w = w;
  face.h = h;
  MXC_TFT_FillRect(&face, 100);

#ifdef CNN_INFERENCE_TIMER
  printf("Approximate inference time: %u us\n\n", cnn_time);
#endif
printf("\033[0;0f");
}
  cnn_disable(); // Shut down CNN clock, disable peripheral


  return 0;
}

/*
  SUMMARY OF OPS
  Hardware: 12,363,360 ops (12,036,800 macc; 326,560 comp; 0 add; 0 mul; 0 bitwise)

  RESOURCE USAGE
  Weight memory: 28,550 bytes out of 442,368 bytes total (6%)
  Bias memory:   8 bytes out of 2,048 bytes total (0%)
*/
// if ((*((volatile uint32_t *) 0x50401000)) != 0x0003cc03) return CNN_FAIL; // 0,0,0
//   if ((*((volatile uint32_t *) 0x50401004)) != 0x00038891) return CNN_FAIL; // 0,0,1
//   if ((*((volatile uint32_t *) 0x50401008)) != 0x0007f349) return CNN_FAIL; // 0,0,2
//   if ((*((volatile uint32_t *) 0x5040100c)) != 0x00083737) return CNN_FAIL; // 0,0,3
