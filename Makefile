CC = picc18
CFLAGS = --chip=18F258
OUTPUT = abe
SOURCES = adcon.c crc.c sdi-12.c serial.c i2c.c xtend.c modbus.c \
    modbus_ascii.c lcd.c main.c
MODBUS_ADDR = 1
PIC_CLK = 4000000

all: clean compile

compile:
	@$(CC) $(CFLAGS) -DMODBUS_ADDR=$(MODBUS_ADDR) -DPIC_CLK=$(PIC_CLK) \
	    $(SOURCES) -o$(OUTPUT)_$(MODBUS_ADDR).hex
	@echo Compiled with Modbus Address $(MODBUS_ADDR) and freq $(PIC_CLK)Hz

clean:
# Stupid hack to get this thing to work Win32 GNU CoreUtils rm
	@touch temp.sdb temp.obj temp.p1 temp.lst temp.sim temp.cof \
	    temp.hxl tmp.sym tmp~
	@rm -f *.sdb *.obj *.p1 *.lst *.sim *.cof *.hxl *.sym *~
