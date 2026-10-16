# BSEC3_unofficial

Unofficial Arduino framework wrapper maintained by **Mauro Riboni** ([@mauringo](https://github.com/mauringo), 58341265+mauringo@users.noreply.github.com).

## Origin and personal-project disclaimer

This library started from the official [Bosch Sensortec BSEC2 Arduino library](https://github.com/boschsensortec/Bosch-BSEC2-Library) and was adapted by **Mauro Riboni (@mauringo)** for BSEC3 and BME690.

**This is my personal hobby project, made privately for fun and learning.** It is shared as unofficial experimental work, not as an official Bosch product. I am not affiliated with Bosch Sensortec, and this project is not endorsed, certified, or supported by Bosch. No promise of reliability, fitness for a particular purpose, continued maintenance, or support is made; use it at your own risk under the applicable licenses.

Bosch Sensortec remains the author of the original libraries. Original copyright notices and licenses are retained; my changes do not imply ownership of Bosch's work. The proprietary BSEC3 algorithm is **not included** and must be obtained separately from Bosch under its SDK license.

## Install for Arduino ESP32-S3

1. Clone this repository as `Arduino/libraries/BSEC3_unofficial`.
2. Install [BME690_unofficial](https://github.com/mauringo/BME690-Arduino-lib-unofficial) alongside it as `Arduino/libraries/BME690_unofficial`.
3. Download BSEC 3.3.0.1 from Bosch and accept the SDK license.
4. Run `python3 scripts/install_bsec.py /path/to/bsec_v3-3-0-1.zip` from this library directory. It installs the **IAQ ESP32-S3** archive and all eight matching BME690 IAQ profiles locally, ignored by Git.
5. Select an ESP32-S3 Arduino board and open `examples/basic/basic.ino`. Polverine uses SDA 14, SCL 21 and address `0x76`.

For PlatformIO, set `framework = arduino` and add this repository to `lib_deps`. Its BME690 dependency is declared in `library.json`. Install the Bosch archive into the installed library before building. The extra script finds the archive relative to this library, independently of the application's working directory. For a local development workspace, put both libraries directly under `lib/`.

Arduino uses mixed-source/precompiled metadata (`precompiled=true`) so the wrapper source is compiled and the supplied algorithm archive is linked. ESP32-S3 is the validated target; other ESP32 variants require their own matching SDK archive. Other MCU architectures are not claimed by this release.

## API and examples

Include `<bsec3.h>` and create a `Bsec3` instance. Call `begin`, `updateSubscription`, `attachCallback`, then `run` frequently. `examples/basic_config_state` shows ESP32 Preferences state persistence using the default IAQ configuration. All legacy BME680/BME688 and FieldAir selectivity configurations have been removed. Only BME690 IAQ profiles installed from the matching Bosch SDK are supported by the profile selector.

`allocateMemory()` accepts a long-lived caller-owned buffer; `clearMemory()` releases only internally owned memory and resets the pointer. Callback signatures remain compatible with the existing API. Avoid copying instances for ownership or manually freeing memory from callback copies. Keep one long-lived instance per sensor.

Version 1.11.0 fixes Arduino/PlatformIO packaging, binary path resolution, instance memory cleanup, allocation failure handling, and millisecond rollover. ESP32-S3 builds use Arduino 2.0.17 and BSEC 3.3.0.1 IAQ. Physical readings and long-term calibration still require hardware validation. SDK ZIPs and proprietary archives must never be committed.

## BME690 IAQ voltage and timing profiles

Include `<config/bme690_profile.h>` after `<bsec3.h>`. After `begin()` and before loading state or subscribing, call:

```cpp
sensor.setConfig(bsec3_profile_data, sizeof(bsec3_profile_data));
```

Use `BSEC3_PROFILE_SAMPLE_RATE` when subscribing. It selects LP for 3 seconds or ULP for 300 seconds. Define these options before including the profile header, or via compiler build flags:

| Option | Supported values | Default |
| --- | --- | --- |
| `BSEC3_SUPPLY_MV` | `1800`, `3300` | `3300` |
| `BSEC3_SAMPLE_PERIOD_S` | `3`, `300` | `3` |
| `BSEC3_HISTORY_DAYS` | `4`, `28` | `4` |

Bosch filenames `18v`/`33v` represent **1.8 V/3.3 V**, not 18 V/33 V. Choose the sensor's actual supply voltage; selecting a profile does not change hardware voltage. The day value selects the SDK's background calibration history variant. Unsupported combinations produce a compiler error. Changing profile uses a separate Preferences namespace in the state example, preventing state from another voltage/timing/history profile being loaded.

The installer checks all eight arrays against their SDK byte count and CSV representation, and requires headers matching BSEC 3.3.0.1 IAQ. The header now declares the matching 550-byte property blob and 197-byte state blob; `setConfig` also accepts the actual blob length explicitly. Binary/configuration data are installed locally and excluded from commits.

All eight profile combinations compile on ESP32-S3. This verifies selection and build integration, not algorithm acceptance, calibration quality, or physical performance at both voltages. Check `setConfig()` and runtime statuses on the board; those hardware checks remain pending.

Version 1.11.1 removes legacy configuration data, synchronizes IAQ headers with SDK 3.3.0.1, and adds explicit BME690 profile selection and configuration lengths.

Profile label reference: [Bosch Sensortec explanation of voltage, timing, and calibration history](https://community.bosch-sensortec.com/mems-sensors-forum-jrmujtaw/post/bsec-configurations---operating-age-t8kSaFHerGrTclU).

Version 1.12.0 adds `setTemperatureOffsetCallback(float (*)(float))` for board compensation tables. The callback receives the current raw temperature in Celsius before BSEC receives its heat-source input, so temperature and humidity compensation use the current sample. Passing `nullptr` disables the callback; `setTemperatureOffset()` switches back to a constant offset. Board tables remain in the application, outside this sensor-independent library.
