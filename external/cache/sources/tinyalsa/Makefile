CFLAGS := -c -fPIC $(CFLAGS)
INC = include
OBJECTS = mixer.o pcm.o
LIB = libtinyalsa.so
CC=gcc

all: $(LIB) tinyplay tinycap tinymix tinypcminfo
	 cp tinyplay tinycap tinymix tinypcminfo /usr/local/bin
	 cp libtinyalsa.so /usr/lib/

tinyplay: $(LIB) tinyplay.o
	$(CC) tinyplay.o -L. -ltinyalsa -o tinyplay

tinycap: $(LIB) tinycap.o
	$(CC) tinycap.o -L. -ltinyalsa -o tinycap

tinymix: $(LIB) tinymix.o
	$(CC) tinymix.o -L. -ltinyalsa -o tinymix

tinypcminfo: $(LIB) tinypcminfo.o
	$(CC) tinypcminfo.o -L. -ltinyalsa -o tinypcminfo

$(LIB): $(OBJECTS)
	$(CC) -shared $(OBJECTS) -o $(LIB)

.c.o:
	$(CC) $(CFLAGS) $< -I$(INC)

clean:
	-rm -f $(LIB) $(OBJECTS) tinyplay.o tinyplay tinycap.o tinycap \
	tinymix.o tinymix tinypcminfo.o tinypcminfo
