import sqlite3
sqlite_file = 'greenwall.sqlite3'


# Create the database connection
runDevicesConn = sqlite3.connect(sqlite_file)
runDevicesC = runDevicesConn.cursor()

# QUERY to get total number of devices <strike> for the given device type </strike>
query = 'SELECT * FROM {}'.format('pumps')
#query = 'SELECT * FROM foggers WHERE deviceType = "fogger"'
runDevicesC.execute(query)
rows = runDevicesC.fetchall()
totalNumberOfDevices = len(rows)


string = str(rows)
substring = 'refillPump'

count = string.count(substring)

# print count
print("The count is:", count)
