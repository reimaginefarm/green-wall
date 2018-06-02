# Copyright (c) 2014 Adafruit Industries
# Author: Tony DiCola
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

import operator
import time
from gpiozero import *
from signal import pause
from ctypes import *


MSBFIRST = 0
LSBFIRST = 1



def main():
    #spi = BitBang()
    #sent = spi.write([0x80])
    #print(sent)

    clk_pin = 16 #clock
    dat_pin = 20 #data
    lat_pin = 21 #latch
    pwm = 0 #default pwm value

    clk = DigitalOutputDevice(clk_pin)
    dat = DigitalOutputDevice(dat_pin)
    lat = DigitalOutputDevice(lat_pin)

    lat.off()

    c_number = ctypes.c_int16(24)
    b_number = ctypes.c_int18(24)

    #################

    #ledPin = 23

    pwm = 100


    ################

    lat.off()

    # 24 channels per TLC5974
    for c in range(23, -1, -1):
        # 12 bits per channel, send MSB first
        for b in range(11, -1, -1):
            clk.off()

            if(c and (1<<b)):
                dat.on()
            else:
                dat.off()

            clk.off()

        lat.on()
        lat.off()

    pause()

def list_to_bytearray(data):
    return bytearray(list_to_int(data[i:i+8]) for i in range(0, len(data), 8))

def bytearray_to_list(data):
    data = bytearray(data)
    return sum(map(int_to_list, data), [])

def bytearray_to_int(data):
    return list_to_int(bytearray_to_list(data))

def int_to_bytearray(data):
    return list_to_bytearray(int_to_list(data))

def write(self, data, assert_ss=True, deassert_ss=True):
    """Half-duplex SPI write.  If assert_ss is True, the SS line will be
    asserted low, the specified bytes will be clocked out the MOSI line, and
    if deassert_ss is True the SS line be put back high.
    """
    # Fail MOSI is not specified.
    if self._mosi is None:
        raise RuntimeError('Write attempted with no MOSI pin specified.')
    if assert_ss and self._ss is not None:
        self._gpio.set_low(self._ss)
    for byte in data:
        for i in range(8):
            # Write bit to MOSI.
            if self._write_shift(byte, i) & self._mask:
                self._gpio.set_high(self._mosi)
            else:
                self._gpio.set_low(self._mosi)
            # Flip clock off base.
            self._gpio.output(self._sclk, not self._clock_base)
            # Return clock to base.
            self._gpio.output(self._sclk, self._clock_base)
    if deassert_ss and self._ss is not None:
        self._gpio.set_high(self._ss)

main()
