

CFLAGS += -O2 -DLINUX -D_GNU_SOURCE -Wall -DDEBUG

CLDFLAGS += -lpthread

APP_BINARY = id3tags

all: $(APP_BINARY)

$(APP_BINARY):APEV2Tag.o ID3V1Tag.o ID3V2Tag.o common.o cpmain.o
	$(CC) $(CFLAGS) $^ $(CLDFLAGS) -o $@
clean:
	rm -rf *.o
	rm -rf $(APP_BINARY)
