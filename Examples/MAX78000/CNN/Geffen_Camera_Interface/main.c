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
#include "tmr.h"
#include "tft.h"
#include "pb.h"
#include "tornadocnn.h"
#include "mxc_delay.h"
#include "camera.h"
#include "bitmap.h"

/***** Definitions *****/
#define CAMERA_TO_LCD   (1)
#define IMAGE_SIZE_X  (64)
#define IMAGE_SIZE_Y  (64)

#define TFT_BUFF_SIZE   50    // TFT buffer size

/***** Globals *****/
uint32_t input_0_camera[1024];
uint32_t input_1_camera[1024];
uint32_t input_2_camera[1024];

/***** Functions *****/
/* **************************************************************************** */
static uint8_t signed_to_unsigned(int8_t val) {
        uint8_t value;
        if (val < 0) {
                value = ~val + 1;
                return (128 - value);
        }
        return val + 128;
}

/* **************************************************************************** */
int8_t unsigned_to_signed(uint8_t val) {
        return val - 128;
}

/* **************************************************************************** */
void TFT_Print(char *str, int x, int y, int font) {
  // fonts id
  text_t text;
  text.data = str;
  text.len = 36;

  MXC_TFT_PrintFont(x, y, font, &text, NULL);
}

/* **************************************************************************** */
void lcd_show_sampledata(uint32_t *data0, uint32_t *data1, uint32_t *data2, int length) {
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
  for (i = 0; i < length; i++) {
    ptr0 = (uint8_t *)&data0[i];
    ptr1 = (uint8_t *)&data1[i];
    ptr2 = (uint8_t *)&data2[i];
    for (j = 0; j < 4; j++) {
      r = ptr0[j];
      g = ptr1[j];
      b = ptr2[j];        
      color  = (0x01000100 | ((b & 0xF8) << 13) | ((g & 0x1C) << 19) | ((g & 0xE0) >> 5) | (r & 0xF8));
      MXC_TFT_WritePixel(x * scale, y * scale, scale, scale, color);
      x += 1;
      if (x >= (IMAGE_SIZE_X + 47)) {
        x = 47;
        y += 1;
        if ((y + 6) >= (IMAGE_SIZE_Y + 15)) return;
      }
    }
  }
}

/* **************************************************************************** */
void process_camera_img(uint32_t *data0, uint32_t *data1, uint32_t *data2)
{
 	uint8_t   *frame_buffer;
	uint32_t  imgLen;
	uint32_t  w, h, x, y;
  uint8_t *ptr0;
  uint8_t *ptr1;
  uint8_t *ptr2;
  uint8_t *buffer;

	camera_get_image(&frame_buffer, &imgLen, &w, &h);
  ptr0 = (uint8_t *)data0;
  ptr1 = (uint8_t *)data1;
  ptr2 = (uint8_t *)data2;
  buffer = frame_buffer;
  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++, ptr0++, ptr1++, ptr2++) {
      *ptr0 = (*buffer); buffer++;
      *ptr1 = (*buffer); buffer++;
      *ptr2 = (*buffer); buffer++;
    }
  }
}

/* **************************************************************************** */
void capture_camera_img(void) {
  camera_start_capture_image();
  while (1) {
    if (camera_is_image_rcv()) {
      return;
    }
  }
}

/* **************************************************************************** */
void convert_img_unsigned_to_signed(uint32_t *data0, uint32_t *data1, uint32_t *data2) {
  uint8_t *ptr0;
  uint8_t *ptr1;
  uint8_t *ptr2;
  ptr0 = (uint8_t *)data0;
  ptr1 = (uint8_t *)data1;
  ptr2 = (uint8_t *)data2;
  for(int i=0; i<4096; i++) {
    *ptr0 = unsigned_to_signed(*ptr0); ptr0++;
    *ptr1 = unsigned_to_signed(*ptr1); ptr1++;
    *ptr2 = unsigned_to_signed(*ptr2); ptr2++;
  }
}

