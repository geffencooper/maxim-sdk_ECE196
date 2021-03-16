#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "mxc_device.h"
#include "mxc_delay.h"
#include "nvic_table.h"
#include "i2c_regs.h"
#include "i2c.h"

// ========================================================================================= //
// ===================================== MACROS ============================================ //
// ========================================================================================= //

#define TX_SIZE 1
#define RX_SIZE 3
#define SENSOR_ADDRESS 0x5A
#define FREQUENCY 50000
#define TEMPERATURE_REGISTER 0x07


// ========================================================================================= //
// =================================== Global Variables ==================================== //
// ========================================================================================= //
mxc_i2c_req_t i2c_transaction;
uint8_t tx_buffer[TX_SIZE] = {0};
uint8_t rx_buffer[RX_SIZE] = {0};


// ========================================================================================= //
// ================================ FUNCTION DEFINITIONS =================================== //
// ========================================================================================= //

// callback function when I2C transaction complete
static void transaction_complete(mxc_i2c_req_t* req, int result)
{
    // if(result != 0)
    // {
    //     printf("error occured\n");
    // }
    // for(int i = 0; i < 3; i++)
    // {
    //     printf("%02X ", rx_buffer[i]);
    // }
}


// ========================================================================================= //


void init_IR_temp_sensor()
{
    int ret = MXC_I2C_Init(MXC_I2C1, 1, SENSOR_ADDRESS);
    if(ret != 0)
    {
       // printf("I2C init failed\n");
    }
    ret = MXC_I2C_SetFrequency(MXC_I2C1, FREQUENCY);
    if(ret < 0)
    {
        //printf("fail: %i\n", ret);
    }

    i2c_transaction.i2c = MXC_I2C1;
    i2c_transaction.addr = SENSOR_ADDRESS;
    i2c_transaction.tx_buf = tx_buffer;
    i2c_transaction.tx_len = TX_SIZE;
    i2c_transaction.rx_buf = rx_buffer;
    i2c_transaction.rx_len = RX_SIZE;
    i2c_transaction.restart = 0;
    i2c_transaction.callback = transaction_complete;
}

float get_temp()
{
    tx_buffer[0] = 0x07;
    //printf("starting transaction\n");
    int ret = MXC_I2C_MasterTransaction(&i2c_transaction);
    if(ret != 0)
    {
        // #include "i2c_reva.h"
        // printf("transaction failed: %i\n", MXC_I2C_REVA_ERROR & MXC_F_I2C_REVA_INTFL0_ADDR_NACK_ERR);
    }
    else
    {
        uint16_t bytes;
        float temp;
        bytes = (rx_buffer[1] << 8) | rx_buffer[0]; // reverse bytes
        temp = ((((float)bytes*0.02)-273.15)*9 / 5)+32; // convert from Kelvin to C then F
        return temp;
        //printf("temp:%f\n", temp);
        //printf("\033[0;0f");
        // for(int i = 0; i < 3; i++)
        // {
        //     printf("%02X ", rx_buffer[i]);
        // }
        // printf("\n");
    }
    return 0;
}