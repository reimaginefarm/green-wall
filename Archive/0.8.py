##############################################
# Team NYUAD, Green / Vaponic Wall Code v0.7 #
# Python version 3.6.4                       #
# 2018                                       #
##############################################

import datetime
import time
import os
import pprint
from decimal import Decimal

#import Adafruit_PCA9685

# Initialise the PCA9685 using the default address (0x40).
#pwm = Adafruit_PCA9685.PCA9685()

# Set frequency to 60hz
#pwm.set_pwm_freq(60)

from apscheduler.schedulers.background import BackgroundScheduler

import logging

#logging.basicConfig(filename='apscheduler.log')
#logging.getLogger('apscheduler').setLevel(logging.DEBUG)

import sqlite3
sqlite_file = 'jobs.db'
conn = sqlite3.connect(sqlite_file)
c = conn.cursor()

# Sleep time in seconds for checking new jobs from database
sleepTime = 5

# Change to True for enabling terminal comments
simpleDebugging = False
detailedDebugging = True

scheduler = BackgroundScheduler()

def main():

    addJobsFromDB()

    startApscheduler()

    printJobs()

    # try-except case to keep the code running even if there are error
    try:
        while True:

            #updateJobsFromDB()

            time.sleep(sleepTime)

    except (KeyboardInterrupt, SystemExit):
        scheduler.shutdown()


##########################
###### Functions... ######
##########################

#
def tick(message=None):
    print('Tick! The time is: %s' % datetime.datetime.now())

#
def startApscheduler():
    scheduler.start()

#
def printJobs():
    print(' ')
    print('Current time is: %s' % datetime.datetime.now())
    scheduler.print_jobs()
    print(' ')

#
def getTotalNumberOfDevicesFromDB(deviceType):
    c.execute('SELECT * FROM jobs WHERE deviceType = ?', (deviceType,))
    rows = c.fetchall()
    return len(rows)

#
def addJobsFromDB():

    offTime = datetime.datetime.now() + datetime.timedelta(hours=0, minutes=0, seconds=(sleepTime))

    # Start lights
    #scheduler.add_job(runSensors, 'date', name='sensors', run_date=offTime)

    # Start foggers
    scheduler.add_job(runFoggers, 'date', name='foggers', run_date=offTime)

    # Start sensors
    #scheduler.add_job(runSensors, 'date', name='sensors', run_date=offTime)

#
def runFoggers():

    # try-except case to keep the code running even if there are error
    try:
        while True:

            runFoggersConn = sqlite3.connect(sqlite_file)
            runFoggersC = runFoggersConn.cursor()

            runFoggersC.execute('SELECT * FROM jobs WHERE deviceType = "fogger"')
            rows = runFoggersC.fetchall()
            totalNumberOfFoggers = len(rows)

            if (detailedDebugging) or (simpleDebugging): print(' ')

            for currentFogger in range(0, totalNumberOfFoggers):
                runNextFoggerFromDB(currentFogger)

            if (detailedDebugging) or (simpleDebugging): print(' ')

    except (KeyboardInterrupt, SystemExit):
        scheduler.shutdown()

#
def runNextFoggerFromDB(currentFogger):
    nextFogger = currentFogger + 1

    runNextFoggerFromDBConn = sqlite3.connect(sqlite_file)
    runNextFoggerFromDBC = runNextFoggerFromDBConn.cursor()

    # Get next fogger from the database
    runNextFoggerFromDBC.execute('SELECT * FROM jobs WHERE deviceType = "fogger" AND deviceNumber = ?', (nextFogger,))
    rows = runNextFoggerFromDBC.fetchall()

    for row in rows:
        id = str(row[0])
        deviceType = str(row[1])
        deviceNumber = row[2]
        boardNumber = row[3]
        pinNumber = row[4]
        powerPercent = row[5]
        onDuration = row[6]
        offDuration = row[7]
        lastModified = row[8]
        timeSinceEdited = int(time.time())-int(lastModified)

        if(timeSinceEdited < sleepTime):
            if (detailedDebugging) or (simpleDebugging): print(' ')
            if (detailedDebugging) or (simpleDebugging): print('Updated foggers')
            if (detailedDebugging) or (simpleDebugging): print(' ')

    pwmValue = (4095*powerPercent)/100
    pwmValue = int(round(pwmValue))

    jobName = deviceType + ' ' + str(deviceNumber) + ' ON '
    id = id + 'ON'

    # Turn on the fogger
    switchON(boardNumber, pinNumber, pwmValue, jobName)

    # The port stays open for that much time
    time.sleep(onDuration)

    # Schedule the turn off time for the next fogger
    jobName = deviceType + ' ' + str(deviceNumber) + ' OFF '
    id = id + 'OFF'

    # Turn off the fogger
    switchOFF(boardNumber, pinNumber, jobName)

    if (detailedDebugging): print('On duration for ' + deviceType + ' ' + str(deviceNumber) + ' was ' + str(onDuration) + ' seconds')
    if (detailedDebugging): print(' ')

    return nextFogger


# switch on a channel
def switchON(board, chan, pwmValue, name):
    #pwm.set_pwm(chan, 0, pwmValue)
    chan = str(chan)
    if (simpleDebugging): print (name + ' @ ' + str(datetime.datetime.now()))
    if (detailedDebugging): print (name + '(' + 'Chan ' + chan + ')' + ' @ ' + str(datetime.datetime.now()))

# switch off a channel
def switchOFF(board, chan, name):
    #pwm.set_pwm(chan, 0, 0)
    chan = str(chan)
    if (simpleDebugging): print (name + ' @ ' + str(datetime.datetime.now()))
    if (detailedDebugging): print (name + '(' + 'Chan ' + chan + ')' + ' @ ' + str(datetime.datetime.now()))

#
def runSensors():
    a = 1

#
def updateJobsFromDB():

    if detailedDebugging: print(' ')
    if detailedDebugging: print('checked db')

    foundNewJob = False

    # Get jobs from the database
    c.execute('SELECT * FROM jobs')
    rows = c.fetchall()

    for row in rows:
        id = str(row[0])
        onDuration = row[6]
        lastModified = row[8]
        timeSinceEdited = int(time.time())-int(lastModified)

        if(timeSinceEdited < sleepTime):

            foundNewJob = True

            if detailedDebugging: print('found new job(s)')

            #scheduler.remove_job(id)
            #scheduler.add_job(tick, 'interval', seconds=onDuration, replace_existing=True, id=id)

            # Reschedule jobs at apscheduler
            scheduler.reschedule_job(job_id=id, trigger = 'interval', seconds=onDuration)

            if detailedDebugging: print('updated job(s)')

            if (detailedDebugging) or (simpleDebugging): printJobs()


    if not foundNewJob:
        if detailedDebugging: print('no new jobs')

    if detailedDebugging: print('////////////////')
    if detailedDebugging: print(' ')



###########################
#### Run the main code ####
###########################
if __name__ == "__main__":

    print('##############################################')
    print('# Team NYUAD, Green / Vaponic Wall Code v0.7 #')
    print('# Python version 3.6.4                       #')
    print('# 2018                                       #')
    print('#                                            #')
    print('# Press Ctrl+C to exit                       #')
    print('##############################################')

    main()
