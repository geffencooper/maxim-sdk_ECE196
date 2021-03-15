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

// main() is left fairly empty and only initializes the state machine and starts it.
// The main loop is in scanner_state_machine.c

#include "scanner_state_machine.h"
#include "mxc.h"
#include "mxc_device.h"
#include "mxc_sys.h"
#include "stdio.h"
#include <string.h>

int main(void)
{
  // DO NOT DELETE THIS LINE:
  MXC_Delay(SEC(2)); // Let debugger interrupt if needed

  // // Enable cache
  // MXC_ICC_Enable(MXC_ICC0);

  // // Switch to 100 MHz clock
  // MXC_SYS_Clock_Select(MXC_SYS_CLOCK_IPO);
  // SystemCoreClockUpdate();

  // init_ILI_LCD();

  // MXC_TFT_SetRotation(ROTATE_180);
  // MXC_TFT_SetBackGroundColor(RED);
  // memset(buff2,32, TFT_buff2_SIZE);
	// TFT_Print(buff2, 0, 200, (int)&SansSerif19x19[0], sprintf(buff2, "STATE:"));
  // TFT_Print(buff2, 90, 200, (int)&SansSerif16x16[0], sprintf(buff2, "MEASUREMENT"));
  // init_IR_temp_sensor();
  // while(1)
  // {
  //   tx_data();
  // }
  // initialize the state machine and the peripherals
  printf("initializing state machine\n");
  int ret = init_ssm();
  if(ret < 0)
  {
    printf("Error: could not initialize facial scanner. Check error messages.\n");
    return -1;
  }

  // start the state machine
  execute_ssm();
}
#ifdef MASTERDMA
        MXC_DMA_ReleaseChannel(0);
        MXC_DMA_ReleaseChannel(1);
        
        NVIC_EnableIRQ(DMA0_IRQn);
        NVIC_EnableIRQ(DMA1_IRQn);
        MXC_SPI_MasterTransactionDMA(&req);
        
        while (DMA_FLAG == 0);
        
        DMA_FLAG = 0;
#endif
