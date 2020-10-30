# ----------------------------------------------------------------------------
# Makefile for building tapp
#
#

CFLAGS				= -Wall -O2
CC				= gcc
INSTALL				= install

TARGET				= brcm_patchram_plus

all: $(TARGET)

$(TARGET): brcm_patchram_plus.c
	$(CC) $(CFLAGS) $< -o $@
clean distclean:
	rm -rf *.o $(TARGET)
# ----------------------------------------------------------------------------

.PHONY: $(PHONY) install clean distclean
