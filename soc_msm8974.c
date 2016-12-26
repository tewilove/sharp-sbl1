
#include "fldr.h"
#include "reg.h"
#include "consts_msm8974.h"
#include "consts_pm8x41.h"

void set_dload_magic() {
    // reason
    writel(DLOAD, RESTART_REASON_ADDR_V2);
    // cookie
    writel(DLOAD_MODE_COOKIE0, DLOAD_MODE_ADDR);
    writel(DLOAD_MODE_COOKIE1, DLOAD_MODE_ADDR + 4);
}

void set_edload_magic() {
    // reason
    writel(DLOAD, RESTART_REASON_ADDR_V2);
    // cookie
    writel(EDL_MODE_COOKIE0, EDL_MODE_ADDR);
    writel(EDL_MODE_COOKIE1, EDL_MODE_ADDR + 4);
    writel(EDL_MODE_COOKIE2, EDL_MODE_ADDR + 8);
}

void set_recovery_magic() {
    writel(0x77665502, RESTART_REASON_ADDR_V2);
}

static void __reboot(int why) {
    uint32_t val;

    val = readl(GCC_WDOG_DEBUG);
    val &= ~(1 << WDOG_DEBUG_DISABLE_BIT);
    writel(val, GCC_WDOG_DEBUG);
    while(readl(GCC_WDOG_DEBUG) & (1 << WDOG_DEBUG_DISABLE_BIT))
        ;
    pm8x41_configure_reboot(why);
    writel(0, MPM2_MPM_PS_HOLD);
    while (1)
        ;
}

void reboot(int type) {
    switch (type) {
    case REBOOT_COLD:
        __reboot(PON_PSHOLD_HARD_RESET);
        break;
    case REBOOT_WARM:
        __reboot(PON_PSHOLD_WARM_RESET);
        break;
    case REBOOT_SHUTDOWN:
    default:
        __reboot(PON_PSHOLD_SHUTDOWN);
        break;
    }
}

void shutdown() {
    __reboot(PON_PSHOLD_SHUTDOWN);
}

