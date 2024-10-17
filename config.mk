OPTIMISE	:= -O2
DEBUG		:= true
OSNAME		:= EpsilonOS
KARCH		:= x86_64
CORES		:= 1

CC=gcc
CXX=g++
AS=nasm
LD=ld
QM=qemu-system-$(KARCH)

CFLAGS=-m64 $(OPT) $(OPTIMISE)
AFLAGS=-felf64 $(OPTIMISE)
LFLAGS=$(OPTIMISE)
QFLAGS=-M q35 -cpu max -m 128M -rtc base=localtime -smp $(CORES) -net none -serial stdio \
	-cdrom $(OUTPUT_OS)

ifeq ($(DEBUG), true)
	CFLAGS += -ggdb
	AFLAGS += -g
	LFLAGS += -g
endif

KERNEL_DIR		:= kernel
OUTPUT_DIR		:= output
RESOURCES_DIR	:= resources

LIMINE_CFG		:= limine.conf
KERNEL_NAME		:= kernel.bin
OUTPUT_OS		:= $(OUTPUT_DIR)/$(OSNAME).iso
OUTPUT_KERNEL	:= $(OUTPUT_DIR)/$(KERNEL_NAME)