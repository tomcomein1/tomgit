OBJS=dlsrv.o tcpip.o  des.o srvora.o dlsrvd.o mobile.o tasktool.o filechar.o oracle.o \
     bpuser.o telecom.o exchange.o service.o zhw_dec.o airline.o policy.o  \
     power.o tax.o yatelecom.o yzsd.o zydes.o zydesx.o getmac.o dl185.o
EXE=dlsrv

program:$(OBJS)
	cc -dy -o $(EXE) $(OBJS) -lpgw -lsocket -lm -lx 
.c.o:
	cc -O -c -I. $*.c
