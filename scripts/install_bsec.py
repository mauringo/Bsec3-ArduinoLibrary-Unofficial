#!/usr/bin/env python3
"""Install BSEC3 IAQ ESP32-S3 binary and all eight BME690 profiles locally."""
import argparse
from pathlib import Path
import re
from zipfile import ZipFile

root = Path(__file__).resolve().parents[1]
p = argparse.ArgumentParser(description=__doc__)
p.add_argument('archive', type=Path)
a = p.parse_args()
with ZipFile(a.archive) as z:
    binary = z.read('release_bin/IAQ/bin/esp/esp32_s3/libalgobsec.a')
    if not binary.startswith(b'!<arch>\n'): raise SystemExit('Invalid static archive')
    profiles = {}
    for voltage in (18, 33):
        for period in (3, 300):
            for history in (4, 28):
                name = f'bme690_iaq_{voltage}v_{period}s_{history}d'
                base = f'release_bin/IAQ/config/bme690/{name}/bsec_iaq'
                source = z.read(base + '.c').decode()
                match = re.search(r'bsec_config_iaq\[(\d+)\]\s*=\s*\{([^}]+)\}', source)
                if not match: raise SystemExit(f'Invalid SDK profile: {name}')
                values = [int(v.strip()) for v in match[2].split(',') if v.strip()]
                csv = [int(v) for v in z.read(base + '.csv').decode().strip().split(',')]
                if len(values) != int(match[1]) or csv != [len(values), *values]:
                    raise SystemExit(f'Profile size/CSV mismatch: {name}')
                profiles[name] = '#pragma once\n// Bosch SDK data: installed locally, not for redistribution.\nstatic const uint8_t bsec3_profile_data[] = {' + ','.join(map(str, values)) + '};\n'
    # Refuse mismatched wrapper headers rather than silently mixing SDK releases.
    for name in ('bsec_interface.h', 'bsec_datatypes.h'):
        if (root / 'src/inc' / name).read_bytes() != z.read('release_bin/IAQ/inc/' + name):
            raise SystemExit(f'{name} does not match the supplied IAQ SDK; use BSEC 3.3.0.1')
    dest = root / 'src/esp32s3/libalgobsec.a'
    dest.parent.mkdir(parents=True, exist_ok=True)
    dest.write_bytes(binary)
    dest = root / 'src/config/bme690'
    dest.mkdir(parents=True, exist_ok=True)
    for name, source in profiles.items(): (dest / (name + '.h')).write_text(source)
print('Installed IAQ ESP32-S3 binary and 8 verified BME690 profiles; all ignored by Git.')
