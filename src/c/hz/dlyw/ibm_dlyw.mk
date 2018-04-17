include env_dlyw.mk

all:build
build:$(OBJS)
	$(ORACLE_HOME)/bin/oraxlc -q64 -L$(ORACLE_HOME)/lib64 -L$(ORACLE_HOME)/rdbms/lib64 -o $(EXE) $(OBJS) -lclntsh
.c.o:
	$(ORACLE_HOME)/bin/oraxlc -DIBMUNIX -O -c -q64 -qcpluscmt -I$(ORACLE_HOME)/rdbms/demo -I$(ORACLE_HOME)/rdbms/public -I$(ORACLE_HOME)/plsql/public -I$(ORACLE_HOME)/network/public -I. $*.c

