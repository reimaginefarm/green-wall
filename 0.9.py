 #!/usr/bin/python
##############################################
# Team NYUAD, Green / Vaponic Wall Code v0.9 #
# Python version 3.6.4                       #
# 2018                                       #
##############################################

##############################
# Default python libraries
import datetime
import time
import os
import pprint
from decimal import Decimal
##############################


######################################################
# I2C addresses tutorial
# https://learn.adafruit.com/i2c-addresses?view=all
######################################################


########################################################################
# https://github.com/adafruit/Adafruit_Python_PCA9685
# 16 Channel 12 Bit PWM Driver Library
import Adafruit_PCA9685

# Initialise the PCA9685 using the default address (0x40).
# Actually board assignments depends on the information on the database,
# those are just reminders...

# Foggers Board
board0 = Adafruit_PCA9685.PCA9685(address=0x40)

# Lights Board
board1 = Adafruit_PCA9685.PCA9685(address=0x41)

# Pumps Board
#board2 = Adafruit_PCA9685.PCA9685(address=0x42)

#
#board3 = Adafruit_PCA9685.PCA9685(address=0x43)

#
#board4 = Adafruit_PCA9685.PCA9685(address=0x44)

# Set frequency to 60hz
board0.set_pwm_freq(60)
board1.set_pwm_freq(60)
#board2.set_pwm_freq(60)
#board3.set_pwm_freq(60)
#board4.set_pwm_freq(60)


## DON'T FORGET TO UNCOMMENT THE OTHER BOARDS IN THE REST OF THE Code ##

########################################################################


################################################################################
# http://www.pibits.net/code/raspberry-pi-and-tsl2561-luminosity-sensor.php
# The light sensor library
# The Adafruit_TSL2561.py file should be kept in the same directory with this
# python file
# default i2c address is (0x39)
from Adafruit_TSL2561 import *
################################################################################


################################################################################
# https://github.com/pimoroni/enviro-phat
# https://pinout.xyz/pinout/enviro_phat   for pinout
# Temperature and barometer sensor library (+ motion sensor but not used)
#
# i2cdetect -y 1
#
# I2C addresses:
# 0x49: ADS1015 - 4-channel 5v tolerant 12-bit ADC
# 0x29: TCS3472 - light and RGB colour sensor
# 0x1d: LSM303D - accelerometer/magnetometer sensor
# 0x77: BMP280 - temperature/pressure sensor
import envirophat
################################################################################


####################################################################
# Library for the advanced python scheduler (apscheduler)
from apscheduler.schedulers.background import BackgroundScheduler

import logging

#logging.basicConfig(filename='apscheduler.log')
#logging.getLogger('apscheduler').setLevel(logging.DEBUG)

scheduler = BackgroundScheduler()
####################################################################


########################################
# sqlite3 library to use the database
import sqlite3
sqlite_file = 'greenwall.sqlite3'
#conn = sqlite3.connect(sqlite_file)
#c = conn.cursor()
# When a connection is needed, the connection is created seperately
# inside of the functions. Somehow, the generic created connection
# cannot be used inside of functions
#
# Make sure phpliteadmin files are in the same folder
# https://www.phpliteadmin.org/
# use "php -S IP_ADRESS:8000 phpliteadmin.php" to edit database manually
#
# get local ip address of the current machine
from getIPAdress import *

# Enable or disable to reach to the sqlite3 database from browser at
# http://localIPAdressOfTheMachine:8000 adress
enableWebServer = True
########################################


####################################################################
# Global variables used in the code

# Sleep time in seconds for checking new jobs from database
sleepTime = 2

# Change to True for enabling terminal comments
# Don't enable both at the same time!
simpleDebugging = False
detailedDebugging = True

# Growing light adjustment updating interval in minutes
# 15
lightUpdateInterval = 0.3

# Water (for boxes) refill pump adjustment updating interval in minutes
# 15
refillPumpsUpdateInterval = 1

# Adjust the nutrients in the nutrient tanks interval in minutes
# 15
nutrientTankUpdateInterval = 1

# pH Sensor logging interval in minutes
pHLoggerInterval = 5

# Light Sensor logging interval in minutes
# 10
lightLoggerInterval = 5

# Temperature Sensor logging interval in minutes
# 10
temperatureLoggerInterval =  0.3

# Pressure Sensor logging interval in minutes
# 10
pressureLoggerInterval =  0.3

# Extra dim lights after the time given in the database for the
# night lamp effect
nightLampMode = True
####################################################################

