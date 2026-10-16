# BSEC3_unofficial

Unofficial Arduino framework wrapper maintained by **Mauro Riboni** ([@mauringo](https://github.com/mauringo), 58341265+mauringo@users.noreply.github.com).

Derived from the Bosch Sensortec BSEC Arduino wrapper. Original copyright and license notices are retained. This project is independent of Bosch Sensortec. The proprietary Bosch algorithm is **not included**.

## Install for Arduino ESP32-S3

1. Clone this repository as `Arduino/libraries/BSEC3_unofficial`.
2. Install [BME690_unofficial](https://github.com/mauringo/BME690-Arduino-lib-unofficial) alongside it as `Arduino/libraries/BME690_unofficial`.
3. Download BSEC 3.3.0.1 from Bosch and accept the SDK license.
4. Run `python3 scripts/install_bsec.py /path/to/bsec_v3-3-0-1.zip` from this library directory. It installs the **IAQ ESP32-S3** archive to `src/esp32s3/libalgobsec.a`, ignored by Git.
5. Select an ESP32-S3 Arduino board and open `examples/basic/basic.ino`. Polverine uses SDA 14, SCL 21 and address `0x76`.

For PlatformIO, set `framework = arduino` and add this repository to `lib_deps`. Its BME690 dependency is declared in `library.json`. Install the Bosch archive into the installed library before building. The extra script finds the archive relative to this library, independently of the application's working directory. For a local development workspace, put both libraries directly under `lib/`.

Arduino uses mixed-source/precompiled metadata (`precompiled=true`) so the wrapper source is compiled and the supplied algorithm archive is linked. ESP32-S3 is the validated target; other ESP32 variants require their own matching SDK archive. Other MCU architectures are not claimed by this release.

## API and examples

Include `<bsec3.h>` and create a `Bsec3` instance. Call `begin`, `updateSubscription`, `attachCallback`, then `run` frequently. `examples/basic_config_state` shows ESP32 Preferences state persistence using the default IAQ configuration. Older bundled selectivity configuration files are legacy data, not validated BME690 configurations; do not apply them to the IAQ binary.

`allocateMemory()` accepts a long-lived caller-owned buffer; `clearMemory()` releases only internally owned memory and resets the pointer. Callback signatures remain compatible with the existing API. Avoid copying instances for ownership or manually freeing memory from callback copies. Keep one long-lived instance per sensor.

Version 1.11.0 fixes Arduino/PlatformIO packaging, binary path resolution, instance memory cleanup, allocation failure handling, and millisecond rollover. ESP32-S3 builds use Arduino 2.0.17 and BSEC 3.3.0.1 IAQ. Physical readings and long-term calibration still require hardware validation. SDK ZIPs and proprietary archives must never be committed.
