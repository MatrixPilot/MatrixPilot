#!/usr/bin/env python
# Authors Pete Hollands using code from Andrew Tridgell
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
            print "F2:T%li:S%s:N%li:E%li:A%li:W%i:a%i:b%i:c%i:d%i:e%i:f%i:g%i:h%i" \
                     ":i%i:c%u:s%i:cpu%u:bmv%i:as%u:wvx%i:wvy%i:wvz%i:ma%i:mb%i:mc%i:svs%i:hd%i:" % \
                  ( msg.sue_time, bstr( msg.sue_status ), msg.sue_latitude, msg.sue_longitude, \
                    msg.sue_altitude, msg.sue_waypoint_index,  \
                    msg.sue_rmat0, msg.sue_rmat1, msg.sue_rmat2, \
                    msg.sue_rmat3, msg.sue_rmat4, msg.sue_rmat5, \
                    msg.sue_rmat6, msg.sue_rmat7, msg.sue_rmat8, \
                    msg.sue_cog, msg.sue_sog, msg.sue_cpu_load, msg.sue_voltage_milis, \
                    msg.sue_air_speed_3DIMU, \
                    msg.sue_estimated_wind_0, msg.sue_estimated_wind_1, msg.sue_estimated_wind_2, \
                    msg.sue_magFieldEarth0, msg.sue_magFieldEarth1, msg.sue_magFieldEarth2, \
                    msg.sue_svs, msg.sue_hdop ),
        elif msg.get_type() == 'SERIAL_UDB_EXTRA_F2_B':
            sys.stdout.softspace=False
            print "p1i%i:p2i%i:p3i%i:p4i%i:p5i%i:p6i%i:p7i%i:p8i%i:p9i%i:p10i%i:" % \
                  ( msg.sue_pwm_input_1, msg.sue_pwm_input_2, msg.sue_pwm_input_3, msg.sue_pwm_input_4, msg.sue_pwm_input_5, \
                    msg.sue_pwm_input_6, msg.sue_pwm_input_7, msg.sue_pwm_input_8, msg.sue_pwm_input_9, msg.sue_pwm_input_10),
            sys.stdout.softspace=False
            print "p1o%i:p2o%i:p3o%i:p4o%i:p5o%i:p6o%i:p7o%i:p8o%i:p9o%i:p10o%i:" %   \
                  ( msg.sue_pwm_output_1, msg.sue_pwm_output_2, msg.sue_pwm_output_3, \
                    msg.sue_pwm_output_4, msg.sue_pwm_output_5, msg.sue_pwm_output_6, \
                    msg.sue_pwm_output_7, msg.sue_pwm_output_8, msg.sue_pwm_output_9, \
                    msg.sue_pwm_output_10 ),
            sys.stdout.softspace=False
            print "imx%i:imy%i:imz%i:fgs%X:ofc%i:tx%i:ty%i:tz%i:G%d,%d,%d:stk%d:\r\n" %       \
                  ( msg.sue_imu_location_x, msg.sue_imu_location_y, msg.sue_imu_location_z,   \
                    msg.sue_flags, msg.sue_osc_fails,                                         \
	            msg.sue_imu_velocity_x, msg.sue_imu_velocity_y, msg.sue_imu_velocity_z,   \
	            msg.sue_waypoint_goal_x, msg.sue_waypoint_goal_y, msg.sue_waypoint_goal_z,\
                    msg.sue_memory_stack_free ),

        else :
            pass
                                 
# create a mavlink serial instance
master = mavutil.mavlink_connection('COM5', baud=57600)

show_messages(master)
