include ../config.mk

SRC_DIR := src
INC_DIR := include
OBJ_DIR := obj

override CFILES := $(shell cd $(SRC_DIR) && find -L * -type f -name '*.c' | LC_ALL=C sort)
override CXXFILES := $(shell cd $(SRC_DIR) && find -L * -type f -name '*.cpp' | LC_ALL=C sort)
override NASMFILES := $(shell cd $(SRC_DIR) && find -L * -type f -name '*.asm' | LC_ALL=C sort)
override OBJ := $(addprefix $(OBJ_DIR)/,$(CFILES:.c=.c.o) $(CXXFILES:.cpp=.cpp.o) $(NASMFILES:.asm=.asm.o))
override LINKER_FILE := linker.ld

TOTAL_FILE_COUNT := $(words $(OBJ))
FILE_COUNTER := 0

ifeq ($(DEBUG), true)
	CFLAGS += -DSERIAL_OUT -fsanitize=undefined
endif

AFLAGS += -Wall -i$(INC_DIR)
CFLAGS += -I$(INC_DIR) -Ifreestnd-c-hdrs/$(KARCH)/include -Wall -Wextra \
	-nostdinc -ffreestanding -fno-stack-protector -fno-omit-frame-pointer -fno-stack-check \
	-fno-lto -fno-PIC -ffunction-sections -fdata-sections -mno-80387 -mno-red-zone -mcmodel=kernel \
	-Wno-address-of-packed-member -Wno-missing-field-initializers -Wno-attributes
CXXFLAGS = $(CFLAGS) -fno-rtti -fno-exceptions
LFLAGS += -nostdlib -static -z max-page-size=0x1000 -gc-sections -z noexecstack -T $(LINKER_FILE)

COLOR_COMPILE := \033[0;36m
COLOR_RESET := \033[0m

.PHONY: build setup distclean clean

build: $(OBJ)
	@echo "\nBuilding $(OUTPUT_KERNEL)"
	@$(LD) $(LFLAGS) $^ -o $(OUTPUT_KERNEL)
	
	@echo "Building $(KERNEL_MAP)\n"
	@$(eval tmp := $(shell mktemp))
	@nm -C -n $(OUTPUT_KERNEL) | awk '{$$2=""; print $$0}' > $(tmp)
	
	@printf "%08x\n" "$$(wc -l $(tmp) | awk '{print $$1}')" > $(KERNEL_MAP)
	@cat $(tmp) >> $(KERNEL_MAP)
	@rm -rf $(tmp)
	
	@objcopy --update-section .ksyms=$(KERNEL_MAP) $(OUTPUT_KERNEL)

$(OBJ_DIR)/%.c.o: $(SRC_DIR)/%.c
	$(eval COUNTER := $(shell echo $$(($(COUNTER) + 1))))
	@mkdir -p $(@D)

	@printf "\r\033[K[%d:%d] $(COLOR_COMPILE)Building CC object %s$(COLOR_RESET) " $(COUNTER) $(TOTAL_FILE_COUNT) $@
	@$(CC) $(CFLAGS) -c $^ -o $@

$(OBJ_DIR)/%.cpp.o: $(SRC_DIR)/%.cpp
	$(eval COUNTER := $(shell echo $$(($(COUNTER) + 1))))
	@mkdir -p $(@D)

	@printf "\r\033[K[%d:%d] $(COLOR_COMPILE)Building CXX object %s$(COLOR_RESET) " $(COUNTER) $(TOTAL_FILE_COUNT) $@
	@$(CXX) $(CXXFLAGS) -c $^ -o $@

$(OBJ_DIR)/%.asm.o: $(SRC_DIR)/%.asm
	$(eval COUNTER := $(shell echo $$(($(COUNTER) + 1))))
	@mkdir -p $(@D)

	@printf "\r\033[K[%d:%d] $(COLOR_COMPILE)Building NASM object %s$(COLOR_RESET) " $(COUNTER) $(TOTAL_FILE_COUNT) $@
	@$(AS) $(AFLAGS) $^ -o $@

setup:
	rm -rf freestnd-c-hdrs
	
	curl -Lo $(INC_DIR)/limine.h https://raw.githubusercontent.com/limine-bootloader/limine/refs/heads/v8.x/limine.h
	curl -Lo $(SRC_DIR)/cc-runtime.c https://raw.githubusercontent.com/osdev0/cc-runtime/refs/heads/packed/cc-runtime.c
	git clone https://github.com/osdev0/freestnd-c-hdrs.git

distclean:
	rm -rf $(OBJ_DIR)/* freestnd-c-hdrs/ $(SRC_DIR)/cc-runtime.c $(INC_DIR)/limine.h

clean:
	rm -rf $(OBJ_DIR)/*