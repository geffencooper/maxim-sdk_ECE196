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

/**
 * @file    main.c
 * @brief   Camera Interface
 * @details experimenting/understanding the camera interface
 */

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

/***** Definitions *****/
#define TFT_BUFF_SIZE   50    // TFT buffer size

#define CAPTURE_IMAGE
//#define CONTINUOUS_STREAM

/***** Globals *****/
uint32_t input_0_camera[4096];
uint32_t input_1_camera[4096];
uint32_t input_2_camera[4096];

// buffer for touch screen text
char buff[TFT_BUFF_SIZE];

static const uint8_t camera_settings[][2] = {
	{0x0e, 0x08}, // Sleep mode
	{0x69, 0x52}, // BLC window selection, BLC enable (default is 0x12)
	{0x1e, 0xb3}, // AddLT1F (default 0xb1)
	{0x48, 0x42},
	{0xff, 0x01}, // Select MIPI register bank
	{0xb5, 0x30},
	{0xff, 0x00}, // Select system control register bank
	{0x16, 0x03}, // (default)
	{0x62, 0x10}, // (default)
	{0x12, 0x01}, // Select Bayer RAW
	{0x17, 0x65}, // Horizontal Window Start Point Control (LSBs), default is 0x69
	{0x18, 0xa4}, // Horizontal sensor size (default)
	{0x19, 0x0c}, // Vertical Window Start Line Control (default)
	{0x1a, 0xf6}, // Vertical sensor size (default)
	{0x37, 0x04}, // PCLK is double system clock (default is 0x0c)
	{0x3e, 0x20}, // (default)
	{0x81, 0x3f}, // sde_en, uv_adj_en, scale_v_en, scale_h_en, uv_avg_en, cmx_en
	{0xcc, 0x02}, // High 2 bits of horizontal output size (default)
	{0xcd, 0x80}, // Low 8 bits of horizontal output size (default)
	{0xce, 0x01}, // Ninth bit of vertical output size (default)
	{0xcf, 0xe0}, // Low 8 bits of vertical output size (default)
	{0x82, 0x01}, // 01: Raw from CIP (default is 0x00)
	{0xc8, 0x02},
	{0xc9, 0x80},
	{0xca, 0x01},
	{0xcb, 0xe0},
	{0xd0, 0x28},
	{0x0e, 0x00}, // Normal mode (not sleep mode)
	{0x70, 0x00},
	{0x71, 0x34},
	{0x74, 0x28},
	{0x75, 0x98},
	{0x76, 0x00},
	{0x77, 0x64},
	{0x78, 0x01},
	{0x79, 0xc2},
	{0x7a, 0x4e},
	{0x7b, 0x1f},
	{0x7c, 0x00},
	{0x11, 0x01}, // CLKRC, Internal clock pre-scalar divide by 2 (default divide by 1)
	{0x20, 0x00}, // Banding filter (default)
	{0x21, 0x57}, // Banding filter (default is 0x44)
	{0x50, 0x4d},
	{0x51, 0x40}, // 60Hz Banding AEC 8 bits (default 0x80)
	{0x4c, 0x7d},
	{0x0e, 0x00},
	{0x80, 0x7f},
	{0x85, 0x00},
	{0x86, 0x00},
	{0x87, 0x00},
	{0x88, 0x00},
	{0x89, 0x2a},
	{0x8a, 0x22},
	{0x8b, 0x20},
	{0xbb, 0xab},
	{0xbc, 0x84},
	{0xbd, 0x27},
	{0xbe, 0x0e},
	{0xbf, 0xb8},
	{0xc0, 0xc5},
	{0xc1, 0x1e},
	{0xb7, 0x05},
	{0xb8, 0x09},
	{0xb9, 0x00},
	{0xba, 0x18},
	{0x5a, 0x1f},
	{0x5b, 0x9f},
	{0x5c, 0x69},
	{0x5d, 0x42},
	{0x24, 0x78}, // AGC/AEC
	{0x25, 0x68}, // AGC/AEC
	{0x26, 0xb3}, // AGC/AEC
	{0xa3, 0x0b},
	{0xa4, 0x15},
	{0xa5, 0x29},
	{0xa6, 0x4a},
	{0xa7, 0x58},
	{0xa8, 0x65},
	{0xa9, 0x70},
	{0xaa, 0x7b},
	{0xab, 0x85},
	{0xac, 0x8e},
	{0xad, 0xa0},
	{0xae, 0xb0},
	{0xaf, 0xcb},
	{0xb0, 0xe1},
	{0xb1, 0xf1},
	{0xb2, 0x14},
	{0x8e, 0x92},
	{0x96, 0xff},
	{0x97, 0x00},
	{0x14, 0x3b}, 	// AGC value, manual, set banding (default is 0x30)
	{0x0e, 0x00},
	{0x0c, 0xd6},
	{0x82, 0x3},
	{0x11, 0x00},  	// Set clock prescaler
    {0x12, 0x6},
    {0x61, 0x0},
    {0x64, 0x11},
    {0xc3, 0x80},
    {0x81, 0x3f},
    {0x16, 0x3},
    {0x37, 0xc},
    {0x3e, 0x20},
    {0x5e, 0x0},
    {0xc4, 0x1},
    {0xc5, 0x80},
    {0xc6, 0x1},
    {0xc7, 0x80},
    {0xc8, 0x2},
    {0xc9, 0x80},
    {0xca, 0x1},
    {0xcb, 0xe0},
    {0xcc, 0x0},
    {0xcd, 0xFF}, 	// Default to 64 line width
    {0xce, 0x0},
    {0xcf, 0xFF}, 	// Default to 64 lines high
    {0x1c, 0x7f},
    {0x1d, 0xa2},
	{0xee, 0xee}  // End of register list marker 0xee
};

