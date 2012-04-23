#!/usr/bin/env python

'''
Print out SERIAL_UDB_EXTRA telemetry from MAVLink 
'''

import sys, struct, time, os

# allow import from the parent directory, where mavlink.py is
sys.path.insert(0, os.path.join(os.getcwd(), '..'))

os.environ['MAVLINK10'] = '1'
import mavlinkv10 as mavlink
import mavutil

def bstr(n): # n in range 0-255
    return ''.join([str(n >> x & 1) for x in (2,1,0)])

def show_messages(m):
    '''show incoming mavlink messages'''
    while True:
        msg = m.recv_match(blocking=True)
        if not msg:
            return
        if msg.get_type() == "BAD_DATA":
            if mavutil.all_printable(msg.data):
                sys.stdout.write(msg.data)
                sys.stdout.flush()
        elif msg.get_type() == 'SERIAL_UDB_EXTRA_F2_A':
            #print "F2:T%li:S%d:N%li:E%li:A%li:W%i:" % \
            print "F2:T%li:S%s:N%li:E%li:A%li:W%i:" %  \
                  ( msg.sue_time, bstr( msg.sue_status ), \
                    msg.sue_latitude, msg.sue_longitude, msg.sue_altitude, msg.sue_waypoint_index )
            #, msg.sue_latitude, msg.sue_longitude, msg.sue_altitude, msg.sue_waypoint_index
            #a%i:b%i:c%i:d%i:e%i:f%i:g%i:h%i:i%i:c%u:s%i:cpu%u:bmv%i:"" % msg.sue_time
        else :
            pass
                                 
# create a mavlink serial instance
master = mavutil.mavlink_connection('COM5', baud=57600)

show_messages(master)
