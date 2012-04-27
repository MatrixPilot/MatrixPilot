#!/usr/bin/env python
# Authors Pete Hollands using code from Andrew Tridgell
'''
Print out MatrixPilot SERIAL_UDB_EXTRA (SUE) telemetry from a MAVLink connection.
'''

import sys, struct, time, os

# allow import from the parent directory, where mavlinkv10.py is
sys.path.insert(0, os.path.join(os.getcwd(), '..'))

os.environ['MAVLINK10'] = '1'
import mavlinkv10 as mavlink
import mavutil

def bstr(n): # n in range 0-7
    '''Convert number to 3 digit binary string'''
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
            sys.stdout.softspace=False # This stops a space being inserted between print statements
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
        elif msg.get_type() == 'SERIAL_UDB_EXTRA_F4' :
            print "F4:R_STAB_A=%i:R_STAB_RD=%i:P_STAB=%i:Y_STAB_R=%i:Y_STAB_A=%i:AIL_NAV=%i:" \
                      "RUD_NAV=%i:AH_STAB=%i:AH_WP=%i:RACE=%i:\r\n" % \
	          ( msg.sue_ROLL_STABILIZATION_AILERONS, msg.sue_ROLL_STABILIZATION_RUDDER,   \
                    msg.sue_PITCH_STABILIZATION, msg.sue_YAW_STABILIZATION_RUDDER,            \
                    msg.sue_YAW_STABILIZATION_AILERON, msg.sue_AILERON_NAVIGATION,            \
                    msg.sue_RUDDER_NAVIGATION, msg.sue_ALTITUDEHOLD_STABILIZED,               \
                    msg.sue_ALTITUDEHOLD_WAYPOINT, msg.sue_RACING_MODE ),
        elif msg.get_type() == 'SERIAL_UDB_EXTRA_F5' :
            print "F5:YAWKP_A=%5.3f:YAWKD_A=%5.3f:ROLLKP=%5.3f:ROLLKD=%5.3f:A_BOOST=%3.1f:\r\n" % \
                  ( msg.sue_YAWKP_AILERON, msg.sue_YAWKD_AILERON, msg.sue_ROLLKP, \
                    msg.sue_ROLLKD, msg.sue_AILERON_BOOST ),
        elif msg.get_type() == 'SERIAL_UDB_EXTRA_F6' :
            print "F6:P_GAIN=%5.3f:P_KD=%5.3f:RUD_E_MIX=%5.3f:ROL_E_MIX=%5.3f:E_BOOST=%3.1f:\r\n" % \
                  ( msg.sue_PITCHGAIN, msg.sue_PITCHKD, msg.sue_RUDDER_ELEV_MIX, \
                    msg.sue_ROLL_ELEV_MIX, msg.sue_ELEVATOR_BOOST),
        elif msg.get_type() == 'SERIAL_UDB_EXTRA_F7' :
            print "F7:Y_KP_R=%5.4f:Y_KD_R=%5.3f:RLKP_RUD=%5.3f:RLKD_RUD=%5.3f:" \
                  "RUD_BOOST=%5.3f:RTL_PITCH_DN=%5.3f:\r\n"  % \
                   ( msg.sue_YAWKP_RUDDER, msg.sue_YAWKD_RUDDER, msg.sue_ROLLKP_RUDDER , \
                     msg.sue_ROLLKD_RUDDER , msg.sue_RUDDER_BOOST, msg.sue_RTL_PITCH_DOWN),
        elif msg.get_type() == 'SERIAL_UDB_EXTRA_F8' :
            print "F8:H_MAX=%6.1f:H_MIN=%6.1f:MIN_THR=%3.2f:MAX_THR=%3.2f:PITCH_MIN_THR=%4.1f:" \
                  "PITCH_MAX_THR=%4.1f:PITCH_ZERO_THR=%4.1f:\r\n" % \
                  ( msg.sue_HEIGHT_TARGET_MAX, msg.sue_HEIGHT_TARGET_MIN, \
                    msg.sue_ALT_HOLD_THROTTLE_MIN, msg.sue_ALT_HOLD_THROTTLE_MAX, \
                    msg.sue_ALT_HOLD_PITCH_MIN, msg.sue_ALT_HOLD_PITCH_MAX, \
                    msg.sue_ALT_HOLD_PITCH_HIGH),
        elif msg.get_type() == 'SERIAL_UDB_EXTRA_F13' :
            print "F13:week%i:origN%li:origE%li:origA%li:\r\n" % \
                  (msg.sue_week_no, msg.sue_lat_origin, msg.sue_lon_origin, msg.sue_alt_origin),
        elif msg.get_type() == 'SERIAL_UDB_EXTRA_F14' :
            print "\r\nF14:WIND_EST=%i:GPS_TYPE=%i:DR=%i:BOARD_TYPE=%i:AIRFRAME=%i:RCON=0x%X:TRAP_FLAGS=0x%X:TRAP_SOURCE=0x%lX:ALARMS=%i:"  \
                       "CLOCK=%i:FP=%d:\r\n" % \
                  ( msg.sue_WIND_ESTIMATION, msg.sue_GPS_TYPE, msg.sue_DR, msg.sue_BOARD_TYPE, \
                    msg.sue_AIRFRAME, msg.sue_RCON, msg.sue_TRAP_FLAGS, msg.sue_TRAP_SOURCE,   \
                    msg.sue_osc_fail_count, msg.sue_CLOCK_CONFIG, msg.sue_FLIGHT_PLAN_TYPE ),
        elif msg.get_type() == 'SERIAL_UDB_EXTRA_F15' :
            print "F15:IDA=%s:IDB=%s\r\n" % \
                  ( msg.sue_ID_VEHICLE_MODEL_NAME, msg.sue_ID_VEHICLE_REGISTRATION ),
        elif msg.get_type() == 'SERIAL_UDB_EXTRA_F16' :
            print "F16:IDC=%s::IDD=%s\r\n" % \
                  ( msg.sue_ID_LEAD_PILOT, msg.sue_ID_DIY_DRONES_URL ),
        else :
            pass
                                 
# create a mavlink serial instance
master = mavutil.mavlink_connection('COM5', baud=57600)

show_messages(master)
