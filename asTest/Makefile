BASEDIR=.
LOCALLIB=/usr/local/lib
XMLINC=/usr/include/libxml2
ASLIBINC=../ASLib/include
ASLIB=../ASLib/lib
# EXTRAFLAGS=-Wall -Werror
EXTRAFLAGS=

CC=gcc
STRIP=strip
LIBS=-L$(LOCALLIB) -L$(ASLIB) -lxml2 -lrt -lasfw -lasds -lasa -laslib -lpthread
CFLAGS=-I$(BASEDIR)/include -I$(XMLINC) -I$(ASLIBINC)

TARGET=test

#SRC=main.c test_memp.c test_memg.c test_event.c test_thread.c test_timer.c test_list.c test_fifo.c test_time.c test_btree.c test_math.c test_asa.c test_map.c
SRC=*.c

all:clean test
	
test:
	$(CC) -o $(TARGET) $(SRC) $(LIBS) $(CFLAGS) $(EXTRAFLAGS)

dbg:
	$(CC) -o $(TARGET) -g $(SRC) $(LIBS) $(CFLAGS) $(EXTRAFLAGS)

clean:
	rm -f $(TARGET)
