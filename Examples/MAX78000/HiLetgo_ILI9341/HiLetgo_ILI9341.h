#ifndef HILETGO_ILI9341_H
#define HILETGO_ILI9341_H

#include "stdint.h"

void ILI_init();
void ILI_send_command(uint8_t command);
void ILI_send_data(uint8_t* address, uint8_t num_bytes);
#endif