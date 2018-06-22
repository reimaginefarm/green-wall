#!/usr/bin/env python

# Use the Enviro pHAT to check how much light is in a room, then trigger a webhook to control
# Philips Hue lights via IFTTT. By Wesley Archer (@raspberrycoulis).

# Import the relevant modules
import time
import datetime
import os
from envirophat import light
import Adafruit_PCA9685

# Get the current time for displaying in the terminal.
def whats_the_time():
    now = datetime.datetime.now()
    return (now.strftime("%H:%M:%S"))

# The function to turn off the lights. Sends a webhook to IFTTT which
# triggers Hue. Replace the trigger word and tokens from your account.

def turn_off():

    pwm.set_pwm(0, 0, 0)

    print("Lights off!")

# The function to turn on the lights. Sends a webhook to IFTTT which
# triggers Hue. Replace the trigger word and tokens from your account.

def turn_on():


    pwm.set_pwm(0, 0, 4000)

    print("Lights on!")

# Check the light level and determine whether the lights need to
# be turned on or off.

def average_lux():
    # Variables for calculating the average lux levels
    start_time = time.time()
    curr_time = time.time()
    collect_light_time = 5
    collect_light_data = []

    # Calculate the average lux level over 60 seconds
    print("Calculating average light level...")
    while curr_time - start_time < collect_light_time:
	      curr_time = time.time()
	      avg = light.light()
	      collect_light_data.append(avg)
	      time.sleep(1)
    # Take the last 45 data points taken over 60 seconds to calculate the average
    average_light = sum(collect_light_data[-45:]) / 45.0
    now = whats_the_time()
    print("Average over {collect_time} seconds is: {average} lux. Last checked at {time}".format(
        collect_time=collect_light_time,
        average=average_light,
        time=now
    ))
    return average_light

try:

    pwm = Adafruit_PCA9685.PCA9685()

    # Alternatively specify a different address and/or bus:
    #pwm = Adafruit_PCA9685.PCA9685(address=0x40, busnum=1)


    # Set frequency to 60hz, good for servos.
    pwm.set_pwm_freq(60)

    pwm.set_pwm(3, 0, 4000)
except KeyboardInterrupt:
    pass
