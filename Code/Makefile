PROJECT=dc22-dc801-party-badge

LDCRIPT=core/lpc1347.ld

OPTIMIZATION = 0

#########################################################################

SRC=$(wildcard core/*.c) \
    $(wildcard core/libs/*.c) \
    $(wildcard drivers/*.c) \
    $(wildcard drivers/ble/*.c) \
    $(wildcard *.c)


OBJECTS= $(SRC:.c=.o) 
LSSFILES= $(SRC:.c=.lst) 

HEADERS=$(wildcard core/*.h) \
		$(wildcard core/inc/*.h) \
		$(wildcard drivers/*.h) \
		$(wildcard drivers/ble/*.h) \
		$(wildcard *.h)

#  Compiler Options
GCFLAGS = -std=gnu99 -mcpu=cortex-m3 -mthumb -O$(OPTIMIZATION) -I. -Icore -Icore/inc
# Warnings
#GCFLAGS += -Wstrict-prototypes -Wundef -Wall -Wunreachable-code  
# Optimizazions
#GCFLAGS += -fsingle-precision-constant -funsigned-char -funsigned-bitfields -fshort-enums

# Debug stuff
#GCFLAGS += -Wa,-adhlns=$(<:.c=.lst),-gstabs -g 

LDFLAGS =  -mcpu=cortex-m3 -mthumb -O$(OPTIMIZATION) -nostartfiles  -T$(LDCRIPT) -Wl,-Map=$(PROJECT).map

# This is where you stuck your ARM toolchain that you downloaded from
# https://launchpad.net/gcc-arm-embedded
TOOLCHAINPATH = /usr/bin/

#  Compiler/Linker Paths
GCC = $(TOOLCHAINPATH)/arm-none-eabi-gcc
OBJCOPY = $(TOOLCHAINPATH)/arm-none-eabi-objcopy
REMOVE = rm -f
SIZE = $(TOOLCHAINPATH)/arm-none-eabi-size
STRIP = $(TOOLCHAINPATH)/arm-none-eabi-strip

#########################################################################

all: tools firmware.bin Makefile stats

tools:
	make -C tools
	
firmware.bin: $(PROJECT).elf Makefile tools
	$(OBJCOPY) -R .stack -O binary $(PROJECT).elf firmware.bin
	$(OBJCOPY) -R .stack -O ihex $(PROJECT).elf firmware.hex
	tools/lpcrc firmware.bin
	tools/padto firmware.bin 64
	
$(PROJECT).elf: $(OBJECTS) Makefile
	@echo "  \033[1;34mLD \033[0m (\033[1;33m $(OBJECTS)\033[0m) -> $(PROJECT).elf"
	@$(GCC) $(LDFLAGS) $(OBJECTS) -o $(PROJECT).elf -lm
	$(STRIP) -s $(PROJECT).elf

stats: $(PROJECT).elf Makefile
	$(SIZE) $(PROJECT).elf

clean:
	$(REMOVE) $(OBJECTS)
	$(REMOVE) $(LSSFILES)
	$(REMOVE) firmware.bin
	$(REMOVE) $(PROJECT).elf
	$(REMOVE) $(PROJECT).map
	make -C tools clean

#########################################################################

%.o: %.c Makefile $(HEADERS)
	@echo "  \033[1;34mGCC\033[0m $<"
	@$(GCC) $(GCFLAGS) -o $@ -c $<

#########################################################################

# This works well for dev on a Mac
flash: firmware.bin
	cp firmware.bin /Volumes/CRP\ DISABLD/
	diskutil eject `diskutil list | grep -B 2 CRP | grep dev`
	
copy: firmware.bin stats
	cp firmware.bin /media/sf_VM_Shared/


.PHONY : clean all flash tools

