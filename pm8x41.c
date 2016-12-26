
#include "fldr.h"
#include "consts_pm8x41.h"

// see driver/platform/msm/qpnp-power.c

static int pm8x41_read(int addr, char *val, int size) {
    int rc;
    int nb = -1;

    rc = spmi_ext_register_readl(0, 0, addr, val, size, &nb);
    if (rc)
        return rc;
    return 0;
}

static int pm8x41_write(int addr, const char *val, int size) {
    return spmi_ext_register_writel(0, 0, addr, val, size);
}

static int pm8x41_masked_write(int addr, char mask, char val) {
    int rc;
    char tmp = 0;

    rc = pm8x41_read(addr, &tmp, 1);
    if (rc != 1)
        return -1;
    tmp &= ~mask;
    tmp |= val & mask;
    rc = pm8x41_write(addr, &tmp, 1);
    if (rc)
        return rc;

    return 0;
}

int pm8x41_configure_reboot(char type) {
    int rc;
    char val = 0;
    int ctl;

    rc = pm8x41_read(PON_REVISION2, &val, 1);
    if (rc != 1)
        return -1;
    if (val == 0)
        ctl = PON_PS_HOLD_RESET_CTL;
    else
        ctl = PON_PS_HOLD_RESET_CTL2;
    rc = pm8x41_masked_write(ctl, 1 << S2_RESET_EN_BIT, 0);
    udelay(500);
    rc = pm8x41_masked_write(PON_PS_HOLD_RESET_CTL, PON_POWER_OFF_MASK, type);
    rc =  pm8x41_masked_write(ctl, 1 << S2_RESET_EN_BIT, 1 << S2_RESET_EN_BIT);

    return rc;
}

