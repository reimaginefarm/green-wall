from pyfirmata import Arduino, util
import time
from datetime import datetime
from threading import Timer

global board
board = Arduino('/dev/ttyACM0')

#file.readline(line number koy):
# Line 0 > PLACE HOLDER
# Line 1 > Foggers ON time duration [seconds]
# Line 2 > Foggers OFF time duration [seconds]
# Line 3 > Lights start time [hour]
# Line 4 > Lights start time [minute]
# Line 5 > Lights end time [hour]
# Line 6 > Lights end time [minute]

# s for digital, p for PWM
#lights = board.get_pin('d:4:s')
#fogger1 = board.get_pin('d:5:s')
#fogger2 = board.get_pin('d:6:s')

board.digital[4].write(1)

def turnOffLights():
    board.digital[4].write(0)

def turnOnLights():
    board.digital[4].write(1)

while True:

    # Check data file for the values
    dataFile = open("/home/greenwall/greenwall_web/arduino_data.txt").read().splitlines()
    dataFile = list(map(float, dataFile))

    # Apply data
    board.digital[7].write(1)
    time.sleep(dataFile[1])
    board.digital[7].write(0)
    time.sleep(dataFile[2])

    board.digital[8].write(1)
    time.sleep(dataFile[1])
    board.digital[8].write(0)
    time.sleep(dataFile[2])








t = Timer(secs, hello_world)
t.start()
