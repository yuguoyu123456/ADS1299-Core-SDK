"""Compile the G474 transport and shared Core against genuine device headers."""
import argparse,subprocess,tempfile
from pathlib import Path
p=argparse.ArgumentParser(description=__doc__)
for name in ('device','cmsis','cc'): p.add_argument('--'+name,type=Path,required=True)
p.add_argument('--cube',type=Path,help='Official Cube tree for full reference link')
a=p.parse_args(); t=Path(__file__).resolve().parents[1]; root=t.parents[3]
inc=[a.device.resolve()/'Include',a.cmsis.resolve(),t/'ads1299_port',t/'cmsis_adapter',
     root/'firmware/core_driver/ads1299']
sources=[*(root/'firmware/core_driver/ads1299').glob('*.c'),*(t/'ads1299_port').glob('*.c'),
         t/'cmsis_adapter/ads1299_g4_hal.c',t/'examples/main_ads1299.c']
if a.cube:
    sources += [t/'board/reference_image.c',a.device.resolve()/'Source/Templates/system_stm32g4xx.c',
                a.device.resolve()/'Source/Templates/gcc/startup_stm32g474xx.s']
with tempfile.TemporaryDirectory(prefix='.verify-',dir=t/'tests') as tmp:
    objects=[]
    for i,src in enumerate(sources):
        obj=Path(tmp)/f'{i}.o'; objects.append(str(obj))
        subprocess.run([str(a.cc.resolve()),'-mcpu=cortex-m4','-mthumb','-std=c11',
            '-DSTM32G474xx','-Wall','-Wextra','-Werror',*[f'-I{x}' for x in inc],
            '-ffunction-sections','-fdata-sections','-c',str(src),'-o',str(obj)],check=True)
    if a.cube:
        def crt(name):
            path=Path(subprocess.check_output([str(a.cc.resolve()),'-mcpu=cortex-m4',
                '-mthumb','-print-file-name='+name],text=True).strip())
            if not path.is_file(): raise RuntimeError('Missing runtime '+name)
            return str(path)
        elf=Path(tmp)/'reference.elf'
        linker=a.cube.resolve()/'Projects/NUCLEO-G474RE/Templates/STM32CubeIDE/STM32G474RETX_FLASH.ld'
        subprocess.run([str(a.cc.resolve()),'-mcpu=cortex-m4','-mthumb','-nostartfiles',
            '--specs=nano.specs','--specs=nosys.specs',crt('crti.o'),crt('crtbegin.o'),
            *objects,crt('crtend.o'),crt('crtn.o'),'-Wl,--gc-sections','-T'+str(linker),'-o',str(elf)],check=True)
        nm=a.cc.resolve().with_name(a.cc.name.replace('gcc','nm'))
        if subprocess.check_output([str(nm),'-u',str(elf)],text=True).strip():
            raise RuntimeError('Unresolved symbols')
        size=a.cc.resolve().with_name(a.cc.name.replace('gcc','size'))
        subprocess.run([str(size),str(elf)],check=True)
        print('PASS: complete reference link; no unresolved symbols. Hardware boot unverified.')
print(f'PASS: {len(sources)} genuine-header source objects; no hardware test claimed.')
