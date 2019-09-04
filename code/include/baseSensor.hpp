// reimagine.farm
// BaseSensor Class Header
// Copyright (C) 2019 by reimagine.farm <contact at reimagine dot farm>

#ifndef BASE_SENSOR_HPP
#define BASE_SENSOR_HPP

class BaseSensor{
  protected:
    uint address;
    int gain; //set to 1 in the constructor

  public:
    string name;
    int id;
    int version;
    float maximumValue;
    float minimumValue;
    float resolution;
    int delay;
    string unit;

    BaseSensor() {};
    ~BaseSensor() {};

    virtual float getMeasurement(int option=0) const = 0;
    virtual void getEvent(int value) const = 0;
    virtual void setGain(int gain) = 0;
}

#endif
