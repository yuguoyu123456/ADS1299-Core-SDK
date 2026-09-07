"""Execute G474 adapter on modeled MMIO, not physical hardware."""
import argparse,subprocess,tempfile
from pathlib import Path
from elftools.elf.elffile import ELFFile
from unicorn import Uc, UC_ARCH_ARM, UC_MODE_THUMB, UC_MODE_MCLASS, UC_HOOK_MEM_READ, UC_HOOK_MEM_WRITE
from unicorn.arm_const import UC_ARM_REG_SP, UC_ARM_REG_LR, UC_ARM_REG_PC, UC_ARM_REG_R0
p=argparse.ArgumentParser(description=__doc__)
for name in ('device','cmsis','cc'): p.add_argument('--'+name,type=Path,required=True)
a=p.parse_args(); t=Path(__file__).resolve().parents[1]; root=t.parents[3]
inc=[a.device.resolve()/'Include',a.cmsis.resolve(),t/'ads1299_port',t/'cmsis_adapter',root/'firmware/core_driver/ads1299']
with tempfile.TemporaryDirectory(prefix='.verify-',dir=t/'tests') as tmp:
    elf=Path(tmp)/'test.elf'
    subprocess.run([str(a.cc.resolve()),'-mcpu=cortex-m4','-mthumb','-O1','-std=c11','-DSTM32G474xx',
        '-Wall','-Wextra','-Werror','-ffreestanding','-nostdlib',*[f'-I{x}' for x in inc],
        str(t/'cmsis_adapter/ads1299_g4_hal.c'),str(t/'tests/test_polling.c'),
        '-Wl,-Ttext=0x10000,-Tdata=0x20000000,-e,test_main','-lgcc','-o',str(elf)],check=True)
    uc=Uc(UC_ARCH_ARM,UC_MODE_THUMB|UC_MODE_MCLASS)
    uc.mem_map(0x10000,0x100000); uc.mem_map(0x20000000,0x100000)
    uc.mem_map(0x40000000,0x100000); uc.mem_map(0x48000000,0x10000)
    with elf.open('rb') as f:
        e=ELFFile(f)
        for seg in e.iter_segments():
            if seg['p_type']=='PT_LOAD' and seg['p_filesz']: uc.mem_write(seg['p_vaddr'],seg.data())
        symbols={s.name:s['st_value'] for s in e.get_section_by_name('.symtab').iter_symbols()}
        entry=e.header['e_entry']
    def word(name,index=0): return int.from_bytes(uc.mem_read(symbols[name]+index*4,4),'little')
    sr,dr,cr=[word('test_addresses',i) for i in range(3)]
    rxne,txe,bsy,ovr,modf,fre,spe=[word('test_flags',i) for i in range(7)]
    state={'ready':False,'sent':False,'byte':0}
    def read(uc,access,address,size,value,user):
        mode=word('test_mode')
        if address==sr:
            v=0 if mode==5 else txe
            if mode==4 or state['ready'] and mode!=1: v|=rxne
            if mode==2 and state['sent']: v|=bsy
            if mode==3: v|=ovr
            if mode==6: v|=modf
            if mode==7: v|=fre
        elif address==dr:
            v=state['byte']; state['ready']=False
        else: return
        uc.mem_write(address,int(v).to_bytes(size,'little'))
    def write(uc,access,address,size,value,user):
        if address==dr:
            if size!=1: raise RuntimeError('SPI DR access must be byte-sized')
            state.update(ready=True,sent=True,byte=(value^0x5a)&255)
        if address==cr and not value&spe: state.update(ready=False,sent=False)
    uc.hook_add(UC_HOOK_MEM_READ,read,begin=0x40000000,end=0x400fffff)
    uc.hook_add(UC_HOOK_MEM_WRITE,write,begin=0x40000000,end=0x400fffff)
    uc.reg_write(UC_ARM_REG_SP,0x200ff000); uc.reg_write(UC_ARM_REG_LR,0x100001)
    uc.emu_start(entry|1,0x100000,count=2000000)
    if uc.reg_read(UC_ARM_REG_PC)!=0x100000: raise RuntimeError('Instruction limit exceeded')
    result=uc.reg_read(UC_ARM_REG_R0)
    if result: raise RuntimeError(f'test_polling.c assertion failed at line {result}')
print('PASS: modeled normal/null/byte transfers, missing RX/TXE, BSY completion, OVR/MODF/FRE, stale RX, configuration and baud ceiling.')
print('No board startup, electrical timing or hardware acquisition claimed.')
