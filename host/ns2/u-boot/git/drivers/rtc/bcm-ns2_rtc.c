/*
 * $Copyright Open Broadcom Corporation$
 * bcm-ns2_rtc.c - Support for the RTC driver of
 * Broadcom NS2 to enable "date" command
 *
 */

/* #define	DEBUG	*/

#include <asm/io.h>
#include <common.h>
#include <rtc.h>
#include <asm/arch/socregs.h>

#if defined(CONFIG_CMD_DATE)

#define BBL_ISO_DISABLE_FLAG  (~(1 << CRMU_ISO_CELL_CONTROL__CRMU_ISO_PDBBL))
#define RST_CMD (1 << SPRU_BBL_CMD__IND_SOFT_RST_N)
#define WR_CMD  (1 << SPRU_BBL_CMD__IND_WR)
#define RD_CMD  (1 << SPRU_BBL_CMD__IND_RD)

#define M0_BASE 0x62000000
#define rd(x)   readl(M0_BASE + x)
#define wr(x, v) writel(v, (M0_BASE + x))

#define LEAPS_THRU_END_OF(y) ((y)/4 - (y)/100 + (y)/400)

static inline bool is_leap_year(unsigned int year)
{
	return (!(year % 4) && (year % 100)) || !(year % 400);
}

