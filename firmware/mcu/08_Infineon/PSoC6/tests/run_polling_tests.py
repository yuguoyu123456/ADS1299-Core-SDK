"""Real adapter/PDL execution with modeled MMIO, not hardware qualification."""
import argparse, subprocess, tempfile
from pathlib import Path
from elftools.elf.elffile import ELFFile
from unicorn import Uc, UC_ARCH_ARM, UC_MODE_THUMB, UC_MODE_MCLASS, UC_HOOK_MEM_READ, UC_HOOK_MEM_WRITE
from unicorn.arm_const import UC_ARM_REG_SP, UC_ARM_REG_LR, UC_ARM_REG_PC, UC_ARM_REG_R0
p=argparse.ArgumentParser(description=__doc__)
for name in ('pdl','core-lib','bsp','cmsis','cc'): p.add_argument('--'+name,type=Path,required=True)
a=p.parse_args(); t=Path(__file__).resolve().parents[1]; root=t.parents[3]
pdl=a.pdl.resolve()
inc=[a.bsp.resolve(),pdl/'drivers/include',pdl/'devices/COMPONENT_CAT1A/include',
     a.core_lib.resolve()/'include',a.cmsis.resolve(),t/'ads1299_port',t/'pdl_adapter',
     root/'firmware/core_driver/ads1299']
with tempfile.TemporaryDirectory(prefix='.verify-',dir=t/'tests') as tmp:
    elf=Path(tmp)/'test.elf'
    sources=[t/'pdl_adapter/ads1299_psoc6_hal.c',t/'tests/test_polling.c',
             pdl/'drivers/source/cy_scb_spi.c',pdl/'drivers/source/cy_scb_common.c']
    subprocess.run([str(a.cc.resolve()),'-mcpu=cortex-m4','-mthumb','-O1','-std=c11',
        '-DCY8C624ABZI_S2D44','-DCOMPONENT_CM4','-Wall','-Wextra','-Werror',
        '-ffunction-sections','-fdata-sections','-ffreestanding','-nostdlib',
        *[f'-I{x}' for x in inc],*[str(x) for x in sources],
        '-Wl,--gc-sections,-u,test_addresses,-u,test_flags,-Ttext=0x10000,-Tdata=0x20000000,-e,test_main','-lgcc','-o',str(elf)],check=True)
    uc=Uc(UC_ARCH_ARM,UC_MODE_THUMB|UC_MODE_MCLASS)
    uc.mem_map(0x10000,0x100000); uc.mem_map(0x20000000,0x100000)
    uc.mem_map(0x40000000,0x1000000)
    with elf.open('rb') as f:
        e=ELFFile(f)
        for seg in e.iter_segments():
            if seg['p_type']=='PT_LOAD' and seg['p_filesz']: uc.mem_write(seg['p_vaddr'],seg.data())
        symbols={s.name:s['st_value'] for s in e.get_section_by_name('.symtab').iter_symbols()}
        entry=e.header['e_entry']
    def word(name,index=0): return int.from_bytes(uc.mem_read(symbols[name]+index*4,4),'little')
    rs,ts,rd,wr,intr,ctrl=[word('test_addresses',i) for i in range(6)]
    sr,overflow,no_data,enabled=[word('test_flags',i) for i in range(4)]
    state={'ready':False,'byte':0}
    def read(uc,access,address,size,value,user):
        mode=word('test_mode')
        if address==rs: v=1 if mode==4 or (state['ready'] and mode!=1) else 0
        elif address==ts: v=128 if mode==5 else sr if mode==2 else 0
        elif address==intr: v=overflow if mode==3 else 0
        elif address==rd:
            v=no_data if mode==6 else state['byte']; state['ready']=False
        else: return
        uc.mem_write(address,int(v).to_bytes(size,'little'))
    def write(uc,access,address,size,value,user):
        if address==wr: state.update(ready=True,byte=(value^0x5a)&255)
        if address==ctrl and not value&enabled: state['ready']=False
    uc.hook_add(UC_HOOK_MEM_READ,read,begin=0x40000000,end=0x40ffffff)
    uc.hook_add(UC_HOOK_MEM_WRITE,write,begin=0x40000000,end=0x40ffffff)
    uc.reg_write(UC_ARM_REG_SP,0x200ff000); uc.reg_write(UC_ARM_REG_LR,0x100001)
    uc.emu_start(entry|1,0x100000,count=2000000)
    if uc.reg_read(UC_ARM_REG_PC)!=0x100000: raise RuntimeError('Instruction limit exceeded')
    result=uc.reg_read(UC_ARM_REG_R0)
    if result: raise RuntimeError(f'test_polling.c assertion failed at line {result}')
print('PASS: real PDL/adapter with modeled normal/null transfers, missing RX, busy shifter, overflow, stale RX, full TX, empty read and invalid configuration.')
print('No board boot, physical timing or EEG acquisition claimed.')
