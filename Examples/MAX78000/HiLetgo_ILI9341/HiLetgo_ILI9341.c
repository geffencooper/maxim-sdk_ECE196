
/***** Includes *****/
#include "HiLetgo_ILI9341.h"
#include "HiLetgo_ILI9341_macros.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "mxc_device.h"
#include "mxc_delay.h"
#include "mxc_pins.h"
#include "nvic_table.h"
#include "uart.h"
#include "spi.h"
#include "dma.h"
#include "board.h"

#include "spi_reva_regs.h" 

/***** Definitions *****/
#define DATA_LEN        100         // Words
#define DATA_VALUE      0xA5A5      // This is for master mode only...
#define VALUE           0xFFFF
#define SPI_SPEED       1000000      // Bit Rate

/***** Globals *****/
uint16_t tx_data[DATA_LEN];
uint16_t rx_data[DATA_LEN];
volatile int TX_available;
volatile uint8_t DMA_FLAG = 0;
mxc_spi_pins_t spi_pins;
mxc_spi_req_t req;
mxc_gpio_cfg_t LCD_DC_pin;

/***** Functions *****/
//#define SPI_IRQ     SPI0_IRQn
void SPI0_IRQHandler(void)
{
    MXC_SPI_AsyncHandler(MXC_SPI0);
}

void DMA0_IRQHandler(void)
{
    MXC_DMA_Handler();
}

void DMA1_IRQHandler(void)
{
    MXC_DMA_Handler();
    DMA_FLAG = 1;
}

void SPI_Callback(mxc_spi_req_t* req, int error)
{
    TX_available = error;
}

