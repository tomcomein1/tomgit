CC=gcc

exe1=opendbsqlite
obj1=opendbsqlite.o

LIB=-L/usr/local/sqlite3/lib -lsqlite3 -lpthread -ldl -static
INCL=-I/usr/local/sqlite3/include -I./

.SUFFIXES: .c .o
.c.o:
	$(CC) -c $(*).c $(LIB) $(INCL)
all:clean exe1

exe1:$(obj1)
	$(CC) -o opendbsqlite $(obj1) $(LIB)

clean:
	rm -f $(exe1) $(obj1)