# Main function of the code
def main():

    addJobsFromDB()

    startApscheduler()

    printJobs()

    # try-except case to keep the code running even if there are errors
    try:
        while True:

            time.sleep(sleepTime)

    except (KeyboardInterrupt, SystemExit):
        scheduler.shutdown()


##########################
###### Functions... ######
##########################

# Starts the advanced python scheduler
def startApscheduler():
    scheduler.start()


# The main jobs of the control system are defined here. Please add any tasks here
# I mean checking sensors at intervals, turning foggers on and off continously,
# or adjusting lights based on the time of the day kind of stuff
def addJobsFromDB():
    # Loop option should be 'False' for jobs that run at intervals
    # For example: Lights should have args=['lights', 'light', False] arguments
    #              while foggers have args=['foggers', 'fogger', True] arguments

    # Calculate the time everything will start to run
    # this little delay gives some breath to rapi before it runs everything
    runTime = datetime.datetime.now() + datetime.timedelta(hours=0, minutes=0, seconds=(sleepTime))

    #####################################
    #### Physical device controllers ####
    #####################################
    # Start lights
    # Check, control, and change light levels every given minutes
    #scheduler.add_job(runDevices, 'interval', minutes=lightUpdateInterval, name='lights', args=['lights', 'light', False], next_run_time=runTime, max_instances=10)

    # Start foggers
    # Start running and control foggers all the time
    #scheduler.add_job(runDevices, 'date', name='foggers', args=['foggers', 'fogger', True], run_date=runTime)

    # Start box water refilling pumps
    #scheduler.add_job(runDevices, 'interval', minutes=refillPumpsUpdateInterval, name='refillPumps', args=['pumps', 'refillPump', False], next_run_time=runTime, max_instances=10)

    # Manage nutrient tanks
    #scheduler.add_job(runDevices, 'interval', minutes=nutrientTankUpdateInterval, name='nutrientTanks', args=['liquidTanks', 'nutrientTank', False], next_run_time=runTime, max_instances=10)

    ########## bu fikirden vaz gectim
    # Manage water tanks
    #scheduler.add_job(runDevices, 'interval', minutes=waterTankUpdateInterval, name='nutrientPumps', args=['pumps', 'nutrientPump', False], next_run_time=runTime, max_instances=10)
    ##########


    ########################
    #### Sensor logging ####
    ########################
    # Start light sensor logging at the given interval
    #scheduler.add_job(digitalSensorLogger, 'interval', minutes=lightLoggerInterval, name='lightSensors',
    #args=['sensorLogs', 'light', 'lux', getFullSpectrum, 'none', False, 'average', 10], next_run_time=runTime, max_instances=10)

    # Start temperature sensor logging at the given interval
    #scheduler.add_job(digitalSensorLogger, 'interval', minutes=temperatureLoggerInterval, name='temperatureSensors',
    #args=['sensorLogs', 'temperature', 'Celcius', envirophat.weather.temperature, 'none', False, 'once'], next_run_time=runTime, max_instances=10)

    # Start pressure sensor logging at the given interval
    #scheduler.add_job(digitalSensorLogger, 'interval', minutes=pressureLoggerInterval, name='pressureSensors',
    #args=['sensorLogs', 'pressure', 'Pa', envirophat.weather.pressure, 'none', False, 'once'], next_run_time=runTime, max_instances=10)

    ##########
    # Start analog sensor at port '0' logging at the given interval (LDR in this case)
    #scheduler.add_job(digitalSensorLogger, 'interval', minutes=pressureLoggerInterval, name='LDRSensors',
    #args=['sensorLogs', 'LDR', 'volts', envirophat.analog.read, 0, False, 'once'], next_run_time=runTime, max_instances=10)
    ##########

# Prints the current jobs of the system with the current time
def printJobs():
    print(' ')
    print('Current time is: %s' % datetime.datetime.now())
    scheduler.print_jobs()
    print(' ')


#############################################################################################################
############################ fake sensor functions to test functionality - START ############################
# Fake water level sensor reading function for testing purposes
def getWaterLevel():
    return 0
############################ fake sensor functions to test functionality - END ##############################
#############################################################################################################


