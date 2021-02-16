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

// geffen_faces
// Created using ./ai8xize.py --verbose --log --test-dir sdk/Examples/MAX78000/CNN --prefix geffen_faces --checkpoint-file trained/classifier_q.pth.tar --config-file networks/geffen_face_classifier.yaml --softmax --device MAX78000 --compact-data --mexpress --timer 0 --display-checkpoint

// DO NOT EDIT - regenerate this file instead!

// Configuring 6 layers:
// Layer 0: 1x64x64 (CHW data), no pooling, conv2d with kernel size 3x3, stride 1/1, pad 1/1, 30x64x64 output
// Layer 1: 30x64x64 (HWC data), 2x2 max pool with stride 2/2, conv2d with kernel size 3x3, stride 1/1, pad 1/1, 30x32x32 output
// Layer 2: 30x32x32 (HWC data), 2x2 max pool with stride 2/2, conv2d with kernel size 3x3, stride 1/1, pad 1/1, 30x16x16 output
// Layer 3: 30x16x16 (HWC data), 2x2 max pool with stride 2/2, conv2d with kernel size 3x3, stride 1/1, pad 1/1, 30x8x8 output
// Layer 4: 30x8x8 (HWC data), 2x2 max pool with stride 2/2, conv2d with kernel size 3x3, stride 1/1, pad 1/1, 10x4x4 output
// Layer 5: 10x4x4 (flattened HWC data), no pooling, conv2d with kernel size 1x1, stride 1/1, pad 0/0, 2x1x1 output

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "mxc.h"
#include "gcfr_regs.h"
#include "cnn.h"
#include "weights.h"

void CNN_ISR(void)
{
  // Acknowledge interrupt to all groups
  *((volatile uint32_t *) 0x50100000) &= ~((1<<12) | 1);
  *((volatile uint32_t *) 0x50500000) &= ~((1<<12) | 1);

  CNN_COMPLETE; // Signal that processing is complete
#ifdef CNN_INFERENCE_TIMER
  cnn_time = MXC_TMR_SW_Stop(CNN_INFERENCE_TIMER);
#else
  cnn_time = 1;
#endif
}

int cnn_continue(void)
{
  cnn_time = 0;

  *((volatile uint32_t *) 0x50100000) |= 1; // Re-enable group 0

  return CNN_OK;
}

int cnn_stop(void)
{
  *((volatile uint32_t *) 0x50100000) &= ~1; // Disable group 0

  return CNN_OK;
}

void memcpy32(uint32_t *dst, const uint32_t *src, int n)
{
  while (n-- > 0) {
    *dst++ = *src++;
  }
}

// Kernels:
static const uint32_t kernels_0[] = KERNELS_0;
static const uint32_t kernels_1[] = KERNELS_1;
static const uint32_t kernels_2[] = KERNELS_2;
static const uint32_t kernels_3[] = KERNELS_3;
static const uint32_t kernels_4[] = KERNELS_4;
static const uint32_t kernels_5[] = KERNELS_5;
static const uint32_t kernels_6[] = KERNELS_6;
static const uint32_t kernels_7[] = KERNELS_7;
static const uint32_t kernels_8[] = KERNELS_8;
static const uint32_t kernels_9[] = KERNELS_9;
static const uint32_t kernels_10[] = KERNELS_10;
static const uint32_t kernels_11[] = KERNELS_11;
static const uint32_t kernels_12[] = KERNELS_12;
static const uint32_t kernels_13[] = KERNELS_13;
static const uint32_t kernels_14[] = KERNELS_14;
static const uint32_t kernels_15[] = KERNELS_15;
static const uint32_t kernels_16[] = KERNELS_16;
static const uint32_t kernels_17[] = KERNELS_17;
static const uint32_t kernels_18[] = KERNELS_18;
static const uint32_t kernels_19[] = KERNELS_19;
static const uint32_t kernels_20[] = KERNELS_20;
static const uint32_t kernels_21[] = KERNELS_21;
static const uint32_t kernels_22[] = KERNELS_22;
static const uint32_t kernels_23[] = KERNELS_23;
static const uint32_t kernels_24[] = KERNELS_24;
static const uint32_t kernels_25[] = KERNELS_25;
static const uint32_t kernels_26[] = KERNELS_26;
static const uint32_t kernels_27[] = KERNELS_27;
static const uint32_t kernels_28[] = KERNELS_28;
static const uint32_t kernels_29[] = KERNELS_29;
static const uint32_t kernels_30[] = KERNELS_30;
static const uint32_t kernels_31[] = KERNELS_31;

