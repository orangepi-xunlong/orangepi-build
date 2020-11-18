CC = $(CROSS_COMPILE)gcc

#IFLAGS := -I../include -I../wl -I../shared -g -Wall -static
IFLAGS := -I../include -I../wl -I../shared -g -Wall
DFLAGS := -DBCMWPA2 -DWLCNT -DWLBTAMP -Wextra -DWLPFN -DWLPFN_AUTO_CONNECT -DLINUX -DRWLASD -DRWL_SOCKET -DRWL_DONGLE -DRWL_WIFI
#IFLAGS += -DIL_BIGENDIAN

obj-dhd = dhdu.o dhdu_linux.o ucode_download.o

DHD = dhd
ALL = all
LIBLINK := ../shared/libshared.a
LIBDHD = libdhd.a

CFLAGS += $(IFLAGS) $(DFLAGS)
ifeq ($(LIB),y)
CFLAGS += -DLIB
endif

all: $(ALL)

$(ALL):
	make clean
	make $(DHD)
	mv dhd dhd.back
	make clean
	make $(LIBDHD) LIB=y
	mv dhd.back dhd

$(DHD): $(obj-dhd)
	$(CC) -o $@ $(obj-dhd) $(LIBLINK)

$(LIBDHD): $(obj-dhd)
	ar rscv $(LIBDHD) $(obj-dhd) $(LIBLINK)
#	$(CC) $(obj-dhd) -shared -o $(LIBDHD) $(LIBLINK)

# for sequance
#	$(CC) -Wl,-elf2flt -o $@ $(obj-dhd)

clean:
	-rm -f $(DHD) *.o *.a *.so