void ILI_init()
{
    /* Setup output pin. */
    LCD_DC_pin.port = MXC_GPIO1;
    LCD_DC_pin.mask = MXC_GPIO_PIN_6;
    LCD_DC_pin.pad = MXC_GPIO_PAD_NONE;
    LCD_DC_pin.func = MXC_GPIO_FUNC_OUT;
    MXC_GPIO_Config(&LCD_DC_pin);

    printf("initializing SPI\n");
    // initialize the SPI configurations

    spi_pins.clock = TRUE;
    spi_pins.miso = TRUE;
    spi_pins.mosi = TRUE;
    spi_pins.sdio2 = FALSE;
    spi_pins.sdio3 = FALSE;
    spi_pins.ss0 = FALSE;
    spi_pins.ss1 = TRUE; // pin 11 is available on the board
    spi_pins.ss2 = FALSE;

    for (int j = 0; j < DATA_LEN; j++)
    {
        tx_data[j] = DATA_VALUE;
    }

    // Configure the peripheral
    if (MXC_SPI_Init(MXC_SPI0, 1, 0, 1, 0, SPI_SPEED, spi_pins) != E_NO_ERROR) 
    {
        printf("\nSPI INITIALIZATION ERROR\n");
        
        while (1) {}
    }

    // LCD clock configuration: shift on first edge (falling) and sample on second edge (rising)
    //MXC_SPI0->ctrl2 |= MXC_F_SPI_REVA_CTRL2_CLKPHA;
    //MXC_SPI0->ctrl2 |= MXC_F_SPI_REVA_CTRL2_CLKPOL;
   

    //SPI Request
    req.spi = MXC_SPI0;
    req.txData = (uint8_t*) tx_data;
    req.rxData = (uint8_t*) rx_data;
    req.txLen = DATA_LEN;
    req.rxLen = DATA_LEN;
    req.ssIdx = 1;
    req.ssDeassert = 1;
    req.txCnt = 0;
    req.rxCnt = 0;
    req.completeCB = (spi_complete_cb_t) SPI_Callback;
    TX_available = 1;
    
    int retVal = MXC_SPI_SetDataSize(MXC_SPI0, 8);
    
    if (retVal != E_NO_ERROR) 
    {
        printf("\nSPI SET DATASIZE ERROR: %d\n", retVal);
        
        while (1) {}
    }
    
    retVal = MXC_SPI_SetWidth(MXC_SPI0, SPI_WIDTH_STANDARD);
    
    if (retVal != E_NO_ERROR) 
    {
        printf("\nSPI SET WIDTH ERROR: %d\n", retVal);
        
        while (1) {}
    }
    NVIC_EnableIRQ(SPI0_IRQn);
    printf("spi initialized with no errors\n");
    printf("spi phase and polarity: %i\n", MXC_SPI0->ctrl2 & 3);

// high to send data
    
printf("reset display\n");
MXC_GPIO_OutSet(LCD_DC_pin.port, LCD_DC_pin.mask);
ILI_send_command(ILI9341_SWRESET);
MXC_Delay(5000);
    uint8_t data = 0;
  ILI_send_command(0xCB);
  data= 0x39;
  ILI_send_data(&data,1);
  data=0x2C;
  ILI_send_data(&data,1);
  data=0x00;
  ILI_send_data(&data,1);
  data=0x34;
  ILI_send_data(&data,1);
  data=0x02;
  ILI_send_data(&data,1);
  
  ILI_send_command(0xCF);
  data=0x00;
  ILI_send_data(&data,1);
  data=0xC1;
  ILI_send_data(&data,1);
  data=0x30;
  ILI_send_data(&data,1);
  
  ILI_send_command(0xE8);
  data=0x85;
  ILI_send_data(&data,1);
  data=0x00;
  ILI_send_data(&data,1);
  data=0x78;
  ILI_send_data(&data,1);

  ILI_send_command(0xEA);
  data=0x00;
  ILI_send_data(&data,1);
  data=0x00;
  ILI_send_data(&data,1);

  ILI_send_command(0xED);
  data=0x64;
  ILI_send_data(&data,1);
  data=0x03;
  ILI_send_data(&data,1);
  data=0x12;
  ILI_send_data(&data,1);
  data=0x81;
  ILI_send_data(&data,1);
  
  
  ILI_send_command(0xF7);
  data=0x20;
  ILI_send_data(&data,1);
  
  
  // PWCTR1
  ILI_send_command(0xC0);
  data=0x23;
  ILI_send_data(&data,1);
  // PWCTR2
  ILI_send_command(0xC1);
  data=0x10;
  ILI_send_data(&data,1);
  // VMCTR1
  ILI_send_command(0xC5);
  data=0x3E;
  ILI_send_data(&data,1);
  data=0x28;
  ILI_send_data(&data,1);
  // VMCTR2
  ILI_send_command(0xC7);
  data=0x86;
  ILI_send_data(&data,1);
  // MADCTL
  ILI_send_command(0x36);
  data=0x48;
  ILI_send_data(&data,1);

  ILI_send_command(0x3A);
  data=0x55;
  ILI_send_data(&data,1);

  ILI_send_command(0xB1);
  data=0x00;
  ILI_send_data(&data,1);
  data=0x18;
  ILI_send_data(&data,1);

  // DFUNCTR
  ILI_send_command(0xB6);
  data=0x08;
  ILI_send_data(&data,1);
  data=0x82;
  ILI_send_data(&data,1);
  data=0x27;
  ILI_send_data(&data,1);
  
//   // VSCRSADD
//   ILI_send_command(0x37);
//   data=0x00;
//   ILI_send_data(&data,1);
//   // PIXFMT
  

//   ILI_send_command(0xF2);
//   data=0x00;
//   ILI_send_data(&data,1);
//   // GAMMASET
//   ILI_send_command(0x26);
//   data=0x01;
//   ILI_send_data(&data,1);
//   // (Actual gamma settings)
//   ILI_send_command(0xE0);
//   data=0x0F;
//   ILI_send_data(&data,1);
//   data=0x31;
//   ILI_send_data(&data,1);
//   data=0x2B;
//   ILI_send_data(&data,1);
//   data=0x0C;
//   ILI_send_data(&data,1);
//   data=0x0E;
//   ILI_send_data(&data,1);
//   data=0x08;
//   ILI_send_data(&data,1);
//   data=0x4E;
//   ILI_send_data(&data,1);
//   data=0xF1;
//   ILI_send_data(&data,1);
//   data=0x37;
//   ILI_send_data(&data,1);
//   data=0x07;
//   ILI_send_data(&data,1);
//   data=0x10;
//   ILI_send_data(&data,1);
//   data=0x03;
//   ILI_send_data(&data,1);
//   data=0x0E;
//   ILI_send_data(&data,1);
//   data=0x09;
//   ILI_send_data(&data,1);
//   data=0x00;
//   ILI_send_data(&data,1);
//   data=0xE1;
//   ILI_send_command(0xE1);
//   data=0x00;
//   ILI_send_data(&data,1);
//   data=0x0E;
//   ILI_send_data(&data,1);
//   data=0x14;
//   ILI_send_data(&data,1);
//   data=0x03;
//   ILI_send_data(&data,1);
//   data=0x11;
//   ILI_send_data(&data,1);
//   data=0x07;
//   ILI_send_data(&data,1);
//   data=0x31;
//   ILI_send_data(&data,1);
//   data=0xC1;
//   ILI_send_data(&data,1);
//   data=0x48;
//   ILI_send_data(&data,1);
//   data=0x08;
//   ILI_send_data(&data,1);
//   data=0x0F;
//   ILI_send_data(&data,1);
//   data=0x0C;
//   ILI_send_data(&data,1);
//   data=0x31;
//   ILI_send_data(&data,1);
//   data=0x36;
//   ILI_send_data(&data,1);
//   data=0x0F;
//   ILI_send_data(&data,1);
  // Exit sleep mode.
  ILI_send_command(0x11);
  MXC_Delay(10000);
  // Display on.
  ILI_send_command(0x29);
  ILI_send_command(0x2C);
  //MXC_Delay(200000);
  // 'Normal' display mode.
  //ILI_send_command(0x13);
}

void ILI_send_command(uint8_t command)
{
    while(TX_available == -1){}
    //printf("sending command byte\n");
    req.txLen=1;
    req.rxLen = req.txLen;
    memcpy(tx_data, &command, 1);
    // low to send command
    MXC_GPIO_OutClr(LCD_DC_pin.port, LCD_DC_pin.mask);
    int ret = MXC_SPI_MasterTransactionAsync(&req);
    TX_available = -1;
    while(TX_available == -1){}

    // high to send data
    MXC_GPIO_OutSet(LCD_DC_pin.port, LCD_DC_pin.mask);
    //printf("status: %i\n", ret);
    //MXC_Delay(25);
}

void ILI_send_data(uint8_t* data, uint8_t num_bytes)
{    
    if(num_bytes != 0 && data != NULL)
    {
        while(TX_available == -1){}
       // printf("sending data bytes\n");
        req.txLen=num_bytes;
        req.rxLen = req.txLen;
        memcpy(tx_data, data, num_bytes);
        int ret = MXC_SPI_MasterTransactionAsync(&req);
        TX_available = -1;
        //printf("status: %i\n", ret);
    }
    //MXC_Delay(25);
    
   // printf("transaction complete: %i\n", SPI_FLAG);
    
}