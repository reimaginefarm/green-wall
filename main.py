##############################################
# Team NYUAD, Green / Vaponic Wall Code v0.1 #
# Python version 2.7.13                      #
# gpiozero library is needed                 #
# 2018                                       #
##############################################

from gpiozero import * # The library used to enable and use GPIO pins on the Raspi Zero
from time import sleep
from signal import pause

import raspiConfig


def main():

    #runFoggers()

    measureLightLevel()  

    pause()

def measureLightLevel():
    lightLevel = InputDevice(20)
    print(lightLevel)
    return lightLevel

def runFoggers():

    foggers = list(raspiConfig.fogger.values())

    for i in range(4):
        pin = raspiConfig.fogger[i]['pin']
        onTime = raspiConfig.fogger[i]['onTime']
        DigitalOutputDevice(pin).blink(on_time = onTime)

    #print(raspiConfig.fogger[3]['onTime'])


main()


# The huge comment
while False:
    import raspiPins

    raspiPins.set()

    print valve1

    while True:
        try:
            LED(raspiConfig.fogger[2]).blink()
        except ValueError:
            print("Oops!  That was no valid number.  Try again...")


    DigitalOutputDevice(raspiConfig.fogger[1]).blink()

    DigitalOutputDevice(raspiConfig.fogger[2]).blink(on_time=3)

    pause()

    for i in foggers:
            DigitalOutputDevice(i).blink(on_time = raspiConfig.fogger[i]['onTime'])

    pause()
