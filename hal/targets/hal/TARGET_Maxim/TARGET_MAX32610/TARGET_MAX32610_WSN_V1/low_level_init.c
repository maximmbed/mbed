/*******************************************************************************
 * Copyright (C) 2015 Maxim Integrated Products, Inc., All Rights Reserved.
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
 *******************************************************************************
 */

#include "cmsis.h"
#include "gpio_regs.h"
#include "ioman_regs.h"
#include "clkman_regs.h"

//******************************************************************************
// This function will get called early in system initialization
void low_level_init(void)
{
    volatile unsigned int waitcnt;

    /* set power switch pins to output high to hold switches in static state */
    MXC_GPIO->out_val[1] = 0x03;
    MXC_GPIO->out_mode[1] = 0x00000055;

    /* set pins connected to EM9301 to output low so it is not back powered */
    MXC_GPIO->out_val[2] = 0x00;
    MXC_GPIO->out_mode[2] = 0x00555555;

    /* turn on PADX to pull EM9301 supply to GND */
    MXC_IOMAN->padx_control = 0x00000440;

    /* Configure and enable the oscillator */
    if (!(MXC_CLKMAN->clk_config & MXC_F_CLKMAN_CLK_CONFIG_HFX_ENABLE)) {

        MXC_CLKMAN->clk_config = (0x4 << MXC_F_CLKMAN_CLK_CONFIG_HFX_GM_ADJUST_POS);

        /* Enable the external crystal */
        MXC_CLKMAN->clk_config |= MXC_F_CLKMAN_CLK_CONFIG_HFX_ENABLE;
    }

    /* Wait for external crystal to stabilize */
    for (waitcnt = 0; waitcnt < 0x4000; waitcnt++);    // 0x4000 ~10ms 0x10000 ~35ms, 0x20000 ~75ms

    /* Switch to the external crystal */
    MXC_CLKMAN->clk_ctrl = (MXC_CLKMAN->clk_ctrl & ~MXC_F_CLKMAN_CLK_CTRL_SYSTEM_SOURCE_SELECT) |
                           ((MXC_E_CLKMAN_SYSTEM_SOURCE_SELECT_HFX << MXC_F_CLKMAN_CLK_CTRL_SYSTEM_SOURCE_SELECT_POS));
}
