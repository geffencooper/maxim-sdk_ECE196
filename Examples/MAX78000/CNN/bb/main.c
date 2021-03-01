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

// bb
// Created using ./ai8xize.py --verbose --log --test-dir sdk/Examples/MAX78000/CNN --prefix bb --checkpoint-file trained/bb_q.pth.tar --config-file networks/bb.yaml --device MAX78000 --compact-data --mexpress --timer 0 --display-checkpoint

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
#define TFT_BUFF_SIZE 50 // TFT buffer size
#define CAMERA_FREQ (10 * 1000 * 1000)

void fail(void)
{
printf("\n*** FAIL ***\n\n");
while (1);
}

// 1-channel 80x80 data input (6400 bytes / 1600 32-bit words):
// CHW 80x80, channel 0
static const uint32_t input_0[] = SAMPLE_INPUT_0;
uint32_t cnn_buffer[1600];

void load_input(void)
{
// This function loads the sample data input -- replace with actual data

memcpy32((uint32_t *) 0x50400000, cnn_buffer, 1600);
}

// Expected output of layer 10 for bb given the sample input
int check_output(void)
{
if ((*((volatile uint32_t *) 0x50402000)) != 0x0002191c) return CNN_FAIL; // 0,0,0
if ((*((volatile uint32_t *) 0x50402004)) != 0x00015ef7) return CNN_FAIL; // 0,0,1
if ((*((volatile uint32_t *) 0x50402008)) != 0x00061837) return CNN_FAIL; // 0,0,2
if ((*((volatile uint32_t *) 0x5040200c)) != 0x00074cc9) return CNN_FAIL; // 0,0,3

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
set_image_dimensions(80, 80); // gets decimated to 28x28

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
display_grayscale_img(0,0,cnn_buffer);

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
area_t top = {100, 150, 4, 4};
area_t left = {100, 150, 4, 4};
area_t bottom = {100, 150, 4, 4};
area_t right = {100, 150, 4, 4};
while(1)
{
capture_camera_img();
display_grayscale_img(0,0,cnn_buffer);
cnn_init(); // Bring state machine into consistent state
cnn_configure(); // Configure state machine

load_input();
cnn_start();
while (cnn_time == 0)
__WFI(); // Wait for CNN

//if (check_output() != CNN_OK) fail();
cnn_unload((uint32_t *) ml_data);

printf("\n*** PASS ***\n\n");
int x = (*((volatile uint32_t *) 0x50402000))/950;
int y = (*((volatile uint32_t *) 0x50402004))/950;
int w = (*((volatile uint32_t *) 0x50402008))/1150;
int h = (*((volatile uint32_t *) 0x5040200c))/1150;
printf("%i\n",x); // 0,0,0 
printf("%i\n",y); // 0,0,1
printf("%i\n",w); // 0,0,2
printf("%i\n",h); // 0,0,3

top.x = (x >= 0 && x < 80) ? x : 0;
top.y = (y >= 0 && y < 80) ? y : 0;
top.w = (top.x+w) <= 80 ? w : 80-top.x;
top.h = 2;

bottom.x = top.x;
bottom.y = (y+h-2) <= 78 ? y+h-2 : 78;
bottom.w = top.w;
bottom.h = 2;

left.x = top.x;
left.y = top.y;
left.w = 2;
left.h = (left.y+h) <= 80 ? h : 80-left.y;

right.x = (x+w) <= 78 ? (x+w): 78;
right.y = top.y;
right.w = 2;
right.h = left.h;

top.x = 80-top.x-top.w;
bottom.x = 80-bottom.x-bottom.w;
left.x = 80-left.x-2;
right.x = 80-right.x-2;

MXC_TFT_FillRect(&top, 100);
MXC_TFT_FillRect(&bottom, 100);
MXC_TFT_FillRect(&left, 100);
MXC_TFT_FillRect(&right, 100);
// the camera on the EVkit is flipped by 180 degrees so we also rotate the
// output and CNN input. We also need to rotate the bounding box by 180.
// top.x = (80-x-w) >= 0 ? (80-x-w) : 0;
// top.y = y >= 0 ? y : 0;
// top.w = (top.x+w) < 80 ? w : (80-top.x);
// top.h = 2;
// MXC_TFT_FillRect(&top, 100);

// bottom.x = (80-x-w) >= 0 ? (80-x-w) : 0;
// bottom.y = (y+h-2) <= 78 ? (y+h-2) : 78;
// bottom.w = (bottom.x+w < 80) ? w : 80-bottom.x;
// bottom.h = 2;
// MXC_TFT_FillRect(&bottom, 100);

// left.x = (80-x >= 0) ? 80-x : 0;
// left.y = (y >= 0) ? y : 0;
// left.w = 2;
// left.h = (left.y+h < 80) ? h : 80-left.y;
// MXC_TFT_FillRect(&left, 100);

// right.x = (78-x-w) <= 78 ? (78-x-w): 78;
// right.y = (y >= 0) ? y : 0;
// right.w = 2;
// right.h = (right.y+h <= 78) ? h : 78-right.y;
// MXC_TFT_FillRect(&right, 100);

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
Hardware: 25,131,840 ops (24,830,208 macc; 301,632 comp; 0 add; 0 mul; 0 bitwise)

RESOURCE USAGE
Weight memory: 149,256 bytes out of 442,368 bytes total (34%)
Bias memory: 4 bytes out of 2,048 bytes total (0%)
*/

