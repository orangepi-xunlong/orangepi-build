CC = $(CROSS_COMPILE)gcc

#IFLAGS := -I../include -I../wl -I../shared -g -Wall -static
IFLAGS := -I../include -I../wl -I../shared -g -Wall
DFLAGS := -DBCMWPA2 -DWLCNT -DWLBTAMP -Wextra -DWLPFN -DWLPFN_AUTO_CONNECT -DLINUX -DRWLASD -DRWL_SOCKET -DRWL_DONGLE -DRWL_WIFI -DD11AC_IOTYPES
#IFLAGS += -DIL_BIGENDIAN

obj-shared = \
	bcmutils.o \
	bcmwifi_channels.o \
	bcm_app_utils.o \
	miniopt.o \
	wlu_client_shared.o \
	wlu_common.o \
	wlu_pipe.o \
	wlu_pipe_linux.o \

CFLAGS := $(IFLAGS) $(DFLAGS)

LIBSHARED = libshared.a

$(LIBSHARED): $(obj-shared)
	ar rscv $(LIBSHARED) $(obj-shared)
#	$(CC) $(obj-shared) -shared -o $(LIBSHARED)

clean:
	-rm -f *.o *.a *.so
