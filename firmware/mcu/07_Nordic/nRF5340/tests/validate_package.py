"""Package/layer checks only; compile_zephyr.py supplies actual build evidence."""
from pathlib import Path
import re

target = Path(__file__).resolve().parents[1]
cmake = (target / 'CMakeLists.txt').read_text(encoding='utf-8')
assert 'zephyr_adapter/ads1299_zephyr_hal.c' in cmake
assert 'ads1299_port/ads1299_spi.c' in cmake
assert 'generic_board_port.c' not in cmake and 'src/board_sdk.c' not in cmake
adapter = (target / 'zephyr_adapter/ads1299_zephyr_hal.c').read_text(encoding='utf-8')
for token in ('spi_transceive', 'SPI_MODE_CPHA', 'SPI_WORD_SET(8)',
              'GPIO_OUTPUT_HIGH', 'gpio_pin_set_raw', 'k_busy_wait'):
    assert token in adapter, token
assert not re.search(r'\b(?:CONFIG[1-4]|CH[1-8]SET|LOFF|BIAS_SENSP)\b', adapter)
assert 'ads1299_write_register' not in adapter
overlay = (target / 'app.overlay').read_text(encoding='utf-8')
assert '/delete-property/ cs-gpios;' in overlay
for role, pin in (('SCK', 4), ('MOSI', 6), ('MISO', 5)):
    assert f'NRF_PSEL(SPIM_{role}, 1, {pin})' in overlay
example = (target / 'examples/main_ads1299.c').read_text(encoding='utf-8')
assert 'ads1299_latest_frame' in example and 'if (ready < 0)' in example
print('nRF5340 hardware-SPI package/layer checks passed; no hardware test implied')
