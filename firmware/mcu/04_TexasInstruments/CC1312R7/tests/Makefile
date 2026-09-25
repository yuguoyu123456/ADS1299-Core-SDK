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

CPPFLAGS += -I$(PORT_DIR)
CPPFLAGS += -I../../../../core_driver/ads1299

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
