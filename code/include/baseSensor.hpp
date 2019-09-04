// reimagine.farm
// BaseSensor Class Header
// Copyright (C) 2019 by reimagine.farm <contact at reimagine dot farm>

#ifndef BASE_SENSOR_HPP
#define BASE_SENSOR_HPP

class BaseSensor{
  private:
    uint address;
    int gain; //set to 1 in the constructor

  public:
    string name;
    string id;
    string version;
    float maximumValue;
    float minimumValue;
    uint resolution;

    BaseSensor() {};
    ~BaseSensor() {};

    float getMeasurement(int option=0) const {};
    void setGain(int gain) {};
}

#endif
