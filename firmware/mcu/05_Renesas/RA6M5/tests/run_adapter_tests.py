"""Real ARM execution of adapter tests, mocked BSP/peripherals. NOT board build."""
import argparse
import subprocess
import tempfile
from pathlib import Path
from elftools.elf.elffile import ELFFile
from unicorn import Uc, UC_ARCH_ARM, UC_MODE_THUMB, UC_MODE_MCLASS
from unicorn.arm_const import UC_ARM_REG_SP, UC_ARM_REG_LR, UC_ARM_REG_R0

p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--fsp', type=Path, required=True)
p.add_argument('--cc', type=Path, required=True)
a = p.parse_args()
target = Path(__file__).resolve().parents[1]
root = target.parents[3]
api = a.fsp.resolve()/'ra/fsp/inc/api'
for name in ('r_spi_api.h', 'r_ioport_api.h', 'r_transfer_api.h'):
    if not (api/name).is_file(): p.error(f'Missing official API header: {api/name}')
with tempfile.TemporaryDirectory(prefix='.verify-', dir=target/'tests') as temp:
    elf = Path(temp)/'adapter-test.elf'
    cmd = [str(a.cc.resolve()), '-mcpu=cortex-m3', '-mthumb', '-std=c11', '-O1',
           '-Wall', '-Wextra', '-Werror', '-ffreestanding', '-fno-builtin', '-nostdlib',
           '-include', str(target/'tests/fixtures/bsp_fixture.h')]
    cmd += [f'-I{x}' for x in (api, target/'fsp_adapter', target/'ads1299_port',
                               root/'firmware/core_driver/ads1299')]
    cmd += [str(target/'fsp_adapter/ads1299_fsp_hal.c'), str(target/'tests/test_fsp_adapter.c'),
            '-Wl,-Ttext=0x10000,-Tdata=0x20000000,-e,test_main', '-lgcc', '-o', str(elf)]
    subprocess.run(cmd, check=True)
    uc = Uc(UC_ARCH_ARM, UC_MODE_THUMB | UC_MODE_MCLASS)
    uc.mem_map(0x10000, 0x100000)
    uc.mem_map(0x20000000, 0x100000)
    with elf.open('rb') as f:
        image = ELFFile(f)
        for segment in image.iter_segments():
            if segment['p_type'] == 'PT_LOAD' and segment['p_filesz']:
                uc.mem_write(segment['p_vaddr'], segment.data())
        entry = image.header['e_entry']
    stop = 0x100000
    uc.reg_write(UC_ARM_REG_SP, 0x200ff000)
    uc.reg_write(UC_ARM_REG_LR, stop | 1)
    uc.reg_write(UC_ARM_REG_R0, 0xffffffff)
    uc.emu_start(entry | 1, stop, count=16000000)
    from unicorn.arm_const import UC_ARM_REG_PC
    if uc.reg_read(UC_ARM_REG_PC) != stop:
        raise RuntimeError('Instruction budget exhausted; likely unbounded wait')
    result = uc.reg_read(UC_ARM_REG_R0)
    if result: raise RuntimeError(f'Adapter assertion failed at test_fsp_adapter.c:{result}')
print('PASS: ARM-emulated adapter behavior using official SPI/IOPORT API headers and TEST-ONLY BSP fixture.')
print('No RA6M5 BSP build, real peripheral execution, board wiring or hardware validation claimed.')
