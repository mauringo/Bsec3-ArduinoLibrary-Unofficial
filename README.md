# BSEC3_unofficial

Unofficial Arduino framework wrapper maintained by **Mauro Riboni** ([@mauringo](https://github.com/mauringo), 58341265+mauringo@users.noreply.github.com).

## Origin and personal-project disclaimer

This library started from the official [Bosch Sensortec BSEC2 Arduino library](https://github.com/boschsensortec/Bosch-BSEC2-Library) and was adapted by **Mauro Riboni (@mauringo)** for BSEC3 and BME690.

**This is my personal hobby project, made privately for fun and learning.** It is shared as unofficial experimental work, not as an official Bosch product. I am not affiliated with Bosch Sensortec, and this project is not endorsed, certified, or supported by Bosch. No promise of reliability, fitness for a particular purpose, continued maintenance, or support is made; use it at your own risk under the applicable licenses.

Bosch Sensortec remains the author of the original libraries. Original copyright notices and licenses are retained; my changes do not imply ownership of Bosch's work. The proprietary BSEC3 algorithm is **not included** and must be obtained separately from Bosch under its SDK license.

## Start with the generic I2C example

After installing the SDK as described below, choose **File → Examples → BSEC3_unofficial → generic_i2c** in Arduino IDE. It explains GPIO choice, sensor address, profile selection, subscriptions, IAQ accuracy, and why `run()` must be called often. It uses ordinary `Serial.print()` rather than an ESP32-specific printing API.

## Install

1. Install [BME690_unofficial](https://github.com/mauringo/BME690-Arduino-lib-unofficial) as `<your Arduino sketchbook>/libraries/BME690_unofficial`.
2. Install this repository alongside it as `libraries/BSEC3_unofficial`. Restart Arduino IDE.
3. Obtain **BSEC 3.3.0.1** directly from Bosch and accept the SDK terms. The algorithm archive and profile data are not included here.
4. From this library directory run the installer, choosing the MCU selected in Arduino IDE:

```sh
# Polverine or another ESP32-S3 board:
python3 scripts/install_bsec.py /path/to/bsec_v3-3-0-1.zip --mcu esp32s3

# Other installer choices (run the ONE matching your MCU):
# --mcu esp32     Classic ESP32
# --mcu esp32s2   ESP32-S2
# --mcu esp32c3   ESP32-C3
```

The default is `esp32s3` when `--mcu` is omitted. The script installs the **IAQ** archive under `src/<mcu>/` and all eight BME690 IAQ profiles under `src/config/bme690/`. It verifies the profiles against the SDK's byte counts/CSV and refuses mismatched SDK headers. Installed Bosch archives/profile data are ignored by Git.

For PlatformIO use `framework = arduino` and this repository in `lib_deps`, or use local source clones under the project's `lib/` directory. Install the SDK archive in the actual installed BSEC3 library. Its extra script checks the exact MCU archive and prints an installation hint when it is missing.

## Choose I2C pins and bus

**The sketch starts the bus; the library uses the `TwoWire` object supplied to `begin()`.**

- **ESP32 family:** edit `I2C_SDA_PIN` and `I2C_SCL_PIN` in `generic_i2c.ino`. Their initial `SDA`/`SCL` values come from the selected Arduino board. Replace them with your wired GPIO numbers. Polverine uses **14/21**; common classic ESP32 defaults are **21/22**, but ESP32-S3/C3 board layouts vary.
- **Other Arduino cores:** the example's `#else` branch calls `Wire.begin()` and uses the board's labeled SDA/SCL pins. Custom mapping or a second bus requires that core's API. Never interpret a core's `Wire.begin(address)` overload as a GPIO selection.
- **Address:** choose `BME69X_I2C_ADDR_LOW` (`0x76`) or `BME69X_I2C_ADDR_HIGH` (`0x77`) independently of pin selection.

The generic Wire branch is portable; **BSEC3 support is a separate constraint**. This release's installer and declared Arduino architecture support the ESP32 family targets listed above. Other MCUs need a suitable Bosch binary, matching ABI/linker setup and enough RAM; they are not claimed as supported by this release. In particular, a small AVR board is not a BSEC3 target just because the raw BME690 example compiles on it.

Check the sensor module's supply/logic voltage and pull-ups. Selecting a BSEC voltage profile does not provide voltage regulation or level shifting.

## Choose a BME690 IAQ profile

Edit the three settings above `#include <config/bme690_profile.h>` in the generic sketch:

| Setting | Allowed values | Default |
| --- | --- | --- |
| `BSEC3_SUPPLY_MV` | `1800` or `3300` | `3300` |
| `BSEC3_SAMPLE_PERIOD_S` | `3` or `300` | `3` |
| `BSEC3_HISTORY_DAYS` | `4` or `28` | `4` |

Use the **sensor's actual supply**, not the USB voltage. Bosch's `18v`/`33v` filenames mean **1.8 V/3.3 V**. The period chooses LP/3 seconds or ULP/300 seconds. The day value selects the background IAQ calibration history. Unsupported combinations fail at compile time. Only BME690 IAQ data is used; legacy BME680/BME688/selectivity blobs were removed.

After `begin()` and before loading state/subscribing:

```cpp
sensor.setConfig(bsec3_profile_data, sizeof(bsec3_profile_data));
```

Use `BSEC3_PROFILE_SAMPLE_RATE` for subscriptions. The installer/header use BSEC 3.3.0.1 IAQ's **550-byte** configuration and **197-byte** state sizes. Do not mix headers, state, profiles or algorithm releases.

## How it runs

1. Start Wire and initialize the sensor.
2. Apply the chosen configuration; check the return value.
3. Subscribe to outputs and attach a data callback.
4. Call `run()` frequently in `loop()`. BSEC schedules sampling and heater operation itself. A `delay(10)` is sufficient; do not delay for 3 or 300 seconds.
5. Read results in the callback. IAQ accuracy ranges from 0 to 3 and needs conditioning/environmental variation. Positive status codes are warnings; negative codes are errors. Inspect **both** `sensor.status` and `sensor.sensor.status`.

BSEC's heat-compensated temperature/humidity correct the sensor heater. Extra board/enclosure heat needs measured compensation, not an arbitrary copied offset. `setTemperatureOffset(celsius)` selects a fixed offset; `setTemperatureOffsetCallback(function)` computes an offset from the current raw temperature before processing that sample. Passing `nullptr` disables the callback. The full Polverine project contains BlackIoT's board-specific tables.

Keep one long-lived `Bsec3` per sensor. Existing callback signatures pass a copy of the wrapper; do not call `clearMemory()` or transfer memory ownership from that copy. `allocateMemory()` uses a long-lived caller buffer; `clearMemory()` frees only internally owned storage. No automatic persistence is performed.

## Examples and full project

- `generic_i2c`: recommended first sketch; ESP32 pins and other-core Wire behavior are explained in place.
- `basic`: short Polverine/ESP32-S3 IAQ example.
- `basic_config_state`: ESP32 Preferences state persistence, separated by voltage/period/history profile.

[Polverine Arduino Examples](https://github.com/mauringo/Polverine_Arduino_Examples) combines both libraries with the 7Semi BMV080 driver, matching profiles, board compensation and Serial JSON. Its [Arduino IDE sketch](https://github.com/mauringo/Polverine_Arduino_Examples/tree/main/Arduino_project) is independently documented.

## Version 1.13.0

Adds a commented generic I2C example, clearer installation/troubleshooting, explicit installer selection for ESP32/S2/S3/C3, and project links. Original Bosch notices are preserved. Compilation verifies integration, not calibration quality or physical performance. Hardware validation remains necessary.

References: [Espressif I2C API](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/i2c.html), [Bosch's profile-label explanation](https://community.bosch-sensortec.com/mems-sensors-forum-jrmujtaw/post/bsec-configurations---operating-age-t8kSaFHerGrTclU).

Verified generic-example compilation: **ESP32, ESP32-S2, ESP32-S3 and ESP32-C3 with ESP32 core 3.3.11 and matching BSEC 3.3.0.1 IAQ archives**. These checks did not upload firmware or validate physical measurements.

Version 1.13.1 fixes forced-mode timing: `run()` starts the measurement, returns to the application while T/P/H and the heater finish, then reads/processes the sample without waiting for the next BSEC control cycle. Late samples get a bounded retry window. Pressure remains in Pa throughout, matching the SDK input/output units. Scheduling regression checks cover delayed reads, no premature retrigger, late/error paths and Pa preservation.

Polverine hardware check: profile loading succeeded, but subscribing to `BSEC_OUTPUT_BREATH_VOC_EQUIVALENT` with the BME690 3.3 V / 3-second / 4-day IAQ profile returned `BSEC_E_CONFIG_FEATUREMISMATCH` (-35). The full examples therefore use the verified IAQ, static IAQ, CO2 equivalent, raw and compensated environmental values, and status outputs. Eight live BME callbacks were observed in 22 seconds after correcting the subscription and forced-mode timing. This is a functional output check, not a long-term calibration validation.
