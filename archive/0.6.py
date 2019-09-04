##############################################
# Team NYUAD, Green / Vaponic Wall Code v0.6 #
# Python version 3.6.4                       #
# 2018                                       #
##############################################

from datetime import datetime
import time
import os
import pprint
from decimal import Decimal

#import Adafruit_PCA9685

from apscheduler.schedulers.background import BackgroundScheduler

import logging

#logging.basicConfig(filename='example.log')
#logging.getLogger('apscheduler').setLevel(logging.DEBUG)

import sqlite3
sqlite_file = 'jobs.db'
conn = sqlite3.connect(sqlite_file)
c = conn.cursor()


# Sleep time in seconds for checking new jobs from database
sleepTime = 5

# Change to True for enabling terminal comments
debugging = True

scheduler = BackgroundScheduler()

# Initialise the PCA9685 using the default address (0x40).
#pwm = Adafruit_PCA9685.PCA9685()

# Set frequency to 60hz
#pwm.set_pwm_freq(60)

def main():

    addJobsFromDB()

    startApscheduler()

    printJobs()

    print('Press Ctrl+C to exit')

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

def tick():
    print('Tick! The time is: %s' % datetime.now())

def startApscheduler():
    scheduler.start()

def printJobs():
    print('Current time is: %s' % datetime.now())
    scheduler.print_jobs()


def printCollection(collection_name):
    for i in collection_name.find():
        pprint.pprint(i)

def addJobsFromDB():

    # Get jobs from the database
    c.execute('SELECT * FROM jobs')
    rows = c.fetchall()

    for row in rows:
        id = str(row[0])
        deviceType = row[1]
        deviceNumber = row[2]
        #pinNumber = row[3]
        #powerPercent = row[4]
        onDuration = row[5]
        #offDuration = row[6]
        #lastModified = row[7]

        # Add new jobs to apscheduler
        jobName = str(deviceType) + ' ' + str(deviceNumber)
        scheduler.add_job(tick, 'interval', seconds=onDuration, replace_existing=True, id=id, name=jobName)

def updateJobsFromDB():

    if debugging == True: print(' ')
    if debugging == True: print('checked db')

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

            if debugging == True: print('found new job(s)')

            #scheduler.remove_job(id)
            #scheduler.add_job(tick, 'interval', seconds=onDuration, replace_existing=True, id=id)

            # Reschedule jobs at apscheduler
            scheduler.reschedule_job(job_id=id, trigger = 'interval', seconds=onDuration)

            if debugging == True: print('updated job(s)')

            printJobs()


    if not foundNewJob:
        if debugging == True: print('no new jobs')

    if debugging == True: print('////////////////')
    if debugging == True: print(' ')

def some_task_1():
	#print('the task 1')
    return

def some_task_2():
	#print('the task 2')
    return

def some_task_3():
	#print('the task 3')
    return

###########################
#### Run the main code ####
###########################

main()
