include env_bkfxd.mk

all:build
build:$(OBJS)
	cc +DA2.0W +DS2.0 -L$(ORACLE_HOME)/lib64 -L$(ORACLE_HOME)/rdbms/lib64 -Wl,+s -Wl,+n -o $(EXE) $(OBJS) -lclntsh -l:libcl.a -l:librt.sl -lpthread -l:libnss_dns.1 -l:libdld.sl
.c.o:
	cc +DA2.0W -DSS_64BIT_SERVER -DHPUNIX +DS2.0 +Z -c -I$(ORACLE_HOME)/rdbms/demo -I$(ORACLE_HOME)/rdbms/public -I$(ORACLE_HOME)/plsql/public -I$(ORACLE_HOME)/network/public -I. $*.c
