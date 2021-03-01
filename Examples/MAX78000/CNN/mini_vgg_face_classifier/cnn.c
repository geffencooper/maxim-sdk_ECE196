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

// mini_vgg_face_classifier
// Created using ./ai8xize.py --verbose --log --test-dir sdk/Examples/MAX78000/CNN --prefix mini_vgg_face_classifier --checkpoint-file trained/test_q.pth.tar --config-file networks/mini_vgg_net.yaml --device MAX78000 --softmax --compact-data --mexpress --timer 0 --display-checkpoint

// DO NOT EDIT - regenerate this file instead!

// Configuring 12 layers:
// Layer 0: 1x80x80 (CHW data), no pooling, conv2d with kernel size 3x3, stride 1/1, pad 1/1, 8x80x80 output
// Layer 1: 8x80x80 (HWC data), no pooling, conv2d with kernel size 3x3, stride 1/1, pad 1/1, 8x80x80 output
// Layer 2: 8x80x80 (HWC data), 2x2 max pool with stride 2/2, conv2d with kernel size 3x3, stride 1/1, pad 1/1, 16x40x40 output
// Layer 3: 16x40x40 (HWC data), no pooling, conv2d with kernel size 3x3, stride 1/1, pad 1/1, 16x40x40 output
// Layer 4: 16x40x40 (HWC data), 2x2 max pool with stride 2/2, conv2d with kernel size 3x3, stride 1/1, pad 1/1, 32x20x20 output
// Layer 5: 32x20x20 (HWC data), no pooling, conv2d with kernel size 3x3, stride 1/1, pad 1/1, 32x20x20 output
// Layer 6: 32x20x20 (HWC data), 2x2 max pool with stride 2/2, conv2d with kernel size 3x3, stride 1/1, pad 2/2, 64x12x12 output
// Layer 7: 64x12x12 (HWC data), no pooling, conv2d with kernel size 3x3, stride 1/1, pad 1/1, 64x12x12 output
// Layer 8: 64x12x12 (HWC data), 2x2 max pool with stride 2/2, conv2d with kernel size 3x3, stride 1/1, pad 1/1, 64x6x6 output
// Layer 9: 64x6x6 (HWC data), 2x2 max pool with stride 2/2, conv2d with kernel size 3x3, stride 1/1, pad 1/1, 64x3x3 output
// Layer 10: 64x3x3 (flattened HWC data), no pooling, conv2d with kernel size 1x1, stride 1/1, pad 0/0, 10x1x1 output
// Layer 11: 10x1x1 (flattened HWC data), no pooling, conv2d with kernel size 1x1, stride 1/1, pad 0/0, 2x1x1 output

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
  *((volatile uint32_t *) 0x50900000) &= ~((1<<12) | 1);
  *((volatile uint32_t *) 0x50d00000) &= ~((1<<12) | 1);

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
static const uint32_t kernels_32[] = KERNELS_32;
static const uint32_t kernels_33[] = KERNELS_33;
static const uint32_t kernels_34[] = KERNELS_34;
static const uint32_t kernels_35[] = KERNELS_35;
static const uint32_t kernels_36[] = KERNELS_36;
static const uint32_t kernels_37[] = KERNELS_37;
static const uint32_t kernels_38[] = KERNELS_38;
static const uint32_t kernels_39[] = KERNELS_39;
static const uint32_t kernels_40[] = KERNELS_40;
static const uint32_t kernels_41[] = KERNELS_41;
static const uint32_t kernels_42[] = KERNELS_42;
static const uint32_t kernels_43[] = KERNELS_43;
static const uint32_t kernels_44[] = KERNELS_44;
static const uint32_t kernels_45[] = KERNELS_45;
static const uint32_t kernels_46[] = KERNELS_46;
static const uint32_t kernels_47[] = KERNELS_47;
static const uint32_t kernels_48[] = KERNELS_48;
static const uint32_t kernels_49[] = KERNELS_49;
static const uint32_t kernels_50[] = KERNELS_50;
static const uint32_t kernels_51[] = KERNELS_51;
static const uint32_t kernels_52[] = KERNELS_52;
static const uint32_t kernels_53[] = KERNELS_53;
static const uint32_t kernels_54[] = KERNELS_54;
static const uint32_t kernels_55[] = KERNELS_55;
static const uint32_t kernels_56[] = KERNELS_56;
static const uint32_t kernels_57[] = KERNELS_57;
static const uint32_t kernels_58[] = KERNELS_58;
static const uint32_t kernels_59[] = KERNELS_59;
static const uint32_t kernels_60[] = KERNELS_60;
static const uint32_t kernels_61[] = KERNELS_61;
static const uint32_t kernels_62[] = KERNELS_62;
static const uint32_t kernels_63[] = KERNELS_63;

