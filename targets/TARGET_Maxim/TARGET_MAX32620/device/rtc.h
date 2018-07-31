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

#ifndef _RTC_H_
#define _RTC_H_

#include "rtc_regs.h"
#include "clkman.h"
#include "pwrseq_regs.h"

/**
 *  Field values
 */
#define MXC_V_RTC_PRESCALE_DIV_2_0                          ((uint32_t)(0x00000000UL))
#define MXC_V_RTC_PRESCALE_DIV_2_1                          ((uint32_t)(0x00000001UL))
#define MXC_V_RTC_PRESCALE_DIV_2_2                          ((uint32_t)(0x00000002UL))
#define MXC_V_RTC_PRESCALE_DIV_2_3                          ((uint32_t)(0x00000003UL))
#define MXC_V_RTC_PRESCALE_DIV_2_4                          ((uint32_t)(0x00000004UL))
#define MXC_V_RTC_PRESCALE_DIV_2_5                          ((uint32_t)(0x00000005UL))
#define MXC_V_RTC_PRESCALE_DIV_2_6                          ((uint32_t)(0x00000006UL))
#define MXC_V_RTC_PRESCALE_DIV_2_7                          ((uint32_t)(0x00000007UL))
#define MXC_V_RTC_PRESCALE_DIV_2_8                          ((uint32_t)(0x00000008UL))
#define MXC_V_RTC_PRESCALE_DIV_2_9                          ((uint32_t)(0x00000009UL))
#define MXC_V_RTC_PRESCALE_DIV_2_10                         ((uint32_t)(0x0000000AUL))
#define MXC_V_RTC_PRESCALE_DIV_2_11                         ((uint32_t)(0x0000000BUL))
#define MXC_V_RTC_PRESCALE_DIV_2_12                         ((uint32_t)(0x0000000CUL))

#define MXC_V_RTC_CTRL_SNOOZE_DISABLE                       ((uint32_t)(0x00000000UL))
#define MXC_V_RTC_CTRL_SNOOZE_MODE_A                        ((uint32_t)(0x00000001UL))
#define MXC_V_RTC_CTRL_SNOOZE_MODE_B                        ((uint32_t)(0x00000002UL))

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Enumeration type for scaling down the 4096Hz input clock to the RTC.
 */
typedef enum {
    RTC_PRESCALE_DIV_2_0 = MXC_V_RTC_PRESCALE_DIV_2_0,
    RTC_PRESCALE_DIV_2_1 = MXC_V_RTC_PRESCALE_DIV_2_1,
    RTC_PRESCALE_DIV_2_2 = MXC_V_RTC_PRESCALE_DIV_2_2,
    RTC_PRESCALE_DIV_2_3 = MXC_V_RTC_PRESCALE_DIV_2_3,
    RTC_PRESCALE_DIV_2_4 = MXC_V_RTC_PRESCALE_DIV_2_4,
    RTC_PRESCALE_DIV_2_5 = MXC_V_RTC_PRESCALE_DIV_2_5,
    RTC_PRESCALE_DIV_2_6 = MXC_V_RTC_PRESCALE_DIV_2_6,
    RTC_PRESCALE_DIV_2_7 = MXC_V_RTC_PRESCALE_DIV_2_7,
    RTC_PRESCALE_DIV_2_8 = MXC_V_RTC_PRESCALE_DIV_2_8,
    RTC_PRESCALE_DIV_2_9 = MXC_V_RTC_PRESCALE_DIV_2_9,
    RTC_PRESCALE_DIV_2_10 = MXC_V_RTC_PRESCALE_DIV_2_10,
    RTC_PRESCALE_DIV_2_11 = MXC_V_RTC_PRESCALE_DIV_2_11,
    RTC_PRESCALE_DIV_2_12 = MXC_V_RTC_PRESCALE_DIV_2_12,
} rtc_prescale_t;

/**
 * Mask used to clear all RTC interrupt flags, see \ref RTC_FLAGS_Register Register.
 */
#define RTC_FLAGS_CLEAR_ALL            (MXC_F_RTC_FLAGS_COMP0  | \
                                        MXC_F_RTC_FLAGS_COMP1| \
                                        MXC_F_RTC_FLAGS_PRESCALE_COMP | \
                                        MXC_F_RTC_FLAGS_OVERFLOW | \
                                        MXC_F_RTC_FLAGS_TRIM)
/**
 * Enumeration type to select the type of RTC Snooze Mode for an alarm condition.
 */
typedef enum {
    RTC_SNOOZE_DISABLE = MXC_V_RTC_CTRL_SNOOZE_DISABLE,
    RTC_SNOOZE_MODE_A  = MXC_V_RTC_CTRL_SNOOZE_MODE_A,
    RTC_SNOOZE_MODE_B  = MXC_V_RTC_CTRL_SNOOZE_MODE_B,
} rtc_snooze_t;

/**
* Number of RTC Compare registers for this peripheral instance.
*/
#define RTC_NUM_COMPARE 2

