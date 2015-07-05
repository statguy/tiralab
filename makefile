#--*-Makefile-*--
CC=g++
CFLAGS=-c -O3
LDFLAGS=
SOURCES=test.cc btree.cc skiplist.cc rng.cc
INCLUDES=btree.h skiplist.h rng.h
OBJECTS=$(SOURCES:.cc=.o)
TARGET=test

all: $(SOURCES) $(TARGET)

$(OBJECTS): $(INCLUDES)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cc.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET) *~
