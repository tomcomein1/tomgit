include env_sqld.mk

all:build
build:$(OBJS)
	cc -dy -L$(ORACLE_HOME)/lib -L$(ORACLE_HOME)/rdbms/lib -o $(EXE) $(OBJS) -lsqlnet -lncr -lclient -lcommon -lgeneric -ltk21etc -lsqlnet -lncr -ltk21etc -lsqlnet -lclient -lcommon -lgeneric -lepc -lnlsrtl3 -lc3v6 -lcore3 -lnlsrtl3 -lcore3 -lnlsrtl3 -lsocket -lnsl_s -lm -lcore3 -lsocket -lnsl_s -lm -lc
.c.o:
	cc -O -c -I$(ORACLE_HOME)/rdbms/demo -I$(ORACLE_HOME)/rdbms/public -I. $*.c
