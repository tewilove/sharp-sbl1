
#ifndef _REG_H_
#define _REG_H_

#define REG64(addr) ((volatile uint64_t *)(addr))
#define REG32(addr) ((volatile uint32_t *)(addr))
#define REG16(addr) ((volatile uint16_t *)(addr))
#define REG8(addr) ((volatile uint8_t *)(addr))

#define writel(v, a) (*REG32(a) = (v))
#define readl(a) (*REG32(a))

#define writeb(v, a) (*REG8(a) = (v))
#define readb(a) (*REG8(a))

#define writehw(v, a) (*REG16(a) = (v))
#define readhw(a) (*REG16(a))

#endif

