"""Compile repository code with real SDK headers; never substitutes mock headers."""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import tempfile

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--sdk', type=Path, required=True)
parser.add_argument('--cmsis', type=Path, required=True, help='CMSIS Core Include directory')
parser.add_argument('--cc', required=True, help='arm-none-eabi-gcc executable')
parser.add_argument('--cmake', help='Optionally verify the real CMake library build')
parser.add_argument('--ninja', help='Ninja executable, required with --cmake')
parser.add_argument('--link-reference-ram', action='store_true',
                    help='Also link the EVKB RAM-debug image using external SDK startup/linker')
args = parser.parse_args()
if args.cmake and not args.ninja:
    parser.error('--cmake requires --ninja')
if args.link_reference_ram and not args.cmake:
    parser.error('--link-reference-ram requires --cmake and --ninja')
target = Path(__file__).resolve().parents[1]
core = target.parents[2] / 'core_driver/ads1299'
includes = [args.sdk / p for p in ('devices/MIMXRT1062',
    'devices/MIMXRT1062/drivers', 'drivers/common', 'drivers/igpio', 'drivers/lpspi')]
required = [includes[0] / 'MIMXRT1062.h', includes[2] / 'fsl_common.h',
            includes[3] / 'fsl_gpio.h', includes[4] / 'fsl_lpspi.h', args.cmsis / 'core_cm7.h']
for path in required:
    if not path.is_file():
        parser.error(f'Required official header missing: {path}')
if not (core / 'ads1299.c').is_file():
    parser.error(f'Core source missing: {core}')
sdk_includes = includes + [args.cmsis, args.sdk / 'boards/evkbmimxrt1060']
includes = [core, target / 'ads1299_port']
sources = sorted(core.glob('*.c')) + sorted((target / 'ads1299_port').glob('*.c'))
sources += sorted((target / 'mcuxpresso_adapter').glob('*.c'))
includes += [target / 'mcuxpresso_adapter']
sources += [target / 'examples/main_ads1299.c', target / 'board/evkb_reference.c']
version = subprocess.check_output([args.cc, '--version'], text=True).splitlines()[0]
image_evidence = None
with tempfile.TemporaryDirectory(prefix='.official-sdk-', dir=target / 'tests') as temporary:
    for index, source in enumerate(sources):
        subprocess.run([args.cc, '-std=c11', '-Wall', '-Wextra', '-Werror',
            '-mcpu=cortex-m7', '-mthumb', '-DCPU_MIMXRT1062DVL6B',
            *[part for path in sdk_includes for part in ('-isystem', str(path))],
            *[f'-I{p}' for p in includes], '-c', str(source),
            '-o', str(Path(temporary) / f'{index}.o')], check=True)
    if args.cmake:
        build = Path(temporary) / 'cmake'
        subprocess.run([args.cmake, '-S', str(target / 'tests/cmake'), '-B', str(build),
            '-G', 'Ninja', f'-DCMAKE_MAKE_PROGRAM={args.ninja}',
            '-DCMAKE_SYSTEM_NAME=Generic', f'-DCMAKE_C_COMPILER={args.cc}',
            f'-DCMAKE_ASM_COMPILER={args.cc}',
            f'-DADS1299_LINK_REFERENCE_RAM={"ON" if args.link_reference_ram else "OFF"}',
            f'-DMCUX_SDK_ROOT={args.sdk.resolve().as_posix()}',
            f'-DCMSIS_CORE_INCLUDE={args.cmsis.resolve().as_posix()}'], check=True)
        subprocess.run([args.cmake, '--build', str(build)], check=True)
        if args.link_reference_ram:
            elf = build / 'ads1299_evkb_reference.elf'
            suffix = '.exe' if Path(args.cc).suffix == '.exe' else ''
            nm = Path(args.cc).parent / ('arm-none-eabi-nm' + suffix)
            size = Path(args.cc).parent / ('arm-none-eabi-size' + suffix)
            undefined = subprocess.check_output([str(nm), '-u', str(elf)], text=True)
            if undefined.strip():
                raise RuntimeError(f'Unresolved ELF symbols: {undefined}')
            image_evidence = {'undefined_symbols': 0,
                'size': subprocess.check_output([str(size), str(elf)], text=True).splitlines()[1].split()[:4],
                'sha256': hashlib.sha256(elf.read_bytes()).hexdigest()}
print(json.dumps({'result': 'official-header object compilation passed',
    'cmake_library_build': 'passed' if args.cmake else 'not run',
    'evkb_reference_ram_link': 'passed' if args.link_reference_ram else 'not run',
    'image_evidence': image_evidence,
    'compiler': version, 'translation_units': len(sources),
    'scope': 'EVKB reference only; no hardware execution or acquisition validation',
    'headers_sha256': {str(p): hashlib.sha256(p.read_bytes()).hexdigest() for p in required}}, indent=2))
