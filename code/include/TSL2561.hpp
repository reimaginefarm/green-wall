// reimagine.farm
// Lights (TSL2561) Class Header
// Copyright (C) 2019 by reimagine.farm <contact at reimagine dot farm>

// Note: This sensor uses I2C


// This driver uses the Adafruit unified sensor library (Adafruit_Sensor),
// which provides a common 'type' for sensor data and some helper functions.
//
// To use this driver you will also need to download the Adafruit_Sensor
// library and include it in your libraries folder.
//
// You should also assign a unique ID to this sensor for use with
// the Adafruit Sensor API so that you can identify this particular
// sensor in any data logs, etc.  To assign a unique ID, simply
// provide an appropriate value in the constructor below (12345
// is used by default in this example).
//
// Connections
// ===========
// Connect SCL to I2C SCL Clock
// Connect SDA to I2C SDA Data
// Connect VCC/VDD to 3.3V or 5V (depends on sensor's logic level, check the datasheet)
// Connect GROUND to common ground
//
// I2C Address
// ===========
// The address will be different depending on whether you leave
// the ADDR pin floating (addr 0x39), or tie it to ground or vcc.
// The default addess is 0x39, which assumes the ADDR pin is floating
// (not connected to anything).  If you set the ADDR pin high
// or low, use TSL2561_ADDR_HIGH (0x49) or TSL2561_ADDR_LOW
// (0x29) respectively.
//
// History
// =======
// 2013/JAN/31  - First version (KTOWN)

#include "baseSensor.hpp"

class TSL2561 : virtual Sensor{
  public:
    Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
    sensor_t sensor;

    TSL2561();

    void sensorSetup();
    float getMeasurement();

    //     /* If event.light = 0 lux the sensor is probably saturated
    //        and no reliable data could be generated! */
};
