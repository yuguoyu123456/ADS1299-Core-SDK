"""Compile and link against the real, externally installed TI SDK. No mocks."""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import tempfile

p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--sdk', type=Path, required=True)
p.add_argument('--cmsis', type=Path, required=True)
p.add_argument('--cc', required=True)
p.add_argument('--cmake', required=True)
p.add_argument('--ninja', required=True)
a = p.parse_args()
target = Path(__file__).resolve().parents[1]
required = [a.sdk / 'source/ti/driverlib/dl_spi.h',
            a.sdk / 'source/ti/devices/msp/m0p/mspm0g350x.h',
            a.cmsis / 'core_cm0plus.h']
for path in required:
    if not path.is_file():
        p.error(f'Missing official header: {path}')
with tempfile.TemporaryDirectory(prefix='.official-sdk-', dir=target / 'tests') as td:
    build = Path(td)
    subprocess.run([a.cmake, '-S', str(target / 'tests/cmake'), '-B', str(build),
        '-G', 'Ninja', f'-DCMAKE_MAKE_PROGRAM={a.ninja}', '-DCMAKE_SYSTEM_NAME=Generic',
        f'-DCMAKE_C_COMPILER={a.cc}', f'-DTI_SDK_ROOT={a.sdk.resolve().as_posix()}',
        f'-DCMSIS_CORE_INCLUDE={a.cmsis.resolve().as_posix()}'], check=True)
    subprocess.run([a.cmake, '--build', str(build)], check=True)
    elf = build / 'ads1299_launchpad_reference.elf'
    suffix = '.exe' if Path(a.cc).suffix == '.exe' else ''
    tool_dir = Path(a.cc).parent
    undefined = subprocess.check_output([str(tool_dir / ('arm-none-eabi-nm' + suffix)),
        '-u', str(elf)], text=True)
    if undefined.strip():
        raise RuntimeError(f'Undefined ELF symbols: {undefined}')
    evidence = {'result': 'real TI SDK reference ELF linked',
        'scope': 'no device execution, flash operation, or acquisition test',
        'undefined_symbols': 0,
        'compiler': subprocess.check_output([a.cc, '--version'], text=True).splitlines()[0],
        'size': subprocess.check_output([str(tool_dir / ('arm-none-eabi-size' + suffix)),
            str(elf)], text=True).splitlines()[1].split()[:4],
        'elf_sha256': hashlib.sha256(elf.read_bytes()).hexdigest(),
        'headers_sha256': {str(path): hashlib.sha256(path.read_bytes()).hexdigest()
                           for path in required}}
print(json.dumps(evidence, indent=2))
