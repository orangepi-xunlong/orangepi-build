
all: wl_tool dhd_tool

wl_tool:
	$(MAKE) -C shared CROSS_COMPILE=$(CROSS_COMPILE)
	$(MAKE) -C wl CROSS_COMPILE=$(CROSS_COMPILE)

dhd_tool:
	$(MAKE) -C shared CROSS_COMPILE=$(CROSS_COMPILE)
	$(MAKE) -C dhd CROSS_COMPILE=$(CROSS_COMPILE)

clean:
	$(MAKE) clean -C shared CROSS_COMPILE=$(CROSS_COMPILE)
	$(MAKE) clean -C wl CROSS_COMPILE=$(CROSS_COMPILE)
	$(MAKE) clean -C dhd CROSS_COMPILE=$(CROSS_COMPILE)
