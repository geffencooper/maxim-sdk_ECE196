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

// This file defines the functions needed to interact with the LCD 

#ifndef HILETGO_ILI9341_H
#define HILETGO_ILI9341_H

#include "stdint.h"

/*
    Description: This function initializes the SPI drivers
                 and sends the startup commands to the LCD.

    Parameters: none

    Return: none
*/
void init_ILI_LCD();

/*
    Description: This function is called once the camera has captured
                 an image. It gets a pointer the raw frame buffer
                 and passes it to MXC_TFT_ShowImageCameraRGB565() which
                 writes the image data to the LCD display using RGB565. RGB565
                 means that the RGB channels are 5, 6, and 5 bits accordingly.
                 Overall this is an even 16 bits (2 bytes). This method is much
                 faster than RGB888, use this for continuous streaming.

    Parameters: The location to display the image on the LCD (top left corner)

    Return: none
*/
void display_RGB565_img(int x_coord, int y_coord);


/*
    Description: This function writes text to the LCD

    Parameters: Pointer to the text buffer, location, and font

    Return: none
*/
void TFT_Print(char *str, int x, int y, int font, int length);

/*
    Description: This function is similar to display_RGB565_img
                 except it first extracts the luminance value from
                 the camera data and displays it as grayscale by 
                 each channel equal weight. Refer to the defenition
                for more details.

    Parameters: The location to display the image on the LCD (top left corner)
                and a buffer that gets sent to the CNN

    Return: none
*/
void display_grayscale_img(int x_coord, int y_coord, int8_t* cnn_buffer);
// void ILI_init();
// void ILI_send_command(uint8_t command);
// void ILI_send_data(uint8_t* address, uint8_t num_bytes);
#endif