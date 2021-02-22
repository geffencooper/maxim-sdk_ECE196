/**
 * @file    main.c
 * @brief   SPI Master Demo
 * @details Shows Master loopback demo for SPI
 *          Read the printf() for instructions
 */

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
//#include "HiLetgo_ILI9341.h"
#include "mxc_delay.h"
#include "tft_fthr.h"
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
#include "pb.h"
#include "camera.h"
#include "camera_tft_funcs.h"
#include "dma.h"
#include "spi_reva_regs.h" 
#define TFT_BUFF_SIZE   50    // TFT buffer size
#define CAMERA_FREQ   (10 * 1000 * 1000)
// buffer for touch screen text
char buff[TFT_BUFF_SIZE];

int main(void)
{
    // Enable cache
  MXC_ICC_Enable(MXC_ICC0);

  // Switch to 100 MHz clock
  MXC_SYS_Clock_Select(MXC_SYS_CLOCK_IPO);
  SystemCoreClockUpdate();

  // Initialize DMA for camera interface
	MXC_DMA_Init();
	int dma_channel = MXC_DMA_AcquireChannel();
 //MXC_SPI0->ctrl2 |= MXC_F_SPI_REVA_CTRL2_CLKPHA;
    // MXC_SPI0->ctrl2 |= MXC_F_SPI_REVA_CTRL2_CLKPOL;

    int res = MXC_TFT_Init(MXC_SPI0, 1,NULL,NULL);
    // Initialize camera.
  printf("Init Camera.\n");
  camera_init(CAMERA_FREQ);

  set_image_dimensions(130, 130);
int ret = camera_setup(get_image_x(), get_image_y(), PIXFORMAT_RGB565, FIFO_FOUR_BYTE, USE_DMA, dma_channel);
	if (ret != STATUS_OK) 
  {
		printf("Error returned from setting up camera. Error %d\n", ret);
		return -1;
	}
area_t a = {50,50,30,30};
int8_t frame_buffer[10000];
MXC_TFT_SetRotation(2);
    while(1)
    {
        //printf("test\n");
        //MXC_Delay(50000);
        // MXC_TFT_FillRect(&a,RED);
        // a.w = 20;
        // a.x=0;
        // MXC_TFT_FillRect(&a,BLUE);
        // a.h = 20;
        // a.x=100;
        // MXC_TFT_FillRect(&a,GREEN);
        // a.y=0;
        // MXC_TFT_FillRect(&a,YELLOW);
        // a.y=50;
        // a.x=50;
        capture_camera_img();
        display_RGB565_img(50,50);
        //display_grayscale_img(0,0, frame_buffer);
    }
}
//      MXC_Delay(SEC(2));
//     //printf("start test\n");
//     // Enable cache
//   MXC_ICC_Enable(MXC_ICC0);

//   // Switch to 100 MHz clock
//   MXC_SYS_Clock_Select(MXC_SYS_CLOCK_IPO);
//   SystemCoreClockUpdate();

//   // Initialize DMA for camera interface
// 	MXC_DMA_Init();
// 	int dma_channel = MXC_DMA_AcquireChannel();
//     //MXC_ICC_Enable(MXC_ICC0);

//     /* Set system clock to 100 MHz */
//     //MXC_SYS_Clock_Select(MXC_SYS_CLOCK_IPO);
//     //SystemCoreClockUpdate();
//     // mxc_gpio_cfg_t bl_pin;
//     // mxc_gpio_cfg_t rst_pin;
//     //  /* Setup output pin. */
//     // rst_pin.port = MXC_GPIO1;
//     // rst_pin.mask = MXC_GPIO_PIN_6;
//     // rst_pin.pad = MXC_GPIO_PAD_NONE;
//     // rst_pin.func = MXC_GPIO_FUNC_OUT;
//     // /* Setup output pin. */
//     // bl_pin.port = MXC_GPIO0;
//     // bl_pin.mask = MXC_GPIO_PIN_9;
//     // bl_pin.pad = MXC_GPIO_PAD_NONE;
//     // bl_pin.func = MXC_GPIO_FUNC_OUT;
//     // MXC_GPIO_Config(&rst_pin);
//     // MXC_GPIO_Config(&bl_pin);

