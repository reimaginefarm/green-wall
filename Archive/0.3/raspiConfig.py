# > Use the following function to set the GPIO pinouts of the Raspherry Pi Zero
#   with the rest of the circuit used
# > There are total of 27 GPIO pins are avaliable. Any pin can be used for input
#   and output
# > Check https://www.raspberrypi.org/documentation/usage/gpio/ for more
#   information
# > The order of the pins might seem weird but they are based on the physical
#   locations of the pins on the board

# Follow this structure > DeviceName : GPIO pin

# 24v Foggers
fogger = {
    0 : {'pin' : 14, 'onTime' : 0.5, 'offTime' : 1 },
    1 : {'pin' : 15, 'onTime' : 1, 'offTime' : 0.5 },
    2 : {'pin' : 18, 'onTime' : 0.5, 'offTime' : 1 },
    3 : {'pin' : 23, 'onTime' : 1, 'offTime' : 0.5 },
}

# 12v Valves
valve = {
    1 : 24,
    2 : 25,
    3 : 8,
    4 : 7,
}

# 12v Peristaltic pumps
pump = {
    1 : 1, # Pumps water from water tank to the self flushing siphon
    2 : 12, # Pumps nutrient A to the water tank
    3 : 16, # Pumps nutrient B to the water tank
    4 : 20, # Pumps pH down solution to the water tank
}

# 48v Lights
light = {
    1 : 21,
}

# Sensors
sensor = {
    'phMeter' : 0, # not connected
    'ecMeter' : 5,
    'temperatureSensor' : 6,
    'waterFlowMeter1' : 13,
    'waterFlowMeter2' : 19, # not connected
    'lightSensor' : 26,
}