# This function is used to identify connected number of devices and run the corresponding
# function to do the required changes on the devices. This a kind of an orginer function
# that is used to organize the workflow
#
# For the apscheduler, 'interval' scheduler is recommended if the device should run
# at intervals and don't forget to make loop = False. If the device should be controlled
# continously 'date' scheduler is recommended and don't forget to make loop = True in the
# arguments.
# For example:
# scheduler.add_job(runDevices, 'interval', minutes=lightUpdateInterval, name='lights', args=['lights', 'light', False], next_run_time=runTime, max_instances=10)
# scheduler.add_job(runDevices, 'date', name='foggers', args=['foggers', 'fogger', True], run_date=runTime)
#
# tableName: Table name for this device. This will be used to get total number of
#            given devices avaliable
#            -> must be a string
# deviceType: Type of the device that will be run. This will be used to identify this
#             device and run the correct function
#             -> must be a string
# loop: Make this true if you want to read values from sensor continously. Simple it keeps this
#       function run all the time. If you used 'interval' to schedule this functions, this option
#       doesn't make sense. For this reason, it is false for default which means this function will
#       run only one time when called
#       -> must be a boolean value like True or False
def runDevices(tableName, deviceType, loop = False):

    # Just incase of not receiving a string
    deviceType = str(deviceType)

    # try-except case to keep the code running even if there are error
    try:
        while True:

            # Create the database connection
            runDevicesConn = sqlite3.connect(sqlite_file)
            runDevicesC = runDevicesConn.cursor()

            # QUERY to get total number of devices <strike> for the given device type </strike>
            query = 'SELECT * FROM {}'.format(tableName)
            #query = 'SELECT * FROM foggers WHERE deviceType = "fogger"'
            runDevicesC.execute(query)
            rows = runDevicesC.fetchall()

            # Get total number of devices in this table
            #totalNumberOfDevices = len(rows)

            # Not a good way to solve this problem but this counts the number
            # devices in the given device type. Somehow both table name and a column
            # name cannot be variables. So, I cannot make a query that retrieves
            # the specific given type of device (for ex: refill pumps and nutrient pumps)
            # So, here is a quick and dirty solution to count this after the query is made.
            string = str(rows)
            substring = deviceType
            totalNumberOfDevices = string.count(substring)
            totalNumberOfDevices = int(totalNumberOfDevices)

            if (detailedDebugging) or (simpleDebugging): print(' ')

            # This array starts from zero but device numbers start from 1
            # So, this +1 adjusts this difference
            for currentDevice in range(0, totalNumberOfDevices):
                if (deviceType == 'fogger'):
                    runNextFoggerFromDB(currentDevice+1)
                elif (deviceType == 'light'):
                    runNextLightFromDB(currentDevice+1)
                elif (deviceType == 'refillPump'):
                    runNextRefillPumpFromDB(currentDevice+1)
                elif (deviceType == 'nutrientTank'):
                    runNextNutrientTankFromDB(currentDevice+1)

            if (detailedDebugging) or (simpleDebugging): print(' ')

            # Close db connection
            runDevicesConn.close()

            # Run code once if loop is not selected
            if (loop == False):
                break

    except (KeyboardInterrupt, SystemExit):
        scheduler.shutdown()


############################################################################################################
############################################################################################################
############################################################################################################
############################################################################################################
############################################################################################################
# Used to turn .........
#
# nextNutrientTank: The number of the refill pump should be altered
#                   -> must be an interger
def runNextNutrientTankFromDB(nextNutrientTank):


    # db'den bahsi gecen tank icin degerleri al, ona gore gerekli seyleri icine pompala
    # bunları yaparken su seviyesinin de asilmamasina dikkat et

    return 1

############################################################################################################
############################################################################################################
############################################################################################################
############################################################################################################
############################################################################################################



