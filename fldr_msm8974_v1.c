
#include "fldr.h"
#include "reg.h"
#include "consts_msm8974.h"

void fldr_init_ng() {
    uint32_t i;
    uint32_t pat1 = 0x20000030;
    uint32_t pat2 = 0x2000004c;

    for (i = SBL1_BASE; i < SBL1_BASE + SBL1_SIZE; i += 4) {
        if (readl(i) == pat1) {
#if PB25 || PA28 || DL75
            spmi_ext_register_readl = (spmi_ext_register_readl_t) readl(i - 8);
#else
            spmi_ext_register_readl = (spmi_ext_register_readl_t) readl(i + 4);
            spmi_ext_register_writel = (spmi_ext_register_writel_t) readl(i + 8);
#endif
        }
        if (readl(i) == pat2) {
            sh_reboot = (sh_reboot_t) readl(i + 4);
            sh_power_off = (sh_power_off_t) readl(i + 8);
        }
    }
}

