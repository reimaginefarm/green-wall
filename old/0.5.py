##############################################
# Team NYUAD, Green / Vaponic Wall Code v0.5 #
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

#logging.basicConfig(filename='example.log')
#logging.getLogger('apscheduler').setLevel(logging.DEBUG)

from pymongo import MongoClient
client = MongoClient('localhost', 27017)
db = client['apscheduler']
jobsCol = db['jobs']

#db.jobs.insert( { lastModified: ISODate().getTime()/1000 , deviceType: 'fogger', deviceNumber:2, pinNumber:1, powerPercent:100, onDuration:30, offDuration:50} )

# Sleep time in seconds for checking new jobs from database
sleepTime = 5

# Change to True for enabling terminal comments
debugging = True

scheduler = BackgroundScheduler()
#scheduler.add_jobstore('mongodb', collection='jobs')

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
	print(scheduler.print_jobs())

def printCollection(collection_name):
    for i in collection_name.find():
        pprint.pprint(i)

def addJobsFromDB():
    # Get jobs from the database
    for i in jobsCol.find():
        id = str(i['_id'])
        #deviceType = i['deviceType']
        #deviceNumber = i['deviceNumber']
        #pinNumber = i['pinNumber']
        #powerPercent = i['powerPercent']
        onDuration = i['onDuration']
        #offDuration = i['offDuration']
        #lastModified = i['lastModified']

        # Add new jobs to apscheduler
        scheduler.add_job(tick, 'interval', seconds=onDuration, replace_existing=True, id=id)

def updateJobsFromDB():

    if debugging == True: print(' ')
    if debugging == True: print('checked db')

    foundNewJob = False

    # Get jobs from the database
    for i in jobsCol.find():
        id = str(i['_id'])
        onDuration = i['onDuration']
        lastModified = i['lastModified']
        timeSinceEdited = int(time.time())-int(lastModified)

        if(timeSinceEdited < sleepTime):

            foundNewJob = True

            if debugging == True: print('found new job(s)')

            #scheduler.remove_job(id)
            #scheduler.add_job(tick, 'interval', seconds=onDuration, replace_existing=True, id=id)

            # Reschedule jobs at apscheduler
            scheduler.reschedule_job(job_id=id, trigger = 'interval', seconds=onDuration)

            if debugging == True: print('updated job(s)')

            scheduler.print_jobs()

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
