"""Execute adapter with mock IDF API on ARM emulator; not a C6/IDF runtime test."""
import argparse,subprocess,tempfile
from pathlib import Path
from elftools.elf.elffile import ELFFile
from unicorn import Uc,UC_ARCH_ARM,UC_MODE_THUMB,UC_MODE_MCLASS
from unicorn.arm_const import UC_ARM_REG_SP,UC_ARM_REG_LR,UC_ARM_REG_PC,UC_ARM_REG_R0
p=argparse.ArgumentParser(description=__doc__); p.add_argument('--cc',type=Path,required=True)
a=p.parse_args(); t=Path(__file__).resolve().parents[1]; root=t.parents[3]
with tempfile.TemporaryDirectory(prefix='.verify-',dir=t/'tests') as temp:
    elf=Path(temp)/'test.elf'
    inc=[t/'tests/fixtures',t/'esp_idf_adapter',t/'ads1299_port',root/'firmware/core_driver/ads1299']
    subprocess.run([str(a.cc.resolve()),'-mcpu=cortex-m3','-mthumb','-O1','-std=c11',
        '-Wall','-Wextra','-Werror','-ffreestanding','-fno-builtin','-nostdlib',*[f'-I{x}' for x in inc],
        str(t/'esp_idf_adapter/ads1299_c6_hal.c'),str(t/'tests/test_queue.c'),
        '-Wl,-Ttext=0x10000,-Tdata=0x20000000,-e,test_main','-lgcc','-o',str(elf)],check=True)
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
    if result: raise RuntimeError(f'test_queue.c assertion failed at line {result}')
print('PASS: mock IDF queue boundaries/nulls, queue/result timeout, retained buffers, late completion, wrong descriptor and GPIO fault.')
print('Test-only API doubles; no ESP32-C6 or actual IDF driver execution claimed.')
