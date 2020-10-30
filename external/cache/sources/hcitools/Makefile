#CFLAGS := $(CFLAGS) -g
CFLAGS = -I. -Ilib -Wall -DVERSION=\"5.10\" -Wno-strict-aliasing
CFLAGS  += -Ilib/bluetooth

# include rule.mk
include ./rule.mk

# define objects collection variable
lib = \
	lib/bluetooth.o \
	lib/hci.o \
	lib/sdp.o

hciattach_objs = \
	hciattach.o \
	hciattach_ath3k.o \
	hciattach_bcm43xx.o \
	hciattach_intel.o \
	hciattach_qualcomm.o \
	hciattach_st.o \
	hciattach_ti.o \
	hciattach_tialt.o \
	hciattach_rtk.o \
	hciattach_xr.o \
	hciattach_sprd.o

hciconfig_objs = \
	hciconfig.o \
	csr.o

hcitool_objs = \
	hcitool.o

btmon_objs = \
	monitor/main.o \
	monitor/mainloop.o \
	monitor/display.o \
	monitor/hcidump.o \
	monitor/btsnoop.o \
	monitor/control.o \
	monitor/packet.o \
	monitor/vendor.o \
	monitor/lmp.o \
	monitor/l2cap.o \
	monitor/uuid.o \
	monitor/sdp.o \
	monitor/crc.o \
	monitor/ll.o

# add your target(s) to all
.PHONY: all
all: hciattach_opi hciconfig hcitool btmon

# directory
hciattach_opi: $(hciattach_objs) $(lib)
	$(LINK_MSG)
	$(LINKX)

hciconfig: $(hciconfig_objs) $(lib)
	$(LINK_MSG)
	$(LINKX)

hcitool: $(hcitool_objs) $(lib)
	$(LINK_MSG)
	$(LINKX)

btmon: $(btmon_objs) $(lib)
	$(LINK_MSG)
	$(LINKX)

# change objs to your objects collection variable
$(hciattach_objs): %.o: %.c
	$(COMPILE_MSG)
	$(COMPILEX)

$(hciconfig_objs): %.o: %.c
	$(COMPILE_MSG)
	$(COMPILEX)

$(hcitool_objs): %.o: %.c
	$(COMPILE_MSG)
	$(COMPILEX)

$(btmon_objs): %.o: %.c
	$(COMPILE_MSG)
	$(COMPILEX)

# clean temp files
clean:
	-rm -rf $(hciattach_objs) $(hciattach_objs:.o=.d)
	-rm -rf $(hciconfig_objs) $(hciconfig_objs:.o=.d)
	-rm -rf $(hcitool_objs) $(hcitool_objs:.o=.d)
	-rm -rf $(lib) $(lib:.o=.d)
	-rm -rf $(btmon_objs) $(btmon_objs:.o=.d)
	#-rm -rf output