# Used to manage the water level in the plant boxes. This function checks the
# corresponding water level sensor in the corresponding plant box. If the water
# level is below the required level, it pumps some water for 10 seconds and checks
# the water level again. This proccess repeats until the desired water level is
# reached.
#
# nextRefillPump: The number of the refill pump should be altered
#             -> must be an interger
def runNextRefillPumpFromDB(nextRefillPump):

    # Create the database connection
    runNextRefillPumpFromDBConn = sqlite3.connect(sqlite_file)
    runNextRefillPumpFromDBC = runNextRefillPumpFromDBConn.cursor()

    # Get the next fogger from the database
    runNextRefillPumpFromDBC.execute('SELECT * FROM pumps WHERE deviceType = "refillPump" AND deviceNumber = ?', (nextRefillPump,))
    rows = runNextRefillPumpFromDBC.fetchall()

    for row in rows:
        id = str(row[0])
        deviceType = str(row[1])
        deviceNumber = row[2]
        boardNumber = row[3]
        pinNumber = row[4]
        powerPercent = row[5]
        lastModified = row[6]

    pwmValue = powerPercentToPwmValue(powerPercent)

    # Get the water level for the corresponding sensor
    # These functions depend on the external library for the water level sensor
    waterLevel = getSensorValue(sensorReadingFunction = getWaterLevel,
    sensorName = 'Water level sensor', sensorUnit = 'cm', sensorReadingFunctionArgument = 'none',
    sensorNumber=deviceNumber, readingType = 'once', length=10)

    # This part is designed for the water level sensor that gives 1 when the water level is correct
    # and it gives 0 when the water level is not correct. So, this part should be changed depending
    # on the water level sensor used.
    # This gives water for 10 seconds and check the water level to see if desired water level is
    # achieved
    while (waterLevel == 0):

        # Some information to be printed to the terminal
        jobName = deviceType + ' ' + str(deviceNumber) + ' ON '
        id = id + 'ON'

        # Turn on the pump
        switchON(boardNumber, pinNumber, pwmValue, jobName)

        # Fill water for 10 seconds check the water level
        time.sleep(10)

        # Some information to be printed to the terminal
        jobName = deviceType + ' ' + str(deviceNumber) + ' OFF '
        id = id + 'OFF'

        # Turn off the pump
        switchOFF(boardNumber, pinNumber, jobName)

        # Get the water level for the corresponding sensor
        # These functions depend on the external library for the water level sensor
        waterLevel = getSensorValue(sensorReadingFunction = getWaterLevel,
        sensorName = 'Water level sensor', sensorUnit = 'cm', sensorReadingFunctionArgument = 'none',
        sensorNumber=deviceNumber, readingType = 'once', length=10)

    # Close db connection
    runNextRefillPumpFromDBConn.close()


# Used to adjust a light based on the ambient light at the moment of run
# Also, checks the clock to turn lights ON and OFF based on the time of the day.
# Timings can be changed in the database
#
# Possible spectrum types are full, ir, and visible for the sensors to check.
# Gets full spectrum by default. Here are avaliable spectrum option functions:
# getFullSpectrum()
# getInfraredSpectrum()
# getVisibleSpectrum()
#
# nextFogger: The number of the fogger should be altered
#             -> must be an interger
def runNextLightFromDB(nextLight):

    # Create the database connection
    runNextLightFromDBConn = sqlite3.connect(sqlite_file)
    runNextLightFromDBC = runNextLightFromDBConn.cursor()

    # Get the next light from the database
    runNextLightFromDBC.execute('SELECT * FROM lights WHERE deviceType = "light" AND deviceNumber = ?', (nextLight,))
    rows = runNextLightFromDBC.fetchall()

    for row in rows:
        id = str(row[0])
        deviceType = str(row[1])
        deviceNumber = row[2]
        boardNumber = row[3]
        pinNumber = row[4]
        #powerPercent = row[5]
        lastModified = row[6]
        onTime = str(row[7])
        offTime = str(row[8])
        extraDimAfter = str(row[9])
        #timeSinceEdited = int(time.time())-int(lastModified)

        #if(timeSinceEdited < sleepTime):
        #    if (detailedDebugging) or (simpleDebugging): print(' ')
        #    if (detailedDebugging) or (simpleDebugging): print('Updated lights')
        #    if (detailedDebugging) or (simpleDebugging): print(' ')

    # Check if this light should be running at this time of the day
    if isNowInTimePeriod(onTime, offTime):
        # Get the corresponding spectrum type based on the selected type
        # These functions depend on the external library for the sensor
        ambientLightValue = getSensorValue(sensorReadingFunction = getFullSpectrum,
        sensorName = 'Light sensor', sensorUnit = 'lux', sensorReadingFunctionArgument = 'none',
        sensorNumber=1, readingType = 'average', length=10)

        # Map the lux value from the light sensor to the Adafruit PCA9685 values
        lightValue = translate(ambientLightValue, 0, 8800, 3700, 4095)

        # Invert the light value
        lightValue = 7700 - lightValue

        # Extra dimming check
        # Check if the current time is between extra dimming time and 6 AM
        # if it is true, extra dim the lights for night lamp mode
        if (isNowInTimePeriod(extraDimAfter, '0600') and nightLampMode):
            if (detailedDebugging) or (simpleDebugging): print(' ')
            if (detailedDebugging) or (simpleDebugging):
                print('Night lamp mode is enabled for light ' + str(deviceNumber)
                + ' @ ' + str(datetime.datetime.now()))
            if (detailedDebugging) or (simpleDebugging): print(' ')

            lightValue  = lightValue*0.5

        # Round to the nearest integer
        lightValue = int(round(lightValue))

        # Some information to be printed to the terminal
        jobName = deviceType + ' ' + str(deviceNumber) + ' ON '
        id = id + 'ON'

        # Turn on the light
        switchON(boardNumber, pinNumber, lightValue, jobName)

    else:
        # Some information to be printed to the terminal
        jobName = deviceType + ' ' + str(deviceNumber) + ' OFF '
        id = id + 'OFF'

        # Turn off the light
        switchOFF(boardNumber, pinNumber, jobName)

    # Close db connection
    runNextLightFromDBConn.close()


