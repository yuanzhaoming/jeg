.PHONY: all clean

SRCS_CPU6502=cpu6502.c cpu6502_opcodes.c cpu6502_debug.c
SRCS_NES=$(SRCS_CPU6502) ppu.c nes.c cartridge.c
SRCS_UI=$(SRCS_NES) ui.c

all: jeg

jeg: $(SRCS_UI)
	$(CC) $(SRCS_UI) -I. -O3 -o $@ `sdl-config --cflags --libs` -Wall -pedantic -DWITHOUT_DECIMAL_MODE

test: run_klaus2m5 run_test_roms

test_klaus2m5: $(SRCS_CPU6502) test/klaus2m5/system.c 
	cat test/klaus2m5/6502_functional_test.bin|xxd -i >test/klaus2m5/rom.inc
	$(CC) $(SRCS_CPU6502) -I. -Itest/klaus2m5 test/klaus2m5/system.c -o $@ -O3

run_klaus2m5: test_klaus2m5
	./test_klaus2m5 test/klaus2m5/6502_functional_test.bin

test_roms: $(SRCS_NES) test/nes_roms/test_roms.c
	$(CC) $(SRCS_NES) -I. test/nes_roms/test_roms.c `sdl-config --cflags --libs` -o $@ -O3

run_test_roms: test_roms
	cd test/nes_roms && ../../test_roms -p test.key

clean:
	rm jeg test_klaus2m5 test/klaus2m5/rom.inc test_roms -rf
