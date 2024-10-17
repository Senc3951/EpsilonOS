include config.mk

.PHONY: run debug build kernel setup limine distclean clean

run: $(OUTPUT_OS)
	$(QM) $(QFLAGS)

debug: $(OUTPUT_OS)
	$(QM) -s -S $(QFLAGS)

build: $(OUTPUT_OS)
$(OUTPUT_OS): kernel
	@cp $(OUTPUT_KERNEL) iso/boot
	@cp $(RESOURCES_DIR)/* iso/boot
	@cp limine/limine-bios.sys limine/limine-bios-cd.bin limine/limine-uefi-cd.bin iso/boot/limine/
	@cp limine/BOOTX64.EFI iso/EFI/BOOT/
	@cp limine/BOOTIA32.EFI iso/EFI/BOOT/
	@xorriso -as mkisofs -b boot/limine/limine-bios-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot boot/limine/limine-uefi-cd.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso -o $@
	@./limine/limine bios-install $@

kernel:
	@clear
	@$(MAKE) -C $(KERNEL_DIR) build OUTPUT_KERNEL=$(abspath $(OUTPUT_KERNEL)) DEBUG=$(DEBUG)

setup:
	mkdir -p $(OUTPUT_DIR)
		
	rm -rf iso
	mkdir -p iso
	mkdir -p iso/boot
	mkdir -p iso/boot/limine
	mkdir -p iso/EFI/BOOT
	cp $(LIMINE_CFG) iso/boot/limine/
		
	@$(MAKE) -C $(KERNEL_DIR) setup
	
limine:
	rm -rf limine
	git clone https://github.com/limine-bootloader/limine.git --branch=v8.x-binary --depth=1
	$(MAKE) -C limine	

distclean:
	@$(MAKE) -C $(KERNEL_DIR) distclean

clean:
	@$(MAKE) -C $(KERNEL_DIR) clean