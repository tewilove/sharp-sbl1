
#ifndef _FLDR_H_
#define _FLDR_H_

#include <stdint-gcc.h>

/* ARMv7 */
void abort();
void dflush();

/* SoC */
void tdelay(uint64_t);
void udelay(uint64_t);
void mdelay(uint64_t);
void set_dload_magic();
void set_edload_magic();
void set_recovery_magic();

/* PMIC */
int pm8x41_configure_reboot(char);

#define REBOOT_COLD 0
#define REBOOT_WARM 1
#define REBOOT_SHUTDOWN 2
void reboot(int type);
void shutdown();

/* those are searched from SBL1 directly */
//
typedef int (*sh_reboot_t)(int);
extern sh_reboot_t sh_reboot;

//
typedef void (*sh_power_off_t)(void);
extern sh_power_off_t sh_power_off;

// int spmi_ext_register_readl(int slave_id, int priority, int address, void *data, int size, int *nbtr);
typedef int (*spmi_ext_register_readl_t)(int, int, int, void *, int, int *);
extern spmi_ext_register_readl_t spmi_ext_register_readl;

// int spmi_ext_register_writel(int slave_id, int priority, int address, const void *data, int size);
typedef int (*spmi_ext_register_writel_t)(int, int, int, const void *, int);
extern spmi_ext_register_writel_t spmi_ext_register_writel;

int usb_tx(const char *, int);

typedef int (*shdiag_read_t)(int, char *, int, int);
extern shdiag_read_t shdiag_read;

typedef int (*shdiag_write_t)(int, const char *, int, int);
extern shdiag_write_t shdiag_write;

#endif

