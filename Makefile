all: test.bin

CROSS_COMPILE=rx-elf-
CC=$(CROSS_COMPILE)gcc-5
OBJCOPY=$(CROSS_COMPILE)objcopy
OBJDUMP=$(CROSS_COMPILE)objdump
SIZE=$(CROSS_COMPILE)size
CFLAGS += -Os -ffunction-sections -fdata-sections -Wl,--build-id=none
LDFLAGS = -Wl,--gc-sections -Wl,-Map,test.map
LDSCRIPT=r5f563nb.ld
SYSROOT=/usr/rx-elf/sys-root

BASE_LDSCRIPT=$(SYSROOT)/usr/lib/rx.ld

r5f563nb.ld: $(BASE_LDSCRIPT) Makefile
	@sed -e 's|\(ROM[^=]*=[ ]*\)[^,]*\(,[ ]*LENGTH[ ]*=[ ]*\).*$$|\10xfff00000\20x000fffd0 /* 1 MB */|' \
	     -e 's|\(.vectors\)[ ]*([^)]*)[ ]*:|\1 :|' \
	     -e 's|^\(/\*.*This memory layout corresponds to \).*\(\.[ ]*\*/\)$$|\1R5F563NBxxxx\2|' \
	     -e 's|^/\*.*This is the largest RX6.*\*/$$||' \
	     -e 's|^/\*.*ROM.*1 MB \*/$$||' \
	     $(BASE_LDSCRIPT) | \
	perl -0pe 's|(\(_start\)\;\s*})|$$1 > ROM|g' > $@

test.elf: main.c $(LDSCRIPT) Makefile
	$(CC) -T $(LDSCRIPT) $(CFLAGS) $(LDFLAGS) -o $@ main.c

test.bin: test.elf Makefile
	$(OBJCOPY) -Obinary test.elf $@
	$(OBJDUMP) -S test.elf > test.lst
	$(SIZE) test.elf

clean:
	-rm -f test.elf test.bin test.lst
	-rm -f r5f563nb.ld
