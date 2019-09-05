// reimagine.farm
// Lights (TSL2561) Class Implementation
// Copyright (C) 2019 by reimagine.farm <contact at reimagine dot farm>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>

#include "../include/baseSensor.hpp"
#include "../include/sensors/TSL2561.hpp"

TSL2561 :: TSL2561() : Sensor("light_sensor", sensor.version, 1, sensor.max_value, sensor.min_value, sensor.resolution, sensor.min_delay, "lux"){

};

void TSL2561 :: sensorSetup() {

    /* You can also manually set the gain or enable auto-gain support */
    // tsl.setGain(TSL2561_GAIN_1X);      /* No gain ... use in bright light to avoid sensor saturation */
    // tsl.setGain(TSL2561_GAIN_16X);     /* 16x gain ... use in low light to boost sensitivity */
    tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */

    /* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
    tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
    // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
    // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */

    tsl.begin();
};

float TSL2561 :: getMeasurement() {
    sensors_event_t event;
    tsl.getEvent(&event);
    return event.light;
};
