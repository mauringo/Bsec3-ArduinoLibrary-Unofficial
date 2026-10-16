/**
 * Copyright (C) 2021 Bosch Sensortec GmbH
 * Modifications (C) 2026 Mauro Riboni
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Mauro Riboni (@mauringo): personal hobby work, made for fun and learning.
// Unofficial; not affiliated with, endorsed by, or supported by Bosch.


// Complete Polverine Arduino + PlatformIO examples, profiles and compensation:
// https://github.com/mauringo/Polverine_Arduino_Examples

#include <Wire.h>
#include <bsec3.h>
#include <config/bme690_profile.h>

Bsec3 sensor;
bool ready = false;

void onData(const bme69xData, const bsecOutputs outputs, const Bsec3) {
    for (uint8_t i = 0; i < outputs.nOutputs; ++i) {
        const auto &o = outputs.output[i];
        if (o.sensor_id == BSEC_OUTPUT_IAQ)
            Serial.printf("IAQ: %.2f, accuracy: %u\n", o.signal, o.accuracy);
    }
}

void setup() {
    Serial.begin(115200);
    const uint32_t start = millis();
    while (!Serial && millis() - start < 3000) delay(10);
    Wire.begin(14, 21, 100000); // Polverine; change for your board.
    ready = sensor.begin(BME69X_I2C_ADDR_LOW, Wire);
    if (ready) ready = sensor.setConfig(bsec3_profile_data, sizeof(bsec3_profile_data));
    bsecSensor outputs[] = {BSEC_OUTPUT_IAQ};
    if (ready) ready = sensor.updateSubscription(outputs, ARRAY_LEN(outputs), BSEC3_PROFILE_SAMPLE_RATE);
    sensor.attachCallback(onData);
    if (!ready) Serial.printf("BSEC: %d, BME690: %d\n", sensor.status, sensor.sensor.status);
}

void loop() {
    if (ready && !sensor.run()) {
        Serial.printf("BSEC: %d, BME690: %d\n", sensor.status, sensor.sensor.status);
        if (sensor.status < BSEC_OK || sensor.sensor.status < BME69X_OK) ready = false;
    }
    delay(10);
}
