CC = gcc
LDFLAGS = -lsqlite3 -lcrypto
SOURCES = src/avdaemon.c src/util/db.c src/util/misc.c src/crypto/crypto_handler.c src/log/log.c
OBJECTS=$(SOURCES:.c=.o)
TARGET = avdaemon

.PHONY: all clean

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
	rm -f $(OBJECTS) $(EXECUTABLE)

.c.o:
	$(CC)  -c $< -o $@
	