FILENAME    = main
OBJECTS     = main.o

MCU         = atmega1280

CCLD        = avr-gcc
CCFLAGS     = -mmcu=$(MCU) -std=gnu99 -Wall -Wstrict-prototypes -Os -frename-registers 
CCFLAGS    += -fshort-enums -fpack-struct -funsigned-char -funsigned-bitfields
LDFLAGS     = -mmcu=$(MCU) -Wl,-u,vfprintf -lprintf_min -lwiimote -lglcd -lsdcard -lmp3 -L.
ADD_OBJECTS = rand.o spi.o adc.o hal_wt41_fc_uart.o game.o

PROG        = avrprog2
PRFLAGS     = -m$(MCU)

all: $(FILENAME).elf

$(FILENAME).elf: $(OBJECTS) $(ADD_OBJECTS)
	$(CCLD) $(OBJECTS) $(ADD_OBJECTS) $(LDFLAGS) -o $@ 

%.o: %.c
	$(CCLD) $(CCFLAGS) -c -o $@ $<

install: $(FILENAME).elf
	$(PROG) $(PRFLAGS) --flash w:$<

verify: $(FILENAME).elf
	$(PROG) $(PRFLAGS) --flash v:$<

clean:
	rm -f *.elf *.o
