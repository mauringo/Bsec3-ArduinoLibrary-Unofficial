#!/usr/bin/env python3
"""Install only the ESP32-S3 IAQ binary from a user-supplied Bosch SDK ZIP."""
import argparse
from pathlib import Path
from zipfile import ZipFile

root = Path(__file__).resolve().parents[1]
p = argparse.ArgumentParser(description=__doc__)
p.add_argument('archive', type=Path)
a = p.parse_args()
with ZipFile(a.archive) as z:
    member = 'release_bin/IAQ/bin/esp/esp32_s3/libalgobsec.a'
    data = z.read(member)
    if not data.startswith(b'!<arch>\n'):
        raise SystemExit('Not a static library')
    dest = root / 'src/esp32s3/libalgobsec.a'
    dest.parent.mkdir(parents=True, exist_ok=True)
    dest.write_bytes(data)
print(f'Installed {dest}; ignored by Git. SDK headers must match your wrapper version.')
