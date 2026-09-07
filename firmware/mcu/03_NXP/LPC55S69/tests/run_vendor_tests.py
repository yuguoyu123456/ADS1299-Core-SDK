"""Run genuine NXP fsl_spi.c with a narrow FIFO model, not a board simulation."""
import argparse,struct,subprocess,tempfile
from pathlib import Path
from elftools.elf.elffile import ELFFile
from unicorn import Uc,UC_ARCH_ARM,UC_MODE_THUMB,UC_MODE_MCLASS,UC_HOOK_MEM_READ,UC_HOOK_MEM_WRITE,UC_MEM_READ
from unicorn.arm_const import UC_ARM_REG_SP,UC_ARM_REG_LR,UC_ARM_REG_PC,UC_ARM_REG_R0
p=argparse.ArgumentParser(description=__doc__)
for name in ('sdk','cmsis','cc'): p.add_argument('--'+name,type=Path,required=True)
a=p.parse_args(); t=Path(__file__).resolve().parents[1]; root=t.parents[3]; sdk=a.sdk.resolve()
inc=[sdk/'devices/LPC55S69',sdk/'devices/LPC55S69/drivers',sdk/'drivers/common',
     sdk/'drivers/flexcomm',sdk/'drivers/flexcomm/spi',sdk/'drivers/lpc_gpio',
     a.cmsis.resolve(),t/'mcux_adapter',t/'ads1299_port',root/'firmware/core_driver/ads1299']
with tempfile.TemporaryDirectory(prefix='.verify-',dir=t/'tests') as temp:
    elf=Path(temp)/'test.elf'
    subprocess.run([str(a.cc.resolve()),'-mcpu=cortex-m3','-mthumb','-O1','-std=c11',
        '-DCPU_LPC55S69JBD100_cm33_core0','-DSPI_RETRY_TIMES=1000','-DNDEBUG',
        '-Wall','-Wextra','-Werror','-Wno-error=unused-parameter',
        '-ffunction-sections','-fdata-sections','-nostdlib',*[f'-I{x}' for x in inc],
        str(t/'mcux_adapter/ads1299_lpc55_hal.c'),str(t/'tests/test_vendor_spi.c'),
        str(sdk/'drivers/flexcomm/spi/fsl_spi.c'),
        '-Wl,--gc-sections,--undefined=model_layout,-Ttext=0x10000,-Tdata=0x20000000,-e,test_main',
        '-lc','-lgcc','-o',str(elf)],check=True)
    for scenario in range(4):
        uc=Uc(UC_ARCH_ARM,UC_MODE_THUMB|UC_MODE_MCLASS)
        for address,size in ((0x10000,0x100000),(0x20000000,0x100000),(0x40000000,0x200000)):
            uc.mem_map(address,size)
        with elf.open('rb') as f:
            e=ELFFile(f)
            for seg in e.iter_segments():
                if seg['p_type']=='PT_LOAD' and seg['p_filesz']: uc.mem_write(seg['p_vaddr'],seg.data())
            entry=e.header['e_entry']
            layout=e.get_section_by_name('.symtab').get_symbol_by_name('model_layout')[0]['st_value']
        spi,gpio,cfg,stat,rd,wr,idle,txroom,txempty,rxready,idlemask,rxerr,depth,eot=struct.unpack('<14I',uc.mem_read(layout,56))
        state={'fifo':[],'tx':[],'eot':0}
        def io(u,access,address,size,value,user):
            offset=address-spi
            if access==UC_MEM_READ:
                if offset==cfg: value=depth
                elif offset==stat:
                    value=(txroom|txempty if scenario!=1 else 0)|(rxready if state['fifo'] else 0)
                    if scenario==3 and state['tx']: value|=rxerr
                elif offset==idle: value=0 if scenario==2 else idlemask
                elif offset==rd:
                    if not state['fifo']: raise RuntimeError('RX underflow')
                    value=state['fifo'].pop(0)
                else: return
                u.mem_write(address,int(value).to_bytes(size,'little'))
            elif offset==wr:
                if u.mem_read(gpio,1)!=b'\x00': raise RuntimeError('CS released during SPI')
                state['tx'].append(value&255); state['fifo'].append((value&255)^0x5a)
                if value&eot: state['eot']+=1
        uc.hook_add(UC_HOOK_MEM_READ|UC_HOOK_MEM_WRITE,io,begin=spi,end=spi+0x1000)
        uc.reg_write(UC_ARM_REG_SP,0x200ff000); uc.reg_write(UC_ARM_REG_LR,0x100001)
        uc.reg_write(UC_ARM_REG_R0,scenario)
        uc.emu_start(entry|1,0x100000,count=2000000)
        if uc.reg_read(UC_ARM_REG_PC)!=0x100000: raise RuntimeError('Instruction budget exceeded')
        result=uc.reg_read(UC_ARM_REG_R0)
        if result: raise RuntimeError(f'Scenario {scenario}: C assertion line {result}')
        if scenario==0 and (state['tx']!=list(range(65)) or state['eot']!=3): raise RuntimeError('Chunk/EOT mismatch')
        print(f'PASS real vendor SPI / modeled FIFO scenario {scenario}')
print('Clock/reset are doubled; no physical hardware or cycle-accurate timing claim.')
