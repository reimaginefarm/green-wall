// reimagine.farm
// MAIN
// Copyright (C) 2019 by reimagine.farm <contact at reimagine dot farm>

#include <Arduino.h>
#include "green-wall.hpp"

using namespace std;

const byte led = 2;

TSL2561 temperatureSensor;

void setup() {
    Serial.begin(115200);
    
    otaSetup();

    pinMode(led, OUTPUT);
    digitalWrite(led, 1);
}

unsigned long previousTime = millis();

const unsigned long interval = 1000;

void loop() {
    otaLoop();
    // unsigned long diff = millis() - previousTime;
    // if(diff > interval) {
    //     digitalWrite(led, !digitalRead(led));  // Change the state of the LED
    //     previousTime += diff;
    // }

    Serial.println(temperatureSensor.getMeasurement());
    Serial.println("test");
}