# Used to turn a fogger on or off based on the database record for this specific
# fogger
#
# nextFogger: The number of the fogger should be altered
#             -> must be an interger
def runNextFoggerFromDB(nextFogger):

    # Create the database connection
    runNextFoggerFromDBConn = sqlite3.connect(sqlite_file)
    runNextFoggerFromDBC = runNextFoggerFromDBConn.cursor()

    # Get the next fogger from the database
    runNextFoggerFromDBC.execute('SELECT * FROM foggers WHERE deviceType = "fogger" AND deviceNumber = ?', (nextFogger,))
    rows = runNextFoggerFromDBC.fetchall()

    for row in rows:
        id = str(row[0])
        deviceType = str(row[1])
        deviceNumber = row[2]
        boardNumber = row[3]
        pinNumber = row[4]
        powerPercent = row[5]
        lastModified = row[6]
        onDuration = row[7]
        offDuration = row[8]
        #timeSinceEdited = int(time.time())-int(lastModified)

        #if(timeSinceEdited < sleepTime):
        #    if (detailedDebugging) or (simpleDebugging): print(' ')
        #    if (detailedDebugging) or (simpleDebugging): print('Updated foggers')
        #    if (detailedDebugging) or (simpleDebugging): print(' ')

    pwmValue = powerPercentToPwmValue(powerPercent)

    # Some information to be printed to the terminal
    jobName = deviceType + ' ' + str(deviceNumber) + ' ON '
    id = id + 'ON'

    # Turn on the fogger
    switchON(boardNumber, pinNumber, pwmValue, jobName)

    # The port stays open for that much time
    time.sleep(onDuration)

    # Some information to be printed to the terminal
    jobName = deviceType + ' ' + str(deviceNumber) + ' OFF '
    id = id + 'OFF'

    # Turn off the fogger
    switchOFF(boardNumber, pinNumber, jobName)

    if (detailedDebugging):
        print('On duration for ' + deviceType + ' ' + str(deviceNumber) + ' was ' + str(onDuration) + ' seconds')

    if (detailedDebugging): print(' ')

    # Close db connection
    runNextFoggerFromDBConn.close()


