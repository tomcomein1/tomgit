include env_dlyw.mk

all:build
build:$(OBJS)
	cc +DA2.0W +DS2.0 -Wl,+s -Wl,+n -o $(EXE) $(OBJS) -lm 
.c.o:
	cc +DA2.0W -DHPUNIX +DS2.0 +Z -O -c -I. $*.c

