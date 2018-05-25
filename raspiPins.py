# > Use the following function to set the GPIO pinouts of the Raspherry Pi Zero
#   with the rest of the circuit used
# > There are total of 27 GPIO pins are avaliable. Any pin can be used for input
#   and output
# > Check https://www.raspberrypi.org/documentation/usage/gpio/ for more
#   information
# > The order of the pins might seem weird but they are based on the physical
#   locations of the pins on the board

def set():

    # ---- Some kind of array structure can be implemented to make life easier

    # Follow this structure > DeviceName = GPIO pin

    # 24v Foggers
    fogger1 = 14
    fogger2 = 15
    fogger3 = 18 # not connected
    fogger4 = 23 # not connected

    # 12v Valves
    valve1 = 24
    valve2 = 25
    valve3 = 8
    valve4 = 7 # not connected

    # 12v Peristaltic pumps
    pump1 = 1 # Pumps water from water tank to the self flushing siphon
    pump2 = 12 # Pumps nutrient A to the water tank
    pump3 = 16 # Pumps nutrient B to the water tank
    pump4 = 20 # Pumps pH down solution to the water tank

    # 48v Lights
    light1 = 21

    # Sensors
    phMeter = 0 # not connected
    ecMeter = 5
    temperatureSensor = 6
    waterFlowMeter1 = 13
    waterFlowMeter2 = 19 # not connected
    lightSensor = 26

    print "Raspherry Pi Zero pins are all set"