int cnn_load_weights(void)
{
  *((volatile uint8_t *) 0x50180001) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50180000, kernels_0, 324);
  *((volatile uint8_t *) 0x50184081) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50184000, kernels_1, 252);
  *((volatile uint8_t *) 0x50188231) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50188000, kernels_2, 9);
  *((volatile uint8_t *) 0x5018c231) = 0x01; // Set address
  memcpy32((uint32_t *) 0x5018c000, kernels_3, 9);
  *((volatile uint8_t *) 0x50190081) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50190000, kernels_4, 252);
  *((volatile uint8_t *) 0x50194081) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50194000, kernels_5, 252);
  *((volatile uint8_t *) 0x50198081) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50198000, kernels_6, 252);
  *((volatile uint8_t *) 0x5019c081) = 0x01; // Set address
  memcpy32((uint32_t *) 0x5019c000, kernels_7, 252);
  *((volatile uint8_t *) 0x501a0081) = 0x01; // Set address
  memcpy32((uint32_t *) 0x501a0000, kernels_8, 252);
  *((volatile uint8_t *) 0x501a4081) = 0x01; // Set address
  memcpy32((uint32_t *) 0x501a4000, kernels_9, 252);
  *((volatile uint8_t *) 0x501a8081) = 0x01; // Set address
  memcpy32((uint32_t *) 0x501a8000, kernels_10, 239);
  *((volatile uint8_t *) 0x501ac081) = 0x01; // Set address
  memcpy32((uint32_t *) 0x501ac000, kernels_11, 239);
  *((volatile uint8_t *) 0x501b0081) = 0x01; // Set address
  memcpy32((uint32_t *) 0x501b0000, kernels_12, 239);
  *((volatile uint8_t *) 0x501b4081) = 0x01; // Set address
  memcpy32((uint32_t *) 0x501b4000, kernels_13, 239);
  *((volatile uint8_t *) 0x501b8081) = 0x01; // Set address
  memcpy32((uint32_t *) 0x501b8000, kernels_14, 239);
  *((volatile uint8_t *) 0x501bc081) = 0x01; // Set address
  memcpy32((uint32_t *) 0x501bc000, kernels_15, 239);
  *((volatile uint8_t *) 0x50580081) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50580000, kernels_16, 239);
  *((volatile uint8_t *) 0x50584081) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50584000, kernels_17, 239);
  *((volatile uint8_t *) 0x50588081) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50588000, kernels_18, 239);
  *((volatile uint8_t *) 0x5058c081) = 0x01; // Set address
  memcpy32((uint32_t *) 0x5058c000, kernels_19, 239);
  *((volatile uint8_t *) 0x50590081) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50590000, kernels_20, 239);
  *((volatile uint8_t *) 0x50594081) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50594000, kernels_21, 239);
  *((volatile uint8_t *) 0x50598081) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50598000, kernels_22, 239);
  *((volatile uint8_t *) 0x5059c081) = 0x01; // Set address
  memcpy32((uint32_t *) 0x5059c000, kernels_23, 239);
  *((volatile uint8_t *) 0x505a0081) = 0x01; // Set address
  memcpy32((uint32_t *) 0x505a0000, kernels_24, 239);
  *((volatile uint8_t *) 0x505a4081) = 0x01; // Set address
  memcpy32((uint32_t *) 0x505a4000, kernels_25, 239);
  *((volatile uint8_t *) 0x505a8081) = 0x01; // Set address
  memcpy32((uint32_t *) 0x505a8000, kernels_26, 239);
  *((volatile uint8_t *) 0x505ac081) = 0x01; // Set address
  memcpy32((uint32_t *) 0x505ac000, kernels_27, 239);
  *((volatile uint8_t *) 0x505b0081) = 0x01; // Set address
  memcpy32((uint32_t *) 0x505b0000, kernels_28, 239);
  *((volatile uint8_t *) 0x505b4081) = 0x01; // Set address
  memcpy32((uint32_t *) 0x505b4000, kernels_29, 239);
  *((volatile uint8_t *) 0x505b8081) = 0x01; // Set address
  memcpy32((uint32_t *) 0x505b8000, kernels_30, 239);
  *((volatile uint8_t *) 0x505bc081) = 0x01; // Set address
  memcpy32((uint32_t *) 0x505bc000, kernels_31, 239);

  return CNN_OK;
}

