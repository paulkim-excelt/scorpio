/*
 * $Copyright (C) Broadcom Corporation $
 */

#include <config.h>
#include <common.h>
#include <asm/arch-bcm_pegasus/socregs.h>
#include <asm/io.h>
#include "pegasus_pwm.h"

#define PEGASUS_MAX_PWM_CH	(4)

#define PRESCALE_VAL	(0x3f)
#define PERIOD_CNT	(0xffff)
#define DUTY_CYCLE_HIGH	(0x7fff)

/* Pwm Configuration. Kept Simple */
int pegasus_pwm_cfg(unsigned char ch)
{
	unsigned int reg_val;
	unsigned char *reg_period, *reg_dutyhi;

	if (ch > PEGASUS_MAX_PWM_CH) {
		printf("Invalid PWM Channel %d to Configure\n", ch);
		return -1;
	}

#if 0
	reg_val = readl(ChipcommonG_PWMCTL);
	reg_val = reg_val & ~(1 << ch);
	/*printf("Channel (%d) 0x%08X 0x%08X\n", ch, ChipcommonG_PWMCTL, reg_val);*/
	writel(0xFFFF0000, ChipcommonG_PWMCTL);
	udelay(1000);
#endif
	pegasus_pwm_shutdown(ch);

	reg_val = readl(ChipcommonG_PWM_PRESCALE);
	reg_val = reg_val | (PRESCALE_VAL << (18 - (6 * ch)));
	/*printf("Channel (%d) 0x%08X 0x%08X\n", ch, ChipcommonG_PWM_PRESCALE, reg_val);*/
	writel(reg_val, ChipcommonG_PWM_PRESCALE);
	udelay(1000);

	reg_period = (unsigned char *)ChipcommonG_PWM_PERIOD_COUNT0;
	reg_period += ch * 8;
	reg_val =  readl(reg_period);
	reg_val = reg_val | PERIOD_CNT;
	/*printf("Channel (%d) 0x%08X 0x%08X\n", ch, ChipcommonG_PWM_PERIOD_COUNT0 + (8 * ch), reg_val);*/
	writel(reg_val, reg_period);
	udelay(1000);

	reg_dutyhi = (unsigned char *)ChipcommonG_PWM_DUTYHI_COUNT0;
	reg_dutyhi += ch * 8;
	reg_val = readl(reg_dutyhi);
	reg_val = reg_val | DUTY_CYCLE_HIGH;
	/*printf("Channel (%d) 0x%08X 0x%08X\n", ch, (ChipcommonG_PWM_DUTYHI_COUNT0 + (8 * ch)), reg_val);*/
	writel(reg_val, reg_dutyhi);
	udelay(1000);
	return 0;
}

void pegasus_pwm_cfg_all(void)
{
	int ch;
	unsigned int reg_val;
	unsigned char *reg_period, *reg_dutyhi;

	/*writel(0xFFFF0000, ChipcommonG_PWMCTL);*/
	/*writel(0x0, LYNX_PWM_SHUTDOWN);*/
	pegasus_pwm_shutdown_all();

	for (ch = 0; ch < PEGASUS_MAX_PWM_CH; ch++) {
		reg_val = readl(ChipcommonG_PWM_PRESCALE);
		reg_val = reg_val | (PRESCALE_VAL << (18 - (6 * ch)));
		writel(reg_val, ChipcommonG_PWM_PRESCALE);

		reg_period = (unsigned char *)ChipcommonG_PWM_PERIOD_COUNT0;
		reg_period += ch * 8;
		reg_val =  readl(reg_period);
		reg_val = reg_val | PERIOD_CNT;
		writel(reg_val, reg_period);

		reg_dutyhi = (unsigned char *)ChipcommonG_PWM_DUTYHI_COUNT0;
		reg_dutyhi += ch * 8;
		reg_val = readl(reg_dutyhi);
		reg_val = reg_val | DUTY_CYCLE_HIGH;
		writel(reg_val, reg_dutyhi);
	}
}

int pegasus_pwm_enable(unsigned char ch)
{
	unsigned int reg_val;

	if (ch > PEGASUS_MAX_PWM_CH) {
		printf("Invalid PWM Channel %d too Enable\n", ch);
		return -1;
	}
	reg_val = readl(ChipcommonG_PWMCTL);
	reg_val = reg_val | (1 << ch);

	/*printf("Channel (%d) 0x%08X 0x%08X\n", ch, ChipcommonG_PWMCTL, reg_val);*/
	writel(reg_val, ChipcommonG_PWMCTL);
	udelay(1000);
	return 0;
}

void pegasus_pwm_enable_all(void)
{
	unsigned int reg_val;

	reg_val = readl(ChipcommonG_PWMCTL);
	reg_val = reg_val | 0xF;

	writel(reg_val, ChipcommonG_PWMCTL);
	udelay(4000);
}

int pegasus_pwm_shutdown(unsigned char ch)
{
	unsigned int reg_val;

	if (ch > PEGASUS_MAX_PWM_CH) {
		printf("Invalid PWM Channel %d to Shutdown\n", ch);
		return -1;
	}

	reg_val = readl(ChipcommonG_PWMCTL);
	reg_val = reg_val & ~((1 << ch));
	/*printf("Channel (%d) 0x%08X 0x%08X\n", ch, ChipcommonG_PWMCTL, reg_val);*/
	writel(reg_val, ChipcommonG_PWMCTL);

	return 0;
}

void pegasus_pwm_shutdown_all(void)
{
	unsigned int reg_val;

	reg_val = readl(ChipcommonG_PWMCTL);
	reg_val = reg_val & ~(0xF);

	writel(reg_val, ChipcommonG_PWMCTL);
}

