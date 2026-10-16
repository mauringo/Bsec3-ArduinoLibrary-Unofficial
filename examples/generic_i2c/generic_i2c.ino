/**
 * Copyright (C) 2021 Bosch Sensortec GmbH
 * Modifications (C) 2026 Mauro Riboni (@mauringo)
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * My personal hobby project, made for fun and learning. Unofficial;
 * not affiliated with, endorsed by, or supported by Bosch Sensortec.
 * Original Bosch credits and licenses are retained. Provided as-is.
 * Full Polverine examples: https://github.com/mauringo/Polverine_Arduino_Examples
 */
// START HERE: install BME690_unofficial and the matching Bosch BSEC3 SDK first.
// The supplied installer supports ESP32 variants. Other MCU targets require a
// matching Bosch archive and sufficient RAM; portable Wire code is not proof
// of BSEC support. Do not assume a small AVR board can run this example.
#include <Arduino.h>
#include <Wire.h>
#include <bsec3.h>

// Choose the ACTUAL SENSOR supply voltage, not the USB input voltage.
// 1800 = 1.8 V; 3300 = 3.3 V. This setting does not change hardware voltage.
#define BSEC3_SUPPLY_MV 3300
#define BSEC3_SAMPLE_PERIOD_S 3  // 3 = LP; 300 = ULP, one sample every 5 minutes.
#define BSEC3_HISTORY_DAYS 4     // 4 or 28: IAQ background calibration history.
#include <config/bme690_profile.h>

// Choose the I2C ADDRESS separately from the GPIO pins.
// Most modules use 0x76; change to BME69X_I2C_ADDR_HIGH for a module at 0x77.
constexpr uint8_t SENSOR_ADDRESS = BME69X_I2C_ADDR_LOW;

#if defined(ARDUINO_ARCH_ESP32)
// SDA/SCL here are the defaults of the BOARD selected in Arduino IDE.
// To use custom ESP32 pins, replace SDA and SCL with your wired GPIO numbers.
// Example: Polverine -> 14, 21; generic classic ESP32 -> commonly 21, 22.
// ESP32-S3/C3 boards can use different pins. Check your board's pinout.
constexpr int I2C_SDA_PIN = SDA;
constexpr int I2C_SCL_PIN = SCL;
#endif

void startI2c() {
#if defined(ARDUINO_ARCH_ESP32)
    // Only the ESP32 branch uses this pin-selecting overload.
    if (!Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN)) {
        Serial.println("Could not start I2C. Check the ESP32 GPIO choices.");
        while (true) delay(1000);
    }
#else
    // Other Arduino cores: use the board's labeled SDA and SCL pins.
    // Do NOT pass ESP32 GPIO numbers here. Wire.begin(address) on some cores
    // configures an I2C SLAVE address, not the SDA pin!
    // For an alternate bus/pin mapping, follow that board core's Wire API.
    Wire.begin();
#endif
    Wire.setClock(100000); // Conservative 100 kHz bus speed.
    // Start Wire once, then pass that same object to the sensor library.
    // A board with Wire1 can use that object instead, if its core provides it.
}

Bsec3 sensor; // Keep one long-lived object. Do not free memory from callback copies.

void showStatus() {
    Serial.print("BSEC status: "); Serial.println(sensor.status);
    Serial.print("BME690 status: "); Serial.println(sensor.sensor.status);
    // Negative = error, positive = warning, zero = OK.
}

void onData(const bme69xData, const bsecOutputs outputs, const Bsec3) {
    // BSEC calls this when processed outputs arrive. No measurements here.
    for (uint8_t i = 0; i < outputs.nOutputs; ++i) {
        const auto &value = outputs.output[i];
        switch (value.sensor_id) {
            case BSEC_OUTPUT_IAQ:
                Serial.print("IAQ: "); Serial.print(value.signal);
                Serial.print("; accuracy (0..3): "); Serial.println(value.accuracy);
                break;
            case BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE:
                Serial.print("Temperature (C): "); Serial.println(value.signal); break;
            case BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY:
                Serial.print("Humidity (%): "); Serial.println(value.signal); break;
            case BSEC_OUTPUT_RAW_PRESSURE:
                Serial.print("Pressure (Pa): "); Serial.println(value.signal); break;
            default: break;
        }
    }
}

void setup() {
    Serial.begin(115200);
    const uint32_t started = millis();
    while (!Serial && millis() - started < 3000) delay(10);
    startI2c();

    if (!sensor.begin(SENSOR_ADDRESS, Wire) ||
        !sensor.setConfig(bsec3_profile_data, sizeof(bsec3_profile_data))) {
        showStatus();
        while (true) delay(1000); // Check wiring, address, SDK and profile, then reset.
    }
    // No guessed board temperature offset: BSEC compensates the sensor heater.
    // Extra board/enclosure heat needs a measured offset or board-specific table.
    // For Polverine's table use the linked full project, not an arbitrary constant.
    bsecSensor outputs[] = {BSEC_OUTPUT_IAQ,
        BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
        BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY, BSEC_OUTPUT_RAW_PRESSURE};
    if (!sensor.updateSubscription(outputs, ARRAY_LEN(outputs), BSEC3_PROFILE_SAMPLE_RATE)) {
        showStatus();
        while (true) delay(1000);
    }
    sensor.attachCallback(onData);
    Serial.println("BSEC ready. IAQ accuracy needs time and environmental variation.");
}

void loop() {
    // Call often: BSEC schedules the heater, reads data and calls onData().
    // A false return is not always fatal; inspect BOTH status codes.
    if (!sensor.run()) {
        showStatus();
        if (sensor.status < BSEC_OK || sensor.sensor.status < BME69X_OK)
            while (true) delay(1000);
    }
    delay(10); // Do not delay for the sample period here.
}