static const unsigned char rtc_days_in_month[] = {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

int rtc_month_days(unsigned int month, unsigned int year)
{
	return rtc_days_in_month[month] + (is_leap_year(year) && month == 1);
}

/* Converts Gregorian date to seconds since 1970-01-01 00:00:00.
 * Assumes input in normal date format, i.e. 1980-12-31 23:59:59
 * => year=1980, mon=12, day=31, hour=23, min=59, sec=59.
 *
 * [For the Julian calendar (which was used in Russia before 1917,
 * Britain & colonies before 1752, anywhere else before 1582,
 * and is still in use by some communities) leave out the
 * -year/100+year/400 terms, and add 10.]
 *
 * This algorithm was first published by Gauss (I think).
 * This is taken from Linux
 */
unsigned long long
mktime1(const unsigned int year0, const unsigned int mon0,
	const unsigned int day, const unsigned int hour,
	const unsigned int min, const unsigned int sec)
{
	unsigned int mon = mon0, year = year0;

	/* 1..12 -> 11,12,1..10 */
	if (0 >= (int)(mon -= 2)) {
		mon += 12;	/* Puts Feb last since it has leap day */
		year -= 1;
	}

	return ((((unsigned long long)
		(year/4 - year/100 + year/400 + 367*mon/12 + day) +
			year*365 - 719499
				)*24 + hour /* now have hours */
					)*60 + min /* now have minutes */
					)*60 + sec; /* finally seconds */
}

int rtc_tm_to_time(struct rtc_time *tm, unsigned long long *time)
{
	*time = mktime1(tm->tm_year, tm->tm_mon, tm->tm_mday,
		tm->tm_hour, tm->tm_min, tm->tm_sec);
	return 0;
}

/*
 * Convert seconds since 01-01-1970 00:00:00 to Gregorian date.
 */
void rtc_time_to_tm(unsigned long long time, struct rtc_time *tm)
{
	unsigned int month, year;
	int days;

	days = time / 86400;
	time -= (unsigned int) days * 86400;

	/* day of the week, 1970-01-01 was a Thursday */
	tm->tm_wday = (days + 4) % 7;

	year = 1970 + days / 365;
	days -= (year - 1970) * 365
			+ LEAPS_THRU_END_OF(year - 1)
				- LEAPS_THRU_END_OF(1970 - 1);
	if (days < 0) {
		year -= 1;
		days += 365 + is_leap_year(year);
	}
	tm->tm_year = year;
	tm->tm_yday = days + 1;
	for (month = 0; month < 11; month++) {
		int newdays;

		newdays = days - rtc_month_days(month, year);
		if (newdays < 0)
			break;
		days = newdays;
	}
	tm->tm_mon = month + 1;
	tm->tm_mday = days + 1;

	tm->tm_hour = time / 3600;
	time -= tm->tm_hour * 3600;
	tm->tm_min = time / 60;
	tm->tm_sec = time - tm->tm_min * 60;
	tm->tm_isdst = 0;
}

/*
 * Does the rtc_time represent a valid date/time?
 */
int rtc_valid_tm(struct rtc_time *tm)
{
	if (tm->tm_year < 1970 ||
		((u32)tm->tm_mon) > 12
		|| tm->tm_mday < 1
		|| ((u32)tm->tm_hour) >= 24
		|| ((u32)tm->tm_min) >= 60
		|| ((u32)tm->tm_sec) >= 60)
		return -1;
	return 0;
}

static uint32_t bbl_rd(u32 addr)
{
	u32 cmd;

	udelay(1000 * 2);
	cmd = (addr & (0xFFFFFFFF >> (32 - SPRU_BBL_CMD__BBL_ADDR_WIDTH)))
							| RD_CMD | RST_CMD;
	wr(SPRU_BBL_CMD, cmd);
	udelay(1000 * 2);

	if (rd(SPRU_BBL_STATUS) & (1 << SPRU_BBL_STATUS__ACC_DONE))
		return  rd(SPRU_BBL_RDATA);

	printf("fail to write bbl cmd for addr 0x%x\n", addr);
	return -1;
}

static int bbl_wr(u32 addr, u32 v)
{
	u32 cmd;

	udelay(1000 * 2);
	wr(SPRU_BBL_WDATA, v);
	cmd = (addr & (0xFFFFFFFF >> (32 - SPRU_BBL_CMD__BBL_ADDR_WIDTH)))
							| WR_CMD | RST_CMD;
	wr(SPRU_BBL_CMD, cmd);
	udelay(1000 * 2);

	if (rd(SPRU_BBL_STATUS) & (1 << SPRU_BBL_STATUS__ACC_DONE))
		return 0;

	printf("fail to write bbl cmd for addr 0x%x\n", addr);
	return -1;
}

static void bbl_init(void)
{
	uint32_t v;

	wr(SPRU_BBL_CMD, 0);
	mdelay(10);
	wr(SPRU_BBL_CMD, RST_CMD);
	v = rd(CRMU_ISO_CELL_CONTROL);
	v &= BBL_ISO_DISABLE_FLAG;
	wr(CRMU_ISO_CELL_CONTROL, v);
	wr(CRMU_BBL_AUTH_CODE, 0x12345678);
	wr(CRMU_BBL_AUTH_CHECK, 0x12345678);
	wr(CRMU_BBL_CLEAR_ENABLE, 0x3f);
	wr(CRMU_SOTP_NEUTRALIZE_ENABLE, 0x7f);
}

int rtc_get(struct rtc_time *tmp)
{
	unsigned long long seconds;
	int ret = 0;

	bbl_init();
	seconds = bbl_rd(BBL_RTC_SECOND_OFFSET);

	if (seconds == -1) {
		printf("Error in reading the time\n");
		return seconds;
	}
	rtc_time_to_tm(seconds, tmp);
	ret = rtc_valid_tm(tmp);
	if (ret == -1) {
		printf("Invalid data for date\n");
		return ret;
	}
	debug("Get DATE: %4d-%02d-%02d (wday=%d)  TIME: %2d:%02d:%02d\n",
	      tmp->tm_year, tmp->tm_mon, tmp->tm_mday, tmp->tm_wday,
	      tmp->tm_hour, tmp->tm_min, tmp->tm_sec);

	return ret;
}

int rtc_set(struct rtc_time *tmp)
{
	unsigned long long t;
	int ret;

	rtc_tm_to_time(tmp, &t);

	/* bbl_rtc_stop = 1, RTC halt */
	ret = bbl_wr(BBL_CONTROL, 1);
	if (ret < 0) {
		printf("RTC: iproc_rtc_set_time failed");
		return ret;
	}
	/* Update DIV */
	ret = bbl_wr(BBL_RTC_DIV_OFFSET, 0);
	if (ret < 0) {
		printf("RTC: iproc_rtc_set_time failed");
		return ret;
	}
	/* Update second */
	ret = bbl_wr(BBL_RTC_SECOND_OFFSET, t);
	if (ret < 0) {
		printf("RTC: iproc_rtc_set_time failed");
		return ret;
	}
	/* bbl_rtc_stop = 0, RTC release */
	ret = bbl_wr(BBL_CONTROL, 0);
	if (ret < 0) {
		printf("RTC: iproc_rtc_set_time failed");
		return ret;
	}

	debug("Set DATE: %4d-%02d-%02d (wday=%d)  TIME: %2d:%02d:%02d\n",
	      tmp->tm_year, tmp->tm_mon, tmp->tm_mday, tmp->tm_wday,
	      tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
	return ret;
}

void rtc_reset(void)
{
}

#endif