int cnn_load_weights(void)
{
  *((volatile uint8_t *) 0x50180001) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50180000, kernels_0, 779);
  *((volatile uint8_t *) 0x50184041) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50184000, kernels_1, 743);
  *((volatile uint8_t *) 0x50188041) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50188000, kernels_2, 743);
  *((volatile uint8_t *) 0x5018c041) = 0x01; // Set address
  memcpy32((uint32_t *) 0x5018c000, kernels_3, 743);
  *((volatile uint8_t *) 0x50190001) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50190000, kernels_4, 779);
  *((volatile uint8_t *) 0x50194001) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50194000, kernels_5, 779);
  *((volatile uint8_t *) 0x50198001) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50198000, kernels_6, 779);
  *((volatile uint8_t *) 0x5019c001) = 0x01; // Set address
  memcpy32((uint32_t *) 0x5019c000, kernels_7, 779);
  *((volatile uint8_t *) 0x501a0001) = 0x01; // Set address
  memcpy32((uint32_t *) 0x501a0000, kernels_8, 779);
  *((volatile uint8_t *) 0x501a4001) = 0x01; // Set address
  memcpy32((uint32_t *) 0x501a4000, kernels_9, 779);
  *((volatile uint8_t *) 0x501a8001) = 0x01; // Set address
  memcpy32((uint32_t *) 0x501a8000, kernels_10, 779);
  *((volatile uint8_t *) 0x501ac001) = 0x01; // Set address
  memcpy32((uint32_t *) 0x501ac000, kernels_11, 779);
  *((volatile uint8_t *) 0x501b0001) = 0x01; // Set address
  memcpy32((uint32_t *) 0x501b0000, kernels_12, 779);
  *((volatile uint8_t *) 0x501b4001) = 0x01; // Set address
  memcpy32((uint32_t *) 0x501b4000, kernels_13, 779);
  *((volatile uint8_t *) 0x501b8001) = 0x01; // Set address
  memcpy32((uint32_t *) 0x501b8000, kernels_14, 779);
  *((volatile uint8_t *) 0x501bc001) = 0x01; // Set address
  memcpy32((uint32_t *) 0x501bc000, kernels_15, 779);
  *((volatile uint8_t *) 0x50580001) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50580000, kernels_16, 779);
  *((volatile uint8_t *) 0x50584001) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50584000, kernels_17, 779);
  *((volatile uint8_t *) 0x50588001) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50588000, kernels_18, 779);
  *((volatile uint8_t *) 0x5058c001) = 0x01; // Set address
  memcpy32((uint32_t *) 0x5058c000, kernels_19, 779);
  *((volatile uint8_t *) 0x505900c1) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50590000, kernels_20, 671);
  *((volatile uint8_t *) 0x505940c1) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50594000, kernels_21, 671);
  *((volatile uint8_t *) 0x505980c1) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50598000, kernels_22, 671);
  *((volatile uint8_t *) 0x5059c0c1) = 0x01; // Set address
  memcpy32((uint32_t *) 0x5059c000, kernels_23, 671);
  *((volatile uint8_t *) 0x505a0001) = 0x01; // Set address
  memcpy32((uint32_t *) 0x505a0000, kernels_24, 779);
  *((volatile uint8_t *) 0x505a4001) = 0x01; // Set address
  memcpy32((uint32_t *) 0x505a4000, kernels_25, 779);
  *((volatile uint8_t *) 0x505a8001) = 0x01; // Set address
  memcpy32((uint32_t *) 0x505a8000, kernels_26, 779);
  *((volatile uint8_t *) 0x505ac001) = 0x01; // Set address
  memcpy32((uint32_t *) 0x505ac000, kernels_27, 779);
  *((volatile uint8_t *) 0x505b0001) = 0x01; // Set address
  memcpy32((uint32_t *) 0x505b0000, kernels_28, 779);
  *((volatile uint8_t *) 0x505b4001) = 0x01; // Set address
  memcpy32((uint32_t *) 0x505b4000, kernels_29, 779);
  *((volatile uint8_t *) 0x505b8001) = 0x01; // Set address
  memcpy32((uint32_t *) 0x505b8000, kernels_30, 779);
  *((volatile uint8_t *) 0x505bc001) = 0x01; // Set address
  memcpy32((uint32_t *) 0x505bc000, kernels_31, 779);
  *((volatile uint8_t *) 0x50980001) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50980000, kernels_32, 779);
  *((volatile uint8_t *) 0x50984001) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50984000, kernels_33, 779);
  *((volatile uint8_t *) 0x50988001) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50988000, kernels_34, 779);
  *((volatile uint8_t *) 0x5098c001) = 0x01; // Set address
  memcpy32((uint32_t *) 0x5098c000, kernels_35, 779);
  *((volatile uint8_t *) 0x50990001) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50990000, kernels_36, 779);
  *((volatile uint8_t *) 0x50994001) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50994000, kernels_37, 779);
  *((volatile uint8_t *) 0x50998001) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50998000, kernels_38, 779);
  *((volatile uint8_t *) 0x5099c001) = 0x01; // Set address
  memcpy32((uint32_t *) 0x5099c000, kernels_39, 779);
  *((volatile uint8_t *) 0x509a0241) = 0x01; // Set address
  memcpy32((uint32_t *) 0x509a0000, kernels_40, 455);
  *((volatile uint8_t *) 0x509a4241) = 0x01; // Set address
  memcpy32((uint32_t *) 0x509a4000, kernels_41, 455);
  *((volatile uint8_t *) 0x509a8241) = 0x01; // Set address
  memcpy32((uint32_t *) 0x509a8000, kernels_42, 455);
  *((volatile uint8_t *) 0x509ac241) = 0x01; // Set address
  memcpy32((uint32_t *) 0x509ac000, kernels_43, 455);
  *((volatile uint8_t *) 0x509b0241) = 0x01; // Set address
  memcpy32((uint32_t *) 0x509b0000, kernels_44, 455);
  *((volatile uint8_t *) 0x509b4241) = 0x01; // Set address
  memcpy32((uint32_t *) 0x509b4000, kernels_45, 455);
  *((volatile uint8_t *) 0x509b8241) = 0x01; // Set address
  memcpy32((uint32_t *) 0x509b8000, kernels_46, 455);
  *((volatile uint8_t *) 0x509bc241) = 0x01; // Set address
  memcpy32((uint32_t *) 0x509bc000, kernels_47, 455);
  *((volatile uint8_t *) 0x50d80241) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50d80000, kernels_48, 455);
  *((volatile uint8_t *) 0x50d84241) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50d84000, kernels_49, 455);
  *((volatile uint8_t *) 0x50d88241) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50d88000, kernels_50, 455);
  *((volatile uint8_t *) 0x50d8c241) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50d8c000, kernels_51, 455);
  *((volatile uint8_t *) 0x50d90241) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50d90000, kernels_52, 462);
  *((volatile uint8_t *) 0x50d94241) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50d94000, kernels_53, 462);
  *((volatile uint8_t *) 0x50d98241) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50d98000, kernels_54, 462);
  *((volatile uint8_t *) 0x50d9c241) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50d9c000, kernels_55, 462);
  *((volatile uint8_t *) 0x50da0241) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50da0000, kernels_56, 462);
  *((volatile uint8_t *) 0x50da4241) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50da4000, kernels_57, 462);
  *((volatile uint8_t *) 0x50da8241) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50da8000, kernels_58, 462);
  *((volatile uint8_t *) 0x50dac241) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50dac000, kernels_59, 462);
  *((volatile uint8_t *) 0x50db0241) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50db0000, kernels_60, 462);
  *((volatile uint8_t *) 0x50db4241) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50db4000, kernels_61, 462);
  *((volatile uint8_t *) 0x50db8241) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50db8000, kernels_62, 455);
  *((volatile uint8_t *) 0x50dbc241) = 0x01; // Set address
  memcpy32((uint32_t *) 0x50dbc000, kernels_63, 455);

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
  memcpy_8to32((uint32_t *) 0x50108000, bias_0, sizeof(uint8_t) * 12);

  return CNN_OK;
}

