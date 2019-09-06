// reimagine.farm
// MAIN MODULE
// Copyright (C) 2019 by reimagine.farm <contact at reimagine dot farm>

#include <Arduino.h>
#include "greenWall.hpp"

using namespace std;

int main() {

    TSL2561 lightSensor;
    lightSensor.sensorSetup();
    lightSensor.getMeasurement();

    unsigned long previousMillis = 0;
    const long interval = 5000;

    while (true) {

        otaSetup();
        otaLoop();

        unsigned long currentMillis = millis();

        if (currentMillis - previousMillis >= interval) {
            previousMillis = currentMillis;
            Serial.println("Light:");
            Serial.println(lightSensor.getMeasurement());
        }

    }

    return 0;
}

void setup () {
}
void loop () {
}
