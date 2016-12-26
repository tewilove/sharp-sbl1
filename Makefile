
# FLDR code base
# for IPL
#BASEADDR = 0x20000000
# for custom fldr
BASEADDR = 0x20000600

# device built for
DEVICE ?= PA23

# Don't use arm-linux*, will generate GOT rather than link
CROSS=arm-eabi-
CC=$(CROSS)gcc
LD=$(CROSS)ld
OBJCOPY=$(CROSS)objcopy
STRIP=$(CROSS)strip

CP=cp -f
RM=rm -f

ASFLAGS = -Wa,-march=armv7-a -Wa,-mthumb
CFLAGS = -fno-builtin -fno-stack-protector
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -I./include
CFLAGS += -D$(DEVICE)=1
CFLAGS += -O2 -Wall -march=armv7-a -mthumb -D__thumb__
LDFLAGS =

linker := fldr.ld
libgcc := $(shell $(CC) $(CFLAGS) -print-libgcc-file-name)

PROJECTS := app_root.raw

all: $(PROJECTS)

start_objs := crt0.o 
common_objs := fldr.o \
               insn.o \
               mmc.o
libc_objs := $(libgcc) \
             libgcc-compat.o \
             memchr.o \
             memcmp.o \
             memmem.o \
             memcpy.o \
             memset.o \
             strlen.o
device_shared_objs := soc_shared.o
device_msm8974_objs := cache-v7.o \
                       pm8x41.o \
                       soc_msm8974.o \
                       fldr_msm8974_v1.o

root_objs := $(start_objs) $(common_objs) $(device_shared_objs)
root_objs += $(device_msm8974_objs)
root_objs += $(libc_objs)
root_objs += app_flash.o

app_root.elf: $(linker) $(root_objs)
	$(LD) -o $@ -T $^ $(LDFLAGS)

%.o: %.S
	$(CC) -o $@ $(ASFLAGS) $^ -c

%.o: %.c
	$(CC) -o $@ $(CFLAGS) $^ -c

%.raw: %.elf
	$(OBJCOPY) -O binary $^ $@

clean:
	$(RM) *.o *.elf *.raw

.PHONY: clean