static const uint8_t bias_0[] = BIAS_0;

static void memcpy_8to32(uint32_t *dst, const uint8_t *src, int n)
{
  while (n-- > 0) {
    *dst++ = *src++;
  }
}

int cnn_load_bias(void)
{
  memcpy_8to32((uint32_t *) 0x50108000, bias_0, sizeof(uint8_t) * 2);

  return CNN_OK;
}

int cnn_init(void)
{
  *((volatile uint32_t *) 0x50001000) = 0x00000000; // AON control
  *((volatile uint32_t *) 0x50100000) = 0x00100008; // Stop SM
  *((volatile uint32_t *) 0x50100004) = 0x0000040e; // SRAM control
  *((volatile uint32_t *) 0x50100008) = 0x00000005; // Layer count
  *((volatile uint32_t *) 0x50500000) = 0x00100008; // Stop SM
  *((volatile uint32_t *) 0x50500004) = 0x0000040e; // SRAM control
  *((volatile uint32_t *) 0x50500008) = 0x00000005; // Layer count

  return CNN_OK;
}

int cnn_configure(void)
{
  // Layer 0 group 0
  *((volatile uint32_t *) 0x50100010) = 0x00010041; // Rows
  *((volatile uint32_t *) 0x50100090) = 0x00010041; // Columns
  *((volatile uint32_t *) 0x50100310) = 0x00000800; // SRAM write ptr
  *((volatile uint32_t *) 0x50100410) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50100590) = 0x00000360; // Layer control
  *((volatile uint32_t *) 0x50100a10) = 0x0000f800; // Layer control 2
  *((volatile uint32_t *) 0x50100610) = 0x000000f8; // Mask offset and count
  *((volatile uint32_t *) 0x50100690) = 0x0000003f; // TRAM ptr max
  *((volatile uint32_t *) 0x50100790) = 0x00022000; // Post processing register
  *((volatile uint32_t *) 0x50100710) = 0x00010001; // Mask and processor enables

  // Layer 0 group 1
  *((volatile uint32_t *) 0x50500010) = 0x00010041; // Rows
  *((volatile uint32_t *) 0x50500090) = 0x00010041; // Columns
  *((volatile uint32_t *) 0x50500310) = 0x00002800; // SRAM write ptr
  *((volatile uint32_t *) 0x50500410) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50500590) = 0x00000360; // Layer control
  *((volatile uint32_t *) 0x50500a10) = 0x0000f800; // Layer control 2
  *((volatile uint32_t *) 0x50500610) = 0x000000f8; // Mask offset and count
  *((volatile uint32_t *) 0x50500690) = 0x0000003f; // TRAM ptr max
  *((volatile uint32_t *) 0x50500790) = 0x00022000; // Post processing register

  // Layer 1 group 0
  *((volatile uint32_t *) 0x50100014) = 0x00010041; // Rows
  *((volatile uint32_t *) 0x50100094) = 0x00010041; // Columns
  *((volatile uint32_t *) 0x50100194) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x50100214) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x50100294) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x50100414) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50100514) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x50100594) = 0x000023a0; // Layer control
  *((volatile uint32_t *) 0x50100a14) = 0x0000f800; // Layer control 2
  *((volatile uint32_t *) 0x50100614) = 0x010001f8; // Mask offset and count
  *((volatile uint32_t *) 0x50100694) = 0x0000001f; // TRAM ptr max
  *((volatile uint32_t *) 0x50100794) = 0x00024000; // Post processing register
  *((volatile uint32_t *) 0x50100714) = 0xfff3fff3; // Mask and processor enables

  // Layer 1 group 1
  *((volatile uint32_t *) 0x50500014) = 0x00010041; // Rows
  *((volatile uint32_t *) 0x50500094) = 0x00010041; // Columns
  *((volatile uint32_t *) 0x50500194) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x50500214) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x50500294) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x50500314) = 0x00002000; // SRAM write ptr
  *((volatile uint32_t *) 0x50500414) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50500514) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x50500594) = 0x000003a0; // Layer control
  *((volatile uint32_t *) 0x50500a14) = 0x0000f800; // Layer control 2
  *((volatile uint32_t *) 0x50500614) = 0x010001f8; // Mask offset and count
  *((volatile uint32_t *) 0x50500694) = 0x0000001f; // TRAM ptr max
  *((volatile uint32_t *) 0x50500794) = 0x00024000; // Post processing register
  *((volatile uint32_t *) 0x50500714) = 0xffffffff; // Mask and processor enables

  // Layer 2 group 0
  *((volatile uint32_t *) 0x50100018) = 0x00010021; // Rows
  *((volatile uint32_t *) 0x50100098) = 0x00010021; // Columns
  *((volatile uint32_t *) 0x50100198) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x50100218) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x50100298) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x50100318) = 0x00000800; // SRAM write ptr
  *((volatile uint32_t *) 0x50100418) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50100598) = 0x000023a0; // Layer control
  *((volatile uint32_t *) 0x50100a18) = 0x0000f800; // Layer control 2
  *((volatile uint32_t *) 0x50100618) = 0x020002f8; // Mask offset and count
  *((volatile uint32_t *) 0x50100698) = 0x0000000f; // TRAM ptr max
  *((volatile uint32_t *) 0x50100798) = 0x00024000; // Post processing register
  *((volatile uint32_t *) 0x50100718) = 0xfff3fff3; // Mask and processor enables

  // Layer 2 group 1
  *((volatile uint32_t *) 0x50500018) = 0x00010021; // Rows
  *((volatile uint32_t *) 0x50500098) = 0x00010021; // Columns
  *((volatile uint32_t *) 0x50500198) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x50500218) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x50500298) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x50500318) = 0x00002800; // SRAM write ptr
  *((volatile uint32_t *) 0x50500418) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50500598) = 0x000003a0; // Layer control
  *((volatile uint32_t *) 0x50500a18) = 0x0000f800; // Layer control 2
  *((volatile uint32_t *) 0x50500618) = 0x020002f8; // Mask offset and count
  *((volatile uint32_t *) 0x50500698) = 0x0000000f; // TRAM ptr max
  *((volatile uint32_t *) 0x50500798) = 0x00024000; // Post processing register
  *((volatile uint32_t *) 0x50500718) = 0xffffffff; // Mask and processor enables

  // Layer 3 group 0
  *((volatile uint32_t *) 0x5010001c) = 0x00010011; // Rows
  *((volatile uint32_t *) 0x5010009c) = 0x00010011; // Columns
  *((volatile uint32_t *) 0x5010019c) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x5010021c) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x5010029c) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x5010041c) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5010051c) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x5010059c) = 0x000023a0; // Layer control
  *((volatile uint32_t *) 0x50100a1c) = 0x0000f800; // Layer control 2
  *((volatile uint32_t *) 0x5010061c) = 0x030003f8; // Mask offset and count
  *((volatile uint32_t *) 0x5010069c) = 0x00000007; // TRAM ptr max
  *((volatile uint32_t *) 0x5010079c) = 0x00024000; // Post processing register
  *((volatile uint32_t *) 0x5010071c) = 0xfff3fff3; // Mask and processor enables

  // Layer 3 group 1
  *((volatile uint32_t *) 0x5050001c) = 0x00010011; // Rows
  *((volatile uint32_t *) 0x5050009c) = 0x00010011; // Columns
  *((volatile uint32_t *) 0x5050019c) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x5050021c) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x5050029c) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x5050031c) = 0x00002000; // SRAM write ptr
  *((volatile uint32_t *) 0x5050041c) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5050051c) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x5050059c) = 0x000003a0; // Layer control
  *((volatile uint32_t *) 0x50500a1c) = 0x0000f800; // Layer control 2
  *((volatile uint32_t *) 0x5050061c) = 0x030003f8; // Mask offset and count
  *((volatile uint32_t *) 0x5050069c) = 0x00000007; // TRAM ptr max
  *((volatile uint32_t *) 0x5050079c) = 0x00024000; // Post processing register
  *((volatile uint32_t *) 0x5050071c) = 0xffffffff; // Mask and processor enables

  // Layer 4 group 0
  *((volatile uint32_t *) 0x50100020) = 0x00010009; // Rows
  *((volatile uint32_t *) 0x501000a0) = 0x00010009; // Columns
  *((volatile uint32_t *) 0x501001a0) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x50100220) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x501002a0) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x50100320) = 0x00000800; // SRAM write ptr
  *((volatile uint32_t *) 0x50100420) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x501005a0) = 0x00002ba0; // Layer control
  *((volatile uint32_t *) 0x50100a20) = 0x00004800; // Layer control 2
  *((volatile uint32_t *) 0x50100620) = 0x04000448; // Mask offset and count
  *((volatile uint32_t *) 0x501006a0) = 0x00000003; // TRAM ptr max
  *((volatile uint32_t *) 0x501007a0) = 0x00022000; // Post processing register
  *((volatile uint32_t *) 0x50100720) = 0xfff3fff3; // Mask and processor enables

  // Layer 4 group 1
  *((volatile uint32_t *) 0x50500020) = 0x00010009; // Rows
  *((volatile uint32_t *) 0x505000a0) = 0x00010009; // Columns
  *((volatile uint32_t *) 0x505001a0) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x50500220) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x505002a0) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x50500320) = 0x00000800; // SRAM write ptr
  *((volatile uint32_t *) 0x50500420) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x505005a0) = 0x00000ba0; // Layer control
  *((volatile uint32_t *) 0x50500a20) = 0x00004800; // Layer control 2
  *((volatile uint32_t *) 0x50500620) = 0x04000448; // Mask offset and count
  *((volatile uint32_t *) 0x505006a0) = 0x00000003; // TRAM ptr max
  *((volatile uint32_t *) 0x505007a0) = 0x00022000; // Post processing register
  *((volatile uint32_t *) 0x50500720) = 0xffffffff; // Mask and processor enables

  // Layer 5 group 0
  *((volatile uint32_t *) 0x50100324) = 0x00000400; // SRAM write ptr
  *((volatile uint32_t *) 0x501003a4) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50100424) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50100524) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x501005a4) = 0x00010920; // Layer control
  *((volatile uint32_t *) 0x50100a24) = 0x0000080f; // Layer control 2
  *((volatile uint32_t *) 0x50100624) = 0x27602858; // Mask offset and count
  *((volatile uint32_t *) 0x50100124) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x501007a4) = 0x00023000; // Post processing register
  *((volatile uint32_t *) 0x50100724) = 0x03ff03ff; // Mask and processor enables

  // Layer 5 group 1
  *((volatile uint32_t *) 0x50500324) = 0x00000400; // SRAM write ptr
  *((volatile uint32_t *) 0x505003a4) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50500424) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50500524) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x505005a4) = 0x00010920; // Layer control
  *((volatile uint32_t *) 0x50500a24) = 0x0000080f; // Layer control 2
  *((volatile uint32_t *) 0x50500624) = 0x27602858; // Mask offset and count
  *((volatile uint32_t *) 0x50500124) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x505007a4) = 0x00022000; // Post processing register


  return CNN_OK;
}

