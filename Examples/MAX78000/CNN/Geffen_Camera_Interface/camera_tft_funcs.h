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
 * @file    camera_tft_funcs.h
 * @brief   camera and touch screen helper function
 * @details organize the maxim provided functions into a separate file
 */

#ifndef CAMERA_TFT_FUNCS
#define CAMERA_TFT_FUNCS

/***** Includes *****/
#include <stdint.h>

/***** Camera Functions *****/
void set_image_dimensions(uint16_t x_dim, uint16_t y_dim);

int get_image_x();

int get_image_y();

void capture_camera_img(void);

void process_camera_img(uint32_t *data0, uint32_t *data1, uint32_t *data2);

void process_img(int x_coord, int y_coord);

/***** Touch Screen Functions *****/
void init_touchscreen();

void lcd_show_sampledata(uint32_t *data0, uint32_t *data1, uint32_t *data2, int length);

void TFT_Print(char *str, int x, int y, int font);

#endif