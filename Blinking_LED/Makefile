# 交叉編譯工具鏈
CROSS_COMPILE = aarch64-linux-gnu-
AS = $(CROSS_COMPILE)as
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

# 編譯選項
CFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles
CFLAGS += -I./include -mgeneral-regs-only

# 連結選項
LDFLAGS = -nostdlib -nostartfiles

# 目標檔案
TARGET = kernel8
ELF = $(TARGET).elf
IMG = $(TARGET).img

# 原始檔案
SRCS_C = src/main.c src/gpio.c
SRCS_S = src/boot.S
OBJS = $(SRCS_C:.c=.o) $(SRCS_S:.S=.o)

# 預設目標
all: $(IMG)

# 編譯規則
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(AS) -c $< -o $@

# 連結
$(ELF): $(OBJS)
	$(LD) $(LDFLAGS) -T linker.ld $(OBJS) -o $@

# 產生 kernel8.img
$(IMG): $(ELF)
	$(OBJCOPY) -O binary $(ELF) $(IMG)

# 反組譯（除錯用）
dump: $(ELF)
	$(OBJDUMP) -D $(ELF) > $(TARGET).dump

# 清理
clean:
	rm -f src/*.o $(ELF) $(IMG) $(TARGET).dump

.PHONY: all clean dump