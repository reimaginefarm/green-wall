// reimagine.farm
// MAIN MODULE
// Copyright (C) 2019 by reimagine.farm <contact at reimagine dot farm>

#include <Arduino.h>
#include "greenWall.hpp"

TSL2561 lightSensor;

unsigned long previousMillis = 0;
const long interval = 2000;

void setup() {
    Serial.begin(115200);
    otaSetup();
    lightSensor.setup();
}

void loop() {

    otaLoop();

    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        Serial.println("Light:");
        Serial.println(lightSensor.getMeasurement());
    }

}
