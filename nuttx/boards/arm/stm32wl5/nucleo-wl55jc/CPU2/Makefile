############################################################################
# boards/arm/stm32wl5/nucleo-wl55jc/cpu2/Makefile
#
# Build M0+ (CPU2) bare-metal IPCC firmware independently of NuttX.
#
# Usage (from this directory):
#   make          — compile and link → ipcc_m0plus.bin
#   make flash    — flash to CPU2 flash region (0x08020000) via OpenOCD
#   make clean    — remove all build artifacts
#   make size     — show section sizes
############################################################################

TARGET   = ipcc_m0plus

# Toolchain
CC       = arm-none-eabi-gcc
AS       = arm-none-eabi-gcc
OBJCOPY  = arm-none-eabi-objcopy
SIZE     = arm-none-eabi-size

# Linker script (one level up in scripts/)
LDSCRIPT = ../scripts/wl55jc_cm0p.ld

# CPU flags — Cortex-M0+ thumb only (no Thumb-2 extensions)
CPU      = -mcpu=cortex-m0plus -mthumb

# Compiler flags
CFLAGS   = $(CPU)              \
           -O2                 \
           -Wall               \
           -Wextra             \
           -g                  \
           -fdata-sections     \
           -ffunction-sections \
           -DSTM32WL55xx       \
           -DCORE_CM0PLUS

# Assembler flags
ASFLAGS  = $(CPU) -g

# Linker flags
LDFLAGS  = $(CPU)                    \
           -T $(LDSCRIPT)            \
           -Wl,--gc-sections         \
           -Wl,-Map=$(TARGET).map    \
           --specs=nano.specs        \
           --specs=nosys.specs       \
           -nostartfiles

# Source files
C_SRCS   = ipcc_m0plus_main.c
S_SRCS   = startup_cm0p.s

# Object files
C_OBJS   = $(C_SRCS:.c=.o)
S_OBJS   = $(S_SRCS:.s=.o)
ALL_OBJS = $(C_OBJS) $(S_OBJS)

# ── Default target ─────────────────────────────────────────────────────────
all: $(TARGET).bin

# ── Link ───────────────────────────────────────────────────────────────────
$(TARGET).elf: $(ALL_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

# ── Binary ─────────────────────────────────────────────────────────────────
$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@
	@echo ""
	@echo "╔══════════════════════════════════════╗"
	@echo "║  Built: $(TARGET).bin                 "
	@echo "║  Flash target: 0x08020000             "
	@echo "╚══════════════════════════════════════╝"
	@$(SIZE) $(TARGET).elf

# ── Compile C ──────────────────────────────────────────────────────────────
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# ── Assemble ───────────────────────────────────────────────────────────────
%.o: %.s
	$(AS) $(ASFLAGS) -c $< -o $@

# ── Flash via OpenOCD ──────────────────────────────────────────────────────
flash: $(TARGET).bin
	openocd \
	  -f interface/stlink.cfg \
	  -f target/stm32wlx.cfg \
	  -c "init" \
	  -c "targets" \
	  -c "reset halt" \
	  -c "halt" \
	  -c "flash probe 0" \
	  -c "program ipcc_m0plus.bin 0x08020000 verify" \
	  -c "reset run" \
	  -c "exit"

# ── Size report ────────────────────────────────────────────────────────────
size: $(TARGET).elf
	$(SIZE) $<

# ── Clean ──────────────────────────────────────────────────────────────────
clean:
	rm -f $(ALL_OBJS) $(TARGET).elf $(TARGET).bin $(TARGET).map

.PHONY: all flash size clean