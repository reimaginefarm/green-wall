// reimagine.farm
// MAIN MODULE
// Copyright (C) 2019 by reimagine.farm <contact at reimagine dot farm>

#include <Arduino.h>
#include "greenWall.hpp"

TSL2561 lightSensor;

void setup() {
  otaSetup();
}

void loop() {
  otaLoop();
  Serial.println("Light:");
  Serial.println(lightSensor.getMeasurement());
}
