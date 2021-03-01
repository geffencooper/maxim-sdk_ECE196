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

#ifndef CAMERA_FUNCS_H
#define CAMERA_FUNCS_H

#include <stdint.h>

/*
    Description: This function initializes the camera sensor.
                 The max is (X, Y) = (320, 240). Beyond, (200, 150)
                 there is noticeable update latency for the LCD.

    Parameters: The desired X and Y dimensions

    Return: -1 if initialization failed
*/
int init_camera_sensor(uint16_t x_dim, uint16_t y_dim);

/*
    Description: get the x dimension of the image

    Parameters: none

    Return: x dimension of the image
*/
int get_image_x();


/*
    Description: get the y dimension of the image

    Parameters: none

    Return: y dimension of the image
*/
int get_image_y();


/*
    Description: This is a blocking function that starts an image
                 capture and only returns once the camera has
                 captured the image

    Parameters: none

    Return: none
*/
void capture_camera_img(void);


#endif