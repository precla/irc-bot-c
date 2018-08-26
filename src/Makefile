TARGET = irc-bot-c
TARGET_DEBUG = irc-bot-c_dev
CC = gcc
CFLAGS = -Wall -std=c18 -pedantic
BUILD = -O2
LDFLAGS = -lcurl -lircclient -lcrypto -lssl
SRC = $(wildcard *.c)
SRCTOOBJ = $(patsubst %.c,%.o, $(SRC))
#SRCTOOBJDEV = $(patsubst %.c,%.o, $(SRC))
DEST = build/

default: $(TARGET)
all: default
debug: $(TARGET_DEBUG)

# build for production, with optimization flag:
$(TARGET): $(SRCTOOBJ)
	$(CC) $(CFLAGS) -o $(DEST)$@ $(SRCTOOBJ) $(LDFLAGS)
	-rm *.o

$(SRCTOOBJ): %.o: %.c
	$(CC) $(CFLAGS) $(BUILD) -c $< -o $@

# build for debug, with debug flag:
#$(TARGET_DEBUG): $(SRCTOOBJDEV)
#	$(CC) $(CFLAGS) -g -o $(DEST)$@ $(SRCTOOBJDEV) $(LDFLAGS)
#	-rm *.o

#$(SRCTOOBJDEV): %.o: %.c
#	$(CC) $(CFLAGS) -g -c $< -o $@

# clean up the mess
clean:
	-rm *.o $(DEST)$(TARGET)
