import smbus
import time


def getData():
    global ch0
    global ch1

    # Get I2C bus
    bus = smbus.SMBus(1)

    bus.write_byte_data(0x39, 0x00 | 0x80, 0x03)
    bus.write_byte_data(0x39, 0x01 | 0x80, 0x02)

    time.sleep(0.5)
    data = bus.read_i2c_block_data(0x39, 0x0C | 0x80, 2)
    data1 = bus.read_i2c_block_data(0x39, 0x0E | 0x80, 2)

    # Convert the data
    ch0 = data[1] * 256 + data[0]
    ch1 = data1[1] * 256 + data1[0]

# Output data to screen

def getFullSpectrum():
    getData()
    #print ("Full Spectrum(IR + Visible) :%d lux" %ch0)
    return ch0

def getInfraredSpectrum():
    getData()
    #print ("Infrared Value :%d lux" %ch1)
    return ch0

def getVisibleSpectrum():
    getData()
    #print ("Visible Value :%d lux" %(ch0 - ch1))
    value = ch0 - ch1
    return value
