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
uint32_t input_0_camera[1024];
uint32_t input_1_camera[1024];
uint32_t input_2_camera[1024];

// buffer for touch screen text
char buff[TFT_BUFF_SIZE];

int main(void)
{
  // Enable cache
  MXC_ICC_Enable(MXC_ICC0);

  // Switch to 100 MHz clock
  MXC_SYS_Clock_Select(MXC_SYS_CLOCK_IPO);
  SystemCoreClockUpdate();

  // Initialize TFT display.
  printf("Init LCD.\n");
  init_LCD(); // GC helper function
  MXC_TFT_ClearScreen();

	/* Set the screen rotation */
	MXC_TFT_SetRotation(SCREEN_ROTATE);
  // MXC_TFT_ShowImage(0, 0, img_1_bmp);

  // Initialize camera.
  printf("Init Camera.\n");
  camera_init();
  set_image_dimensions(200,150); // GC helper function
  
  // Setup the camera image dimensions, pixel format and data acquiring details.
	int ret = camera_setup(get_image_x(), get_image_y(), PIXFORMAT_RGB565, FIFO_FOUR_BYTE, USE_DMA);
	if (ret != STATUS_OK) 
  {
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

  //sprintf(buff, "Camera Interface        ");
  //TFT_Print(buff, 55, 90, urw_gothic_12_white_bg_grey);

  //sprintf(buff, "PRESS PB1 TO START!          ");
  //TFT_Print(buff, 55, 130, urw_gothic_13_white_bg_grey);

  int frame = 0;

  while (1) 
  {
    // capture an image by pressing a button
    #ifdef CAPTURE_IMAGE
    printf("********** Press PB1 to capture an image **********\r\n");
    //while(!PB_Get(0));
    //MXC_TFT_ClearScreen();

    //MXC_TFT_ShowImage(1, 1, logo_white_bg_darkgrey_bmp);
   // sprintf(buff, "CAPTURING IMAGE....           ");
    //TFT_Print(buff, 55, 110, urw_gothic_13_white_bg_grey);

    // Capture a single camera frame.
    printf("\nCapture a camera frame %d\n", ++frame);
    capture_camera_img();

    // Copy the image data to the CNN input arrays.
    printf("Copy camera frame to CNN input buffers.\n");
    //process_camera_img(input_0_camera, input_1_camera, input_2_camera);
    display_RGB565_img(0,0);
    // Show the input data on the lcd.
    //MXC_TFT_ClearScreen();
    // MXC_TFT_ShowImage(1, 1, logo_white_bg_darkgrey_bmp);
    printf("Show camera frame on LCD.\n");
    //lcd_show_sampledata(input_0_camera, input_1_camera, input_2_camera, 4096);

    //sprintf(buff, "PRESS PB1 TO CAPTURE IMAGE      ");
    //TFT_Print(buff, 10, 210, urw_gothic_12_white_bg_grey);
    #endif


    // see a continuous image stream
    #ifdef CONTINUOUS_STREAM
    
    #endif
  }

  return 0;
}
