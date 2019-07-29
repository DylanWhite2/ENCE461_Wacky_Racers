TARGET = hat.bin

SRC = hat.cpp

BOARD = hat

DRIVERS = syslog

WACKYLIB_DIR = ../../wackylib
include $(WACKYLIB_DIR)/Pacer.mk
include $(WACKYLIB_DIR)/Adc.mk
include $(WACKYLIB_DIR)/UsbSerial.mk
include $(WACKYLIB_DIR)/UsartSerial.mk
include $(WACKYLIB_DIR)/Mpu9250.mk
include $(WACKYLIB_DIR)/Nrf24.mk

include ../../boards/board.mk
