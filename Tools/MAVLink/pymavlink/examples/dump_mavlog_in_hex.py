""" Dump MAVLog File in Hex"""
# Copyright Pete Hollands 2012
# Licensed for reuse under GPL V3 a

import os, sys
import binascii

maxlines = 1000
myfile = os.open('LOG00087.LOG', os.O_RDONLY)
for i in range(1000) :
    try:
        a = os.read(myfile,1)
        if ( a == "" ):
            print "\r\nEnd of File Reached"
            break 
        b = binascii.hexlify(a)
        if (b == 'fe') :
            print
        print b,

    except:
        print "Strange issues"
