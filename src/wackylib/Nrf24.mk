PERIPHERALS += spi

VPATH += $(WACKYLIB_DIR)
SRC += Nrf24.cpp
INCLUDES += -I$(WACKYLIB_DIR)
include $(WACKYLIB_DIR)/Spi.mk
