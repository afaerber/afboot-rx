all: test.bin

CROSS_COMPILE=rx-elf-
CC=$(CROSS_COMPILE)gcc-5
OBJCOPY=$(CROSS_COMPILE)objcopy
OBJDUMP=$(CROSS_COMPILE)objdump
SIZE=$(CROSS_COMPILE)size
CFLAGS += -Os -ffunction-sections -fdata-sections -Wl,--build-id=none
LDFLAGS = -Wl,--gc-sections -Wl,-Map,test.map
LDSCRIPT=gr-sakura.ld
SYSROOT=/usr/rx-elf/sys-root

#BASE_LDSCRIPT=$(SYSROOT)/usr/lib/rx.ld
BASE_LDSCRIPT=/usr/rx-elf/lib/rx.ld

r5f563nb.ld: $(BASE_LDSCRIPT) Makefile
	@sed -e 's|\(ROM[^=]*=[ ]*\)[^,]*\(,[ ]*LENGTH[ ]*=[ ]*\).*$$|\10xfff00000\20x000fffd0 /* 1 MB */|' \
	     -e 's|^\(/\*.*This memory layout corresponds to the \).*\(\.[ ]*\*/\)$$|\1R5F563NBxxxx\2|' \
	     -e 's|^/\*.*ROM.*1 MB \*/$$||' \
	     -e 's|^/\*.*This is the largest RX6.*\*/$$||' \
	     $(BASE_LDSCRIPT) > $@

gr-sakura.ld: r5f563nb.ld Makefile
	@sed -e 's|\(ROM[^=]*=[ ]*\)[^,]*\(,[ ]*LENGTH[ ]*=[ ]*\).*$$|\10xfff00000\20x00070000 /* 448 KB */|' \
	     -e 's|\(.vectors\)[ ]*([^)]*)[ ]*:|\1 :|' \
	     -e 's|R5F563NBxxxx|GR-Sakura board (R5F563NBDDFP)|' \
	     r5f563nb.ld | \
	perl -0pe 's|(\(_start\)\;\s*})|$$1 > ROM|g' > $@

test.elf: main.c $(LDSCRIPT) Makefile
	$(CC) -T $(LDSCRIPT) $(CFLAGS) $(LDFLAGS) -o $@ main.c

test.bin: test.elf Makefile
	$(OBJCOPY) -Obinary test.elf $@
	$(OBJDUMP) -S test.elf > test.lst
	$(SIZE) test.elf

clean:
	-rm -f test.elf test.bin test.lst
	-rm -f r5f563nb.ld gr-sakura.ld