/**
 * Structure type that represents the current configuration of the RTC.
 */
typedef struct {
    rtc_prescale_t prescaler;               // prescale value for the input 4096Hz clock.
    rtc_prescale_t prescalerMask;           // Mask value used to compare to the rtc prescale value
    uint32_t compareCount[RTC_NUM_COMPARE]; // Values used for setting the RTC alarms.
    uint32_t snoozeCount;                   // The number of RTC ticks to snooze if enabled.
    rtc_snooze_t snoozeMode;                // The desired snooze mode.
} rtc_cfg_t;

static inline int RTC_Init(const rtc_cfg_t *cfg);
static inline void SYS_RTC_Init(void);

static inline uint32_t RTC_IsActive(void)
{
    return (MXC_RTCTMR->ctrl & MXC_F_RTC_CTRL_ENABLE);
}

static inline void RTC_Start(void)
{
    MXC_RTCTMR->ctrl |= MXC_F_RTC_CTRL_ENABLE;

    //wait for pending actions to complete
    while(MXC_RTCTMR->ctrl & MXC_F_RTC_CTRL_PENDING);
}

static inline void RTC_Stop(void)
{
    MXC_RTCTMR->ctrl &= ~(MXC_F_RTC_CTRL_ENABLE);

    //wait for pending actions to complete
    while(MXC_RTCTMR->ctrl & MXC_F_RTC_CTRL_PENDING);
}

static inline void RTC_EnableINT(uint32_t mask)
{
    MXC_RTCTMR->inten |= mask;
}

static inline void RTC_DisableINT(uint32_t mask)
{
    MXC_RTCTMR->inten &= ~mask;
}

static inline void RTC_ClearFlags(uint32_t mask)
{
    MXC_RTCTMR->flags = mask;

    //wait for pending actions to complete
    while(MXC_RTCTMR->ctrl & MXC_F_RTC_CTRL_PENDING);
}

void SYS_RTC_Init(void)
{
    // Enable power for RTC for all LPx states
    MXC_PWRSEQ->reg0 |= (MXC_F_PWRSEQ_REG0_PWR_RTCEN_RUN |
                         MXC_F_PWRSEQ_REG0_PWR_RTCEN_SLP);

    // Enable clock to synchronizers
    CLKMAN_SetClkScale(CLKMAN_CLK_SYNC, CLKMAN_SCALE_DIV_1);
}

int RTC_Init(const rtc_cfg_t *cfg)
{
    int i = 0;

    // Validate configuration pointer is not NULL
    if (cfg == NULL)
        return -1;
    // Validate the passed in parameters
    if ((cfg->prescalerMask > ((rtc_prescale_t)cfg->prescaler)) || (cfg->snoozeCount > MXC_F_RTC_SNZ_VAL_VALUE))
        return -1;

    // Set system level configurations
    SYS_RTC_Init();

    // Disable RTC
    MXC_RTCTMR->ctrl &= ~(MXC_F_RTC_CTRL_ENABLE);

    // Disable all interrupts
    MXC_RTCTMR->inten = 0;

    // Clear all interrupts
    MXC_RTCTMR->flags = RTC_FLAGS_CLEAR_ALL;

    // Reset starting count
    MXC_RTCTMR->timer = 0;

    // Set the compare registers to the values passed in
    for(i = 0; i < RTC_NUM_COMPARE; i++)
        MXC_RTCTMR->comp[i] = cfg->compareCount[i];

    // Set the prescaler
    MXC_RTCTMR->prescale = cfg->prescaler;
    // Set the prescale mask, checked it for validity on entry
    MXC_RTCTMR->prescale_mask = cfg->prescalerMask;

    // Set snooze mode (rtc_snooze_t)
    MXC_RTCTMR->ctrl &= (~MXC_F_RTC_CTRL_SNOOZE_ENABLE);
    MXC_RTCTMR->ctrl |= (cfg->snoozeMode << MXC_F_RTC_CTRL_SNOOZE_ENABLE_POS);

    // Set the snooze count. Checked for validity on entry.
    MXC_RTCTMR->snz_val = (cfg->snoozeCount << MXC_F_RTC_SNZ_VAL_VALUE_POS) & MXC_F_RTC_SNZ_VAL_VALUE;

    // Wait for pending actions to complete
    while(MXC_RTCTMR->ctrl & MXC_F_RTC_CTRL_PENDING);

    // Reset trim to defaults, trim disabled, trim faster override disabled
    MXC_RTCTMR->trim_ctrl &= ~(MXC_F_RTC_TRIM_CTRL_TRIM_ENABLE_R | MXC_F_RTC_TRIM_CTRL_TRIM_FASTER_OVR_R);

    // Set trim slower control bit to 0, which is trim faster by default
    MXC_RTCTMR->trim_value &= ~(MXC_F_RTC_TRIM_VALUE_TRIM_SLOWER_CONTROL);

    // Return No Error
    return 0;
}

#ifdef __cplusplus
}
#endif

#endif // _RTC_H_
