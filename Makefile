
PIC=16f84
#PIC=16f628
PICTYPE=pic14
SDCCPREFIX=/usr/local/sdcc
INCDIR=$(SDCCPREFIX)/share/sdcc/include/
LIBDIR=$(SDCCPREFIX)/share/sdcc/lib/
PROCOPTS=-m$(PICTYPE) -p$(PIC) --use-non-free 
PICOPTS=$(PROCOPTS) -I$(INCDIR) -L$(LIBDIR) \
	-DPIC=$(PIC) -Dp$(PIC) -DKHZ=10000LU -DPICTYPE=pic14 --stack-size 10
DEPS=tabule.c tabule.h timer.h serial.h

all: sender.hex receiver.hex sender.ihex receiver.ihex

sender.hex: $(DEPS)
	sdcc $(PICOPTS) -DIS_SENDER tabule.c
	@mv tabule.hex sender.hex
	@mv tabule.lst sender.lst
	@mv tabule.asm sender.asm
	@mv tabule.cod sender.cod

receiver.hex: $(DEPS)
	sdcc $(PICOPTS) -DIS_RECEIVER tabule.c
	@mv tabule.hex receiver.hex
	@mv tabule.lst receiver.lst
	@mv tabule.asm receiver.asm
	@mv tabule.cod receiver.cod

receiver.ihex: receiver.hex
	objcopy  -O ihex receiver.hex receiver.ihex

sender.ihex: sender.hex
	objcopy  -O ihex sender.hex sender.ihex

clean:
	rm -f *.cod *.hex *.ihex *.asm *.o *.p *.err *.lst
	
