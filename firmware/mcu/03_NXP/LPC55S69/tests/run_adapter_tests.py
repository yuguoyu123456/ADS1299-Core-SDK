"""Execute LPC55 adapter with genuine SDK headers and SPI API doubles."""
import argparse,subprocess,tempfile
from pathlib import Path
from elftools.elf.elffile import ELFFile
from unicorn import Uc,UC_ARCH_ARM,UC_MODE_THUMB,UC_MODE_MCLASS
from unicorn.arm_const import UC_ARM_REG_SP,UC_ARM_REG_LR,UC_ARM_REG_PC,UC_ARM_REG_R0
p=argparse.ArgumentParser(description=__doc__); p.add_argument('--cc',type=Path,required=True)
p.add_argument('--sdk',type=Path,required=True); p.add_argument('--cmsis',type=Path,required=True)
a=p.parse_args(); t=Path(__file__).resolve().parents[1]; root=t.parents[3]
with tempfile.TemporaryDirectory(prefix='.verify-',dir=t/'tests') as temp:
    elf=Path(temp)/'test.elf'
    sdk=a.sdk.resolve()
    inc=[sdk/'devices/LPC55S69',sdk/'devices/LPC55S69/drivers',sdk/'drivers/common',sdk/'drivers/flexcomm',sdk/'drivers/flexcomm/spi',sdk/'drivers/lpc_gpio',a.cmsis.resolve(),t/'mcux_adapter',t/'ads1299_port',root/'firmware/core_driver/ads1299']
    subprocess.run([str(a.cc.resolve()),'-mcpu=cortex-m3','-mthumb','-O1','-std=c11',
        '-DCPU_LPC55S69JBD100_cm33_core0','-DSPI_RETRY_TIMES=100000','-Wall','-Wextra','-Werror','-ffreestanding','-fno-builtin','-nostdlib',*[f'-I{x}' for x in inc],
        str(t/'mcux_adapter/ads1299_lpc55_hal.c'),str(t/'tests/test_adapter.c'),
        '-Wl,-Ttext=0x10000,-Tdata=0x20000000,-e,test_main','-lc','-lgcc','-o',str(elf)],check=True)
    uc=Uc(UC_ARCH_ARM,UC_MODE_THUMB|UC_MODE_MCLASS)
    uc.mem_map(0x10000,0x100000); uc.mem_map(0x20000000,0x100000)
    with elf.open('rb') as f:
        e=ELFFile(f)
        for s in e.iter_segments():
            if s['p_type']=='PT_LOAD' and s['p_filesz']: uc.mem_write(s['p_vaddr'],s.data())
        entry=e.header['e_entry']
    uc.reg_write(UC_ARM_REG_SP,0x200ff000); uc.reg_write(UC_ARM_REG_LR,0x100001)
    uc.emu_start(entry|1,0x100000,count=2000000)
    if uc.reg_read(UC_ARM_REG_PC)!=0x100000: raise RuntimeError('Instruction budget exceeded')
    result=uc.reg_read(UC_ARM_REG_R0)
    if result: raise RuntimeError(f'test_adapter.c assertion failed at line {result}')
print('PASS: LPC55 adapter chunking, CS ownership, null buffers, timeout, FIFO faults and validation.')
print('SPI APIs doubled; no actual vendor driver execution or hardware test claimed.')
