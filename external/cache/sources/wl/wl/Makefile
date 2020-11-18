CC = $(CROSS_COMPILE)gcc

#IFLAGS := -I../include -I../shared -g -Wall -static
IFLAGS := -I../include -I../shared -g -Wall
DFLAGS := -DBCMWPA2 -DWLCNT -DWLBTAMP -Wextra -DWLPFN -DWLPFN_AUTO_CONNECT -DLINUX -DRWLASD -DRWL_SOCKET -DRWL_DONGLE -DRWL_WIFI
#IFLAGS += -DIL_BIGENDIAN
IFLAGS += -DSDTEST -DTARGETENV_android -Dlinux -DLINUX -DD11AC_IOTYPES
#IFLAGS += -DSDTEST -Dlinux -DLINUX -DD11AC_IOTYPES

obj-wl = wlu.o \
         wlu_common.o \
         wlu_linux.o \
         wlu_cmd.o \
         wlu_iov.o \
         wlu_pipe.o \
         wlu_pipe_linux.o \
         wlu_client_shared.o \
         wlc_ppr.o \
         wlu_rates_matrix.o

WL = wl
ALL = all
LIBLINK := ../shared/libshared.a
LIBWL = libwl.a

CFLAGS += $(IFLAGS) $(DFLAGS)
ifeq ($(LIB),y)
CFLAGS += -DLIB
endif

all: $(ALL) 

$(ALL):
	make clean
	make $(WL)
	mv wl wl.back
	make clean
	make $(LIBWL) LIB=y
	mv wl.back wl

$(WL): $(obj-wl)
	$(CC) -o $@ $(obj-wl) $(LIBLINK)

$(LIBWL): $(obj-wl)
	ar rscv $(LIBWL) $(obj-wl) $(LIBLINK)
#	$(CC) $(obj-wl) -shared -o $(LIBWL) $(LIBLINK)

# for sequance
#	$(CC) -Wl,-elf2flt -o $@ $(obj-wl)

clean:
	-rm -f $(WL) *.o *.so *.a