static void process_img(void)
{
	uint8_t   *raw;
	uint32_t  imgLen;
	uint32_t  w, h;

    // Get the details of the image from the camera driver.
	camera_get_image(&raw, &imgLen, &w, &h);

	// Send the image through the UART to the console.
    // A python program will read from the console and write to an image file.
//	utils_send_img_to_pc(raw, imgLen, w, h, camera_get_pixel_format());

	uint16_t *image = (uint16_t*)raw;	// 2bytes per pixel RGB565

#define HEIGHT 		160
#define WIDTH		120
#define THICKNESS	4
#define IMAGE_H		150
#define IMAGE_W		200
#define FRAME_COLOR	0x535A


	// left line
	image+=((IMAGE_H - (WIDTH+2*THICKNESS))/2)*IMAGE_W;
	for (int i = 0; i<THICKNESS; i++) {
		image+=((IMAGE_W - (HEIGHT+2*THICKNESS))/2);
		for(int j=0; j< HEIGHT+2*THICKNESS; j++) {
			*(image++) = FRAME_COLOR; //color
		}
		image+=((IMAGE_W - (HEIGHT+2*THICKNESS))/2);
	}

	//right line
	image = ((uint16_t*)raw) + (((IMAGE_H - (WIDTH+2*THICKNESS))/2) + WIDTH + THICKNESS )*IMAGE_W;
	for (int i = 0; i<THICKNESS; i++) {
		image+=((IMAGE_W - (HEIGHT+2*THICKNESS))/2);
		for(int j =0; j< HEIGHT+2*THICKNESS; j++) {
			*(image++) = FRAME_COLOR; //color
		}
		image+=((IMAGE_W - (HEIGHT+2*THICKNESS))/2);
	}

	//top + bottom lines
	image = ((uint16_t*)raw) + ((IMAGE_H - (WIDTH+2*THICKNESS))/2)*IMAGE_W;
	for (int i = 0; i<WIDTH+2*THICKNESS; i++) {
		image+=((IMAGE_W - (HEIGHT+2*THICKNESS))/2);
		for(int j =0; j< THICKNESS; j++) {
			*(image++) = FRAME_COLOR; //color
		}
		image+=HEIGHT;
		for(int j =0; j< THICKNESS; j++) {
			*(image++) = FRAME_COLOR; //color
		}
		image+=((IMAGE_W - (HEIGHT+2*THICKNESS))/2);
	}

#define X_START	45
#define Y_START	30


	MXC_TFT_ShowImageCameraRGB565(X_START, Y_START, raw, h, w);

}


