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

/**
 * @file    camera_tft_funcs.c
 * @brief   camera and touch screen helper function
 * @details consolidate the Maxim Integrated provided functions into a separate file
 */

/***** Includes *****/
#include "camera_tft_funcs.h"
#include "tft.h"
#include "camera.h"

// variables
static int IMAGE_SIZE_X;
static int IMAGE_SIZE_Y;

mxc_gpio_cfg_t tft_reset_pin = {MXC_GPIO0, MXC_GPIO_PIN_19, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIOH};

/***** Camera Functions *****/

void set_image_dimensions(uint16_t x_dim, uint16_t y_dim)
{
    IMAGE_SIZE_X = x_dim;
    IMAGE_SIZE_Y = y_dim;
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


void process_camera_img(uint32_t *data0, uint32_t *data1, uint32_t *data2)
{
    uint8_t *frame_buffer;
    uint32_t imgLen;
    uint32_t w, h, x, y;
    uint8_t *ptr0;
    uint8_t *ptr1;
    uint8_t *ptr2;
    uint8_t *buffer;

    camera_get_image(&frame_buffer, &imgLen, &w, &h);
    ptr0 = (uint8_t *)data0;
    ptr1 = (uint8_t *)data1;
    ptr2 = (uint8_t *)data2;
    buffer = frame_buffer;
    for (y = 0; y < h; y++) 
    {
        for (x = 0; x < w; x++, ptr0++, ptr1++, ptr2++) 
        {
            *ptr0 = (*buffer); buffer++;
            *ptr1 = (*buffer); buffer++;
            *ptr2 = (*buffer); buffer++;
        }
    }
}


void process_img(int x_coord, int y_coord)
{
	uint8_t   *raw;
	uint32_t  imgLen;
	uint32_t  w, h;

  // Get the details of the image from the camera driver.
	camera_get_image(&raw, &imgLen, &w, &h);
	MXC_TFT_ShowImageCameraRGB565(x_coord, y_coord, raw, h, w);
}

/***** Touch Screen Functions *****/
void init_touchscreen()
{
  printf("Init LCD.\n");
  MXC_TFT_Init(MXC_SPI0, 1, &tft_reset_pin, NULL);
  MXC_TFT_ClearScreen();
}


void lcd_show_sampledata(uint32_t *data0, uint32_t *data1, uint32_t *data2, int length) 
{
  int i;
  int j;
  int x;
  int y;
  int r;
  int g;
  int b;
  int scale = 2.2;
    
  uint32_t color;
  uint8_t *ptr0;
  uint8_t *ptr1;
  uint8_t *ptr2;

  x = 47;
  y = 15;
  for (i = 0; i < length; i++) 
  {
    ptr0 = (uint8_t *)&data0[i];
    ptr1 = (uint8_t *)&data1[i];
    ptr2 = (uint8_t *)&data2[i];
    for (j = 0; j < 4; j++) 
    {
      r = ptr0[j];
      g = ptr1[j];
      b = ptr2[j];        
      color  = (0x01000100 | ((b & 0xF8) << 13) | ((g & 0x1C) << 19) | ((g & 0xE0) >> 5) | (r & 0xF8));
      MXC_TFT_WritePixel(x * scale, y * scale, scale, scale, color);
      x += 1;
      if (x >= (IMAGE_SIZE_X + 47)) 
      {
        x = 47;
        y += 1;
        if ((y + 6) >= (IMAGE_SIZE_Y + 15))
        { 
            return;
        }
      }
    }
  }
}


void TFT_Print(char *str, int x, int y, int font) 
{
  // fonts id
  text_t text;
  text.data = str;
  text.len = 36;

  MXC_TFT_PrintFont(x, y, font, &text, NULL);
}




