"""Real TivaWare header/source checks; optional Energia-startup reference ELF."""
import argparse,subprocess,tempfile
from pathlib import Path
p=argparse.ArgumentParser(description=__doc__)
for name in ('sdk','cc'): p.add_argument('--'+name,type=Path,required=True)
p.add_argument('--energia',type=Path,help='External pinned Energia checkout for startup/linker')
a=p.parse_args(); t=Path(__file__).resolve().parents[1]; root=t.parents[3]
inc=[a.sdk.resolve(),t/'tivaware_adapter',t/'ads1299_port',root/'firmware/core_driver/ads1299']
sources=[*(root/'firmware/core_driver/ads1299').glob('*.c'),*(t/'ads1299_port').glob('*.c'),
    t/'tivaware_adapter/ads1299_tm4c_hal.c',t/'examples/main_ads1299.c',
    a.sdk.resolve()/'driverlib/ssi.c',a.sdk.resolve()/'driverlib/gpio.c']
if a.energia:
    energia=a.energia.resolve()
    inc += [energia/'cores/tivac',energia/'variants/EK-TM4C1294XL']
    sources += [t/'board/reference_image.c',a.sdk.resolve()/'driverlib/sysctl.c',energia/'cores/tivac/startup_gcc.c']
cpu=['-mcpu=cortex-m4','-mthumb']
with tempfile.TemporaryDirectory(prefix='.verify-',dir=t/'tests') as tmp:
    objects=[]
    for i,src in enumerate(sources):
        obj=Path(tmp)/f'{i}.o'; objects.append(str(obj))
        extra=['-Dmain=ads1299_example_main'] if a.energia and src==t/'examples/main_ads1299.c' else []
        if a.energia and src in (energia/'cores/tivac/startup_gcc.c',a.sdk.resolve()/'driverlib/sysctl.c'):
            extra+=['-Wno-error=unused-parameter']
        subprocess.run([str(a.cc.resolve()),*cpu,'-Os','-std=gnu11','-ffunction-sections','-fdata-sections',
            '-Dgcc','-D__TM4C1294NCPDT__=',*extra,
            '-DPART_TM4C1294NCPDT=','-DTARGET_IS_TM4C129_RA1','-Wall','-Wextra','-Werror',
            *[f'-I{x}' for x in inc],'-c',str(src),'-o',str(obj)],check=True)
    if a.energia:
        def crt(name):
            path=Path(subprocess.check_output([str(a.cc.resolve()),*cpu,'-print-file-name='+name],text=True).strip())
            if not path.is_file(): raise RuntimeError('Missing CRT '+name)
            return str(path)
        elf=Path(tmp)/'reference.elf'
        subprocess.run([str(a.cc.resolve()),*cpu,'-nostartfiles','--specs=nano.specs','--specs=nosys.specs',
            crt('crti.o'),*objects,crt('crtn.o'),'-Wl,--gc-sections,-e,ResetISR',
            '-T'+str(energia/'variants/EK-TM4C1294XL/lm4fcpp_snowflake.ld'),'-o',str(elf)],check=True)
        nm=a.cc.resolve().with_name(a.cc.name.replace('gcc','nm'))
        if subprocess.check_output([str(nm),'-u',str(elf)],text=True).strip(): raise RuntimeError('Undefined symbols')
        symbols={}
        for line in subprocess.check_output([str(nm),'-n',str(elf)],text=True).splitlines():
            f=line.split()
            if len(f)==3: symbols[f[2]]=int(f[0],16)
        if symbols['g_pfnVectors']!=0 or symbols['_estack']!=0x20040000: raise RuntimeError('Vector/stack placement')
        for name in ('board_ads1299_hal','SSIDataPutNonBlocking','ads1299_read_frame_continuous'):
            if name not in symbols: raise RuntimeError('Missing integration path '+name)
        subprocess.run([str(a.cc.resolve().with_name(a.cc.name.replace('gcc','size'))),str(elf)],check=True)
        print('PASS full reference ELF, zero unresolved symbols, vectors 0, stack 0x20040000; no board boot claim.')
print(f'PASS {len(sources)} genuine-header objects; no hardware claim.')
