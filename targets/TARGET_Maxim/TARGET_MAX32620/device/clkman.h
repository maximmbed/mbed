/*******************************************************************************
 * Copyright (C) 2018 Maxim Integrated Products, Inc., All Rights Reserved.
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

#ifndef _CLKMAN_H_
#define _CLKMAN_H_

#include "clkman_regs.h"
#include "max32620.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Selects an internal module clock for clock scaling configuration.
 */
typedef enum {
    CLKMAN_CLK_CPU          = 0,       // CPU clock
    CLKMAN_CLK_SYNC         = 1,       // Synchronizer clock
    CLKMAN_CLK_SPIX         = 2,       // SPI XIP module clock
    CLKMAN_CLK_PRNG         = 3,       // PRNG module clock
    CLKMAN_CLK_WDT0         = 4,       // Watchdog Timer 0 clock
    CLKMAN_CLK_WDT1         = 5,       // Watchdog Timer 1 clock
    CLKMAN_CLK_GPIO         = 6,       // GPIO module clock
    CLKMAN_CLK_PT           = 7,       // Pulse Train engine clock
    CLKMAN_CLK_UART         = 8,       // UART clock
    CLKMAN_CLK_I2CM         = 9,       // I2C Master module clock (for all instances)
    CLKMAN_CLK_I2CS         = 10,      // I2C Slave module clock
    CLKMAN_CLK_SPIM0        = 11,      // SPI Master instance 0 module clock
    CLKMAN_CLK_SPIM1        = 12,      // SPI Master instance 1 module clock
    CLKMAN_CLK_SPIM2        = 13,      // SPI Master instance 2 module clock
    CLKMAN_CLK_SPIB         = 14,      // SPI Bridge module clock
    CLKMAN_CLK_OWM          = 15,      // OWM module cloc
    CLKMAN_CLK_SPIS         = 16,      // SPI Slave module cloc
    CLKMAN_CRYPTO_CLK_AES   = 17,      // AES engine clock
    CLKMAN_CRYPTO_CLK_MAA   = 18,      // Modular Arithmetic Accelerator (MAA) clock
    CLKMAN_CRYPTO_CLK_PRNG  = 19,      // Pseudo-random number Generator (PRNG) clock
    CLKMAN_CLK_MAX          = 19
} clkman_clk_t;

/**
 * Defines clock scales for various clocks.
 */
typedef enum {
    CLKMAN_SCALE_DISABLED   = MXC_V_CLKMAN_CLK_SCALE_DISABLED,      // Clock disable
    CLKMAN_SCALE_DIV_1      = MXC_V_CLKMAN_CLK_SCALE_DIV_1,         // Clock scale for dividing by
    CLKMAN_SCALE_DIV_2      = MXC_V_CLKMAN_CLK_SCALE_DIV_2,         // Clock scale for dividing by
    CLKMAN_SCALE_DIV_4      = MXC_V_CLKMAN_CLK_SCALE_DIV_4,         // Clock scale for dividing by
    CLKMAN_SCALE_DIV_8      = MXC_V_CLKMAN_CLK_SCALE_DIV_8,         // Clock scale for dividing by
    CLKMAN_SCALE_DIV_16     = MXC_V_CLKMAN_CLK_SCALE_DIV_16,        // Clock scale for dividing by 1
    CLKMAN_SCALE_DIV_32     = MXC_V_CLKMAN_CLK_SCALE_DIV_32,        // Clock scale for dividing by 3
    CLKMAN_SCALE_DIV_64     = MXC_V_CLKMAN_CLK_SCALE_DIV_64,        // Clock scale for dividing by 6
    CLKMAN_SCALE_DIV_128    = MXC_V_CLKMAN_CLK_SCALE_DIV_128,       // Clock scale for dividing by 12
    CLKMAN_SCALE_DIV_256    = MXC_V_CLKMAN_CLK_SCALE_DIV_256,       // Clock scale for dividing by 25
    CLKMAN_SCALE_AUTO                                               // Clock scale to auto select divide
} clkman_scale_t;

void CLKMAN_SetClkScale(clkman_clk_t clk, clkman_scale_t scale)
{
    volatile uint32_t *clk_ctrl_reg;

    MBED_ASSERT(clk <= CLKMAN_CLK_MAX);
    MBED_ASSERT(scale != CLKMAN_SCALE_AUTO);

    if (clk < CLKMAN_CRYPTO_CLK_AES) {
        clk_ctrl_reg = &MXC_CLKMAN->sys_clk_ctrl_0_cm4 + clk;
    } else {
        clk_ctrl_reg = &MXC_CLKMAN->crypt_clk_ctrl_0_aes + (clk - CLKMAN_CRYPTO_CLK_AES);
    }

    *clk_ctrl_reg = scale;
}

#ifdef __cplusplus
}
#endif

#endif // _CLKMAN_H_