int main(void)
{
  // Enable cache
  MXC_ICC_Enable(MXC_ICC0);

  // Switch to 100 MHz clock
  MXC_SYS_Clock_Select(MXC_SYS_CLOCK_IPO);
  SystemCoreClockUpdate();

  // Initialize TFT display.
  printf("Init LCD.\n");
  init_touchscreen(); // GC helper function
  MXC_TFT_ClearScreen();
  // MXC_TFT_ShowImage(0, 0, img_1_bmp);

  // Initialize camera.
  printf("Init Camera.\n");
  camera_init();
  set_image_dimensions(64,64); // GC helper function
  
  // set camera registers with default values
	for (int i = 0; (camera_settings[i][0] != 0xee); i++) {
		camera_write_reg(camera_settings[i][0], camera_settings[i][1]);
	}
  // Setup the camera image dimensions, pixel format and data acquiring details.
	int ret = camera_setup(get_image_x(), get_image_y(), PIXFORMAT_RGB888, FIFO_THREE_BYTE, USE_DMA);
	if (ret != STATUS_OK) {
		printf("Error returned from setting up camera. Error %d\n", ret);
		return -1;
	}
  // MXC_Delay(1000000);
  // MXC_TFT_SetPalette(logo_white_bg_darkgrey_bmp);
  MXC_TFT_SetBackGroundColor(4);

  // MXC_TFT_ShowImage(1, 1, logo_white_bg_darkgrey_bmp);

  memset(buff,32,TFT_BUFF_SIZE);
  // sprintf(buff, "MAXIM INTEGRATED             ");
  // TFT_Print(buff, 55, 50, urw_gothic_13_white_bg_grey);

  sprintf(buff, "Camera Interface        ");
  TFT_Print(buff, 55, 90, urw_gothic_12_white_bg_grey);

  sprintf(buff, "PRESS PB1 TO START!          ");
  TFT_Print(buff, 55, 130, urw_gothic_13_white_bg_grey);

  int frame = 0;

  while (1) 
  {
    // capture an image by pressing a button
    #ifdef CAPTURE_IMAGE
    printf("********** Press PB1 to capture an image **********\r\n");
    while(!PB_Get(0));
    MXC_TFT_ClearScreen();

    //MXC_TFT_ShowImage(1, 1, logo_white_bg_darkgrey_bmp);
    sprintf(buff, "CAPTURING IMAGE....           ");
    TFT_Print(buff, 55, 110, urw_gothic_13_white_bg_grey);

    // Capture a single camera frame.
    printf("\nCapture a camera frame %d\n", ++frame);
    capture_camera_img();

    // Copy the image data to the CNN input arrays.
    printf("Copy camera frame to CNN input buffers.\n");
    process_camera_img(input_0_camera, input_1_camera, input_2_camera);
    //process_img();
    // Show the input data on the lcd.
    MXC_TFT_ClearScreen();
    // MXC_TFT_ShowImage(1, 1, logo_white_bg_darkgrey_bmp);
    printf("Show camera frame on LCD.\n");
    lcd_show_sampledata(input_0_camera, input_1_camera, input_2_camera, 4096);

    sprintf(buff, "PRESS PB1 TO CAPTURE IMAGE      ");
    TFT_Print(buff, 10, 210, urw_gothic_12_white_bg_grey);
    #endif


    // see a continuous image stream
    #ifdef CONTINUOUS_STREAM
    
    #endif
  }

  return 0;
}
