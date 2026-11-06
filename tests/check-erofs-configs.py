#!/usr/bin/env python3
"""Check EROFS defaults without downloading or compiling kernel sources."""
from pathlib import Path
import re

configs = Path(__file__).resolve().parents[1] / 'external/config/kernel'
required = ('EROFS_FS', 'EROFS_FS_ZIP', 'EROFS_FS_XATTR',
            'EROFS_FS_POSIX_ACL', 'EROFS_FS_SECURITY', 'LZ4_DECOMPRESS')
checked = 0
skipped = []
for path in sorted(configs.glob('*.config')):
    text = path.read_text()
    if not re.search(r'^(?:CONFIG_EROFS_FS=|# CONFIG_EROFS_FS is not set)', text, re.M):
        # Old vendor kernels without EROFS are not retrofitted with a driver.
        skipped.append(path.name)
        continue
    for option in required:
        entries = re.findall(rf'^(?:CONFIG_{option}=.*|# CONFIG_{option} is not set)$', text, re.M)
        assert entries == [f'CONFIG_{option}=y'], (path.name, option, entries)
    checked += 1
assert checked > 0, 'No EROFS kernel configurations found'
print(f'PASS: {checked} EROFS-capable kernel configurations; {len(skipped)} older configs skipped')
for name in skipped:
    print(f'  no EROFS symbol: {name}')
