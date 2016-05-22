INSTALL_HEADER_PATH = ../include/cppmysql/
INSTALL_LIB_PATH = ../lib/

INC_COMM_DIR = -I../include/

CC = g++
AR = ar

CFLAGS = -g $(INC_COMM_DIR) 

HEADERS = $(wildcard *.h)

OBJS =  Connection.o ResultSet.o Fields.o ConnectionPool.o ConnectionPoolMgr.o Error.o 

LIB = libcppmysql
OUTPUT_STATIC = $(LIB).a

.cc.o:
	$(CC) $(CFLAGS) -o $@ -c $^ 

all: $(OUTPUT_STATIC) 

$(OUTPUT_STATIC):$(OBJS) $(HEADERS)
	$(AR) rv $@ $?
	
install:
	-mkdir -p $(INSTALL_HEADER_PATH)
	-mkdir -p $(INSTALL_LIB_PATH)
	-cp -f $(HEADERS) $(INSTALL_HEADER_PATH)
	-cp -f $(LIB)* $(INSTALL_LIB_PATH)

uninstall:
	-rm -fr $(INSTALL_HEADER_PATH)
	-rm -f $(INSTALL_LIB_PATH)$(OUTPUT_STATIC)

clean:
	rm -f $(LIB)* *.o
		
	
