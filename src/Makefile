TARGET = irc-bot-c
TARGET_DEBUG = irc-bot-c_dev
CC = gcc
CFLAGS = -Wall -std=c18 -pedantic
BUILD = -O2
LDLIBS = -lcurl -lircclient -lcrypto -lssl
SRC = $(wildcard *.c)
SRCTOOBJ = $(patsubst %.c,%.o, $(SRC))
SRCTOOBJDEV = $(patsubst %.c,%.o, $(SRC))
DEST = build/

.PHONY default: t_prod
.PHONY all: default
.PHONY debug: t_dev
.PHONY clean: clean

# build for production, with optimization flag:
t_prod: $(SRCTOOBJ)
	mkdir -p $(DEST)
	$(CC) $(CFLAGS) -o $(DEST)$(TARGET) $(SRCTOOBJ) $(LDLIBS)
	-rm *.o

$(SRCTOOBJ): %.o: %.c
	$(CC) $(CFLAGS) $(BUILD) -c $< -o $@

# build for debug, with debug flag:
t_dev: $(SRCTOOBJDEV)
	$(CC) $(CFLAGS) -g -o $(DEST)$(TARGET_DEBUG) $(SRCTOOBJDEV) $(LDLIBS)
	-rm *.o

$(SRCTOOBJDEV): %.o: %.c
	$(CC) $(CFLAGS) -g -c $< -o $@

# clean up the mess
clean:
	-rm -rf $(DEST)
