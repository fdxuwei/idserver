#BOOST_DIR :=
MUDUO_DIR = ../third_party/muduo 
CC = g++
CCFLAGS = -g -I../ -I${MUDUO_DIR} #-I${BOOST_DIR}
LDLIBS = -pthread -lmuduo_base -lmuduo_net -lcppmysql -lmysqlclient
LDFLAGS = -L../third_party/build/release/lib/ -L../cppmysql -L/usr/local/mysql/lib 

OBJS = main.o Config.o Daemon.o IdCache.o IdServer.o #InitLog.o
TARGET = idserver

%.o:%.cc
	${CC} -o $@ -c $^ ${CCFLAGS}
${TARGET}: ${OBJS}
	${CC} -o $@ $^ ${LDLIBS} ${LDFLAGS}

clean:
	rm -f *.o