#########################################################################################################################
# Sensor numarasina gore birden fazla sensoru kontrol edebilme ozelligi gerekiyor                                       #
# su an ben de bilemedim nasıl yapabilirim ama bir sekilde o sensorun numarasinin                                       #
# aktarilmasi gerekiyor                                                                                                 #
# Multiple sensor okuma ozelligi hic test edilmedi, edilemedi                                                           #
#########################################################################################################################
# This function is similar to the runDevices() function but this function is used
# to read sensors and record the readings into the database. Again this is function
# used to orginize workflow and increase readibility. This function uses getSensorValue()
# function to read sensor values
#
# For the apscheduler, 'interval' scheduler is recommended to log sensors at intervals
# For example:
# scheduler.add_job(digitalSensorLogger, 'interval', minutes=lightLoggerInterval,
# name='lightSensors', args=['sensorLogs', 'light', 'lux', getFullSpectrum, False, 10, False],
# next_run_time=runTime, max_instances=10)
#
# tableName: Table name for this sensor (actually never used in the function!)
#            -> must be a string
# sensorType: Type of the sensor that will be logged. This will be used to identify this
#             sensor in the database
#             -> must be a string
# sensorUnit: Sensor unit is the unit of the measurement, mostly used for terminal outputs.
#             For example, pH for pH sensor
#             -> must be a string
# sensorReadFunction: Name of the function that will be used to read the sensor.
#                     The function used should return 'something' and it can be anything.
#                     The result be directly transferred to the database. Please don't drop the table :)
#                     Type name of the function without brackets. For example, myFunction for myFunction()
#                     -> must be a function that returns something
#                     !!!W-A-R-N-I-N-G!!! ////////If you want to get average value for this sensor,
#                     the sensor reading function should return numbers! We cannot sum and divide strings! ////////
# sensorReadFunctionArgument: This argument is transferred directly to the sensor reading function given above.
#                             It is 'none' (nothing) by default.
#                             -> can be any variable
# loop: Make this true if you want to read values from sensor continously. Simple it keeps this
#       function run all the time. If you used 'interval' to schedule this functions, this option
#       doesn't make sense. For this reason, it is false for default which means this function will
#       run only one time when called
#       -> must be a boolean value like True or False
# readingType: Make this 'average' or 'once' to get average over some time or single reading
#             It is average by default.
#             -> must be a string, either 'average' or 'once'
# getAverageOver: Duration for sensor to be get the average in <b>seconds</b>. 10 seconds by default
#                 -> must be an integer
def digitalSensorLogger(tableName, sensorType, sensorUnit, sensorReadFunction, sensorReadFunctionArgument = 'none', loop = False, readingType = 'average', getAverageOver = 10):

    # try-except case to keep the code running even if there are error
    try:
        while True:

            # Create the database connection
            digitalSensorLoggerConn = sqlite3.connect(sqlite_file)
            digitalSensorLoggerC = digitalSensorLoggerConn.cursor()

            # QUERY to get total number of devices for the given sensor type
            #digitalSensorLoggerC.execute('SELECT * FROM sensors WHERE sensorType = ? AND digitalSensor = 1', (sensorType,))
            digitalSensorLoggerC.execute('SELECT * FROM sensors WHERE sensorType = ?', (sensorType,))
            rows = digitalSensorLoggerC.fetchall()
            totalNumberOfSensors = len(rows)

            if (detailedDebugging) or (simpleDebugging): print(' ')

            if (totalNumberOfSensors == 0):
                if (detailedDebugging) or (simpleDebugging):
                    print('There are no ' + str(sensorType) + ' sensor(s) matching in the database')
                    print('OR this is not a digital sensor!')

            else:
                # Go through all sensors one by one
                for currentSensor in range(0, totalNumberOfSensors):

                    if (detailedDebugging) or (simpleDebugging):
                        if (readingType == 'average'):
                            print('Getting ' + str(sensorType) + ' sensor ' + str(currentSensor+1)
                            + ' average over ' + str(getAverageOver) + ' seconds')

                        elif (readingType == 'once'):
                            print('Getting direct ' + str(sensorType) + ' sensor ' + str(currentSensor+1) + ' reading')

                    # Get the sensor value
                    sensorReading = getSensorValue(sensorReadingFunction = sensorReadFunction,
                    sensorName = (str(sensorType)+' sensor'), sensorUnit = str(sensorUnit),
                    sensorReadingFunctionArgument = sensorReadFunctionArgument,
                    sensorNumber = (currentSensor+1), readingType = readingType, length = int(getAverageOver))

                    # Insert the reading into the database
                    currentTime = time.time() # UNIX time
                    #currentTime = datetime.datetime.now() # Human date
                    digitalSensorLoggerC.execute('INSERT INTO sensorLogs(sensorType, sensorNumber, dateTime, reading1) VALUES(?,?,?,?)',
                    (sensorType, (currentSensor+1), currentTime, sensorReading,))

                    # Save (commit) the changes
                    digitalSensorLoggerConn.commit()

                    if (detailedDebugging): print('Updated database')

            # Close db connection
            digitalSensorLoggerConn.close()

            if (detailedDebugging) or (simpleDebugging): print(' ')

            # Run code once if loop is not selected
            if (loop == False):
                break


    except (KeyboardInterrupt, SystemExit):
        scheduler.shutdown()


