//****************************************************************************//
// Team NYUAD, Green / Vaponic Wall Code 2018
// runSwitches.cpp v0.1
//
// wiringPi library is required to run this code
// visit http://wiringpi.com/download-and-install/ for installation instructions
//
// Inspired by https://github.com/adafruit/Adafruit_TLC5947/blob/master/Adafruit_TLC5947.cpp
//
// Use the following command to COMPILE:
// g++ -Wall -o runSwitches runSwitches.cpp -lwiringPi
//
// Use the following command to RUN:
// sudo ./runSwitches <pin number> <pwm value> <blank state, true or false>
//
// For example: sudo ./runSwitches 9 4000 false
//****************************************************************************//

// wiringPi is used to use the GPIO pins of raspi
#include <wiringPi.h>
#include <stdint.h>
#include <cstdlib>
#include <iostream>
#include <string.h>
using std::cout;
using std::string;



// Variable declarations
uint16_t chan;
uint16_t pwm;
uint16_t pwmbuffer[48]; // used to store PWM values
bool b; // used to store the results of the functions

// GPIO pins used to connect the TLC5974 board
// Check https://pinout.xyz/pinout/wiringpi for correct pin numbers
uint8_t lat = 15; // latch / LAT
uint8_t blank = 16; // blank / /OE
uint8_t clk = 5; // clock / CLK
uint8_t dat = 6; // data / DIN


// chan: Channel of the TLC5974 board (from 0 to 23)
// pwm: PWM value for the specified channel (from 0 to 4095)
bool setPWM(uint16_t chan, uint16_t pwm);

// blank: Used to temporaryly turn off all ports while data is sent (true or false)
bool write(bool blank);

void setup();



// main gets the arguments from command line when executed
int main(int argc, char* argv[]) {

    // getting GPIO pins ready to use
    setup();

    // using arguments retrieved from command line to assign pins to their pwm values

    for (int i = 0; i < 24; i++) {
      setPWM(i, atoi(argv[i+1]));
    }


    //setPWM(atoi(argv[1]), atoi(argv[2]));

    // get the true or false argument and write data to TLC5974
    //write(argv[sizeof(argv[])]);

    write(false);

    return 0;
}



// getting GPIO pins ready to use
void setup() {
    wiringPiSetup();
    pinMode(clk, OUTPUT);
    pinMode(dat, OUTPUT);
    pinMode(lat, OUTPUT);
    pinMode(blank, OUTPUT);
}


// store pins and corresponding pwm values
bool setPWM(uint16_t chan, uint16_t pwm) {
    try {
      // Used to prevent accidents
      // 4095 is the max value
      if (pwm > 4095) pwm = 4095;
      pwmbuffer[chan] = pwm;

        // return true if everything is OK
        cout << "Pins are set" << "\n";
        return true;

    } catch (const std::exception& e) {
        // return false if there is an error
        cout << "Pins are not set" << "\n";
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
        cout << "Data is transferred" << "\n";
        return true;

    } catch (const std::exception& e) {
        // return false if there is an error
        cout << "Data is not transferred" << "\n";
        return false;
    }

}
