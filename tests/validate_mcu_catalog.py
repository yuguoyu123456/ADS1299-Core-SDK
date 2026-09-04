#!/usr/bin/env python3
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MCU = ROOT / 'firmware' / 'mcu'
CATALOG = json.loads((MCU / 'catalog.json').read_text(encoding='utf-8'))
projects = CATALOG.get('projects', [])
assert len(projects) >= 30, f'expected at least 30 MCU projects, got {len(projects)}'
seen = set()
allowed = {'compiles', 'reference-source-build-pending'}
for item in projects:
    key = (item['vendor'], item['project'])
    assert key not in seen, f'duplicate project {key}'
    seen.add(key)
    assert item['status'] in allowed, f"invalid status {item['status']} for {key}"
    p = MCU / item['vendor'] / item['project']
    for required in ('README.md', 'project.json'):
        assert (p / required).is_file(), f'missing {p / required}'
    manifest = json.loads((p / 'project.json').read_text(encoding='utf-8'))
    assert manifest['status'] == item['status'], f'status mismatch for {key}'
    for required_key in ('vendor', 'family', 'mcu', 'board', 'sdk', 'status'):
        assert manifest.get(required_key), f'missing manifest field {required_key!r} for {key}'
    style = manifest.get('style', item.get('style', 'shared-reference-app'))
    if style == 'shared-reference-app':
        assert (p / 'port' / 'board_sdk.h').is_file(), f'missing BSP contract for {key}'
    elif style == 'native':
        assert (p / 'CMakeLists.txt').is_file() or (p / 'platformio.ini').is_file(), f'missing native build file for {key}'
        entry_sources = list(p.rglob('main.c')) + list(p.rglob('app_main.c'))
        assert entry_sources, f'missing native entry source for {key}'
        port_sources = list(p.rglob('ads1299_port_*.c'))
        assert port_sources, f'missing native ADS1299 port source for {key}'
    else:
        raise AssertionError(f'unknown project style {style!r} for {key}')
print(f'MCU catalog OK: {len(projects)} projects, {sum(p["status"]=="compiles" for p in projects)} compiled, {sum(p["status"]!="compiles" for p in projects)} build-pending')
