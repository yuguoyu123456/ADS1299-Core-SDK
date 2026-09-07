"""Compile real LPC55 Core0 headers and vendor SPI source; object-only check."""
import argparse,subprocess,tempfile
from pathlib import Path
p=argparse.ArgumentParser(description=__doc__)
for name in ('sdk','cmsis','cc'): p.add_argument('--'+name,type=Path,required=True)
a=p.parse_args(); t=Path(__file__).resolve().parents[1]; root=t.parents[3]; sdk=a.sdk.resolve()
inc=[sdk/'devices/LPC55S69',sdk/'devices/LPC55S69/drivers',sdk/'drivers/common',
     sdk/'drivers/flexcomm',sdk/'drivers/flexcomm/spi',sdk/'drivers/lpc_gpio',
     a.cmsis.resolve(),t/'ads1299_port',t/'mcux_adapter',root/'firmware/core_driver/ads1299']
sources=[*(root/'firmware/core_driver/ads1299').glob('*.c'),*(t/'ads1299_port').glob('*.c'),
         t/'mcux_adapter/ads1299_lpc55_hal.c',t/'examples/main_ads1299.c',sdk/'drivers/flexcomm/spi/fsl_spi.c']
with tempfile.TemporaryDirectory(prefix='.verify-',dir=t/'tests') as tmp:
    for i,src in enumerate(sources):
        subprocess.run([str(a.cc.resolve()),'-mcpu=cortex-m33','-mthumb','-std=c11',
            '-DCPU_LPC55S69JBD100_cm33_core0','-DSPI_RETRY_TIMES=100000',
            '-Wall','-Wextra','-Werror',*[f'-I{x}' for x in inc],
            '-c',str(src),'-o',str(Path(tmp)/f'{i}.o')],check=True)
print(f'PASS: {len(sources)} genuine-header objects; no full link, behavior or hardware claim.')
