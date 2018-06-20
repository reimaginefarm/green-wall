from __future__ import division
import time

from time import sleep
from signal import pause
import subprocess
import sys

import time
import datetime

# Import the PCA9685 module.
import Adafruit_PCA9685


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

        # Alternatively specify a different address and/or bus:
        #pwm = Adafruit_PCA9685.PCA9685(address=0x40, busnum=1)


        # Set frequency to 60hz, good for servos.
        pwm.set_pwm_freq(60)

        # Turn on the light
        pwm.set_pwm(0, 0, 4000)
        pwm.set_pwm(3, 0, 4000)
        pwm.set_pwm(2, 0, 4000)

        pwm.set_pwm(5, 0, 1800)
        pwm.set_pwm(4, 0, 3000)


        while True:
            pwm.set_pwm(6, 0, 4000)
            time.sleep(60)
            pwm.set_pwm(6, 0, 0)
            time.sleep(1)

            pwm.set_pwm(7, 0, 4000)
            time.sleep(60)
            pwm.set_pwm(7, 0, 0)
            time.sleep(1)

            pwm.set_pwm(7, 0, 4000)
            time.sleep(60)
            pwm.set_pwm(7, 0, 0)
            time.sleep(1)

            time.sleep(600)

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

        pwm.set_pwm(1, 0, 4000)
