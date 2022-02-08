TARGET = irc-bot-c
CC = gcc
CFLAGS = -Wall -std=c18 -pedantic
LDLIBS = -lcurl -lircclient -lcrypto -lssl
SRC = $(wildcard *.c)
SRCTOOBJ = $(patsubst %.c,%.o, $(SRC))
DEST = build

debug: CFLAGS += -g
default: CFLAGS += -O2

default: $(SRCTOOBJ)
	mkdir -p $(DEST)
	$(CC) $(CFLAGS) -o $(DEST)/$(TARGET) $(SRCTOOBJ) $(LDLIBS)
	-rm *.o

debug: default

$(SRCTOOBJ): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# clean up the mess
clean:
	-rm -rf $(DEST)
