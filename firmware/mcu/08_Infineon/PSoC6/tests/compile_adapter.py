"""Compile actual CM4 port and PDL SPI source with official headers; no fixtures."""
import argparse,subprocess,tempfile
from pathlib import Path
p=argparse.ArgumentParser(description=__doc__)
for name in ('pdl','core-lib','bsp','cmsis','cc'): p.add_argument('--'+name,type=Path,required=True)
p.add_argument('--cm0p',type=Path,help='Link complete reference including official PSOC6_02 CM0P_SLEEP image')
a=p.parse_args()
t=Path(__file__).resolve().parents[1]; root=t.parents[3]
pdl=a.pdl.resolve()
inc=[a.bsp.resolve(),pdl/'drivers/include',pdl/'devices/COMPONENT_CAT1A/include',
     a.core_lib.resolve()/'include',a.cmsis.resolve(),t/'ads1299_port',t/'pdl_adapter',
     root/'firmware/core_driver/ads1299']
sources=[*(root/'firmware/core_driver/ads1299').glob('*.c'),*(t/'ads1299_port').glob('*.c'),
         t/'pdl_adapter/ads1299_psoc6_hal.c',t/'examples/main_ads1299.c',
         pdl/'drivers/source/cy_scb_spi.c']
if a.cm0p:
    bsp=a.bsp.resolve()
    sources += [t/'board/reference_image.c',bsp/'COMPONENT_CM4/system_psoc6_cm4.c',
                bsp/'COMPONENT_CM4/TOOLCHAIN_GCC_ARM/startup_psoc6_02_cm4.S',
                a.cm0p.resolve()/'COMPONENT_CM0P_SLEEP/psoc6_02_cm0p_sleep.c',
                pdl/'devices/COMPONENT_CAT1A/source/cy_device.c',
                pdl/'drivers/source/TOOLCHAIN_GCC_ARM/cy_syslib_ext.S']
    sources += [pdl/'drivers/source'/('cy_'+name+'.c') for name in
                ('scb_common','gpio','sysclk','syslib','sysint','ipc_drv','ipc_pipe','ipc_sema',
                 'flash','sysanalog','syspm','wdt')]
with tempfile.TemporaryDirectory(prefix='.verify-',dir=t/'tests') as temp:
    objects=[]
    for i,source in enumerate(sources):
        obj=Path(temp)/f'{i}.o'; objects.append(str(obj))
        subprocess.run([str(a.cc.resolve()),'-mcpu=cortex-m4','-mthumb','-std=c11',
            '-DCY8C624ABZI_S2D44','-DCOMPONENT_CM4','-Wall','-Wextra','-Werror',
            '-ffunction-sections','-fdata-sections',
            *[f'-I{x}' for x in inc],'-c',str(source),'-o',str(obj)],check=True)
    if a.cm0p:
        elf=Path(temp)/'psoc6-reference.elf'
        # Use the compiler's genuine C runtime entry/exit objects, not stubs.
        def crt(name):
            result=Path(subprocess.check_output([str(a.cc.resolve()),'-mcpu=cortex-m4',
                '-mthumb','-print-file-name='+name],text=True).strip())
            if not result.is_file(): raise RuntimeError('Missing compiler runtime: '+name)
            return str(result)
        subprocess.run([str(a.cc.resolve()),'-mcpu=cortex-m4','-mthumb',
                        '-nostartfiles','--specs=nano.specs','--specs=nosys.specs',
                        crt('crti.o'),crt('crtbegin.o'),*objects,
                        crt('crtend.o'),crt('crtn.o'),'-Wl,--gc-sections',
                        '-T'+str(bsp/'COMPONENT_CM4/TOOLCHAIN_GCC_ARM/linker.ld'),
                        '-o',str(elf)],check=True)
        nm=a.cc.resolve().with_name(a.cc.name.replace('gcc','nm'))
        undefined=subprocess.check_output([str(nm),'-u',str(elf)],text=True)
        if undefined.strip(): raise RuntimeError(undefined)
        symbols={}
        for line in subprocess.check_output([str(nm),'-n',str(elf)],text=True).splitlines():
            fields=line.split()
            if len(fields)==3: symbols[fields[2]]=int(fields[0],16)
        assert symbols['__cy_m0p_code_start']==0x10000000
        assert 0<symbols['__cy_m0p_code_end']-symbols['__cy_m0p_code_start']<=0x2000
        assert symbols['__Vectors']==0x10002000
        print('PASS: nonempty CM0P image at 0x10000000; CM4 vectors at 0x10002000; unresolved symbols: 0')
        import hashlib
        print('ELF SHA256:',hashlib.sha256(elf.read_bytes()).hexdigest())
        size=a.cc.resolve().with_name(a.cc.name.replace('gcc','size'))
        subprocess.run([str(size),str(elf)],check=True)
        subprocess.run([str(size),'-A',str(elf)],check=True)
        print('PASS: CM4 reference link with vendor CM0P_SLEEP; no boot/hardware execution claimed.')
print(f'PASS: {len(sources)} real source objects; no hardware execution claimed.')
