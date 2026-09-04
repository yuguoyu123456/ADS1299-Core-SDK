#!/usr/bin/env python3
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MCU = ROOT / 'firmware' / 'mcu'
CATALOG = json.loads((MCU / 'catalog.json').read_text(encoding='utf-8'))
projects = CATALOG.get('projects', [])
assert len(projects) >= 23, f'expected at least 23 MCU projects, got {len(projects)}'
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
    if item['status'] == 'reference-source-build-pending':
        assert (p / 'port' / 'board_sdk.h').is_file(), f'missing BSP contract for {key}'
print(f'MCU catalog OK: {len(projects)} projects, {sum(p["status"]=="compiles" for p in projects)} compiled, {sum(p["status"]!="compiles" for p in projects)} build-pending')
