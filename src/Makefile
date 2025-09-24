UNAME_M := $(shell uname -m)

ifeq ($(UNAME_M),aarch64)
PREFIX:=i386-unknown-elf-
BOOTIMG:=/usr/local/grub/lib/grub/i386-pc/boot.img
GRUBLOC:=/usr/local/grub/bin/
else
PREFIX:=
BOOTIMG:=/usr/local/grub/lib/grub/i386-pc/boot.img
GRUBLOC:=/usr/local/grub/bin/
endif

CC := $(PREFIX)gcc
LD := $(PREFIX)ld
OBJDUMP := $(PREFIX)objdump
OBJCOPY := $(PREFIX)objcopy
SIZE := $(PREFIX)size
CFLAGS := -ffreestanding -mgeneral-regs-only -mno-mmx -m32 -march=i386 -fno-pie -fno-stack-protector -g3 -Wall -I$(SDIR)

QEMU := qemu-system-x86_64

ODIR = obj
SDIR = src

OBJS = kernel_main.o rprintf.o
OBJ = $(patsubst %,$(ODIR)/%,$(OBJS))

# Build .o files from C sources
$(ODIR)/%.o: $(SDIR)/%.c | $(ODIR)
	$(CC) $(CFLAGS) -c -g -o $@ $<

# Build .o files from assembly sources
$(ODIR)/%.o: $(SDIR)/%.s | $(ODIR)
	$(CC) $(CFLAGS) -c -g -o $@ $<

# Ensure obj directory exists
$(ODIR):
	mkdir -p $(ODIR)

# Default target
all: bin rootfs.img

# Link kernel from all object files
bin: $(OBJ)
	$(LD) -melf_i386 $(OBJ) -Tkernel.ld -o kernel
	$(SIZE) kernel

# Build rootfs image
rootfs.img: bin
	dd if=/dev/zero of=rootfs.img bs=1M count=32
	$(GRUBLOC)grub-mkimage -p "(hd0,msdos1)/boot" -o grub.img -O i386-pc normal biosdisk multiboot multiboot2 configfile fat exfat part_msdos
	dd if=$(BOOTIMG) of=rootfs.img conv=notrunc
	dd if=grub.img of=rootfs.img conv=notrunc bs=512 seek=1
	echo 'start=2048, type=83, bootable' | sfdisk rootfs.img
	mkfs.vfat --offset 2048 -F16 rootfs.img
	mcopy -i rootfs.img@@1M kernel ::/
	mmd -i rootfs.img@@1M boot
	mcopy -i rootfs.img@@1M grub.cfg ::/boot
	@echo " -- BUILD COMPLETED SUCCESSFULLY --"

# Run in QEMU
run: rootfs.img
	$(QEMU) -m 512M -hda rootfs.img

# Debug target
debug: rootfs.img
	./launch_qemu.sh

# Clean up
clean:
	rm -f grub.img kernel rootfs.img $(OBJ)

UNAME_M := $(shell uname -m)

ifeq ($(UNAME_M),aarch64)
PREFIX:=i386-unknown-elf-
BOOTIMG:=/usr/local/grub/lib/grub/i386-pc/boot.img
GRUBLOC:=/usr/local/grub/bin/
else
PREFIX:=
BOOTIMG:=/usr/local/grub/lib/grub/i386-pc/boot.img
GRUBLOC:=/usr/local/grub/bin/
endif

CC := $(PREFIX)gcc
LD := $(PREFIX)ld`
OBJDUMP := $(PREFIX)objdump
OBJCOPY := $(PREFIX)objcopy
SIZE := $(PREFIX)size
CFLAGS := -ffreestanding -mgeneral-regs-only -mno-mmx -m32 -march=i386 -fno-pie -fno-stack-protector -g3 -Wall -I$(SDIR)

QEMU := qemu-system-x86_64

ODIR = obj
SDIR = src

OBJS = kernel_main.o rprintf.o
OBJ = $(patsubst %,$(ODIR)/%,$(OBJS))

# Build .o files from C sources
$(ODIR)/%.o: $(SDIR)/%.c | $(ODIR)
	$(CC) $(CFLAGS) -c -g -o $@ $<

# Build .o files from assembly sources
$(ODIR)/%.o: $(SDIR)/%.s | $(ODIR)
	$(CC) $(CFLAGS) -c -g -o $@ $<

# Ensure obj directory exists
$(ODIR):
	mkdir -p $(ODIR)

# Default target
all: bin rootfs.img

# Link kernel from all object files
bin: $(OBJ)
	$(LD) -melf_i386 $(OBJ) -Tkernel.ld -o kernel
	$(SIZE) kernel

# Build rootfs image
rootfs.img: bin
	dd if=/dev/zero of=rootfs.img bs=1M count=32
	$(GRUBLOC)grub-mkimage -p "(hd0,msdos1)/boot" -o grub.img -O i386-pc normal biosdisk multiboot multiboot2 configfile fat exfat part_msdos
	dd if=$(BOOTIMG) of=rootfs.img conv=notrunc
	dd if=grub.img of=rootfs.img conv=notrunc bs=512 seek=1
	echo 'start=2048, type=83, bootable' | sfdisk rootfs.img
	mkfs.vfat --offset 2048 -F16 rootfs.img
	mcopy -i rootfs.img@@1M kernel ::/
	mmd -i rootfs.img@@1M boot
	mcopy -i rootfs.img@@1M grub.cfg ::/boot
	@echo " -- BUILD COMPLETED SUCCESSFULLY --"

# Run in QEMU
run: rootfs.img
	$(QEMU) -m 512M -hda rootfs.img

# Debug target
debug: rootfs.img
	./launch_qemu.sh

# Clean up
clean:
	rm -f grub.img kernel rootfs.img $(OBJ)
