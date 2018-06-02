// Team NYUAD, Green / Vaponic Wall Code 2018
// runSwitches.cpp v0.1
//
// use the following command to COMPILE:
// g++ -Wall -o runSwitches runSwitches.cpp -lwiringPi
//
// use the following command to RUN:
// sudo ./runSwitches <pin number> <pwm value> <blank state, true or false>
// for example: sudo ./runSwitches 9 4000 false

// Inspired by https://github.com/adafruit/Adafruit_TLC5947/blob/master/Adafruit_TLC5947.cpp

// wiringPi is used to use the GPIO pins of raspi
#include <wiringPi.h>
#include <stdint.h>
#include <cstdlib>
#include <iostream>
#include <string.h>
#include <limits>
using std::cout;
using std::string;


// Variable declarations
uint16_t chan;
uint16_t pwm;
uint16_t pwmbuffer[48];
bool b;

// GPIO pins used to connect the TLC5974 board
// Check https://pinout.xyz/pinout/wiringpi for correct pin numbers
uint8_t clk = 27; // clock / CLK
uint8_t dat = 28; // data / DIN
uint8_t lat = 29; // latch / LAT
uint8_t blank = 37; // blank / /OE

// chan: Channel of the TLC5974 board (from 0 to 23)
// pwm: PWM value for the specified channel (from 0 to 4095)
bool setPWM(uint16_t chan, uint16_t pwm);

// blank: Used to temporaryly turn off all ports while data is sent (true or false)
bool write(bool blank);


int main(int argc, char* argv[]) {

    wiringPiSetup();

    b = setPWM(atoi(argv[1]), atoi(argv[2]));
    cout << "Pins are set: " << std::boolalpha << b << "\n";

    b = write(argv[3]);
    cout << "Data is transferred: " << std::boolalpha << b << "\n";

    return 0;
}


// store pins and corresponding pwm values
bool setPWM(uint16_t chan, uint16_t pwm) {
    try {
      // Used to prevent accidents
      // 4095 is the max value
      if (pwm > 4095) pwm = 4095;
      pwmbuffer[chan] = pwm;

        // return true if everything is OK
        return true;

    } catch (const std::exception& e) {

        // return false if there is an error
        return false;
    }

}


// send data to TLC5974 IC after setting up pins and pwm values
bool write(bool blank) {
    try {
      if(blank == false){
        digitalWrite(blank, LOW);
      } else {
        digitalWrite(blank, HIGH);
      }

        digitalWrite(lat, LOW);
        // 24 channels per TLC5974
        for (int16_t c = 24 - 1; c >= 0; c--) {
            // 12 bits per channel, send MSB first
            for (int8_t b = 11; b >= 0; b--) {
                digitalWrite(clk, LOW);

                if (pwmbuffer[c] & (1 << b))
                    digitalWrite(dat, HIGH);
                else
                    digitalWrite(dat, LOW);

                digitalWrite(clk, HIGH);
            }
        }
        digitalWrite(clk, LOW);

        digitalWrite(lat, HIGH);
        digitalWrite(lat, LOW);

        if(blank == false){
          digitalWrite(blank, HIGH);
        } else {
          digitalWrite(blank, LOW);
        }

        // return true if everything is OK
        return true;

    } catch (const std::exception& e) {

        // return false if there is an error
        return false;
    }

}
