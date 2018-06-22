

from signal import pause
import subprocess
import sys

import time
import datetime
import os

from envirophat import light


# Import the PCA9685 module.
import Adafruit_PCA9685

def whats_the_time():
    now = datetime.datetime.now()
    return (now.strftime("%H:%M:%S"))

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

def translate(value, leftMin, leftMax, rightMin, rightMax):
    # Figure out how 'wide' each range is
    leftSpan = leftMax - leftMin
    rightSpan = rightMax - rightMin

    # Convert the left range into a 0-1 range (float)
    valueScaled = float(value - leftMin) / float(leftSpan)

    # Convert the 0-1 range into a value in the right range.
    return rightMin + (valueScaled * rightSpan)

def lights():
    room_light = average_lux()

    light_value = translate(room_light, 0, 8800, 3700, 4095)

    light_value = 7795 - light_value

    light_value = int(round(light_value))

    print(light_value)

    # Turn on the light
    pwm.set_pwm(0, 0, light_value)
    pwm.set_pwm(3, 0, light_value)
    pwm.set_pwm(2, 0, light_value)
    pwm.set_pwm(1, 0, light_value)

    return

# L2 - 0
# L3 - 1
# L1 - 2 - germination unit light
# L4 - 3
# F1 - 4 - Big germination unit fog
# F2 - 5 - Nurpeiis germination unit fog
# F3 - 6
# F4 - 7
# F5 - 8
# F6 - 9


print('Let the games begin')

while True:
    try:
        # Initialise the PCA9685 using the default address (0x40).
        pwm = Adafruit_PCA9685.PCA9685()

        # Set frequency to 60hz, good for servos.
        pwm.set_pwm_freq(60)

        lights()

        pwm.set_pwm(5, 0, 1800)
        pwm.set_pwm(4, 0, 3000)


        while True:
            pwm.set_pwm(6, 0, 4000)
            time.sleep(60)
            pwm.set_pwm(6, 0, 0)
            time.sleep(1)

            lights()

            pwm.set_pwm(7, 0, 4000)
            time.sleep(60)
            pwm.set_pwm(7, 0, 0)
            time.sleep(1)

            lights()

            pwm.set_pwm(7, 0, 4000)
            time.sleep(60)
            pwm.set_pwm(7, 0, 0)
            time.sleep(1)

            lights()

            pwm.set_pwm(8, 0, 4000)
            time.sleep(60)
            pwm.set_pwm(8, 0, 0)
            time.sleep(1)

            lights()

            time.sleep(2000)

            lights()

    except KeyboardInterrupt:
        break

    except:
        print("Unexpected error:", sys.exc_info()[0])
        ts = time.time()
        st = datetime.datetime.fromtimestamp(ts).strftime('%Y-%m-%d %H:%M:%S')
        print st
        time.sleep(1)
        print("Recovering...")
        print("Recovered...")

        print(">>>>>>>>>>>>>>")

        pass