/* **************************************************************************** */
void convert_img_signed_to_unsigned(uint32_t *data0, uint32_t *data1, uint32_t *data2) {
  uint8_t *ptr0;
  uint8_t *ptr1;
  uint8_t *ptr2;
  ptr0 = (uint8_t *)data0;
  ptr1 = (uint8_t *)data1;
  ptr2 = (uint8_t *)data2;
  for(int i=0; i<4096; i++) {
    *ptr0 = signed_to_unsigned(*ptr0); ptr0++;
    *ptr1 = signed_to_unsigned(*ptr1); ptr1++;
    *ptr2 = signed_to_unsigned(*ptr2); ptr2++;
  }
}

/* **************************************************************************** */
int main(void)
{
  int ret = 0;
  char buff[TFT_BUFF_SIZE];

  MXC_ICC_Enable(MXC_ICC0); // Enable cache

  // Switch to 100 MHz clock
  MXC_SYS_Clock_Select(MXC_SYS_CLOCK_IPO);
  SystemCoreClockUpdate();

  // Configure P2.5, turn on the CNN Boost
  mxc_gpio_cfg_t gpio_out;
  gpio_out.port = MXC_GPIO2;
  gpio_out.mask = MXC_GPIO_PIN_5;
  gpio_out.pad = MXC_GPIO_PAD_NONE;
  gpio_out.func = MXC_GPIO_FUNC_OUT;
  MXC_GPIO_Config(&gpio_out);
  MXC_GPIO_OutSet(gpio_out.port, gpio_out.mask);

  // Initialize TFT display.
  printf("Init LCD.\n");
  mxc_gpio_cfg_t tft_reset_pin = {MXC_GPIO0, MXC_GPIO_PIN_19, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIOH};
  MXC_TFT_Init(MXC_SPI0, 1, &tft_reset_pin, NULL);
  MXC_TFT_ClearScreen();
  MXC_TFT_ShowImage(0, 0, img_1_bmp);

  // Initialize camera.
  printf("Init Camera.\n");
  camera_init();

  ret = camera_setup(IMAGE_SIZE_X, IMAGE_SIZE_Y, PIXFORMAT_RGB888, FIFO_THREE_BYTE, USE_DMA);
  if (ret != STATUS_OK) {
      printf("Error returned from setting up camera. Error %d\n", ret);
      return -1;
  }

  MXC_Delay(1000000);
  MXC_TFT_SetPalette(logo_white_bg_darkgrey_bmp);
  MXC_TFT_SetBackGroundColor(4);

  MXC_TFT_ShowImage(1, 1, logo_white_bg_darkgrey_bmp);

  memset(buff,32,TFT_BUFF_SIZE);
  sprintf(buff, "MAXIM INTEGRATED             ");
  TFT_Print(buff, 55, 50, urw_gothic_13_white_bg_grey);

  sprintf(buff, "Camera Interface        ");
  TFT_Print(buff, 55, 90, urw_gothic_12_white_bg_grey);

  sprintf(buff, "PRESS PB1 TO START!          ");
  TFT_Print(buff, 55, 130, urw_gothic_13_white_bg_grey);

  int frame = 0;

  while (1) {
    printf("********** Press PB1 to capture an image **********\r\n");
    while(!PB_Get(0));
    MXC_TFT_ClearScreen();
    MXC_TFT_ShowImage(1, 1, logo_white_bg_darkgrey_bmp);
    sprintf(buff, "CAPTURING IMAGE....           ");
    TFT_Print(buff, 55, 110, urw_gothic_13_white_bg_grey);

    // Capture a single camera frame.
    printf("\nCapture a camera frame %d\n", ++frame);
    capture_camera_img();
    // Copy the image data to the CNN input arrays.
    printf("Copy camera frame to CNN input buffers.\n");
    process_camera_img(input_0_camera, input_1_camera, input_2_camera);

    // Show the input data on the lcd.
    MXC_TFT_ClearScreen();
    MXC_TFT_ShowImage(1, 1, logo_white_bg_darkgrey_bmp);
    printf("Show camera frame on LCD.\n");
    lcd_show_sampledata(input_0_camera, input_1_camera, input_2_camera, 1024);

    sprintf(buff, "PRESS PB1 TO CAPTURE IMAGE      ");
    TFT_Print(buff, 10, 210, urw_gothic_12_white_bg_grey);
  }

  return 0;
}
