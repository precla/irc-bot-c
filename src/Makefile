TARGET = irc-bot-c
CC = gcc
CFLAGS = -Wall
BUILD = -O2
LIBS = -lcurl -lircclient -lonig
SRC = $(wildcard *.c)
OBJ = $(wildcard *.o)

default: $(TARGET)
all: default
	echo build done

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $^

clean:
	rm *.o $(TARGET)