//     // MXC_GPIO_SetVSSEL(MXC_GPIO0, MXC_GPIO_VSSEL_VDDIOH, MXC_GPIO_PIN_9);
//     // MXC_GPIO_SetVSSEL(MXC_GPIO1, MXC_GPIO_VSSEL_VDDIOH, MXC_GPIO_PIN_6);
//     //MXC_GPIO_OutClr(MAX_GPIO)
// MXC_SPI0->ctrl2 |= MXC_F_SPI_REVA_CTRL2_CLKPHA;
//     MXC_SPI0->ctrl2 |= MXC_F_SPI_REVA_CTRL2_CLKPOL;
//     int res = MXC_TFT_Init(MXC_SPI0, 1,NULL,NULL);
//     printf("res: %i\n", res);
//     area_t a = {50,50,30,30};
//      // Initialize camera.
//   printf("Init Camera.\n");
//   camera_init(CAMERA_FREQ);
//   set_image_dimensions(200, 150);
//   int ret = camera_setup(get_image_x(), get_image_y(), PIXFORMAT_RGB565, FIFO_FOUR_BYTE, USE_DMA, dma_channel);
// 	if (ret != STATUS_OK) 
//   {
// 		printf("Error returned from setting up camera. Error %d\n", ret);
// 		return -1;
// 	}
//     //printf("test.\n");
//     MXC_TFT_SetBackGroundColor(4);
//     MXC_TFT_FillRect(&a,RED);
//     a.x=100;
//     MXC_TFT_FillRect(&a,RED);
//     while(1)
//     {
//         //printf("test.\n");
//       // capture_camera_img();
//     //display_RGB565_img(0,0);
//     }
    // while(1)
    // {
    //     //MXC_TFT_SetBackGroundColor(0x12);
    //     MXC_TFT_FillRect(&a,0x12);
    // }//MXC_TFT_WritePixel(50,50,5,5,0x0);
    // while(1)
    // {
    //      MXC_TFT_SetBackGroundColor(0x12);
    // MXC_TFT_WritePixel(50,50,5,5,0x0);
    // }
    //ILI_init();
//     while(1);
//     //uint8_t data[4] = {0x01,0x02,0x03,0x04};
//     uint16_t data = 0x0000;
//     // Main loop - empty the screen as a test.
// int tft_iter = 0;
// int tft_on = 0;
// // Set column range.
// ILI_send_command(0x2A);
// ILI_send_data((uint8_t*)&data, 2);
// data = 239;
// ILI_send_data((uint8_t*)&data, 2);
// // Set row range.
// ILI_send_command(0x2B);
// data = 0;
// ILI_send_data((uint8_t*)&data, 2);
// data = 319;
// ILI_send_data((uint8_t*)&data, 2);
// // Set 'write to RAM'
// ILI_send_command(0x2C);
// while (1) {
//   // Write 320 * 240 pixels.
//   for (tft_iter = 0; tft_iter < (320*240); ++tft_iter) {
//     // Write a 16-bit color.
//     if (tft_on) {
//         data = 0xF802;
//       ILI_send_data((uint8_t*)&data, 2);
//     }
//     else {
//         data = 0x001F;
//       ILI_send_data((uint8_t*)&data, 2);
//     }
//   }
//   tft_on = !tft_on;
//}
#ifdef MASTERDMA
        MXC_DMA_ReleaseChannel(0);
        MXC_DMA_ReleaseChannel(1);
        
        NVIC_EnableIRQ(DMA0_IRQn);
        NVIC_EnableIRQ(DMA1_IRQn);
        MXC_SPI_MasterTransactionDMA(&req);
        
        while (DMA_FLAG == 0);
        
        DMA_FLAG = 0;
#endif
//}
