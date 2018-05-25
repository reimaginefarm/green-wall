from pyfirmata import Arduino, util
import time
board = Arduino('/dev/ttyACM0')

board.digital[4].write(1)

while True:
	board.digital[7].write(1)

	time.sleep(5)

	board.digital[7].write(0)

	time.sleep(10)

	board.digital[8].write(1)

	time.sleep(5)

	board.digital[8].write(0)

	time.sleep(10)
