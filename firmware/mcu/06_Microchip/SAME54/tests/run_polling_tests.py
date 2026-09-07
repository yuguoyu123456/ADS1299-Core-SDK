"""Execute original polling adapter on modeled MMIO; not device qualification."""
import argparse, subprocess, tempfile
from pathlib import Path
from elftools.elf.elffile import ELFFile
from unicorn import Uc, UC_ARCH_ARM, UC_MODE_THUMB, UC_MODE_MCLASS, UC_HOOK_MEM_READ, UC_HOOK_MEM_WRITE
from unicorn.arm_const import UC_ARM_REG_SP, UC_ARM_REG_LR, UC_ARM_REG_PC, UC_ARM_REG_R0
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--reference',type=Path,required=True)
p.add_argument('--cc',type=Path,required=True)
a=p.parse_args()
t=Path(__file__).resolve().parents[1]; root=t.parents[3]
src=a.reference.resolve()/'apps/sercom/spi/master/spi_self_loopback_interrupt/firmware/src'
inc=[src/'config/sam_e54_xpro',src/'packs/ATSAME54P20A_DFP',
     src/'packs/CMSIS/CMSIS/Core/Include',t/'ads1299_port',t/'harmony_adapter',
     root/'firmware/core_driver/ads1299']
with tempfile.TemporaryDirectory(prefix='.verify-',dir=t/'tests') as tmp:
    elf=Path(tmp)/'test.elf'
    subprocess.run([str(a.cc.resolve()),'-mcpu=cortex-m4','-mthumb','-O1','-std=c11',
        '-Wall','-Wextra','-Werror','-ffreestanding','-nostdlib',*[f'-I{x}' for x in inc],
        str(t/'harmony_adapter/ads1299_same54_hal.c'),str(t/'tests/test_polling.c'),
        '-Wl,-Ttext=0x10000,-Tdata=0x20000000,-e,test_main','-lgcc','-o',str(elf)],check=True)
    uc=Uc(UC_ARCH_ARM,UC_MODE_THUMB|UC_MODE_MCLASS)
    uc.mem_map(0x10000,0x100000); uc.mem_map(0x20000000,0x100000)
    with elf.open('rb') as f:
        e=ELFFile(f)
        for seg in e.iter_segments():
            if seg['p_type']=='PT_LOAD' and seg['p_filesz']: uc.mem_write(seg['p_vaddr'],seg.data())
        symbols={s.name:s['st_value'] for s in e.get_section_by_name('.symtab').iter_symbols()}
        entry=e.header['e_entry']
    def word(name): return int.from_bytes(uc.mem_read(symbols[name],4),'little')
    spi=word('test_spi_base'); port=word('test_port_base')
    for page in {spi&~0xfff,port&~0xfff}: uc.mem_map(page,0x1000)
    flags=[int.from_bytes(uc.mem_read(symbols['test_flags']+4*i,4),'little') for i in range(5)]
    dre,rxc,txc,error,overflow=flags
    addresses={key:spi+word('test_'+key+'_offset') for key in ('flag','data','status','sync')}
    state={'ready':False,'byte':0}
    def read(uc,access,address,size,value,user):
        mode=word('test_mode')
        if address==addresses['flag']:
            v=dre|(0 if mode==2 else txc)
            if state['ready'] and mode!=1 or mode==4: v|=rxc
            if mode==3: v|=error
        elif address==addresses['status']: v=overflow if mode==3 else 0
        elif address==addresses['sync']: v=1 if mode==5 else 0
        elif address==addresses['data']:
            v=state['byte']; state['ready']=False
        else: return
        uc.mem_write(address,int(v).to_bytes(size,'little'))
    def write(uc,access,address,size,value,user):
        if address==addresses['data']: state.update(ready=True,byte=(value^0x5a)&255)
        # A disabled SERCOM flushes receive state in this model.
        if address==spi and not value&2: state['ready']=False
    uc.hook_add(UC_HOOK_MEM_READ,read,begin=spi,end=spi+0x100)
    uc.hook_add(UC_HOOK_MEM_WRITE,write,begin=spi,end=spi+0x100)
    uc.reg_write(UC_ARM_REG_SP,0x200ff000); uc.reg_write(UC_ARM_REG_LR,0x100001)
    uc.emu_start(entry|1,0x100000,count=2000000)
    if uc.reg_read(UC_ARM_REG_PC)!=0x100000: raise RuntimeError('Instruction limit exceeded')
    result=uc.reg_read(UC_ARM_REG_R0)
    if result: raise RuntimeError(f'test_polling.c assertion failed at line {result}')
print('PASS: modeled MMIO normal/null transfers, timeout, TXC wait, overflow, stale RX, sync stall and baud ceiling.')
print('No vendor startup/PLIB executed in emulator; no physical timing, routing or acquisition test claimed.')
