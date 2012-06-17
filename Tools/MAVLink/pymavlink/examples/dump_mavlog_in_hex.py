""" Dump MAVLog File in Hex"""
# Copyright Pete Hollands 2012
# Licensed for reuse under GPL V3 a

import os, sys
import binascii

myfile = open('flight.log.raw', 'rb')
#myfile.seek(-300,2) # Seek to 300 bytes back from the end of file (-300,2)
while True:
        a = myfile.read(1)
        if ( a == "" ):
            print "\r\nEnd of File Reached"
            break 
        b = binascii.hexlify(a[0])
        if (b == 'fe') :
            print
        print b,

