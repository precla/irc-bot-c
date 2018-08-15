TARGET = irc-bot-c
TARGET_DEBUG = irc-bot-c_dev
CC = gcc
CFLAGS = -Wall
BUILD = -O2
LIBS = -lcurl -lircclient -lonig
SRC = $(wildcard *.c)
OBJ = $(wildcard *.o)

default: $(TARGET)
all: default

# build for produciton, with optimization flag:
$(TARGET): objfilesprod
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LIBS)

objfilesprod:
	$(CC) $(CFLAGS) $(BUILD) -c $(SRC)

# build for debug, with debug flag:
$(TARGET_DEBUG): objfilesdebug
	$(CC) $(CFLAGS) -g -o $@ $(OBJ) $(LIBS)

objfilesdebug:
	$(CC) $(CFLAGS) -g -c $(SRC)


clean:
	rm *.o $(TARGET) $(TARGET_DEBUG)
