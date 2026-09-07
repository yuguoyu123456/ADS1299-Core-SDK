"""Compile real FSP BSP objects; optionally link the complete reference image.

TinyUSB 0.18.0 provides externally installed RA6M5 reference BSP configuration.
The optional original SPI binding is separate from a user's generated project.
"""
import argparse
import subprocess
import tempfile
from pathlib import Path

p = argparse.ArgumentParser(description=__doc__)
for name in ('fsp', 'tinyusb', 'cmsis', 'cc'):
    p.add_argument('--'+name, type=Path, required=True)
p.add_argument('--link-reference', action='store_true', help='Link original SPI reference against external FSP startup/linker')
a = p.parse_args()
target = Path(__file__).resolve().parents[1]
root = target.parents[3]
fsp = a.fsp.resolve()/'ra/fsp'
family = a.tinyusb.resolve()/'hw/bsp/ra'
board = family/'boards/ra6m5_ek'
includes = [target/'tests/fsp_config', family, board, board/'ra_cfg/fsp_cfg', board/'ra_cfg/fsp_cfg/bsp',
            board/'ra_gen', a.cmsis.resolve(), fsp/'inc', fsp/'inc/api',
            fsp/'inc/instances', fsp/'src/bsp/cmsis/Device/RENESAS/Include',
            fsp/'src/bsp/mcu/all', fsp/'src/bsp/mcu/ra6m5',
            root/'firmware/core_driver/ads1299', target/'ads1299_port', target/'fsp_adapter']
sources = [*(root/'firmware/core_driver/ads1299').glob('*.c'),
           *(target/'ads1299_port').glob('*.c'), target/'fsp_adapter/ads1299_fsp_hal.c',
           target/'examples/main_ads1299.c', fsp/'src/r_spi/r_spi.c',
           fsp/'src/r_ioport/r_ioport.c']
if a.link_reference:
    sources += [target/'board/reference_image.c']
    sources += [fsp/'src/bsp/cmsis/Device/RENESAS/Source'/name for name in ('startup.c', 'system.c')]
    sources += [fsp/'src/bsp/mcu/all'/('bsp_'+name+'.c') for name in
                ('clocks','common','delay','group_irq','guard','io','irq',
                 'register_protection','sbrk','security','rom_registers')]
with tempfile.TemporaryDirectory(prefix='.verify-', dir=target/'tests') as temp:
    objects = []
    for index, source in enumerate(sources):
        obj = Path(temp)/f'{index}.o'
        objects.append(str(obj))
        cmd = [str(a.cc.resolve()), '-mcpu=cortex-m33', '-mthumb', '-mfloat-abi=soft',
               '-std=c11', '-Wall', '-Wextra', '-Werror', '-c', str(source),
               '-o', str(obj)]
        cmd += [f'-I{x}' for x in includes]
        subprocess.run(cmd, check=True)
    if a.link_reference:
        elf = Path(temp)/'ra6m5-reference.elf'
        script = board/'script'
        subprocess.run([str(a.cc.resolve()), '-mcpu=cortex-m33', '-mthumb',
                        '-mfloat-abi=soft', '-nostartfiles', '--specs=nano.specs',
                        '--specs=nosys.specs', *objects, '-L'+str(script),
                        '-T'+str(script/'fsp.ld'), '-Wl,--defsym=end=__bss_end__',
                        '-Wl,-e,Reset_Handler', '-o', str(elf)], check=True)
        nm = a.cc.resolve().with_name(a.cc.name.replace('gcc','nm'))
        undefined = subprocess.check_output([str(nm), '-u', str(elf)], text=True)
        if undefined.strip(): raise RuntimeError('Undefined symbols: '+undefined)
        size = a.cc.resolve().with_name(a.cc.name.replace('gcc','size'))
        subprocess.run([str(size), str(elf)], check=True)
        import hashlib
        print('ELF SHA256:', hashlib.sha256(elf.read_bytes()).hexdigest())
        print('PASS: reference ELF linked, undefined symbols=0; not flashed or hardware-tested.')
print(f'PASS: {len(sources)} library/example/vendor objects with actual FSP BSP headers, no test fixture.')
if not a.link_reference:
    print('Compile-only run: no board ELF produced.')
