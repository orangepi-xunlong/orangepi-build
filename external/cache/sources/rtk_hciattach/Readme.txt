The document describes how to support Realtek UART Bluetooth driver in Linux system.

The supported kernel version is 2.6.32 - 4.15

The default serial protocol of Realtek Bluetooth chip is Three-wire (H5) protocol
with flow control on, parity even and internel 32k clock.

The default baud rate is 115200.

To support Three-wire (H5) protocol, you need to install Realtek hci_uart driver
and rtk_hciattach tool.

1. make sure your UART setting is correct.
   host tx  - controller rx
   host rx  - controller tx
   host rts - controller cts
   host cts - ground
        NC  - controller rts

2. Install Bluetooth kernel driver and rtk_hciattach tool
   $ sudo make install
   If you encounter an error like "depmod: ERROR: Bad version passed /lib/modules/...",
   please change the lines "depmod -a $(MDL_DIR)" to "depmod -a $(shell uname -r)"

3. Initialize Realtek Bluetooth chip by rtk_hciattach
   $ sudo rtk_hciattach -n -s 115200 ttyUSB0 rtk_h5

   for H4 protocol chip
   $ sudo rtk_hciattach -n -s 115200 ttyUSB0 rtk_h4

Tips: ttyUSB0 is serial port name in your system, you should change it
      according to hardware such as ttyS0.

4. Uninstall
   $ sudo make uninstall

If you want to change the parameter such as baud rate and pcm settings, you
should modify rtl8xxx_config file.

