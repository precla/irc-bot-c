TARGET = irc-bot-c
TARGET_DEBUG = irc-bot-c_dev
CC = gcc
CFLAGS = -Wall -std=c18 -pedantic
BUILD = -O2
LDLIBS = -lcurl -lircclient -lcrypto -lssl
SRC = $(wildcard *.c)
SRCTOOBJ = $(patsubst %.c,%.o, $(SRC))
#SRCTOOBJDEV = $(patsubst %.c,%.o, $(SRC))
DEST = build/

.PHONY default: $(TARGET)
.PHONY all: default
.PHONY debug: $(TARGET_DEBUG)
.PHONY clean: clean

# build for production, with optimization flag:
$(TARGET): $(SRCTOOBJ)
	mkdir -p $(DEST)
	$(CC) $(CFLAGS) -o $(DEST)$@ $(SRCTOOBJ) $(LDLIBS)
	-rm *.o

$(SRCTOOBJ): %.o: %.c
	$(CC) $(CFLAGS) $(BUILD) -c $< -o $@

# build for debug, with debug flag:
#$(TARGET_DEBUG): $(SRCTOOBJDEV)
#	$(CC) $(CFLAGS) -g -o $(DEST)$@ $(SRCTOOBJDEV) $(LDLIBS)
#	-rm *.o

#$(SRCTOOBJDEV): %.o: %.c
#	$(CC) $(CFLAGS) -g -c $< -o $@

# clean up the mess
clean:
	-rm *.o $(DEST)$(TARGET)
