OBJS=pgwtest.o
EXE=pgw

program:$(OBJS)
	cc -dy -o $(EXE) $(OBJS) -lpgw -lsocket -lm -lx 
.c.o:
	cc -O -c -I. $*.c
