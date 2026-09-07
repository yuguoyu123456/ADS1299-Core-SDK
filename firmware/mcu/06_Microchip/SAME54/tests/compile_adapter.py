"""Compile actual SAME54 adapter against official Harmony/DFP headers, no stubs."""
import argparse
import subprocess
import tempfile
from pathlib import Path
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--reference',type=Path,required=True)
p.add_argument('--cc',type=Path,required=True)
p.add_argument('--gcc-reference',type=Path)
p.add_argument('--linker',type=Path)
a=p.parse_args()
target=Path(__file__).resolve().parents[1]
root=target.parents[3]
src=a.reference.resolve()/'apps/sercom/spi/master/spi_self_loopback_interrupt/firmware/src'
includes=[src/'config/sam_e54_xpro',src/'packs/ATSAME54P20A_DFP',
          src/'packs/CMSIS/CMSIS/Core/Include',target/'ads1299_port',
          target/'harmony_adapter',root/'firmware/core_driver/ads1299']
sources=[*(root/'firmware/core_driver/ads1299').glob('*.c'),
         *(target/'ads1299_port').glob('*.c'),target/'harmony_adapter/ads1299_same54_hal.c',
         target/'examples/main_ads1299.c']
if bool(a.gcc_reference) != bool(a.linker): p.error('Provide both --gcc-reference and --linker')
startup=None
if a.gcc_reference:
    sdk=a.gcc_reference.resolve()/'same54'
    startup=sdk/'gcc/gcc/startup_same54.c'
    sources += [target/'board/reference_image.c',
                src/'config/sam_e54_xpro/peripheral/clock/plib_clock.c',
                src/'config/sam_e54_xpro/peripheral/nvmctrl/plib_nvmctrl.c',
                src/'config/sam_e54_xpro/peripheral/port/plib_port.c',startup]
with tempfile.TemporaryDirectory(prefix='.verify-',dir=target/'tests') as temp:
    objects=[]
    for i,source in enumerate(sources):
        obj=Path(temp)/f'{i}.o'; objects.append(str(obj))
        inc=includes if source!=startup else [sdk/'include',src/'packs/CMSIS/CMSIS/Core/Include']
        subprocess.run([str(a.cc.resolve()),'-mcpu=cortex-m4','-mthumb','-std=c11',
                        '-D__SAME54P20A__','-Wall','-Wextra','-Werror',*[f'-I{x}' for x in inc],
                        '-c',str(source),'-o',str(obj)],check=True)
    if startup:
        elf=Path(temp)/'same54-reference.elf'
        subprocess.run([str(a.cc.resolve()),'-mcpu=cortex-m4','-mthumb','-nostartfiles',
                        '--specs=nano.specs','--specs=nosys.specs',*objects,
                        '-T'+str(a.linker.resolve()),'-o',str(elf)],check=True)
        nm=a.cc.resolve().with_name(a.cc.name.replace('gcc','nm'))
        undefined=subprocess.check_output([str(nm),'-u',str(elf)],text=True)
        if undefined.strip(): raise RuntimeError(undefined)
        import hashlib
        print('ELF SHA256:',hashlib.sha256(elf.read_bytes()).hexdigest())
        size=a.cc.resolve().with_name(a.cc.name.replace('gcc','size'))
        subprocess.run([str(size),str(elf)],check=True)
        print('PASS: complete reference ELF, zero undefined symbols; not flashed.')
print(f'PASS: {len(sources)} real-header objects. No runtime/hardware test claimed.')
