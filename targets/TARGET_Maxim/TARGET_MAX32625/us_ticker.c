/*******************************************************************************
 * Copyright (c) 2016,2018 Maxim Integrated Products, Inc., All Rights Reserved.
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

#include <stddef.h>
#include "us_ticker_api.h"
#include "tmr.h"

#define US_TIMER          MXC_TMR0
#define US_TIMER_IRQn     TMR0_0_IRQn
#define US_TIMER_PRESCALE TMR_PRESCALE_DIV_2_5
#define US_TIMER_WIDTH    32

static int us_ticker_inited = 0;

//******************************************************************************
void us_ticker_init(void)
{
    if (us_ticker_inited) {
        return;
    }

    us_ticker_inited = 1;

    TMR_Init(US_TIMER, US_TIMER_PRESCALE, NULL);

    tmr32_cfg_t cfg = {
        TMR32_MODE_CONTINUOUS,
        TMR_POLARITY_UNUSED,
        -1
    };
    TMR32_Config(US_TIMER, &cfg);

    NVIC_SetVector(US_TIMER_IRQn, (uint32_t)ticker_irq_handler);
    NVIC_EnableIRQ(US_TIMER_IRQn);

    TMR32_Start(US_TIMER);
}

//******************************************************************************
uint32_t us_ticker_read(void)
{
    if (!us_ticker_inited) {
        us_ticker_init();
    }

    return TMR32_GetCount(US_TIMER);
}

//******************************************************************************
void us_ticker_set_interrupt(timestamp_t timestamp)
{
    TMR32_SetCompare(US_TIMER, timestamp);
    TMR32_EnableINT(US_TIMER);
}

//******************************************************************************
void us_ticker_fire_interrupt(void)
{
    NVIC_SetPendingIRQ(US_TIMER_IRQn);
}

//******************************************************************************
void us_ticker_disable_interrupt(void)
{
    TMR32_DisableINT(US_TIMER);
}

//******************************************************************************
void us_ticker_clear_interrupt(void)
{
    TMR32_ClearFlag(US_TIMER);
}

//******************************************************************************
const ticker_info_t* us_ticker_get_info(void)
{
    static const ticker_info_t info = {
        RO_FREQ >> US_TIMER_PRESCALE,
        US_TIMER_WIDTH
    };

    return &info;
}
