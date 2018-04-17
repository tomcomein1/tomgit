include env_dlywd.mk

all:build
build:$(OBJS)
	cc -q64 -o $(EXE) $(OBJS) -lm
.c.o:
	cc -DIBMUNIX -O -c -q64 -qcpluscmt -I. $*.c

