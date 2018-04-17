all:bkfx

BKFX=oracle.o common.o tcpip.o  dltx.o dlywxt.o dlyw_chk.o fgtask.o \
tranbase.o remote.o tools.o exchfile.o connect.o \
d0511.o formfile.o revfile.o tranfile.o statue.o filetool.o datelog.o \
tasktool.o filechar.o


bkfx:$(BKFX)
	cc -L/usr/oracle/app/oracle/product/7.3.4/lib -L/usr/oracle/app/oracle/product/7.3.4/rdbms/lib -o dltx $(BKFX) /usr/oracle/app/oracle/product/7.3.4/lib/libclient.a -ltk21etc -lsqlnet -lncr -ltk21etc -lsqlnet -lclient -lcommon -lgeneric -ltk21etc -lsqlnet -lncr -ltk21etc -lsqlnet -lclient -lcommon -lgeneric -lepc -lnlsrtl3 -lc3v6 -lcore3 -lnlsrtl3 -lcore3 -lnlsrtl3 -lsocket -lnsl_s -lm -lcore3 -lsocket -lnsl_s -lm -lc
.c.o:
	cc  -I/usr/oracle/app/oracle/product/7.3.4/rdbms/demo -I/usr/oracle/app/oracle/product/7.3.4/rdbms/public -I.     -c $*.c
