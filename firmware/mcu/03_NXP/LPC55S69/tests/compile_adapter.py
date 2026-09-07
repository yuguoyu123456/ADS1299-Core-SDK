"""Compile real LPC55 Core0 headers; optionally link the complete reference ELF."""
import argparse,subprocess,tempfile
from pathlib import Path
p=argparse.ArgumentParser(description=__doc__)
for name in ('sdk','cmsis','cc'): p.add_argument('--'+name,type=Path,required=True)
p.add_argument('--link',action='store_true',help='Link cold-boot Core0 reference with genuine vendor startup/linker')
a=p.parse_args(); t=Path(__file__).resolve().parents[1]; root=t.parents[3]; sdk=a.sdk.resolve()
inc=[sdk/'devices/LPC55S69',sdk/'devices/LPC55S69/drivers',sdk/'drivers/common',
     sdk/'drivers/flexcomm',sdk/'drivers/flexcomm/spi',sdk/'drivers/lpc_gpio',
     sdk/'drivers/lpc_iocon',a.cmsis.resolve(),t/'ads1299_port',t/'mcux_adapter',root/'firmware/core_driver/ads1299']
sources=[*(root/'firmware/core_driver/ads1299').glob('*.c'),*(t/'ads1299_port').glob('*.c'),
         t/'mcux_adapter/ads1299_lpc55_hal.c',t/'examples/main_ads1299.c',sdk/'drivers/flexcomm/spi/fsl_spi.c']
if a.link:
    sources += [t/'board/reference_image.c',sdk/'drivers/flexcomm/fsl_flexcomm.c',
        sdk/'drivers/lpc_gpio/fsl_gpio.c',sdk/'devices/LPC55S69/drivers/fsl_clock.c',
        sdk/'devices/LPC55S69/drivers/fsl_reset.c',sdk/'devices/LPC55S69/system_LPC55S69_cm33_core0.c',
        sdk/'devices/LPC55S69/gcc/startup_LPC55S69_cm33_core0.S']
cpu=['-mcpu=cortex-m33','-mthumb']
with tempfile.TemporaryDirectory(prefix='.verify-',dir=t/'tests') as tmp:
    objects=[]
    for i,src in enumerate(sources):
        obj=Path(tmp)/f'{i}.o'; objects.append(str(obj))
        # SDK 2.16 SPI/GPIO and USB clock helpers have unused parameters.
        vendor_flags=['-Wno-error=unused-parameter'] if src in (
            sdk/'drivers/flexcomm/spi/fsl_spi.c',sdk/'drivers/lpc_gpio/fsl_gpio.c',
            sdk/'devices/LPC55S69/drivers/fsl_clock.c') else []
        subprocess.run([str(a.cc.resolve()),*cpu,'-std=c11','-Os','-ffunction-sections','-fdata-sections',
            '-D__STARTUP_CLEAR_BSS','-D__START=ads1299_reference_start',
            '-DCPU_LPC55S69JBD100_cm33_core0','-DSPI_RETRY_TIMES=100000',
            '-Wall','-Wextra','-Werror',*vendor_flags,*[f'-I{x}' for x in inc],
            '-c',str(src),'-o',str(obj)],check=True)
    if a.link:
        def crt(name):
            result=Path(subprocess.check_output([str(a.cc.resolve()),*cpu,'-print-file-name='+name],text=True).strip())
            if not result.is_file(): raise RuntimeError('Missing CRT '+name)
            return str(result)
        elf=Path(tmp)/'reference.elf'
        subprocess.run([str(a.cc.resolve()),*cpu,'-nostartfiles','--specs=nano.specs','--specs=nosys.specs',
            crt('crti.o'),crt('crtbegin.o'),*objects,crt('crtend.o'),crt('crtn.o'),
            '-Wl,--gc-sections','-T'+str(sdk/'devices/LPC55S69/gcc/LPC55S69_cm33_core0_flash.ld'),
            '-o',str(elf)],check=True)
        nm=a.cc.resolve().with_name(a.cc.name.replace('gcc','nm'))
        if subprocess.check_output([str(nm),'-u',str(elf)],text=True).strip(): raise RuntimeError('Unresolved symbols')
        symbols={}
        for line in subprocess.check_output([str(nm),'-n',str(elf)],text=True).splitlines():
            f=line.split()
            if len(f)==3: symbols[f[2]]=int(f[0],16)
        if symbols['__Vectors']!=0: raise RuntimeError('Wrong vector origin')
        for required in ('board_ads1299_hal','SPI_MasterTransferBlocking','ads1299_read_frame_continuous'):
            if required not in symbols: raise RuntimeError('Integration path was removed: '+required)
        subprocess.run([str(a.cc.resolve().with_name(a.cc.name.replace('gcc','size'))),str(elf)],check=True)
        print('PASS: full Core0 reference ELF linked, vectors 0, no unresolved symbols; no board execution.')
print(f'PASS: {len(sources)} genuine-header objects; no hardware claim.')
