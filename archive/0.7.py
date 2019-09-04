##############################################
# Team NYUAD, Green / Vaponic Wall Code v0.7 #
# Python version 3.6.4                       #
# 2018                                       #
##############################################

from datetime import datetime
import time
import os
import pprint
from decimal import Decimal

import Adafruit_PCA9685

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
simpleDebugging = True
detailedDebugging = False

scheduler = BackgroundScheduler()

# Initialise the PCA9685 using the default address (0x40).
pwm = Adafruit_PCA9685.PCA9685()

# Set frequency to 60hz
pwm.set_pwm_freq(60)

def main():

    addJobsFromDB()

    startApscheduler()

    printJobs()

    print('////////////////////')
    print('Press Ctrl+C to exit')
    print('////////////////////')

    # try-except case to keep the code running even if there are error
    try:
        while True:

            updateJobsFromDB()

            time.sleep(sleepTime)

    except (KeyboardInterrupt, SystemExit):
        scheduler.shutdown()


##########################
###### Functions... ######
##########################

#
def tick(message=None):
    print('Tick! The time is: %s' % datetime.now())

#
def startApscheduler():
    scheduler.start()

#
def printJobs():
    print(' ')
    print('Current time is: %s' % datetime.now())
    scheduler.print_jobs()
    print(' ')

#
def addJobsFromDB():

    # Get jobs from the database
    c.execute('SELECT * FROM jobs')
    rows = c.fetchall()

    for row in rows:
        id = str(row[0])
        deviceType = str(row[1])
        deviceNumber = row[2]
        pinNumber = row[3]
        powerPercent = row[4]
        onDuration = row[5]
        offDuration = row[6]
        #lastModified = row[7]

        pwmValue = (4095*powerPercent)/100
        pwmValue = int(round(pwmValue))

        if (deviceType == 'fogger'):

            jobName = deviceType + ' ' + str(deviceNumber) + ' ON'
            id = id + 'ON'
            # Define the switching on schedule
            scheduler.add_job(switchON, 'interval', seconds=onDuration+offDuration, replace_existing=True, id=id, name=jobName, args=[pinNumber, pwmValue, jobName], next_run_time=None)

            # Shift the switching off schedule by onDuration, so that the port stays open
            # for that much time
            time.sleep(onDuration)

            jobName = deviceType + ' ' + str(deviceNumber) + ' OFF'
            id = id + 'OFF'
            # Define the switching off schedule
            scheduler.add_job(switchOFF, 'interval', seconds=onDuration+offDuration, replace_existing=True, id=id, name=jobName, args=[pinNumber, jobName], next_run_time=None)

#def addDevice(id, deviceType, deviceNumber, pinNumber, powerPercent, onDuration, offDuration):
#    # Add new jobs for new devices to apscheduler
#    jobName = deviceType + ' ' + str(deviceNumber)
#    scheduler.add_job(tick, 'interval', seconds=onDuration, replace_existing=True, id=id, name=jobName, args=['job executed!!!!'])

# switch on a channel
def switchON(chan, pwmValue, name):
    pwm.set_pwm(chan, 0, pwmValue)
    chan = str(chan)
    if (simpleDebugging): print (name + ' @ ' + str(datetime.now()))
    if (detailedDebugging): print (name + '(' + 'Chan ' + chan + ')' + ' @ ' + str(datetime.now()))

# switch off a channel
def switchOFF(chan, name):
    pwm.set_pwm(chan, 0, 0)
    chan = str(chan)
    if (simpleDebugging): print (name + ' @ ' + str(datetime.now()))
    if (detailedDebugging): print (name + '(' + 'Chan ' + chan + ')' + ' @ ' + str(datetime.now()))

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
        onDuration = row[5]
        lastModified = row[7]
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

main()