int cnn_start(void)
{
  cnn_time = 0;

  *((volatile uint32_t *) 0x50100000) = 0x00100808; // Enable group 0
  *((volatile uint32_t *) 0x50500000) = 0x00100809; // Enable group 1

#ifdef CNN_INFERENCE_TIMER
  MXC_TMR_SW_Start(CNN_INFERENCE_TIMER);
#endif

  CNN_START; // Allow capture of processing time
  *((volatile uint32_t *) 0x50100000) = 0x00100009; // Master enable group 0

  return CNN_OK;
}

// Custom unload for this network: 32-bit data, shape: [2, 1, 1]
int cnn_unload(uint32_t *out_buf)
{
  volatile uint32_t *addr;
  addr = (volatile uint32_t *) 0x50401000;
  *out_buf++ = *addr++;
  *out_buf++ = *addr++;

  return CNN_OK;
}

int cnn_enable(uint32_t clock_source, uint32_t clock_divider)
{
  // Reset all domains, restore power to CNN
  MXC_GCFR->reg3 = 0xf; // Reset
  MXC_GCFR->reg1 = 0x3; // Mask memory
  MXC_GCFR->reg0 = 0x3; // Power
  MXC_GCFR->reg2 = 0xc; // Iso
  MXC_GCFR->reg3 = 0x0; // Reset

  MXC_GCR->pclkdiv = (MXC_GCR->pclkdiv & ~(MXC_F_GCR_PCLKDIV_CNNCLKDIV | MXC_F_GCR_PCLKDIV_CNNCLKSEL))
                     | clock_divider | clock_source;
  MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_CNN); // Enable CNN clock

  NVIC_SetVector(CNN_IRQn, CNN_ISR); // Set CNN complete vector

  return CNN_OK;
}

