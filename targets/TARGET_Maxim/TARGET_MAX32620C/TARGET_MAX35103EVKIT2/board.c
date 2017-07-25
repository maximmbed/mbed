#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "max32620.h"
#include "clkman_regs.h"
#include "adc_regs.h"
#include "pwrseq_regs.h"
#include "pwrman_regs.h"
#include "icc_regs.h"
#include "flc_regs.h"
#include "rtc_regs.h"
#include "trim_regs.h"

extern void (* const __isr_vector[])(void);

void SystemInit(void)
{
    /* Configure the interrupt controller to use the application vector table in */
    /* the application space */
#if defined ( __CC_ARM) || defined ( __GNUC__) 
    /* IAR sets the VTOR pointer prior to SystemInit and setting it here causes stack corruption on IAR startup. */
	  __disable_irq();
    SCB->VTOR = (uint32_t)__isr_vector;
	__DSB();
	__enable_irq();	
#endif /* __CC_ARM || __GNUC__ */
    /* Copy trim information from shadow registers into power manager registers */
    /* NOTE: Checks have been added to prevent bad/missing trim values from being loaded */
    if ((MXC_FLC->ctrl & MXC_F_FLC_CTRL_INFO_BLOCK_VALID) &&
            (MXC_TRIM->for_pwr_reg5 != 0xffffffff) &&
            (MXC_TRIM->for_pwr_reg6 != 0xffffffff)) {
        MXC_PWRSEQ->reg5 = MXC_TRIM->for_pwr_reg5;
        MXC_PWRSEQ->reg6 = MXC_TRIM->for_pwr_reg6;
    } else {
        /* No valid info block, use some reasonable defaults */
        MXC_PWRSEQ->reg6 &= ~MXC_F_PWRSEQ_REG6_PWR_TRIM_OSC_VREF;
        MXC_PWRSEQ->reg6 |= (0x1e0 << MXC_F_PWRSEQ_REG6_PWR_TRIM_OSC_VREF_POS);
    }

    /* Improve flash access timing */
    MXC_FLC->perform |= (MXC_F_FLC_PERFORM_EN_BACK2BACK_RDS |
                         MXC_F_FLC_PERFORM_EN_MERGE_GRAB_GNT |
                         MXC_F_FLC_PERFORM_AUTO_TACC |
                         MXC_F_FLC_PERFORM_AUTO_CLKDIV);

    /* First, eliminate the unnecessary RTC handshake between clock domains. Must be set as a pair. */
    MXC_RTCTMR->ctrl |= (MXC_F_RTC_CTRL_USE_ASYNC_FLAGS |
                         MXC_F_RTC_CTRL_AGGRESSIVE_RST);
    /* Enable fast read of the RTC timer value, and fast write of all other RTC registers */
    MXC_PWRSEQ->rtc_ctrl2 |= (MXC_F_PWRSEQ_RTC_CTRL2_TIMER_AUTO_UPDATE |
                              MXC_F_PWRSEQ_RTC_CTRL2_TIMER_ASYNC_WR);
    MXC_PWRSEQ->rtc_ctrl2 &= ~(MXC_F_PWRSEQ_RTC_CTRL2_TIMER_ASYNC_RD);

    /* Clear the GPIO WUD event if not waking up from LP0 */
    /* this is necessary because WUD flops come up in undetermined state out of POR or SRST*/
    if(MXC_PWRSEQ->reg0 & MXC_F_PWRSEQ_REG0_PWR_FIRST_BOOT ||
       !(MXC_PWRMAN->pwr_rst_ctrl & MXC_F_PWRMAN_PWR_RST_CTRL_POR)) {
        /* Clear GPIO WUD event and configuration registers, globally */
        MXC_PWRSEQ->reg1 |= (MXC_F_PWRSEQ_REG1_PWR_CLR_IO_EVENT_LATCH |
			     MXC_F_PWRSEQ_REG1_PWR_CLR_IO_CFG_LATCH);
        MXC_PWRSEQ->reg1 &= ~(MXC_F_PWRSEQ_REG1_PWR_CLR_IO_EVENT_LATCH |
			      MXC_F_PWRSEQ_REG1_PWR_CLR_IO_CFG_LATCH);
    } else {
	/* Unfreeze the GPIO by clearing MBUS_GATE, when returning from LP0 */
	MXC_PWRSEQ->reg1 &= ~(MXC_F_PWRSEQ_REG1_PWR_MBUS_GATE);
	/* LP0 wake-up: Turn off special switch to eliminate ~50nA of leakage on VDD12 */
	MXC_PWRSEQ->reg1 &= ~MXC_F_PWRSEQ_REG1_PWR_SRAM_NWELL_SW;
    }
    
    /* Turn on retention regulator */
    MXC_PWRSEQ->reg0 |= (MXC_F_PWRSEQ_REG0_PWR_RETREGEN_RUN |
                         MXC_F_PWRSEQ_REG0_PWR_RETREGEN_SLP);

    /* Turn on Auto GPIO Freeze/UnFreeze in sleep modes */
    MXC_PWRSEQ->reg1 |= MXC_F_PWRSEQ_REG1_PWR_AUTO_MBUS_GATE;

    /* Adjust settings in the retention controller for fastest wake-up time */
    MXC_PWRSEQ->retn_ctrl0 |= (MXC_F_PWRSEQ_RETN_CTRL0_RC_REL_CCG_EARLY |
                               MXC_F_PWRSEQ_RETN_CTRL0_RC_POLL_FLASH);
    MXC_PWRSEQ->retn_ctrl0 &= ~(MXC_F_PWRSEQ_RETN_CTRL0_RC_USE_FLC_TWK);

    
    /* Set retention controller TWake cycle count to 1us to minimize the wake-up time */
    /* NOTE: flash polling (...PWRSEQ_RETN_CTRL0_RC_POLL_FLASH) must be enabled before changing POR default! */
    MXC_PWRSEQ->retn_ctrl1 = (MXC_PWRSEQ->retn_ctrl1 & ~MXC_F_PWRSEQ_RETN_CTRL1_RC_TWK) |
                             (1 << MXC_F_PWRSEQ_RETN_CTRL1_RC_TWK_POS);

    /* Improve wake-up time by changing ROSEL to 140ns */
    MXC_PWRSEQ->reg3 = (1 << MXC_F_PWRSEQ_REG3_PWR_ROSEL_POS) |
        (1 << MXC_F_PWRSEQ_REG3_PWR_FAILSEL_POS) |
        (MXC_PWRSEQ->reg3 & ~(MXC_F_PWRSEQ_REG3_PWR_ROSEL |
           MXC_F_PWRSEQ_REG3_PWR_FLTRROSEL));
    
    /* Enable RTOS Mode: Enable 32kHz clock synchronizer to SysTick external clock input */
    MXC_CLKMAN->clk_ctrl |= MXC_F_CLKMAN_CLK_CTRL_RTOS_MODE;

    /* Set this so all bits of PWR_MSK_FLAGS are active low to mask the corresponding flags */
    MXC_PWRSEQ->pwr_misc |= MXC_F_PWRSEQ_PWR_MISC_INVERT_4_MASK_BITS;

    /* Clear this bit to get the latest PT */
    MXC_PWRMAN->pt_regmap_ctrl &= ~MXC_F_PWRMAN_PT_REGMAP_CTRL_ME02A_MODE;

    /* Enable FPU on Cortex-M4, which occupies coprocessor slots 10 & 11 */
    /* Grant full access, per "Table B3-24 CPACR bit assignments". */
    /* DDI0403D "ARMv7-M Architecture Reference Manual" */
    SCB->CPACR |= SCB_CPACR_CP10_Msk | SCB_CPACR_CP11_Msk;
    __DSB();
    __ISB();

	// ensure that we're using the undivided 96MHz clock source
	MXC_CLKMAN->clk_ctrl = ((MXC_CLKMAN->clk_ctrl & ~MXC_F_CLKMAN_CLK_CTRL_SYSTEM_SOURCE_SELECT) | (MXC_V_CLKMAN_CLK_CTRL_SYSTEM_SOURCE_SELECT_96MHZ_RO));

	SystemCoreClockUpdate();
}

