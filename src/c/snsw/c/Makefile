#------------------------------------------------------------------------------#
# 版 本 号 : V1.0 
# 作    者 : wangchuanbao  
# 日    期 : 2012-4-17       
# 所属模块 : HTTPSC_CALL原子交易 
# 名    称 : flw_HTTPSC_CALL.so
# 作    用 :    
# 使用注意 :         
# 修改日志 :            
#     修改作者 :    
#     修改时间 :   
#     修改内容 :   
#------------------------------------------------------------------------------#
USERLIB	= -lpack -lcbm -lbase
APPINCLUDE= -I$(HOME)/include  -I$(HOME)/incl_c
CFLAGS= $(Q64) -g -qcpluscmt -liconv
TARGET=libsms_http.so 
OBJS=sms_http.o
all:testsms
$(TARGET) : $(OBJS)
	$(CC) $(Q64) -brtl -qmkshrobj -g -o   $@ $(OBJS)  -L$(WORKDIR)/lib $(USERLIB) -liconv
	rm -f $(WORKDIR)/lib/$(TARGET)
#	rm *.o	
	mv  $(TARGET)  $(WORKDIR)/lib/
	echo "$@ is ok!"
.c.o:
	$(CC)  $(CFLAGS)  $(APPINCLUDE) -c $*.c

testsms : $(OBJS) testsms.o
	$(CC) $(CFLAGS) -o $@ $(OBJS) testsms.o 
	echo "$@ is ok!"
.c.o:
	$(CC)  $(CFLAGS)  $(APPINCLUDE) -c $*.c

clean:
	rm -f *.o *.so
