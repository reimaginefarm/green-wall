##############################################
# Team NYUAD, Green / Vaponic Wall Code v0.4 #
# Python version 2.7.13                      #
# 2018                                       #
##############################################

from signal import pause
import subprocess
import sys

import schedule

import time
import os
import datetime

import Adafruit_PCA9685
from envirophat import light

sys.stdout = open("log.txt", "a", 1)

# Initialise the PCA9685 using the default address (0x40).
pwm = Adafruit_PCA9685.PCA9685()

# Set frequency to 60hz
pwm.set_pwm_freq(60)

# L2 - 0
# L4 - 1
# L1 - 2 - germination unit light
# L3 - 3
# F1 - 4 - Big germination unit fog
# F2 - 5 - Small germination unit fog
# F3 - 6
# F4 - 7
# F5 - 8
# F6 - 9

def main():

    print 'Started @',  whatsTheTime()
    print("---The main---")

    # Set lights
    setLights()

    # Set pins (foggers, etc) here
    # For example: setPin(number of the channel, power percent, onDuration, offDuration)
    # durations are in seconds!!!

    setPin(4, 100, 100, 3600)
    setPin(5, 100, 100, 3600)

    setPin(6, 100, 60, 1800)
    setPin(7, 100, 60, 1800)
    setPin(8, 100, 60, 1800)
    setPin(9, 100, 60, 1800)

    #schedule.run_all()

    while True:
        # Go through the queue and run one by one
        schedule.run_pending()
        time.sleep(1)


##########################
###### Functions... ######
##########################

# Setup the pins
def setPin(chan, percent, onDuration, offDuration):
    pwmValue = (4095*percent)/100
    pwmValue = int(round(pwmValue))

    # Define the switching on schedule
    schedule.every(onDuration+offDuration).seconds.do(switchON, chan, pwmValue)

    # Shift the switching off schedule by onDuration, so that the port stays open
    # for that much time
    time.sleep(onDuration)

    # Define the switching off schedule
    schedule.every(onDuration+offDuration).seconds.do(switchOFF, chan)

# switch on a channel
def switchON(chan, pwmValue):
    pwm.set_pwm(chan, 0, pwmValue)
    print "Chan", chan, "turned ON @", whatsTheTime()

# switch off a channel
def switchOFF(chan):
    pwm.set_pwm(chan, 0, 0)
    print "Chan", chan, "turned OFF @", whatsTheTime()
    #print(">>>>>>>>>>>>>>>>")

def setLights():
    schedule.every().hour.do(runLights)

# Adjust the lights according to the amount of sunlight in the room
def runLights():

    print("Setting up lights...")

    # Get the average lux value for the light in the room
    room_light = average_lux()

    # Map the lux value from the light sensor to the Adafruit PCA9685 values
    light_value = translate(room_light, 0, 8800, 3700, 4095)

    # Invert the light value
    light_value = 7795 - light_value

    # Round to the nearest integer
    light_value = int(round(light_value))

    # For debugging
    #print(light_value)

    # Predefined channels for the lights
    lights = [0, 1, 2 , 3]
    # Update the light values
    for l in lights:
        switchON(l, light_value)

    print("Completed \n")

# Get the amount of light avaliable in the room in terms of lux
def average_lux():
    # Variables for calculating the average lux levels
    start_time = time.time()
    curr_time = time.time()
    collect_light_time = 5
    collect_light_data = []

    # Calculate the average lux level over 5 seconds
    #print("Calculating average light level...")
    while curr_time - start_time < collect_light_time:
	      curr_time = time.time()
	      avg = light.light()
	      collect_light_data.append(avg)
	      time.sleep(1)
    # Take the last 45 data points taken over 5 seconds to calculate the average
    average_light = sum(collect_light_data[-45:]) / 45.0
    now = whatsTheTime()

    # For debugging
    # print("Average over {collect_time} seconds is: {average} lux. Last checked at {time}".format(
    #    collect_time=collect_light_time,
    #    average=average_light,
    #    time=now
    # ))
    return average_light

# The mapping function
def translate(value, leftMin, leftMax, rightMin, rightMax):
    # Figure out how 'wide' each range is
    leftSpan = leftMax - leftMin
    rightSpan = rightMax - rightMin

    # Convert the left range into a 0-1 range (float)
    valueScaled = float(value - leftMin) / float(leftSpan)

    # Convert the 0-1 range into a value in the right range.
    return rightMin + (valueScaled * rightSpan)

# Get the current time in H:M:S format
def whatsTheTime():
    now = datetime.datetime.now()
    return (now.strftime("%H:%M:%S"))

###########################
#### Run the main code ####
###########################
while True:

    # try-except case to keep the code running even if there are error
    try:

        # Initial light setup
        runLights()

        # Main code
        main()

    # Stop the code if ctrl + c is pressed
    except KeyboardInterrupt:
        break

    # Print any error to the terminal screen with its time stamp
    except:
        print("Unexpected error:", sys.exc_info()[0])
        print datetime.datetime.fromtimestamp(time.time()).strftime('%Y-%m-%d %H:%M:%S')
        time.sleep(1)
        print("Recovering...")
        print("Recovered...")
        print(">>>>>>>>>>>>>> \n")
        pass