# This function is similar to the runDevices() function but this function is used
# to read sensors and record the readings into the database. This function returns
# the calculated average value.
#
# sensorReadingFunction: Name of the function that will be used to read the sensor.
#                        The function should return an interger because we are getting average and
#                        we need numbers to get average. We need to do some math :)
#                        Type name of the function without brackets. For example, myFunction for myFunction()
#                        -> must be a function that returns something
# sensorName: Name of this sensor. Mostly used for terminal comments
#            -> must be a string
# sensorUnit: Sensor unit is the unit of the measurement, mostly used for terminal outputs.
#             For example, pH for pH sensor
#             -> must be a string
# sensorNumber: Number of the sensor that will be logged. This will be used to identify the sensor.
#                This will be transferred to the sensor reading function provided if there are more
#                than one sensor for this sensor type.  Sensor number is 1 by default.
#                -> must be an interger
# sensorReadingFunctionArgument: This argument is transferred directly to the sensor reading function given above.
#                                It is 'none' (nothing) by default.
#                                -> can be any variable
#                                !!!W-A-R-N-I-N-G!!! //////// This doesn't transfer argument to the function if there
#                                are more than one sensor for single ('once') reading types. Instead, it transfers the number of the sensor
#                                to the sensor reading function////////
# readingType: Make this 'average' or 'once' to get average over some time or single reading
#             It is average by default.
#             -> must be a string, either 'average' or 'once'
# length: Duration for sensor to be get the average in <b>seconds</b>. 10 seconds by default
#         -> must be an integer
#
def getSensorValue(sensorReadingFunction, sensorName, sensorUnit, sensorReadingFunctionArgument = 'none', sensorNumber = 1, readingType = 'average', length = 10):

    if (readingType == 'average'):
        # Variables for calculating the average lux levels
        startTime = time.time()
        currTime = time.time()
        collectSensorTime = length
        collectSensorData = []

        # Calculate the average sensor value over 10 seconds
        while currTime - startTime < collectSensorTime:
            currTime = time.time()

            # If there are only one sensor for this sensor type, don't transfer the
            # current sensor number to the sensor reading function because it doesn't
            # make sense
            if (sensorNumber > 1):
                # Get direct sensor reading
                avg = sensorReadingFunction(sensorNumber)

            elif(sensorReadingFunctionArgument == 'none'):
                # Get direct sensor reading
                avg = sensorReadingFunction()

            else:
                # Get direct sensor reading
                avg = sensorReadingFunction(sensorReadingFunctionArgument)

            collectSensorData.append(avg)
            time.sleep(0.5)
        # Take the last 5 data points taken over 10 seconds to calculate the average
        averageSensor = sum(collectSensorData[-length:]) / length

        #if (detailedDebugging) or (simpleDebugging):
        #    print(' ')
        #    print(sensorName + ' data is updated')
        #    print(' ')

        if (detailedDebugging):
            print(str(sensorName) + ' average over ' + str(collectSensorTime) + ' seconds is: ' +
            str(averageSensor) + ' ' + str(sensorUnit))
            print('Last checked at ' + str(datetime.datetime.now()))
            print(' ')

        return averageSensor

    elif(readingType == 'once'):

        # If there are only one sensor for this sensor type, don't transfer the
        # current sensor number to the sensor reading function because it doesn't
        # make sense
        if (sensorNumber > 1):
            # Get direct sensor reading
            sensorReading = sensorReadingFunction(sensorNumber)

        elif(sensorReadingFunctionArgument == 'none'):
            # Get direct sensor reading
            sensorReading = sensorReadingFunction()

        else:
            # Get direct sensor reading
            sensorReading = sensorReadingFunction(sensorReadingFunctionArgument)

        if (detailedDebugging):
            print(str(sensorName) + ' reading is: ' + str(sensorReading) + ' ' + str(sensorUnit))
            print('Last checked at ' + str(datetime.datetime.now()))
            print(' ')

        return sensorReading


# This function checks if the current time is in the given time range.
# Returns True or False
# Please enter time in 24 hours format! Please don't use AM or PM format
#
# For example, enter 1430 for 14:30 or 2:30 PM
#
# startTime: Start time in 0930 format. Don't put ":" between hour and minutes.
#            -> must be a string
# endTime: End time in 2359 format. Don't put ":" between hour and minutes.
#          -> must be a string
def isNowInTimePeriod(startTime, endTime):

    # Split the string
    startTimeHour = startTime[:2]
    startTimeMinute = startTime[2:]

    # Split the string
    endTimeHour = endTime[:2]
    endTimeMinute = endTime[2:]

    nowTime = datetime.datetime.now().time().replace(microsecond=0)
    startTime = datetime.time(int(startTimeHour), int(startTimeMinute))
    endTime = datetime.time(int(endTimeHour), int(endTimeMinute))

    if startTime < endTime:
        return nowTime >= startTime and nowTime <= endTime
    else: #Over midnight
        return nowTime >= startTime or nowTime <= endTime


