// reimagine.farm
// BaseSensor Class Header
// Copyright (C) 2019 by reimagine.farm <contact at reimagine dot farm>

#include <WString.h>

#ifndef BASE_SENSOR_HPP
#define BASE_SENSOR_HPP

class Sensor{
  protected:
    unsigned int address;
    int gain; //set to 1 in the constructor
    float measurement;

  public:
    String name;
    int id;
    int version;
    float maximumValue;
    float minimumValue;
    float resolution;
    int delay;
    String unit;

    Sensor(String initName, int initId, int initVersion, float initMaximumValue, float initMinimumValue, float initResolution, int initDelay, String initUnit) :
            name(initName),
            version(initId),
            id(initVersion),
            maximumValue(initMaximumValue),
            minimumValue(initMinimumValue),
            resolution(initResolution),
            delay(initDelay),
            unit(initUnit){};
    ~Sensor() {};

    //virtual float getMeasurement(int option=0) const = 0;
    //virtual void getEvent(int value) const = 0;
    //virtual void setGain(int gain) = 0;

};

#endif
