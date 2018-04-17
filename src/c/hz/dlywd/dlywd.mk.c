include env_dlywd.mk

all:build
build:$(OBJS)
	cc -o $(EXE) $(OBJS) -lsocket 
