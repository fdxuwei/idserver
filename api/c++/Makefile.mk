CC = g++
AR = ar
CCFLAGS = -fPIC -g -O2
LDFLAGS = 

LIB_NAME = idserver
LIB_STATIC = lib${LIB_NAME}.a
LIB_DYNAMIC = lib${LIB_NAME}.so

OBJS = BlockTcpClient.o IdServer.o IdServerImp.o

%.o:%.cc
	${CC} -o $@ -c $^ ${CCFLAGS}

all: ${LIB_STATIC} #${LIB_DYNAMIC}

${LIB_STATIC}: ${OBJS}
	${AR} rv $@ $^
${LIB_DYNAMIC}: ${OBJS}
	${CC} -o ${LIB_DYNAMIC} -shared $^ 
test: test.o
	${CC} -o $@ $? -lidserver -L. -pthread

clean:
	rm -f *.o *.a *.so *.so.*