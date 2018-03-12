/*******************************************************************************
 * Copyright (C) 2016,2018 Maxim Integrated Products, Inc., All Rights Reserved.
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

#include "rtc_api.h"
#include "lp_ticker_api.h"
#include "rtc.h"
#include "lp.h"


#define LP_TIMER_FREQ_HZ  4096
#define LP_TIMER_PRESCALE (RTC_PRESCALE_DIV_2_0)
#define LP_TIMER_RATE_HZ  (LP_TIMER_FREQ_HZ >> LP_TIMER_PRESCALE)
#define LP_TIMER_WIDTH    32

static int rtc_inited = 0;


//******************************************************************************
void overflow_handler(void)
{
    MXC_RTCTMR->comp[1] += (0xFFFFFFFFU / LP_TIMER_RATE_HZ) + 1;
    RTC_ClearFlags(MXC_F_RTC_FLAGS_ASYNC_CLR_FLAGS);
}

//******************************************************************************
void rtc_init(void)
{
    if (rtc_inited) {
        return;
    }
    rtc_inited = 1;

    /* Enable power for RTC for all LPx states */
    MXC_PWRSEQ->reg0 |= (MXC_F_PWRSEQ_REG0_PWR_RTCEN_RUN |
                         MXC_F_PWRSEQ_REG0_PWR_RTCEN_SLP);

    /* Enable clock to synchronizers */
    CLKMAN_SetClkScale(CLKMAN_CLK_SYNC, CLKMAN_SCALE_DIV_1);

    // Prepare interrupt handlers
    NVIC_SetVector(RTC0_IRQn, (uint32_t)lp_ticker_irq_handler);
    NVIC_EnableIRQ(RTC0_IRQn);
    NVIC_SetVector(RTC3_IRQn, (uint32_t)overflow_handler);
    NVIC_EnableIRQ(RTC3_IRQn);

    // Enable wakeup on RTC overflow
    LP_ConfigRTCWakeUp(0, 0, 0, 1);

    /* RTC registers are only reset on a power cycle. Do not reconfigure the RTC
     * if it is already running.
     */
    if (!RTC_IsActive()) {
        rtc_cfg_t cfg = { 0 };
        cfg.prescaler = LP_TIMER_PRESCALE;
        cfg.snoozeMode = RTC_SNOOZE_DISABLE;

        int retval = RTC_Init(&cfg);
        MBED_ASSERT(retval == E_NO_ERROR);

        RTC_EnableINT(MXC_F_RTC_FLAGS_OVERFLOW);
        RTC_Start();
    }
}

//******************************************************************************
void rtc_free(void)
{
    if (RTC_IsActive()) {
        MXC_RTCTMR->ctrl |= MXC_F_RTC_CTRL_CLEAR;
        RTC_Stop();
    }
}

//******************************************************************************
int rtc_isenabled(void)
{
    return RTC_IsActive();
}

//******************************************************************************
void rtc_write(time_t t)
{
    if (!rtc_inited) {
        rtc_init();
    }

    MXC_RTCTMR->comp[1] = t - (RTC_GetCount() / LP_TIMER_RATE_HZ);

    // Wait for pending transactions
    while (MXC_RTCTMR->ctrl & MXC_F_RTC_CTRL_PENDING);
}

//******************************************************************************
time_t rtc_read(void)
{
    return (RTC_GetCount() / LP_TIMER_RATE_HZ) + MXC_RTCTMR->comp[1];
}

//******************************************************************************
void lp_ticker_init(void)
{
    rtc_init();
}

//******************************************************************************
uint32_t lp_ticker_read(void)
{
    return RTC_GetCount();
}

//******************************************************************************
void lp_ticker_set_interrupt(timestamp_t timestamp)
{
    MXC_RTCTMR->comp[0] = timestamp;
    MXC_RTCTMR->flags = MXC_F_RTC_FLAGS_ASYNC_CLR_FLAGS;
    RTC_EnableINT(MXC_F_RTC_INTEN_COMP0);

    // Enable wakeup from RTC
    LP_ConfigRTCWakeUp(1, 0, 0, 1);

    // Wait for pending transactions
    while (MXC_RTCTMR->ctrl & MXC_F_RTC_CTRL_PENDING);
}

//******************************************************************************
inline void lp_ticker_disable_interrupt(void)
{
    RTC_DisableINT(MXC_F_RTC_INTEN_COMP0);
}

//******************************************************************************
inline void lp_ticker_clear_interrupt(void)
{
    RTC_ClearFlags(MXC_F_RTC_FLAGS_ASYNC_CLR_FLAGS);
}

//******************************************************************************
void lp_ticker_fire_interrupt(void)
{
    NVIC_SetPendingIRQ(RTC0_IRQn);
}

//******************************************************************************
const ticker_info_t *lp_ticker_get_info(void)
{
    static const ticker_info_t info = {
        LP_TIMER_RATE_HZ,
        LP_TIMER_WIDTH
    };

    return &info;
}
