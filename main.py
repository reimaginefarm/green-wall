##############################################
# Team NYUAD, Green / Vaponic Wall Code v0.1 #
# Python version 2.7.13                      #
# 2018                                       #
##############################################


from time import sleep
from signal import pause
import subprocess

import raspiConfig


TLC5974Pins = [0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0]

def main():

    # Compiling
    #setupRunSwitches()

    print("Started")

    while True:
        try:

            TLC5974Pins[1] = 0


            runSwitches(False)

            sleep(2)

            TLC5974Pins[1] = 4000


            runSwitches(False)

            sleep(2)




            #pause()

        except KeyboardInterrupt:
            break

        except:
            print("Unexpected error:", sys.exc_info()[0])


def runSwitches(blank):
    bashCommand = "./runSwitches"
    if(blank == True):
        commandBlank = "true"
    else:
        commandBlank = "false"

    for i in TLC5974Pins:
        bashCommand = bashCommand + " " + str(i)

    bashCommand = bashCommand + " " + commandBlank

    #print(bashCommand)

    process = subprocess.Popen(bashCommand.split(), stdout=subprocess.PIPE)
    output, error = process.communicate()

def setupRunSwitches():
    print("Compiling runSwitches.cpp\n")
    bashCommand = "g++ -Wall -o runSwitches runSwitches.cpp -lwiringPi"

    process = subprocess.Popen(bashCommand.split(), stdout=subprocess.PIPE)
    output, error = process.communicate()
    print("Compiled runSwitches.cpp\n")

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

    TLC5974Pins = {
                    "0": 0, "1": 0, "2": 0, "3": 0,
                    "4": 0, "5": 0, "6": 0, "7": 0,
                    "8": 0, "9": 0, "10": 0, "11": 0,
                    "12": 0, "13": 0, "14": 0, "15": 0,
                    "16": 0, "17": 0, "18": 0, "19": 0,
                    "20": 0, "21": 0, "22": 0, "23": 0
    }
