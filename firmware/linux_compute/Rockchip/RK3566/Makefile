CC ?= gcc
CFLAGS ?= -std=c11 -O2 -Wall -Wextra -Werror
CPPFLAGS += -I. -I../../../core_driver/ads1299

CORE_DIR := ../../../core_driver/ads1299
CORE_SOURCES := \
	$(CORE_DIR)/ads1299.c \
	$(CORE_DIR)/ads1299_frame.c \
	$(CORE_DIR)/ads1299_model.c \
	$(CORE_DIR)/ads1299_multi.c

TARGET := build/rk3566_ads1299_probe
SOURCES := ads1299_linux_port.c probe.c $(CORE_SOURCES)

.PHONY: all clean check

all: $(TARGET)

$(TARGET): $(SOURCES) ads1299_linux_port.h board_config.h
	@mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) -o $@

check: all
	@echo "RK3566 Linux userspace compile OK: $(TARGET)"
	@echo "Hardware probe is intentionally not executed by host validation."

clean:
	rm -rf build
