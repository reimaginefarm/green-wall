from datetime import datetime
import time
import os
import pprint
from decimal import Decimal


import sqlite3
sqlite_file = 'jobs.db'
conn = sqlite3.connect(sqlite_file)
c = conn.cursor()

#db.jobs.insert( { lastModified: ISODate().getTime()/1000 , deviceType: 'fogger', deviceNumber:2, pinNumber:1, powerPercent:100, onDuration:30, offDuration:50} )

def main():

    while False:
        # Get jobs from the database
        c.execute("SELECT * FROM jobs")
        rows = c.fetchall()

        for row in rows:
            id = str(row[0])
            #deviceType = row[1]
            #deviceNumber = row[2]
            #pinNumber = row[3]
            #powerPercent = row[4]
            onDuration = row[5]
            #offDuration = row[6]
            #lastModified = row[7]
    currentTime = time.time()
    id = 5
    c.execute('UPDATE jobs SET lastModified=? WHERE id = ?', (currentTime, id))
    conn.commit()

main()