# Used to convert a given percent value to convert pwm value for the Adafruit
# PCA9685 board
#
# powerPercent: The power percent that will be converted
#               -> must be a number
def powerPercentToPwmValue(powerPercent):

    # Just to correct if there is a mistake
    if(powerPercent > 100):
        powerPercent

    pwmValue = (4095*powerPercent)/100
    pwmValue = int(round(pwmValue))

    return pwmValue


# This is the mapping function. It converts one range to another and calculates
# the given value in the new range and returns it
#
# For example converts value of 100 in (0, 255) range to a value in (0, 4080) range
#
# value, leftMin, leftMax, rightMin, rightMax: -> must be numbers
def translate(value, leftMin, leftMax, rightMin, rightMax):
    # Figure out how 'wide' each range is
    leftSpan = leftMax - leftMin
    rightSpan = rightMax - rightMin

    # Convert the left range into a 0-1 range (float)
    valueScaled = float(value - leftMin) / float(leftSpan)

    # Convert the 0-1 range into a value in the right range.
    return rightMin + (valueScaled * rightSpan)


# Switch ON a channel on the given Adafruit PCA9685 board based on the given PWM value
#
# board: Number of the Adafruit PCA9685 board
#        -> must be an interger
# chan: Number of the Adafruit PCA9685 channel (pin). Something from 0 to 15
#        -> must be an interger
# pwmValue: PWM value that will be applied to the given channel on the board
#        -> must be an interger
# name: Name of the operiton that will be printed to terminal for debugging
#       -> must be a string
def switchON(board, chan, pwmValue, name):
    # Choose the correct board
    if (board == 0):
        board0.set_pwm(chan, 0, pwmValue)
    elif (board == 1):
        board1.set_pwm(chan, 0, pwmValue)
    elif (board == 2):
        #board2.set_pwm(chan, 0, pwmValue)
        print(' ')
    elif (board == 3):
        #board3.set_pwm(chan, 0, pwmValue)
        print(' ')
    elif (board == 4):
        #board4.set_pwm(chan, 0, pwmValue)
        print(' ')

    chan = str(chan)
    pwmValue = str(pwmValue)
    if (simpleDebugging): print (name + ' @ ' + str(datetime.datetime.now()))
    if (detailedDebugging): print (name + '(' + 'Board ' + str(board) + ', Chan ' + chan + ', PWM: ' + pwmValue + ' ) @ ' + str(datetime.datetime.now()))


# Switch OFF a channel on the given Adafruit PCA9685 board
#
# board: Number of the Adafruit PCA9685 board
#        -> must be an interger
# chan: Number of the Adafruit PCA9685 channel (pin). Something from 0 to 15
#        -> must be an interger
# name: Name of the operiton that will be printed to terminal for debugging
#       -> must be a string
def switchOFF(board, chan, name):
    # Choose the correct board
    if (board == 0):
        board0.set_pwm(chan, 0, 0)
    elif (board == 1):
        board1.set_pwm(chan, 0, 0)
    elif (board == 2):
        #board2.set_pwm(chan, 0, 0)
        print(' ')
    elif (board == 3):
        #board3.set_pwm(chan, 0, 0)
        print(' ')
    elif (board == 4):
        #board4.set_pwm(chan, 0, 0)
        print(' ')

    chan = str(chan)
    if (simpleDebugging): print (name + ' @ ' + str(datetime.datetime.now()))
    if (detailedDebugging): print (name + '(' + 'Board ' + str(board) + ', Chan ' + chan + ', PWM: 0 ) @ ' + str(datetime.datetime.now()))


###########################
#### Run the main code ####
###########################
if __name__ == '__main__':

    # Just to have a more clear terminal screen
    os.system ('clear')

    print('##############################################')
    print('# Team NYUAD, Green / Vaponic Wall Code v0.9 #')
    print('# Python version 3.6.4                       #')
    print('# 2018                                       #')
    print('#                                            #')
    print('# Press Ctrl+C to exit                       #')
    print('##############################################')

    # Some camel art :)
    print("""
                                                 //^\\
                                             //^\\  #
       q_/\/\   q_/\/\    q_/\/\   q_/\/\      #   #
         ||||`    /|/|`     <\<\`    |\|\`     #   #
    %*%*%**%**%**%*%*%**%**%**%****%*%**%**%**%*%*%**%**%
        """)

    if enableWebServer:
        print('Starting web server for the sqlite3 database')
        os.system ('php -S ' + str(localIPAdress()) + ':8000 phpliteadmin.php &')
        print('Password is admin')
        print(' ')

    main()
