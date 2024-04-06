DEVKIT_ADV = /usr/local/devkitadv
GCC = $(DEVKIT_ADV)/bin/arm-agb-elf-gcc
OBJCOPY = $(DEVKIT_ADV)/bin/arm-agb-elf-objcopy
GBA_PATCHER = /usr/local/bin/gba-patcher
SRC_DIR = src
INCLUDE_DIR = include
ASM_DIR = asm

flappygba:
	echo "int IntrTable = 0;" > /tmp/$(USER)-intrtable.c
	$(DEVKIT_ADV)/bin/arm-agb-elf-gcc -I./include/ -c -O3 -std=c99 $(SRC_DIR)/*
	$(DEVKIT_ADV)/bin/arm-agb-elf-gcc -c $(ASM_DIR)/*
	$(GCC) -O3 -nostartfiles /tmp/$(USER)-intrtable.c $(DEVKIT_ADV)/arm-agb-elf/lib/crt0.o -o flappygba.elf $(SRC_DIR)/*.o $(ASM_DIR)/*.o -lm
	$(OBJCOPY) -O binary flappygba.elf flappygba.gba
	$(GBA_PATCHER) flappygba.gba
	rm -f $(SRC_DIR)/*.o $(ASM_DIR)/*.o *.elf /tmp/$(USER)-intrtable.c
