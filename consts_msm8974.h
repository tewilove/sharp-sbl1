
#ifndef _CONSTS_MSM8974_H_
#define _CONSTS_MSM8974_H_

#include "consts_shared.h"

#define SBL1_BASE               0xF800C000
#define SBL1_SIZE               0x00064000

#define QTMR_BASE               0xF9021000
#define RESTART_REASON_ADDR_V2  0xFE80565C
#define DLOAD_MODE_ADDR         0xFE805000
#define EDL_MODE_ADDR           0xFE805FE0
#define MPM2_MPM_PS_HOLD        0xFC4AB000
#define GCC_WDOG_DEBUG          0xFC401780
#define DUMP_TZ_ADDR            0xFE805748

#define WDOG_DEBUG_DISABLE_BIT  17

#endif

