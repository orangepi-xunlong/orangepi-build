CFLAGS := -c -fPIC $(CFLAGS)
INC = include
OBJECTS = mixer.o pcm.o
LIB = libtinyalsa.so
CC=gcc

all: $(LIB) tinyplay_hub tinycap_hub tinymix tinypcminfo
	 cp tinyplay_hub tinycap_hub tinymix tinypcminfo /usr/local/bin
	 cp libtinyalsa.so /usr/lib/

tinyplay_hub: $(LIB) tinyplay_hub.o
	$(CC) tinyplay_hub.o -L. -ltinyalsa -o tinyplay_hub

tinycap_hub: $(LIB) tinycap_hub.o
	$(CC) tinycap_hub.o -L. -ltinyalsa -o tinycap_hub

tinymix: $(LIB) tinymix.o
	$(CC) tinymix.o -L. -ltinyalsa -o tinymix

tinypcminfo: $(LIB) tinypcminfo.o
	$(CC) tinypcminfo.o -L. -ltinyalsa -o tinypcminfo

$(LIB): $(OBJECTS)
	$(CC) -shared $(OBJECTS) -o $(LIB)

.c.o:
	$(CC) $(CFLAGS) $< -I$(INC)

clean:
	-rm -f $(LIB) $(OBJECTS) tinyplay_hub.o tinyplay_hub tinycap_hub.o tinycap_hub \
	tinymix.o tinymix tinypcminfo.o tinypcminfo
