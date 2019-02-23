UARM_H		= listx.h /usr/include/uarm/libuarm.h /usr/include/uarm/uARMconst.h /usr/include/uarm/uARMtypes.h /usr/include/uarm/arch.h
PHASE1_H 	= pcb.h asl.h
CONST_H		= const.h
OBJECTS		= pcb.o asl.o p1test_rikaya_v0.o
UARM_LIBS	= /usr/include/uarm/ldscripts/elf32ltsarm.h.uarmcore.x -o mika	 /usr/include/uarm/crtso.o /usr/include/uarm/libuarm.o

Però si può adattare..

Se si fa un match dei due, si riesce.. (speriamo)


all: rikaya

p1: p1test_rikaya_v0.o pcb.o asl.o
	arm-none-eabi-ld -T $(UARM_LIBS)
	elf2uarm -k rikaya

mika: $(OBJECTS)
	arm-none-eabi-ld -T $(UARM_LIBS) $(OBJECTS)
	elf2uarm -k rikaya

asl.o: asl.c include/asl.h
	arm-none-eabi-gcc -mcpu=arm7tdmi -c -Wall -I . -I "/usr/include/uarm" -I "/usr/include" -I "/usr/include/bits"-I "./include" -o asl.o asl.c

pcb.o: pcb.c include/pcb.h
	arm-none-eabi-gcc -mcpu=arm7tdmi -c -Wall -I . -I "/usr/include/uarm" -I "/usr/include" -I "/usr/include/bits"-I "./include" -o pcb.o pcb.c

p1test.o: p1test_rikaya_v0.c $(UARM_H) $(PHASE1_H)
	arm-none-eabi-gcc -mcpu=arm7tdmi -c -Wall -I . -I "/usr/include/uarm" -I "/usr/include" -I "/usr/include/bits" -I "./include" -o p1test_rikaya_v0.o p1test_rikaya_v0.c

clean:
	rm -rf *o rikaya

cleanall:
rm -rf *o rikaya rikaya.core.uarm rikaya.stab.uarm