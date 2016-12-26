
#include <stdint-gcc.h>
#include "reg.h"
#include "consts_shared.h"
#if AS87 || AS97 || PA21 || PA23 || PB25 || PA28 || DL40 || DL75
#include "consts_msm8974.h"
#endif

#define QTMR_V1_CNTPCT_LO (0x00000000 + QTMR_BASE)
#define QTMR_V1_CNTPCT_HI (0x00000004 + QTMR_BASE)

static uint64_t ticks_per_sec = 19200000;

static uint64_t qtimer_get_phy_timer_cnt() {
    uint32_t phy_cnt_lo;
    uint32_t phy_cnt_hi_1;
    uint32_t phy_cnt_hi_2;

    do {
        phy_cnt_hi_1 = readl(QTMR_V1_CNTPCT_HI);
        phy_cnt_lo = readl(QTMR_V1_CNTPCT_LO);
        phy_cnt_hi_2 = readl(QTMR_V1_CNTPCT_HI);
    } while (phy_cnt_hi_1 != phy_cnt_hi_2);

   return ((uint64_t)phy_cnt_hi_1 << 32) | phy_cnt_lo;
}

void tdelay(uint64_t ticks) {
    volatile uint64_t cnt;
    uint64_t init_cnt;
    uint64_t timeout = 0;

    cnt = qtimer_get_phy_timer_cnt();
    init_cnt = cnt;

    timeout = (cnt + ticks) & (uint64_t)(QTMR_PHY_CNT_MAX_VALUE);

    while(timeout < cnt && init_cnt <= cnt)
        cnt = qtimer_get_phy_timer_cnt();

    while(timeout > cnt)
        cnt = qtimer_get_phy_timer_cnt();
}

void udelay(uint64_t nsecs) {
    uint64_t ticks;

    ticks = ((uint64_t) nsecs * ticks_per_sec) / 1000000;

    tdelay(ticks);
}

void mdelay(uint64_t msecs) {
    uint64_t ticks;

    ticks = ((uint64_t) msecs * ticks_per_sec) / 1000;

    tdelay(ticks);
}

