CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -Werror

TARGET := cc1312r7_port_smoke
PORT_DIR := ../ads1299_port

SOURCES := \
	test_port_runner.c \
	test_spi.c \
	test_reset.c \
	test_drdy.c \
	$(PORT_DIR)/ads1299_spi.c \
	$(PORT_DIR)/ads1299_gpio.c \
	$(PORT_DIR)/ads1299_drdy.c


VALIDATION_TARGET := cc1312r7_port_validation
VALIDATION_SOURCES := \
	test_port_validation.c \
	$(PORT_DIR)/ads1299_spi.c \
	$(PORT_DIR)/ads1299_gpio.c \
	$(PORT_DIR)/ads1299_drdy.c

BOARD_DIR := ../board

BOARD_CONFIG_TARGET := cc1312r7_board_config
BOARD_BINDING_TARGET := cc1312r7_board_binding

CPPFLAGS += -I$(PORT_DIR)
CPPFLAGS += -I$(BOARD_DIR)
CPPFLAGS += -I../../../../core_driver/ads1299

.PHONY: all run validation board-tests check clean

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) -o $@

run: $(TARGET)
	./$(TARGET)

$(VALIDATION_TARGET): $(VALIDATION_SOURCES)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(VALIDATION_SOURCES) -o $@

validation: $(VALIDATION_TARGET)
	./$(VALIDATION_TARGET)

$(BOARD_CONFIG_TARGET): test_board_config.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@

$(BOARD_BINDING_TARGET): test_board_binding.c $(BOARD_DIR)/board_ads1299_binding.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@

board-tests: $(BOARD_CONFIG_TARGET) $(BOARD_BINDING_TARGET)
	./$(BOARD_CONFIG_TARGET)
	./$(BOARD_BINDING_TARGET)

check: run validation board-tests

clean:
	rm -f $(TARGET) $(VALIDATION_TARGET) $(BOARD_CONFIG_TARGET) $(BOARD_BINDING_TARGET)
