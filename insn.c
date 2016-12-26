
#include "insn.h"

uint32_t decode_t32_bl(uint32_t mem, uint32_t val) {
    uint32_t hi, lo;
    uint32_t s, j1, j2, i1, i2;

    lo = val & 0xffff;
    hi = val >> 16;

    if (((lo >> 11) & 0x1f) != 0x1e)
        return 0;
    if (((hi >> 12) & 0x0d) != 0x0d)
        return 0;

    s = (lo >> 10) & 1;
    j1 = (hi >> 13) & 1;
    j2 = (hi >> 11) & 1;
    i1 = !(s ^ j1);
    i2 = !(s ^ j2);
    val = (i1 << 23) |
          (i2 << 22) |
          ((lo & 0x3ff) << 12) |
          ((hi & 0x7ff) << 1);
    if (s)
        val |= 0xff000000;

    return (mem + 4 + val);
}

