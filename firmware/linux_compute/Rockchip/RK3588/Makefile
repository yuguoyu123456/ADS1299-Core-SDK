CC ?= gcc
CFLAGS ?= -std=c11 -O2 -Wall -Wextra -Werror
CPPFLAGS += -I. -I../../../core_driver/ads1299

CORE_DIR := ../../../core_driver/ads1299
CORE_SOURCES := \
	$(CORE_DIR)/ads1299.c \
	$(CORE_DIR)/ads1299_frame.c \
	$(CORE_DIR)/ads1299_model.c \
	$(CORE_DIR)/ads1299_multi.c

TARGET := build/rk3588_ads1299_probe
BOARD_CONFIG_TEST := build/test_board_config
SOURCES := ads1299_linux_port.c probe.c $(CORE_SOURCES)

.PHONY: all clean check test

all: $(TARGET)

$(TARGET): $(SOURCES) ads1299_linux_port.h board_config.h
	@mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) -o $@

$(BOARD_CONFIG_TEST): tests/test_board_config.c board_config.h
	@mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_board_config.c -o $@

test: $(BOARD_CONFIG_TEST)
	./$(BOARD_CONFIG_TEST)

check: all test
	@echo "RK3588 Linux userspace compile OK: $(TARGET)"
	@echo "RK3588 board configuration contract OK: $(BOARD_CONFIG_TEST)"
	@echo "Hardware probe is intentionally not executed by host validation."

clean:
	rm -rf build
