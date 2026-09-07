"""Real U575 adapter instructions against modeled MMIO; no hardware claim."""
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
    subprocess.run([str(a.cc.resolve()),'-mcpu=cortex-m33','-mthumb','-O1','-std=c11','-DSTM32U575xx',
        '-Wall','-Wextra','-Werror','-ffreestanding','-nostdlib',*[f'-I{x}' for x in inc],
        str(t/'cmsis_adapter/ads1299_u5_hal.c'),str(t/'tests/test_polling.c'),
        '-Wl,-Ttext=0x10000,-Tdata=0x20000000,-e,test_main','-lgcc','-o',str(elf)],check=True)
    uc=Uc(UC_ARCH_ARM,UC_MODE_THUMB|UC_MODE_MCLASS)
    uc.mem_map(0x10000,0x100000); uc.mem_map(0x20000000,0x100000)
    uc.mem_map(0x40000000,0x1000000); uc.mem_map(0x42000000,0x100000)
    with elf.open('rb') as f:
        e=ELFFile(f)
        for seg in e.iter_segments():
            if seg['p_type']=='PT_LOAD' and seg['p_filesz']: uc.mem_write(seg['p_vaddr'],seg.data())
        symbols={s.name:s['st_value'] for s in e.get_section_by_name('.symtab').iter_symbols()}
        entry=e.header['e_entry']
    def word(name,index=0): return int.from_bytes(uc.mem_read(symbols[name]+index*4,4),'little')
    sr,tx,rx,cr,tsize,ifcr,gpio=[word('test_addresses',i) for i in range(7)]
    rxp,txp,eot,ovr,modf,fre,udr,spe,start=[word('test_flags',i) for i in range(9)]
    state={'ready':False,'left':0,'byte':0,'started':False,'cs':1,'chunks':[],'delay':0}
    def read(uc,access,address,size,value,user):
        mode=word('test_mode')
        if address==sr:
            v=0 if mode==5 else txp
            if mode==4 or state['ready'] and mode!=1: v|=rxp
            if state['started'] and not state['left'] and mode!=2: v|=eot
            if mode==3: v|=ovr
            if mode==6: v|=modf
            if mode==7: v|=fre|udr
        elif address==rx:
            if size!=1: raise RuntimeError('RX access is not byte-sized')
            v=state['byte']; state['ready']=False
        else: return
        uc.mem_write(address,int(v).to_bytes(size,'little'))
    def write(uc,access,address,size,value,user):
        if address==tx:
            if size!=1 or not state['left']: raise RuntimeError('Invalid TX access/count')
            state.update(ready=True,left=state['left']-1,byte=(value^0x5a)&255)
        elif address==tsize:
            state['left']=value
            if not state['cs']: state['chunks'].append(value)
        elif address==cr:
            if value&spe and value&start: state['started']=True
            if not value&spe:
                if state['started'] and not word('test_mode'):
                    if word('test_delays')<=state['delay']: raise RuntimeError('Disabled without EOT delay')
                state.update(ready=False,started=False)
            else: state['delay']=word('test_delays')
        elif address==gpio:
            if value&1:
                if not state['cs'] and state['chunks']!=[65535,1]: raise RuntimeError('CS/chunk discontinuity')
                state['cs']=1
            if value&(1<<16): state['cs']=0; state['chunks']=[]
    uc.hook_add(UC_HOOK_MEM_READ,read,begin=0x40000000,end=0x40ffffff)
    uc.hook_add(UC_HOOK_MEM_WRITE,write,begin=0x40000000,end=0x40ffffff)
    uc.hook_add(UC_HOOK_MEM_WRITE,write,begin=0x42000000,end=0x420fffff)
    uc.reg_write(UC_ARM_REG_SP,0x200ff000); uc.reg_write(UC_ARM_REG_LR,0x100001)
    uc.emu_start(entry|1,0x100000,count=30000000)
    if uc.reg_read(UC_ARM_REG_PC)!=0x100000: raise RuntimeError('Instruction limit exceeded')
    result=uc.reg_read(UC_ARM_REG_R0)
    if result: raise RuntimeError(f'test_polling.c assertion failed at line {result}')
print('PASS: nullable/byte transfers, missing RX/TX/EOT, faults/stale RX, EOT delay/failure, 65535+1 split with continuous CS, actual-baud delay.')
print('MMIO model only; no physical clock, TrustZone or board-startup execution.')
