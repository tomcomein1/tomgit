include env_oci.mk

all:build
build:$(OBJS)
	cc -dy -o $(EXE) $(OBJS) -lsocket -lm -lx
.c.o:
	cc -O -c -I. $*.c 