int cnn_init(void)
{
  *((volatile uint32_t *) 0x50001000) = 0x00000000; // AON control
  *((volatile uint32_t *) 0x50100000) = 0x00100008; // Stop SM
  *((volatile uint32_t *) 0x50100004) = 0x0000040e; // SRAM control
  *((volatile uint32_t *) 0x50100008) = 0x0000000b; // Layer count
  *((volatile uint32_t *) 0x50500000) = 0x00100008; // Stop SM
  *((volatile uint32_t *) 0x50500004) = 0x0000040e; // SRAM control
  *((volatile uint32_t *) 0x50500008) = 0x0000000b; // Layer count
  *((volatile uint32_t *) 0x50900000) = 0x00100008; // Stop SM
  *((volatile uint32_t *) 0x50900004) = 0x0000040e; // SRAM control
  *((volatile uint32_t *) 0x50900008) = 0x0000000b; // Layer count
  *((volatile uint32_t *) 0x50d00000) = 0x00100008; // Stop SM
  *((volatile uint32_t *) 0x50d00004) = 0x0000040e; // SRAM control
  *((volatile uint32_t *) 0x50d00008) = 0x0000000b; // Layer count

  return CNN_OK;
}

int cnn_configure(void)
{
  // Layer 0 group 0
  *((volatile uint32_t *) 0x50100010) = 0x00010051; // Rows
  *((volatile uint32_t *) 0x50100090) = 0x00010051; // Columns
  *((volatile uint32_t *) 0x50100310) = 0x00002000; // SRAM write ptr
  *((volatile uint32_t *) 0x50100410) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50100590) = 0x00000b60; // Layer control
  *((volatile uint32_t *) 0x50100a10) = 0x00003800; // Layer control 2
  *((volatile uint32_t *) 0x50100610) = 0x00000038; // Mask offset and count
  *((volatile uint32_t *) 0x50100690) = 0x0000004f; // TRAM ptr max
  *((volatile uint32_t *) 0x50100710) = 0x00010001; // Mask and processor enables

  // Layer 0 group 1
  *((volatile uint32_t *) 0x50500010) = 0x00010051; // Rows
  *((volatile uint32_t *) 0x50500090) = 0x00010051; // Columns
  *((volatile uint32_t *) 0x50500310) = 0x00002000; // SRAM write ptr
  *((volatile uint32_t *) 0x50500410) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50500590) = 0x00000b60; // Layer control
  *((volatile uint32_t *) 0x50500a10) = 0x00003800; // Layer control 2
  *((volatile uint32_t *) 0x50500610) = 0x00000038; // Mask offset and count
  *((volatile uint32_t *) 0x50500690) = 0x0000004f; // TRAM ptr max

  // Layer 0 group 2
  *((volatile uint32_t *) 0x50900010) = 0x00010051; // Rows
  *((volatile uint32_t *) 0x50900090) = 0x00010051; // Columns
  *((volatile uint32_t *) 0x50900310) = 0x00002000; // SRAM write ptr
  *((volatile uint32_t *) 0x50900410) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50900590) = 0x00000b60; // Layer control
  *((volatile uint32_t *) 0x50900a10) = 0x00003800; // Layer control 2
  *((volatile uint32_t *) 0x50900610) = 0x00000038; // Mask offset and count
  *((volatile uint32_t *) 0x50900690) = 0x0000004f; // TRAM ptr max

  // Layer 0 group 3
  *((volatile uint32_t *) 0x50d00010) = 0x00010051; // Rows
  *((volatile uint32_t *) 0x50d00090) = 0x00010051; // Columns
  *((volatile uint32_t *) 0x50d00310) = 0x00002000; // SRAM write ptr
  *((volatile uint32_t *) 0x50d00410) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50d00590) = 0x00000b60; // Layer control
  *((volatile uint32_t *) 0x50d00a10) = 0x00003800; // Layer control 2
  *((volatile uint32_t *) 0x50d00610) = 0x00000038; // Mask offset and count
  *((volatile uint32_t *) 0x50d00690) = 0x0000004f; // TRAM ptr max

  // Layer 1 group 0
  *((volatile uint32_t *) 0x50100014) = 0x00010051; // Rows
  *((volatile uint32_t *) 0x50100094) = 0x00010051; // Columns
  *((volatile uint32_t *) 0x50100314) = 0x00006000; // SRAM write ptr
  *((volatile uint32_t *) 0x50100414) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50100594) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x50100a14) = 0x00003800; // Layer control 2
  *((volatile uint32_t *) 0x50100614) = 0x00000038; // Mask offset and count
  *((volatile uint32_t *) 0x50100694) = 0x0000004f; // TRAM ptr max
  *((volatile uint32_t *) 0x50100714) = 0x0ff00ff0; // Mask and processor enables

  // Layer 1 group 1
  *((volatile uint32_t *) 0x50500014) = 0x00010051; // Rows
  *((volatile uint32_t *) 0x50500094) = 0x00010051; // Columns
  *((volatile uint32_t *) 0x50500314) = 0x00006000; // SRAM write ptr
  *((volatile uint32_t *) 0x50500414) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50500594) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x50500a14) = 0x00003800; // Layer control 2
  *((volatile uint32_t *) 0x50500614) = 0x00000038; // Mask offset and count
  *((volatile uint32_t *) 0x50500694) = 0x0000004f; // TRAM ptr max

  // Layer 1 group 2
  *((volatile uint32_t *) 0x50900014) = 0x00010051; // Rows
  *((volatile uint32_t *) 0x50900094) = 0x00010051; // Columns
  *((volatile uint32_t *) 0x50900314) = 0x00006000; // SRAM write ptr
  *((volatile uint32_t *) 0x50900414) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50900594) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x50900a14) = 0x00003800; // Layer control 2
  *((volatile uint32_t *) 0x50900614) = 0x00000038; // Mask offset and count
  *((volatile uint32_t *) 0x50900694) = 0x0000004f; // TRAM ptr max

  // Layer 1 group 3
  *((volatile uint32_t *) 0x50d00014) = 0x00010051; // Rows
  *((volatile uint32_t *) 0x50d00094) = 0x00010051; // Columns
  *((volatile uint32_t *) 0x50d00314) = 0x00006000; // SRAM write ptr
  *((volatile uint32_t *) 0x50d00414) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50d00594) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x50d00a14) = 0x00003800; // Layer control 2
  *((volatile uint32_t *) 0x50d00614) = 0x00000038; // Mask offset and count
  *((volatile uint32_t *) 0x50d00694) = 0x0000004f; // TRAM ptr max

  // Layer 2 group 0
  *((volatile uint32_t *) 0x50100018) = 0x00010051; // Rows
  *((volatile uint32_t *) 0x50100098) = 0x00010051; // Columns
  *((volatile uint32_t *) 0x50100198) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x50100218) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x50100298) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x50100318) = 0x0000c000; // SRAM write ptr
  *((volatile uint32_t *) 0x50100418) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50100598) = 0x00002ba0; // Layer control
  *((volatile uint32_t *) 0x50100a18) = 0x00007800; // Layer control 2
  *((volatile uint32_t *) 0x50100618) = 0x00000078; // Mask offset and count
  *((volatile uint32_t *) 0x50100698) = 0x00000027; // TRAM ptr max
  *((volatile uint32_t *) 0x50100798) = 0x00022000; // Post processing register
  *((volatile uint32_t *) 0x50100718) = 0xf000f000; // Mask and processor enables

  // Layer 2 group 1
  *((volatile uint32_t *) 0x50500018) = 0x00010051; // Rows
  *((volatile uint32_t *) 0x50500098) = 0x00010051; // Columns
  *((volatile uint32_t *) 0x50500198) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x50500218) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x50500298) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x50500318) = 0x0000c000; // SRAM write ptr
  *((volatile uint32_t *) 0x50500418) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50500598) = 0x00000ba0; // Layer control
  *((volatile uint32_t *) 0x50500a18) = 0x00007800; // Layer control 2
  *((volatile uint32_t *) 0x50500618) = 0x00000078; // Mask offset and count
  *((volatile uint32_t *) 0x50500698) = 0x00000027; // TRAM ptr max
  *((volatile uint32_t *) 0x50500798) = 0x00022000; // Post processing register
  *((volatile uint32_t *) 0x50500718) = 0x000f000f; // Mask and processor enables

  // Layer 2 group 2
  *((volatile uint32_t *) 0x50900018) = 0x00010051; // Rows
  *((volatile uint32_t *) 0x50900098) = 0x00010051; // Columns
  *((volatile uint32_t *) 0x50900198) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x50900218) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x50900298) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x50900318) = 0x0000c000; // SRAM write ptr
  *((volatile uint32_t *) 0x50900418) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50900598) = 0x00000ba0; // Layer control
  *((volatile uint32_t *) 0x50900a18) = 0x00007800; // Layer control 2
  *((volatile uint32_t *) 0x50900618) = 0x00000078; // Mask offset and count
  *((volatile uint32_t *) 0x50900698) = 0x00000027; // TRAM ptr max
  *((volatile uint32_t *) 0x50900798) = 0x00022000; // Post processing register

  // Layer 2 group 3
  *((volatile uint32_t *) 0x50d00018) = 0x00010051; // Rows
  *((volatile uint32_t *) 0x50d00098) = 0x00010051; // Columns
  *((volatile uint32_t *) 0x50d00198) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x50d00218) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x50d00298) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x50d00318) = 0x0000c000; // SRAM write ptr
  *((volatile uint32_t *) 0x50d00418) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50d00598) = 0x00000ba0; // Layer control
  *((volatile uint32_t *) 0x50d00a18) = 0x00007800; // Layer control 2
  *((volatile uint32_t *) 0x50d00618) = 0x00000078; // Mask offset and count
  *((volatile uint32_t *) 0x50d00698) = 0x00000027; // TRAM ptr max
  *((volatile uint32_t *) 0x50d00798) = 0x00022000; // Post processing register

  // Layer 3 group 0
  *((volatile uint32_t *) 0x5010001c) = 0x00010029; // Rows
  *((volatile uint32_t *) 0x5010009c) = 0x00010029; // Columns
  *((volatile uint32_t *) 0x5010041c) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5010059c) = 0x00006b20; // Layer control
  *((volatile uint32_t *) 0x50100a1c) = 0x00007800; // Layer control 2
  *((volatile uint32_t *) 0x5010061c) = 0x00000078; // Mask offset and count
  *((volatile uint32_t *) 0x5010069c) = 0x00000027; // TRAM ptr max
  *((volatile uint32_t *) 0x5010079c) = 0x00022000; // Post processing register

  // Layer 3 group 1
  *((volatile uint32_t *) 0x5050001c) = 0x00010029; // Rows
  *((volatile uint32_t *) 0x5050009c) = 0x00010029; // Columns
  *((volatile uint32_t *) 0x5050041c) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5050059c) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x50500a1c) = 0x00007800; // Layer control 2
  *((volatile uint32_t *) 0x5050061c) = 0x00000078; // Mask offset and count
  *((volatile uint32_t *) 0x5050069c) = 0x00000027; // TRAM ptr max
  *((volatile uint32_t *) 0x5050079c) = 0x00022000; // Post processing register
  *((volatile uint32_t *) 0x5050071c) = 0xff00ff00; // Mask and processor enables

  // Layer 3 group 2
  *((volatile uint32_t *) 0x5090001c) = 0x00010029; // Rows
  *((volatile uint32_t *) 0x5090009c) = 0x00010029; // Columns
  *((volatile uint32_t *) 0x5090041c) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5090059c) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x50900a1c) = 0x00007800; // Layer control 2
  *((volatile uint32_t *) 0x5090061c) = 0x00000078; // Mask offset and count
  *((volatile uint32_t *) 0x5090069c) = 0x00000027; // TRAM ptr max
  *((volatile uint32_t *) 0x5090079c) = 0x00022000; // Post processing register
  *((volatile uint32_t *) 0x5090071c) = 0x00ff00ff; // Mask and processor enables

  // Layer 3 group 3
  *((volatile uint32_t *) 0x50d0001c) = 0x00010029; // Rows
  *((volatile uint32_t *) 0x50d0009c) = 0x00010029; // Columns
  *((volatile uint32_t *) 0x50d0041c) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50d0059c) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x50d00a1c) = 0x00007800; // Layer control 2
  *((volatile uint32_t *) 0x50d0061c) = 0x00000078; // Mask offset and count
  *((volatile uint32_t *) 0x50d0069c) = 0x00000027; // TRAM ptr max
  *((volatile uint32_t *) 0x50d0079c) = 0x00022000; // Post processing register

  // Layer 4 group 0
  *((volatile uint32_t *) 0x50100020) = 0x00010029; // Rows
  *((volatile uint32_t *) 0x501000a0) = 0x00010029; // Columns
  *((volatile uint32_t *) 0x501001a0) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x50100220) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x501002a0) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x50100320) = 0x00000800; // SRAM write ptr
  *((volatile uint32_t *) 0x50100420) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x501005a0) = 0x00000ba0; // Layer control
  *((volatile uint32_t *) 0x50100a20) = 0x0000f800; // Layer control 2
  *((volatile uint32_t *) 0x50100620) = 0x00800178; // Mask offset and count
  *((volatile uint32_t *) 0x501006a0) = 0x00000013; // TRAM ptr max
  *((volatile uint32_t *) 0x501007a0) = 0x00022000; // Post processing register
  *((volatile uint32_t *) 0x50100720) = 0xffffffff; // Mask and processor enables

  // Layer 4 group 1
  *((volatile uint32_t *) 0x50500020) = 0x00010029; // Rows
  *((volatile uint32_t *) 0x505000a0) = 0x00010029; // Columns
  *((volatile uint32_t *) 0x505001a0) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x50500220) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x505002a0) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x50500320) = 0x00000800; // SRAM write ptr
  *((volatile uint32_t *) 0x50500420) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x505005a0) = 0x00000ba0; // Layer control
  *((volatile uint32_t *) 0x50500a20) = 0x0000f800; // Layer control 2
  *((volatile uint32_t *) 0x50500620) = 0x00800178; // Mask offset and count
  *((volatile uint32_t *) 0x505006a0) = 0x00000013; // TRAM ptr max
  *((volatile uint32_t *) 0x505007a0) = 0x00022000; // Post processing register

  // Layer 4 group 2
  *((volatile uint32_t *) 0x50900020) = 0x00010029; // Rows
  *((volatile uint32_t *) 0x509000a0) = 0x00010029; // Columns
  *((volatile uint32_t *) 0x509001a0) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x50900220) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x509002a0) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x50900320) = 0x00000800; // SRAM write ptr
  *((volatile uint32_t *) 0x50900420) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x509005a0) = 0x00000ba0; // Layer control
  *((volatile uint32_t *) 0x50900a20) = 0x0000f800; // Layer control 2
  *((volatile uint32_t *) 0x50900620) = 0x00800178; // Mask offset and count
  *((volatile uint32_t *) 0x509006a0) = 0x00000013; // TRAM ptr max
  *((volatile uint32_t *) 0x509007a0) = 0x00022000; // Post processing register

  // Layer 4 group 3
  *((volatile uint32_t *) 0x50d00020) = 0x00010029; // Rows
  *((volatile uint32_t *) 0x50d000a0) = 0x00010029; // Columns
  *((volatile uint32_t *) 0x50d001a0) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x50d00220) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x50d002a0) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x50d00320) = 0x00000800; // SRAM write ptr
  *((volatile uint32_t *) 0x50d00420) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50d005a0) = 0x00000ba0; // Layer control
  *((volatile uint32_t *) 0x50d00a20) = 0x0000f800; // Layer control 2
  *((volatile uint32_t *) 0x50d00620) = 0x00800178; // Mask offset and count
  *((volatile uint32_t *) 0x50d006a0) = 0x00000013; // TRAM ptr max
  *((volatile uint32_t *) 0x50d007a0) = 0x00022000; // Post processing register

  // Layer 5 group 0
  *((volatile uint32_t *) 0x50100024) = 0x00010015; // Rows
  *((volatile uint32_t *) 0x501000a4) = 0x00010015; // Columns
  *((volatile uint32_t *) 0x50100424) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50100524) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x501005a4) = 0x00002b20; // Layer control
  *((volatile uint32_t *) 0x50100a24) = 0x0000f800; // Layer control 2
  *((volatile uint32_t *) 0x50100624) = 0x01800278; // Mask offset and count
  *((volatile uint32_t *) 0x501006a4) = 0x00000013; // TRAM ptr max
  *((volatile uint32_t *) 0x501007a4) = 0x00024000; // Post processing register
  *((volatile uint32_t *) 0x50100724) = 0xffffffff; // Mask and processor enables

  // Layer 5 group 1
  *((volatile uint32_t *) 0x50500024) = 0x00010015; // Rows
  *((volatile uint32_t *) 0x505000a4) = 0x00010015; // Columns
  *((volatile uint32_t *) 0x50500424) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50500524) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x505005a4) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x50500a24) = 0x0000f800; // Layer control 2
  *((volatile uint32_t *) 0x50500624) = 0x01800278; // Mask offset and count
  *((volatile uint32_t *) 0x505006a4) = 0x00000013; // TRAM ptr max
  *((volatile uint32_t *) 0x505007a4) = 0x00024000; // Post processing register
  *((volatile uint32_t *) 0x50500724) = 0xffffffff; // Mask and processor enables

  // Layer 5 group 2
  *((volatile uint32_t *) 0x50900024) = 0x00010015; // Rows
  *((volatile uint32_t *) 0x509000a4) = 0x00010015; // Columns
  *((volatile uint32_t *) 0x50900424) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50900524) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x509005a4) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x50900a24) = 0x0000f800; // Layer control 2
  *((volatile uint32_t *) 0x50900624) = 0x01800278; // Mask offset and count
  *((volatile uint32_t *) 0x509006a4) = 0x00000013; // TRAM ptr max
  *((volatile uint32_t *) 0x509007a4) = 0x00024000; // Post processing register

  // Layer 5 group 3
  *((volatile uint32_t *) 0x50d00024) = 0x00010015; // Rows
  *((volatile uint32_t *) 0x50d000a4) = 0x00010015; // Columns
  *((volatile uint32_t *) 0x50d00424) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50d00524) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x50d005a4) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x50d00a24) = 0x0000f800; // Layer control 2
  *((volatile uint32_t *) 0x50d00624) = 0x01800278; // Mask offset and count
  *((volatile uint32_t *) 0x50d006a4) = 0x00000013; // TRAM ptr max
  *((volatile uint32_t *) 0x50d007a4) = 0x00024000; // Post processing register

  // Layer 6 group 0
  *((volatile uint32_t *) 0x50100028) = 0x00020017; // Rows
  *((volatile uint32_t *) 0x501000a8) = 0x00020017; // Columns
  *((volatile uint32_t *) 0x501001a8) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x50100228) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x501002a8) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x50100328) = 0x00000800; // SRAM write ptr
  *((volatile uint32_t *) 0x50100428) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x501005a8) = 0x00002ba0; // Layer control
  *((volatile uint32_t *) 0x50100a28) = 0x0001f800; // Layer control 2
  *((volatile uint32_t *) 0x50100628) = 0x02800478; // Mask offset and count
  *((volatile uint32_t *) 0x501006a8) = 0x0000000b; // TRAM ptr max
  *((volatile uint32_t *) 0x501007a8) = 0x00024000; // Post processing register
  *((volatile uint32_t *) 0x50100728) = 0xffffffff; // Mask and processor enables

  // Layer 6 group 1
  *((volatile uint32_t *) 0x50500028) = 0x00020017; // Rows
  *((volatile uint32_t *) 0x505000a8) = 0x00020017; // Columns
  *((volatile uint32_t *) 0x505001a8) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x50500228) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x505002a8) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x50500328) = 0x00000800; // SRAM write ptr
  *((volatile uint32_t *) 0x50500428) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x505005a8) = 0x00000ba0; // Layer control
  *((volatile uint32_t *) 0x50500a28) = 0x0001f800; // Layer control 2
  *((volatile uint32_t *) 0x50500628) = 0x02800478; // Mask offset and count
  *((volatile uint32_t *) 0x505006a8) = 0x0000000b; // TRAM ptr max
  *((volatile uint32_t *) 0x505007a8) = 0x00024000; // Post processing register
  *((volatile uint32_t *) 0x50500728) = 0xffffffff; // Mask and processor enables

  // Layer 6 group 2
  *((volatile uint32_t *) 0x50900028) = 0x00020017; // Rows
  *((volatile uint32_t *) 0x509000a8) = 0x00020017; // Columns
  *((volatile uint32_t *) 0x509001a8) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x50900228) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x509002a8) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x50900328) = 0x00000800; // SRAM write ptr
  *((volatile uint32_t *) 0x50900428) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x509005a8) = 0x00000ba0; // Layer control
  *((volatile uint32_t *) 0x50900a28) = 0x0001f800; // Layer control 2
  *((volatile uint32_t *) 0x50900628) = 0x02800478; // Mask offset and count
  *((volatile uint32_t *) 0x509006a8) = 0x0000000b; // TRAM ptr max
  *((volatile uint32_t *) 0x509007a8) = 0x00024000; // Post processing register

  // Layer 6 group 3
  *((volatile uint32_t *) 0x50d00028) = 0x00020017; // Rows
  *((volatile uint32_t *) 0x50d000a8) = 0x00020017; // Columns
  *((volatile uint32_t *) 0x50d001a8) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x50d00228) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x50d002a8) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x50d00328) = 0x00000800; // SRAM write ptr
  *((volatile uint32_t *) 0x50d00428) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50d005a8) = 0x00000ba0; // Layer control
  *((volatile uint32_t *) 0x50d00a28) = 0x0001f800; // Layer control 2
  *((volatile uint32_t *) 0x50d00628) = 0x02800478; // Mask offset and count
  *((volatile uint32_t *) 0x50d006a8) = 0x0000000b; // TRAM ptr max
  *((volatile uint32_t *) 0x50d007a8) = 0x00024000; // Post processing register

  // Layer 7 group 0
  *((volatile uint32_t *) 0x5010002c) = 0x0001000d; // Rows
  *((volatile uint32_t *) 0x501000ac) = 0x0001000d; // Columns
  *((volatile uint32_t *) 0x5010042c) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5010052c) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x501005ac) = 0x0000eb20; // Layer control
  *((volatile uint32_t *) 0x50100a2c) = 0x0001f800; // Layer control 2
  *((volatile uint32_t *) 0x5010062c) = 0x04800678; // Mask offset and count
  *((volatile uint32_t *) 0x501006ac) = 0x0000000b; // TRAM ptr max
  *((volatile uint32_t *) 0x501007ac) = 0x00024000; // Post processing register
  *((volatile uint32_t *) 0x5010072c) = 0xffffffff; // Mask and processor enables

  // Layer 7 group 1
  *((volatile uint32_t *) 0x5050002c) = 0x0001000d; // Rows
  *((volatile uint32_t *) 0x505000ac) = 0x0001000d; // Columns
  *((volatile uint32_t *) 0x5050042c) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5050052c) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x505005ac) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x50500a2c) = 0x0001f800; // Layer control 2
  *((volatile uint32_t *) 0x5050062c) = 0x04800678; // Mask offset and count
  *((volatile uint32_t *) 0x505006ac) = 0x0000000b; // TRAM ptr max
  *((volatile uint32_t *) 0x505007ac) = 0x00024000; // Post processing register
  *((volatile uint32_t *) 0x5050072c) = 0xffffffff; // Mask and processor enables

  // Layer 7 group 2
  *((volatile uint32_t *) 0x5090002c) = 0x0001000d; // Rows
  *((volatile uint32_t *) 0x509000ac) = 0x0001000d; // Columns
  *((volatile uint32_t *) 0x5090042c) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5090052c) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x509005ac) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x50900a2c) = 0x0001f800; // Layer control 2
  *((volatile uint32_t *) 0x5090062c) = 0x04800678; // Mask offset and count
  *((volatile uint32_t *) 0x509006ac) = 0x0000000b; // TRAM ptr max
  *((volatile uint32_t *) 0x509007ac) = 0x00024000; // Post processing register
  *((volatile uint32_t *) 0x5090072c) = 0xffffffff; // Mask and processor enables

  // Layer 7 group 3
  *((volatile uint32_t *) 0x50d0002c) = 0x0001000d; // Rows
  *((volatile uint32_t *) 0x50d000ac) = 0x0001000d; // Columns
  *((volatile uint32_t *) 0x50d0042c) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50d0052c) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x50d005ac) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x50d00a2c) = 0x0001f800; // Layer control 2
  *((volatile uint32_t *) 0x50d0062c) = 0x04800678; // Mask offset and count
  *((volatile uint32_t *) 0x50d006ac) = 0x0000000b; // TRAM ptr max
  *((volatile uint32_t *) 0x50d007ac) = 0x00024000; // Post processing register
  *((volatile uint32_t *) 0x50d0072c) = 0xffffffff; // Mask and processor enables

  // Layer 8 group 0
  *((volatile uint32_t *) 0x50100030) = 0x0001000d; // Rows
  *((volatile uint32_t *) 0x501000b0) = 0x0001000d; // Columns
  *((volatile uint32_t *) 0x501001b0) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x50100230) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x501002b0) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x50100330) = 0x00000800; // SRAM write ptr
  *((volatile uint32_t *) 0x50100430) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x501005b0) = 0x0000eba0; // Layer control
  *((volatile uint32_t *) 0x50100a30) = 0x0001f800; // Layer control 2
  *((volatile uint32_t *) 0x50100630) = 0x06800878; // Mask offset and count
  *((volatile uint32_t *) 0x501006b0) = 0x00000005; // TRAM ptr max
  *((volatile uint32_t *) 0x501007b0) = 0x00024000; // Post processing register
  *((volatile uint32_t *) 0x50100730) = 0xffffffff; // Mask and processor enables

  // Layer 8 group 1
  *((volatile uint32_t *) 0x50500030) = 0x0001000d; // Rows
  *((volatile uint32_t *) 0x505000b0) = 0x0001000d; // Columns
  *((volatile uint32_t *) 0x505001b0) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x50500230) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x505002b0) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x50500330) = 0x00000800; // SRAM write ptr
  *((volatile uint32_t *) 0x50500430) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x505005b0) = 0x00000ba0; // Layer control
  *((volatile uint32_t *) 0x50500a30) = 0x0001f800; // Layer control 2
  *((volatile uint32_t *) 0x50500630) = 0x06800878; // Mask offset and count
  *((volatile uint32_t *) 0x505006b0) = 0x00000005; // TRAM ptr max
  *((volatile uint32_t *) 0x505007b0) = 0x00024000; // Post processing register
  *((volatile uint32_t *) 0x50500730) = 0xffffffff; // Mask and processor enables

  // Layer 8 group 2
  *((volatile uint32_t *) 0x50900030) = 0x0001000d; // Rows
  *((volatile uint32_t *) 0x509000b0) = 0x0001000d; // Columns
  *((volatile uint32_t *) 0x509001b0) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x50900230) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x509002b0) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x50900330) = 0x00000800; // SRAM write ptr
  *((volatile uint32_t *) 0x50900430) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x509005b0) = 0x00000ba0; // Layer control
  *((volatile uint32_t *) 0x50900a30) = 0x0001f800; // Layer control 2
  *((volatile uint32_t *) 0x50900630) = 0x06800878; // Mask offset and count
  *((volatile uint32_t *) 0x509006b0) = 0x00000005; // TRAM ptr max
  *((volatile uint32_t *) 0x509007b0) = 0x00024000; // Post processing register
  *((volatile uint32_t *) 0x50900730) = 0xffffffff; // Mask and processor enables

  // Layer 8 group 3
  *((volatile uint32_t *) 0x50d00030) = 0x0001000d; // Rows
  *((volatile uint32_t *) 0x50d000b0) = 0x0001000d; // Columns
  *((volatile uint32_t *) 0x50d001b0) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x50d00230) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x50d002b0) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x50d00330) = 0x00000800; // SRAM write ptr
  *((volatile uint32_t *) 0x50d00430) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50d005b0) = 0x00000ba0; // Layer control
  *((volatile uint32_t *) 0x50d00a30) = 0x0001f800; // Layer control 2
  *((volatile uint32_t *) 0x50d00630) = 0x06800878; // Mask offset and count
  *((volatile uint32_t *) 0x50d006b0) = 0x00000005; // TRAM ptr max
  *((volatile uint32_t *) 0x50d007b0) = 0x00024000; // Post processing register
  *((volatile uint32_t *) 0x50d00730) = 0xffffffff; // Mask and processor enables

  // Layer 9 group 0
  *((volatile uint32_t *) 0x50100034) = 0x00010007; // Rows
  *((volatile uint32_t *) 0x501000b4) = 0x00010007; // Columns
  *((volatile uint32_t *) 0x501001b4) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x50100234) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x501002b4) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x50100434) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50100534) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x501005b4) = 0x0000eba0; // Layer control
  *((volatile uint32_t *) 0x50100a34) = 0x0001f800; // Layer control 2
  *((volatile uint32_t *) 0x50100634) = 0x08800a78; // Mask offset and count
  *((volatile uint32_t *) 0x501006b4) = 0x00000002; // TRAM ptr max
  *((volatile uint32_t *) 0x501007b4) = 0x00024000; // Post processing register
  *((volatile uint32_t *) 0x50100734) = 0xffffffff; // Mask and processor enables

  // Layer 9 group 1
  *((volatile uint32_t *) 0x50500034) = 0x00010007; // Rows
  *((volatile uint32_t *) 0x505000b4) = 0x00010007; // Columns
  *((volatile uint32_t *) 0x505001b4) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x50500234) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x505002b4) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x50500434) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50500534) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x505005b4) = 0x00000ba0; // Layer control
  *((volatile uint32_t *) 0x50500a34) = 0x0001f800; // Layer control 2
  *((volatile uint32_t *) 0x50500634) = 0x08800a78; // Mask offset and count
  *((volatile uint32_t *) 0x505006b4) = 0x00000002; // TRAM ptr max
  *((volatile uint32_t *) 0x505007b4) = 0x00024000; // Post processing register
  *((volatile uint32_t *) 0x50500734) = 0xffffffff; // Mask and processor enables

  // Layer 9 group 2
  *((volatile uint32_t *) 0x50900034) = 0x00010007; // Rows
  *((volatile uint32_t *) 0x509000b4) = 0x00010007; // Columns
  *((volatile uint32_t *) 0x509001b4) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x50900234) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x509002b4) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x50900434) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50900534) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x509005b4) = 0x00000ba0; // Layer control
  *((volatile uint32_t *) 0x50900a34) = 0x0001f800; // Layer control 2
  *((volatile uint32_t *) 0x50900634) = 0x08800a78; // Mask offset and count
  *((volatile uint32_t *) 0x509006b4) = 0x00000002; // TRAM ptr max
  *((volatile uint32_t *) 0x509007b4) = 0x00024000; // Post processing register
  *((volatile uint32_t *) 0x50900734) = 0xffffffff; // Mask and processor enables

  // Layer 9 group 3
  *((volatile uint32_t *) 0x50d00034) = 0x00010007; // Rows
  *((volatile uint32_t *) 0x50d000b4) = 0x00010007; // Columns
  *((volatile uint32_t *) 0x50d001b4) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x50d00234) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x50d002b4) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x50d00434) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50d00534) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x50d005b4) = 0x00000ba0; // Layer control
  *((volatile uint32_t *) 0x50d00a34) = 0x0001f800; // Layer control 2
  *((volatile uint32_t *) 0x50d00634) = 0x08800a78; // Mask offset and count
  *((volatile uint32_t *) 0x50d006b4) = 0x00000002; // TRAM ptr max
  *((volatile uint32_t *) 0x50d007b4) = 0x00024000; // Post processing register
  *((volatile uint32_t *) 0x50d00734) = 0xffffffff; // Mask and processor enables

  // Layer 10 group 0
  *((volatile uint32_t *) 0x50100338) = 0x0001a800; // SRAM write ptr
  *((volatile uint32_t *) 0x501003b8) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50100438) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x501005b8) = 0x0000e920; // Layer control
  *((volatile uint32_t *) 0x50100a38) = 0x00004808; // Layer control 2
  *((volatile uint32_t *) 0x50100638) = 0x5e806148; // Mask offset and count
  *((volatile uint32_t *) 0x50100138) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x501007b8) = 0x00025000; // Post processing register
  *((volatile uint32_t *) 0x50100738) = 0xffffffff; // Mask and processor enables

  // Layer 10 group 1
  *((volatile uint32_t *) 0x50500338) = 0x0001a800; // SRAM write ptr
  *((volatile uint32_t *) 0x505003b8) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50500438) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x505005b8) = 0x00000920; // Layer control
  *((volatile uint32_t *) 0x50500a38) = 0x00004808; // Layer control 2
  *((volatile uint32_t *) 0x50500638) = 0x5e806148; // Mask offset and count
  *((volatile uint32_t *) 0x50500138) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x505007b8) = 0x00024000; // Post processing register
  *((volatile uint32_t *) 0x50500738) = 0xffffffff; // Mask and processor enables

  // Layer 10 group 2
  *((volatile uint32_t *) 0x50900338) = 0x0001a800; // SRAM write ptr
  *((volatile uint32_t *) 0x509003b8) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50900438) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x509005b8) = 0x00000920; // Layer control
  *((volatile uint32_t *) 0x50900a38) = 0x00004808; // Layer control 2
  *((volatile uint32_t *) 0x50900638) = 0x5e806148; // Mask offset and count
  *((volatile uint32_t *) 0x50900138) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x509007b8) = 0x00024000; // Post processing register
  *((volatile uint32_t *) 0x50900738) = 0xffffffff; // Mask and processor enables

  // Layer 10 group 3
  *((volatile uint32_t *) 0x50d00338) = 0x0001a800; // SRAM write ptr
  *((volatile uint32_t *) 0x50d003b8) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50d00438) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50d005b8) = 0x00000920; // Layer control
  *((volatile uint32_t *) 0x50d00a38) = 0x00004808; // Layer control 2
  *((volatile uint32_t *) 0x50d00638) = 0x5e806148; // Mask offset and count
  *((volatile uint32_t *) 0x50d00138) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x50d007b8) = 0x00024000; // Post processing register
  *((volatile uint32_t *) 0x50d00738) = 0xffffffff; // Mask and processor enables

  // Layer 11 group 0
  *((volatile uint32_t *) 0x501003bc) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x5010043c) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5010053c) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x501005bc) = 0x00018920; // Layer control
  *((volatile uint32_t *) 0x50100a3c) = 0x00000800; // Layer control 2
  *((volatile uint32_t *) 0x5010063c) = 0x61e061e8; // Mask offset and count
  *((volatile uint32_t *) 0x5010013c) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x501007bc) = 0x0000100a; // Post processing register

  // Layer 11 group 1
  *((volatile uint32_t *) 0x505003bc) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x5050043c) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5050053c) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x505005bc) = 0x00010920; // Layer control
  *((volatile uint32_t *) 0x50500a3c) = 0x00000800; // Layer control 2
  *((volatile uint32_t *) 0x5050063c) = 0x61e061e8; // Mask offset and count
  *((volatile uint32_t *) 0x5050013c) = 0x00000100; // 1D

  // Layer 11 group 2
  *((volatile uint32_t *) 0x509003bc) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x5090043c) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5090053c) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x509005bc) = 0x00010920; // Layer control
  *((volatile uint32_t *) 0x50900a3c) = 0x00000800; // Layer control 2
  *((volatile uint32_t *) 0x5090063c) = 0x61e061e8; // Mask offset and count
  *((volatile uint32_t *) 0x5090013c) = 0x00000100; // 1D

  // Layer 11 group 3
  *((volatile uint32_t *) 0x50d003bc) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50d0043c) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50d0053c) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x50d005bc) = 0x00010920; // Layer control
  *((volatile uint32_t *) 0x50d00a3c) = 0x00000800; // Layer control 2
  *((volatile uint32_t *) 0x50d0063c) = 0x61e061e8; // Mask offset and count
  *((volatile uint32_t *) 0x50d0013c) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x50d0073c) = 0x3ff03ff0; // Mask and processor enables


  return CNN_OK;
}

int cnn_start(void)
{
  cnn_time = 0;

  *((volatile uint32_t *) 0x50100000) = 0x00100808; // Enable group 0
  *((volatile uint32_t *) 0x50500000) = 0x00100809; // Enable group 1
  *((volatile uint32_t *) 0x50900000) = 0x00100809; // Enable group 2
  *((volatile uint32_t *) 0x50d00000) = 0x00100809; // Enable group 3

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
  addr = (volatile uint32_t *) 0x50400000;
  *out_buf++ = *addr++;
  *out_buf++ = *addr++;

  return CNN_OK;
}

int cnn_enable(uint32_t clock_source, uint32_t clock_divider)
{
  // Reset all domains, restore power to CNN
  MXC_GCFR->reg3 = 0xf; // Reset
  MXC_GCFR->reg1 = 0xf; // Mask memory
  MXC_GCFR->reg0 = 0xf; // Power
  MXC_GCFR->reg2 = 0x0; // Iso
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

