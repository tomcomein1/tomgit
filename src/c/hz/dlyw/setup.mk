all:build
EXE=setup
OBJS=oracle.o setup.o  setuptab.o setupini.o  common.o tcpip.o tools.o \
     filechar.o 
build:$(OBJS)
	cc -o $(EXE) $(OBJS) -lsocket -lm -lx
.c.o:
	cc -O -c -I. $*.c
