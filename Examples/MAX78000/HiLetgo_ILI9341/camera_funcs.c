/*******************************************************************************
* Copyright (C) Maxim Integrated Products, Inc., All Rights Reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
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
*
******************************************************************************/

// This file defines helper functions for interacting with the camera
// Most of these come from the Maxim Integrated Drivers

#include "camera_funcs.h"
#include "camera.h"
#include "dma.h"

#define CAMERA_FREQ   (10 * 1000 * 1000)

// variables
static int IMAGE_SIZE_X;
static int IMAGE_SIZE_Y;

int init_camera_sensor(uint16_t x_dim, uint16_t y_dim)
{
    IMAGE_SIZE_X = x_dim;
    IMAGE_SIZE_Y = y_dim;

    // Initialize DMA for camera interface
  	MXC_DMA_Init();
  	int dma_channel = MXC_DMA_AcquireChannel();

    // Initialize camera.
    printf("Init Camera.\n");
    camera_init(CAMERA_FREQ);

    int ret = camera_setup(x_dim, y_dim, PIXFORMAT_RGB565, FIFO_FOUR_BYTE, USE_DMA, dma_channel);
	if (ret != STATUS_OK) 
    {
		printf("Error returned from setting up camera. Error %d\n", ret);
		return -1;
	}
}

int get_image_x()
{
    return  IMAGE_SIZE_X;
}


int get_image_y()
{
    return  IMAGE_SIZE_Y;
}

void capture_camera_img(void) 
{
  camera_start_capture_image();
  while (1) 
  {
    if (camera_is_image_rcv()) 
    {
      return;
    }
  }
}