int cnn_boost_enable(mxc_gpio_regs_t *port, uint32_t pin)
{
  mxc_gpio_cfg_t gpio_out;
  gpio_out.port = port;
  gpio_out.mask = pin;
  gpio_out.pad = MXC_GPIO_PAD_NONE;
  gpio_out.func = MXC_GPIO_FUNC_OUT;
  MXC_GPIO_Config(&gpio_out);
  MXC_GPIO_OutSet(gpio_out.port, gpio_out.mask);

  return CNN_OK;
}

int cnn_boost_disable(mxc_gpio_regs_t *port, uint32_t pin)
{
  mxc_gpio_cfg_t gpio_out;
  gpio_out.port = port;
  gpio_out.mask = pin;
  gpio_out.pad = MXC_GPIO_PAD_NONE;
  gpio_out.func = MXC_GPIO_FUNC_OUT;
  MXC_GPIO_Config(&gpio_out);
  MXC_GPIO_OutSet(gpio_out.port, gpio_out.mask);

  return CNN_OK;
}

int cnn_disable(void)
{
  // Disable CNN clock
  MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_CNN);

  // Disable power to CNN
  MXC_GCFR->reg3 = 0xf; // Reset
  MXC_GCFR->reg1 = 0x0; // Mask memory
  MXC_GCFR->reg0 = 0x0; // Power
  MXC_GCFR->reg2 = 0xf; // Iso
  MXC_GCFR->reg3 = 0x0; // Reset

  return CNN_OK;
}

