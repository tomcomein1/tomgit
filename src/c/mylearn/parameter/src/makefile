ESQL = esql -cp -q64 -g -DINFORMIX_DATABASE
CC = cc -qcpluscmt -q64
LD = ld -b64 -brtl -G -bnoentry -bexpall
AR = ar -X64 -rv
RM = rm
MV = mv

INCL = -I./ -I$(INFORMIXDIR)/incl/esql
LIB = -L../libs -L./ -L$(INFORMIXDIR)/lib/esql -lc -liconv
INFORMIXLIB=-lc -L${INFORMIXDIR}/lib -L${INFORMIXDIR}/lib/esql  -lifsql -lifasf -lifgen -lifos -lifgls -ltli_r -lm_r ${INFORMIXDIR}/lib/esql/checkapi.o

FLAG = -w -brtl $(INCL)

OBJS = para_lib.o para_db.o para_svr.o para_busi.o ifx_face.o
EXE = para_svr

.SUFFIXES: .ec .o
.ec.o:
	$(ESQL) -c $(INCL) $(<)
	$(CC) -c -D_H_LOCALEDEF $(FLAG) $(LIB) $(INFORMIXLIB) $(*).c
	$(RM) -f $(*).c

all: clean EXE

EXE:$(OBJS)
	$(ESQL) -o $(EXE) $(OBJS) $(LIB)
	$(MV) $(EXE) $(HOME)/bin

clean:
	$(RM) -f $(EXE) $(LD_LIB) $(AR_LIB) $(OBJS) *.c *.icp *.tmp

