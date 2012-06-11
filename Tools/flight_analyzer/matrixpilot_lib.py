import re
import sys
import os


try:
    sys.path.insert(0, os.path.join(os.getcwd(), '..\MAVlink\pymavlink'))
    os.environ['MAVLINK10'] = '1'
    import mavlinkv10 as mavlink
    import mavutil
except:
    print "Not able to find Python MAVlink libraries"

def bstr(n): # n in range 0-7
    '''Convert number to 3 digit binary string'''
    return ''.join([str(n >> x & 1) for x in (2,1,0)])

class raw_mavlink_telemetry_file:
    """Model a raw mavlink file (one without local time stamps inserted)"""
    def __init__(self, filename):
        self.m = mavutil.mavlink_connection(filename, notimestamps = True)
        print "NOTE SUE MAVLINK SUPPORT IS STILL UNDER DEVELOPMENT"
 
    def __iter__(self):
        return(self)
        
    def next(self):
        """return the next good SERIAL UDB EXTRA (SUE) Binary MAVLink record"""
        while True :
            self.msg = self.m.recv_match(blocking=False, end_fragment = True)
            if not self.msg:
                    # Reached end of file
                    raise StopIteration
            elif self.msg.get_type() == "BAD_DATA":
                    print "Got Bad data in Mavlink Message"
                    #if mavutil.all_printable(msg.data):
                    #    sys.stdout.write(msg.data)
                    #    sys.stdout.flush()
            elif self.msg.get_type() == "SERIAL_UDB_EXTRA_F2_A":
                    self.last_F2_A_msg = self.msg
                    continue
            elif self.msg.get_type() == "SERIAL_UDB_EXTRA_F2_B":
                    if self.msg.sue_time == self.last_F2_A_msg.sue_time : #A and B halves of message are a pair
                        return self.msg 
                    else:
                        pass
            elif  self.msg.get_type() == 'SERIAL_UDB_EXTRA_F4'  or \
                  self.msg.get_type() == 'SERIAL_UDB_EXTRA_F5'  or \
                  self.msg.get_type() == 'SERIAL_UDB_EXTRA_F6'  or \
                  self.msg.get_type() == 'SERIAL_UDB_EXTRA_F7'  or \
                  self.msg.get_type() == 'SERIAL_UDB_EXTRA_F8'  or \
                  self.msg.get_type() == 'SERIAL_UDB_EXTRA_F13' or \
                  self.msg.get_type() == 'SERIAL_UDB_EXTRA_F14' or \
                  self.msg.get_type() == 'SERIAL_UDB_EXTRA_F15' or \
                  self.msg.get_type() == 'SERIAL_UDB_EXTRA_F17':
                        return self.msg                
            else :
                    # print "Ignoring", self.msg.get_type()
                    pass
                    
    def parse(self,msg, record_no, max_tm_actual):
        log = mavlink_telemetry() # Make a new empty mavlink log entry
        log.log_format  = log.parse(self,record_no, max_tm_actual)
        return(log)
    def close(self) :
        pass
         
class ascii_telemetry_file:
    """ Get next line of ascii telemetry file"""
    def __init__(self, filename):
        self.f = open(filename, 'r')
        self.record_no = 0
        pass
    
    def __iter__(self):
        return(self)
     
    def next(self):
        while True :
            self.line = self.f.next()
            if self.line == "" :
                raise StopIteration
            self.record_no += 1
            if self.record_no == 1 :
                continue      # The first line of MatrixPilot telemetry line is blank.
            else :
                return(self.line)
            
    def parse(self,msg, record_no, max_tm_actual):
        log = ascii_telemetry() # Make a new empty sue (Serial Udb Extra) log entry
        log.log_format  = log.parse(msg,record_no, max_tm_actual)
        return(log)
     
    def close(self):
        self.f.close()
       

class base_telemetry :
    def __init__(self) :
        """Pattern match against a line of telemetry. max_tm_actual is the maximum
        actual time of week seen so far. It is required for processing a week rollover."""
        # GPS Weeks are measured since 05 January 1980 / morning of 06 January 1980 modulo 1024.
        # GPS week number rolled over to 0 on midnight GPS Time of the evening of
        # 21 August 1999 / morning of 22 August 1999
        # see http://tycho.usno.navy.mil/gps_week.html
        self.week_no = int(520)       # Defaults to a relatively recent year (Aug 2009)
        self.tm_actual = float (0)    # Actual reported time of week from gps
        self.tm = float (0)           # self.tm takes account of weekly rollover of GPS seconds
                                      # and is synthezied if multiple entries are identical.
        self.status =    "0"
        self.latitude =  float(0)
        self.longitude = float(0)
        self.altitude =  float(0)
        self.waypointIndex = int(0)
        self.rmat0 = int(0)
        self.rmat1 = int(0)
        self.rmat2 = int(0)
        self.rmat3 = int(0)
        self.rmat4 = int(0)
        self.rmat5 = int(0)
        self.rmat6 = int(0)
        self.rmat7 = int(0)
        self.rmat8 = int(0)
        self.sog = int (0)  # speed over ground 
        self.cog = int (0)  # course over ground
        self.roll = int (0)
        self.pitch = int (0)
        self.hdop  = int(0) 
        self.vdop  = int(0)
        self.svs   = int(0)
        self.cpu   = int(0)
        self.est_airspeed = int(0)
        self.est_wind_x = 0 # x estimated wind
        self.est_wind_y = 0 # x estimated wind
        self.est_wind_z = 0 # x estimated wind
        self.earth_mag_vec_E = 0
        self.earth_mag_vec_N = 0
        self.earth_mag_vec_Z = 0
        self.max_tm_actual = 0
        self.pwm_input =  [0,0,0,0,0,0,0,0,0,0,0]
        self.pwm_output = [0,0,0,0,0,0,0,0,0,0,0]
        self.lex = 0
        self.ley = 0
        self.lez = 0
        self.gps_week = 0
        self.IMUlocationx_W1 = 0
        self.IMUlocationy_W1 = 0
        self.IMUlocationz_W1 = 0
        self.lon = [0,0] # We came two versions. A GPS version and an IMU version.
        self.lat = [0,0]
        self.alt = [0,0]
        self.dead_reckoning = 0
        self.id_vehicle_model_name = "Not seen in telemetry"
        self.id_vehicle_registration = "Not seen in telemetry"
        self.id_lead_pilot = "Not seen in telemetry"
        self.id_diy_drones_url = "Not seen in telemetry"
        self.inline_waypoint_x = 0 #waypoint data reported inline in F2 format
        self.inline_waypoint_y = 0
        self.inline_waypoint_z = 0
        self.trap_flags = 0
        self.trap_source = 0
        self.alarms = 0
        self.clock_type = 0
        self.flight_plan_type = 0
        self.rollkd_rudder = 0
        self.rollkp_rudder = 0

class mavlink_telemetry(base_telemetry):
    """Parse a single binary mavlink message record"""
    def parse(self,telemetry_file,record_no, max_tm_actual) :
        if (telemetry_file.msg.get_type() == "SERIAL_UDB_EXTRA_F2_B"):
            self.tm_actual = float (telemetry_file.last_F2_A_msg.sue_time)
            if ((self.tm_actual < max_tm_actual) and ( max_tm_actual > 604780000 )):
                    # 604800000 is no. of milliseconds in a week. This extra precaution required because
                    # occausionally the log file can have an entry with  atime which precedes the previous entry.
                    # The following seconds rollover fix only works for flights of less than 1 week
                    # in length. So watch out when analyzing your global solar powered UAV flights.
                    print "Executing code for GPS weekly seconds rollover"
                    self.tm = self.tm_actual + max_tm_actual
            elif (self.tm_actual < max_tm_actual) :
                    self.tm = max_tm_actual
                    # takes account of occassional time entry which precedes time of previous entry.
                    # This can happen when EM406A first starts up near beginning of telemetry.
                    # It is caused by a synchronisation issue between GPS time, and synthesized time
                    # within MatrixPilot. It has been seen to occur once near startup time of the UDB.
            else :
                    self.tm = self.tm_actual
            
            self.status = bstr( telemetry_file.last_F2_A_msg.sue_status )
            self.latitude = float(telemetry_file.last_F2_A_msg.sue_latitude)
            self.longitude = float(telemetry_file.last_F2_A_msg.sue_longitude)
            self.altitude = float(telemetry_file.last_F2_A_msg.sue_altitude)
            self.waypointIndex = int(telemetry_file.last_F2_A_msg.sue_waypoint_index)
            self.rmat0 = int(telemetry_file.last_F2_A_msg.sue_rmat0)
            self.rmat1 = int(telemetry_file.last_F2_A_msg.sue_rmat1)
            self.rmat2 = int(telemetry_file.last_F2_A_msg.sue_rmat2)
            self.rmat3 = int(telemetry_file.last_F2_A_msg.sue_rmat3)
            self.rmat4 = int(telemetry_file.last_F2_A_msg.sue_rmat4)
            self.rmat5 = int(telemetry_file.last_F2_A_msg.sue_rmat5)
            self.rmat6 = int(telemetry_file.last_F2_A_msg.sue_rmat6)
            self.rmat7 = int(telemetry_file.last_F2_A_msg.sue_rmat7)
            self.rmat8 = int(telemetry_file.last_F2_A_msg.sue_rmat8)
            self.sog = int(telemetry_file.last_F2_A_msg.sue_cog)
            self.cog = int(telemetry_file.last_F2_A_msg.sue_sog)
            self.cpu = int(telemetry_file.last_F2_A_msg.sue_cpu_load)
            self.sue_voltage_milis = int(telemetry_file.last_F2_A_msg.sue_voltage_milis)
            self.est_airspeed = int(telemetry_file.last_F2_A_msg.sue_air_speed_3DIMU)
            self.est_wind_x = int(telemetry_file.last_F2_A_msg.sue_estimated_wind_0)
            self.est_wind_y = int(telemetry_file.last_F2_A_msg.sue_estimated_wind_1)
            self.est_wind_z = int(telemetry_file.last_F2_A_msg.sue_estimated_wind_2)
            self.earth_mag_vec_E = - int(telemetry_file.last_F2_A_msg.sue_magFieldEarth0)
            self.earth_mag_vec_N = int(telemetry_file.last_F2_A_msg.sue_magFieldEarth1)
            self.earth_mag_vec_Z = int(telemetry_file.last_F2_A_msg.sue_magFieldEarth2)  
            self.svs = int(telemetry_file.last_F2_A_msg.sue_svs)
            self.hdop = int(telemetry_file.last_F2_A_msg.sue_hdop)

            self.pwm_input[1] = int(telemetry_file.msg.sue_pwm_input_1)
            self.pwm_input[2] = int(telemetry_file.msg.sue_pwm_input_2)
            self.pwm_input[3] = int(telemetry_file.msg.sue_pwm_input_3)
            self.pwm_input[4] = int(telemetry_file.msg.sue_pwm_input_4)
            self.pwm_input[5] = int(telemetry_file.msg.sue_pwm_input_5)
            self.pwm_input[6] = int(telemetry_file.msg.sue_pwm_input_6)
            self.pwm_input[7] = int(telemetry_file.msg.sue_pwm_input_7)
            self.pwm_input[8] = int(telemetry_file.msg.sue_pwm_input_8)
            self.pwm_input[9] = int(telemetry_file.msg.sue_pwm_input_9)
            self.pwm_input[10] = int(telemetry_file.msg.sue_pwm_input_10)
            
            self.pwm_output[1] = int(telemetry_file.msg.sue_pwm_output_1)
            self.pwm_output[2] = int(telemetry_file.msg.sue_pwm_output_2)
            self.pwm_output[3] = int(telemetry_file.msg.sue_pwm_output_3)
            self.pwm_output[4] = int(telemetry_file.msg.sue_pwm_output_4)
            self.pwm_output[5] = int(telemetry_file.msg.sue_pwm_output_5)
            self.pwm_output[6] = int(telemetry_file.msg.sue_pwm_output_6)
            self.pwm_output[7] = int(telemetry_file.msg.sue_pwm_output_7)
            self.pwm_output[8] = int(telemetry_file.msg.sue_pwm_output_8)
            self.pwm_output[9] = int(telemetry_file.msg.sue_pwm_output_9)
            self.pwm_output[10] = int(telemetry_file.msg.sue_pwm_output_10)

            self.lex = int(telemetry_file.msg.sue_imu_location_x)
            self.ley = int(telemetry_file.msg.sue_imu_location_y)
            self.lez = int(telemetry_file.msg.sue_imu_location_z)
            self.sue_flags = int(telemetry_file.msg.sue_flags)

            self.sue_osc_fails = int(telemetry_file.msg.sue_osc_fails)
	    self.sue_imu_velocity_x = int(telemetry_file.msg.sue_imu_velocity_x)
	    self.sue_imu_velocity_y = int(telemetry_file.msg.sue_imu_velocity_y)
	    self.sue_imu_velocity_z = int(telemetry_file.msg.sue_imu_velocity_z)
	    self.inline_waypoint_x = int(telemetry_file.msg.sue_waypoint_goal_x)
	    self.inline_waypoint_y = int(telemetry_file.msg.sue_waypoint_goal_y)
	    self.inline_waypoint_z = int(telemetry_file.msg.sue_waypoint_goal_z)
	    self.sue_memory_stack_free = int(telemetry_file.msg.sue_memory_stack_free)

            return("F2")

        elif telemetry_file.msg.get_type() == 'SERIAL_UDB_EXTRA_F4' :
            self.roll_stabilization = int(telemetry_file.msg.sue_ROLL_STABILIZATION_AILERONS)
            #Following line probably needs adding to GE KML
            self.roll_stabilization_rudder = int(telemetry_file.msg.sue_ROLL_STABILIZATION_RUDDER)
            self.pitch_stabilization = int(telemetry_file.msg.sue_PITCH_STABILIZATION)
            self.yaw_stabilization_rudder = int(telemetry_file.msg.sue_YAW_STABILIZATION_RUDDER)
            self.yaw_stabilization_aileron = int(telemetry_file.msg.sue_YAW_STABILIZATION_AILERON)
            self.aileron_navigation = int(telemetry_file.msg.sue_AILERON_NAVIGATION)
            self.rudder_navigation = int(telemetry_file.msg.sue_RUDDER_NAVIGATION)
            self.use_altitudehold = int(telemetry_file.msg.sue_ALTITUDEHOLD_STABILIZED)
            #Following line probably needs adding to GE KML
            self.use_altitudehold_waypoint = int(telemetry_file.msg.sue_ALTITUDEHOLD_WAYPOINT)
            self.racing_mode = int(telemetry_file.msg.sue_RACING_MODE)

            return("F4")
        
        elif telemetry_file.msg.get_type() == 'SERIAL_UDB_EXTRA_F5' :
            self.yawkp_aileron = float(telemetry_file.msg.sue_YAWKP_AILERON)
            self.yawkd_aileron = float(telemetry_file.msg.sue_YAWKD_AILERON)
            self.rollkp = float(telemetry_file.msg.sue_ROLLKP)
            self.rollkd = float(telemetry_file.msg.sue_ROLLKD)
            self.aileron_boost = float(telemetry_file.msg.sue_AILERON_BOOST)

            return("F5")
        
        elif telemetry_file.msg.get_type() == 'SERIAL_UDB_EXTRA_F6' :
            self.pitchgain = float(telemetry_file.msg.sue_PITCHGAIN)
            self.pitchkd = float(telemetry_file.msg.sue_PITCHKD)
            self.rudder_elev_mix = float(telemetry_file.msg.sue_RUDDER_ELEV_MIX)
            self.roll_elev_mix = float(telemetry_file.msg.sue_ROLL_ELEV_MIX)
            self.elevator_boost = float(telemetry_file.msg.sue_ELEVATOR_BOOST)

            return("F6")
        
        elif telemetry_file.msg.get_type() == 'SERIAL_UDB_EXTRA_F7' :
            self.yawkp_rudder = float(telemetry_file.msg.sue_YAWKP_RUDDER)
            self.yawkd_rudder = float(telemetry_file.msg.sue_YAWKD_RUDDER)
            self.rollkp_rudder = float(telemetry_file.msg.sue_ROLLKP_RUDDER)
            self.rollkd_rudder = float(telemetry_file.msg.sue_ROLLKD_RUDDER)
            self.rudder_boost = float(telemetry_file.msg.sue_RUDDER_BOOST)
            self.rtl_pitch_down = float(telemetry_file.msg.sue_RTL_PITCH_DOWN)

            return("F7")
        
        elif telemetry_file.msg.get_type() == 'SERIAL_UDB_EXTRA_F8' :
            self.heightmax = float(telemetry_file.msg.sue_HEIGHT_TARGET_MAX)
            self.heightmin = float (telemetry_file.msg.sue_HEIGHT_TARGET_MIN)
            self.minimumthrottle = float(telemetry_file.msg.sue_ALT_HOLD_THROTTLE_MIN)
            self.maximumthrottle = float(telemetry_file.msg.sue_ALT_HOLD_THROTTLE_MAX)
            self.pitchatminthrottle = float(telemetry_file.msg.sue_ALT_HOLD_PITCH_MIN)
            self.pitchatmaxthrottle = float(telemetry_file.msg.sue_ALT_HOLD_PITCH_MAX)
            self.pitchatzerothrottle = float(telemetry_file.msg.sue_ALT_HOLD_PITCH_HIGH)

            return("F8")
            
        elif (telemetry_file.msg.get_type() == "SERIAL_UDB_EXTRA_F13"):
            self.gps_week = int(telemetry_file.msg.sue_week_no)
            self.origin_north = int(telemetry_file.msg.sue_lat_origin)
            self.origin_east = int(telemetry_file.msg.sue_lon_origin)
            self.origin_altitude = int(telemetry_file.msg.sue_alt_origin)
            
            return("F13")

        elif telemetry_file.msg.get_type() == 'SERIAL_UDB_EXTRA_F14' :
            self.wind_est = int(telemetry_file.msg.sue_WIND_ESTIMATION)
            self.gps_type = int(telemetry_file.msg.sue_GPS_TYPE)
            self.dead_reckoning = int(telemetry_file.msg.sue_DR)
            self.board_type = int(telemetry_file.msg.sue_BOARD_TYPE)
            self.airframe = int(telemetry_file.msg.sue_AIRFRAME)
            self.rcon = int(telemetry_file.msg.sue_RCON)
            self.trap_flags = int(telemetry_file.msg.sue_TRAP_FLAGS)
            self.trap_source = int(telemetry_file.msg.sue_TRAP_SOURCE)
            self.alarms = int(telemetry_file.msg.sue_osc_fail_count)
            self.clock_type = int(telemetry_file.msg.sue_CLOCK_CONFIG)
            self.flight_plan_type = int(telemetry_file.msg.sue_FLIGHT_PLAN_TYPE)

            return("F14")
        
        elif telemetry_file.msg.get_type() == 'SERIAL_UDB_EXTRA_F15' :
            self.id_vehicle_model_name = telemetry_file.msg.sue_ID_VEHICLE_MODEL_NAME
            self.id_vehicle_registration = telemetry_file.msg.sue_ID_VEHICLE_REGISTRATION

            return("F15")
        
        elif telemetry_file.msg.get_type() == 'SERIAL_UDB_EXTRA_F16' :
            self.id_lead_pilot = telemetry_file.msg.sue_ID_LEAD_PILOT
            self.id_diy_drones_url = telemetry_file.msg.sue_ID_DIY_DRONES_URL

            return("F16")

        else :
            return("Not Known")

class ascii_telemetry(base_telemetry):
    """Parse a variety of historical ascii telemetry formats"""
    def parse(self,line,line_no, max_tm_actual) :
        self.line_no = line_no

        # Discard lines that have non alpha numeric characters and so are corrupt
        match = re.search("[^a-zA-Z:\d\r\n\s\/=_,.-]", line)
        if match :
             print "Bad chars so discarding; [", match.group(0),"]",
             return "Error"
        # Get the Format Revision No.
        # This allows us to change revisions in the future, and
        # have this code still be able to cope with changes.
        match = re.match("^F1:",line) # If line starts with F1: then Revision 1
        if match :
            # Parse the line for revision 1 format
            
            match = re.match(".*:T(.*?):",line) # Time of Week
            if match :
                self.tm_actual = float (match.group(1))
                if self.tm_actual < max_tm_actual :
                    # The following rollover fix only works for flights of less than 1 week
                    # in length. So watch out when anlyzing your global solar powered UAV flights.
                    self.tm = self.tm_actual + max_tm_actual
                else :
                    self.tm = self.tm_actual
            else :
                print "Error: Failure parsing time of week at line", line_no
                return "Error"
                
            match = re.match(".*:S(.*?):",line) # Status Flags
            if match :
                self.status = match.group(1)
            else :
                print "Failure parsing status flags at line", line_no
                return "Error"
            
            match = re.match(".*:N(.*?):",line) # Lat North
            if match :
                self.latitude = float(match.group(1))
            else :
                print "Failure parsing Lat North at line", line_no
                return "Error"
            
            match = re.match(".*:E(.*?):",line) # Lon East
            if match :
                self.longitude = float (match.group(1))
            else :
                print "Failure parsing Lon East at line", line_no
                return "Error"
            
            match = re.match(".*:A(.*?):",line) # Altitude
            if match :
                self.altitude = float(match.group(1))
            else :
                print "Failure parsing Altitude at line", line_no
                return "Error"
            
            match = re.match(".*:W(.*?):",line) # Waypoint Index
            if match :
                self.waypointIndex = int(match.group(1))
            else :
                print "Failure parsing Wapoint Index at line", line_no
                return "Error"
            
            match = re.match(".*:a(.*?):",line) # rmat 0
            if match :
                self.rmat0 = int(match.group(1))
            else :
                print "Failure parsing rmat0 at line", line_no
                return "Error"
            
            match = re.match(".*:b(.*?):",line) # rmat 1
            if match :
                self.rmat1 = int(match.group(1))
            else :
                print "Failure parsing rmat1 at line", line_no
                return "Error"
            
            match = re.match(".*:c(.*?):",line) # rmat 2
            if match :
                self.rmat2 = int(match.group(1))
            else :
                print "Failure parsing rmat2 at line", line_no
                return "Error"
            
            match = re.match(".*:d(.*?):",line) # rmat 3
            
            if match :
                self.rmat3 = int(match.group(1))
            else :
                print "Failure parsing rmat3 at line", line_no
                return "Error"
            
            match = re.match(".*:e(.*?):",line) # rmat 4
            if match :
                self.rmat4 = int(match.group(1))
            else :
                print "Failure parsing rmat4 at line", line_no
                return "Error"
            
            match = re.match(".*:f(.*?):",line) # rmat 5
            if match :
                self.rmat5 = int(match.group(1))
            else :
                print "Failure parsing rmat5 at line", line_no
                return "Error"
            
            match = re.match(".*:g(.*?):",line) # rmat 6
            if match :
                self.rmat6 = int(match.group(1))
            else :
                print "Failure parsing rmat6 at line", line_no
                return "Error"
            
            match = re.match(".*:h(.*?):",line) # rmat 7
            if match :
                    self.rmat7 = int(match.group(1))
            else :
                print "Failure parsing rmat7 at line", line_no
                return "Error"
            
            match = re.match(".*:i(.*?):",line) # rmat 8
            if match :
                self.rmat8 = int(match.group(1))
            else :
                print "Failure parsing rmat8 at line", line_no
                return "Error"
            # Because of a mistake in sprintf in UDB code,
            # sog and cog have to be swapped over in Rev F1 of telemetry
            match = re.match(".*:c(.*?):",line) # Speed Over Ground
            if match :
                self.sog = int(match.group(1))
            else :
                print "Failure parsing Speed Over Ground at line", line_no
                return "Error"
            
            match = re.match(".*:s(.*?):",line) # Course Over Ground
            if match :
                self.cog = int(match.group(1))
            else :
                print "Failure parsing Course Over Ground at line", line_no
                return "Error"
            # line was parsed without Errors
            return "F1"

        #################################################################
        # Try Another format of telemetry
        # print "fist char is", ord(line[0]) # a debug line for determining HKGCS delimter value
        match = re.match("^[0-9]*:\303",line) # If line starts with a number, :, then F2, then HKGCS
        
        if match:
            return "HKGCS_BLANK_LINE"
        
        match = re.match("^\303",line) 
        if match:
            return "HKGCS_BLANK_LINE"

        match = re.match("^F\303",line)
        if match:
            return "HKGCS_BLANK_LINE"
        
        #### HK GCS values seemed to have changed, so we test for ascii 355 (\377) as well.
        match = re.match("^[0-9]*:\377",line) # If line starts with a number, :, then F2, then HKGCS
        
        if match:
            return "HKGCS_BLANK_LINE"
        
        match = re.match("^\377",line) 
        if match:
            return "HKGCS_BLANK_LINE"

        match = re.match("^F\377",line)
        if match:
            return "HKGCS_BLANK_LINE"
        
        match = re.match("^[0-9]*:(F2.*)",line) # If line starts with a number, :, then F2, then HKGCS
        if match:
            line = match.group(1) # Strip off the first time stamp from HKGCS format

            
        match = re.match("^F2:",line) # Normal Line of SERIAL_UDB or SERIAL_UDB_EXTRA telemetry
        if match :
            # Parse the line for revision 2 format
           
            match = re.match(".*:T([-0-9]*?):",line) # Time of Week
            if match :
                try:
                    self.tm_actual = float (match.group(1))
                except:
                    print "Cortupt T: value (GPS Time) at line", line_no
                    return "Error"
                if ((self.tm_actual < max_tm_actual) and ( max_tm_actual > 604780000 )):
                    # 604800000 is no. of milliseconds in a week. This extra precaution required because
                    # occausionally the log file can have an entry with  atime which precedes the previous entry.
                    # The following seconds rollover fix only works for flights of less than 1 week
                    # in length. So watch out when analyzing your global solar powered UAV flights.
                    print "Executing code for GPS weekly seconds rollover"
                    self.tm = self.tm_actual + max_tm_actual
                elif (self.tm_actual < max_tm_actual) :
                    self.tm = max_tm_actual
                    # takes account of occassional time entry which precedes time of previous entry.
                    # This can happen when EM406A first starts up near beginning of telemetry.
                    # It is caused by a synchronisation issue between GPS time, and synthesized time
                    # within MatrixPilot. It has been seen to occur once near startup time of the UDB.
                else :
                    self.tm = self.tm_actual
            else :
                print "Failure parsing time of week at line", line_no
                return "Error"
                
            match = re.match(".*:S([-0-9]*?):",line) # Status Flags
            if match :
                self.status = match.group(1)
            else :
                print "Failure parsing status flags at line", line_no
                return "Error"
            
            match = re.match(".*:N([-0-9]*?):",line) # Lat North
            if match :
                try:
                    self.latitude = float(match.group(1))
                except:
                    print "Corrupt :N value (latitude) at line", line_no
                    return "Error"
            else :
                print "Failure parsing Lat North at line", line_no
                return "Error"
            
            match = re.match(".*:E([-0-9]*?):",line) # Lon East
            if match :
                try:
                    self.longitude = float (match.group(1))
                except:
                    print "Corrupt :E value (longitude) at line", line_no
                    return "Error"
            else :
                print "Failure parsing Lon East at line", line_no
                return "Error"
            
            match = re.match(".*:A([-0-9]*?):",line) # Altitude
            if match :
                try:
                    self.altitude = float(match.group(1))
                except:
                    print "Corrupt :A value (Altitude) at line", line_no
                    return "Error"
            else :
                print "Failure parsing Altitude at line", line_no
                return "Error"
            
            match = re.match(".*:W([-0-9]*?):",line) # Waypoint Index
            if match :
                try:
                    self.waypointIndex = int(match.group(1))
                except:
                    print "Corrupt :W value (Waypoint Index) at line", line_no
                    return "Error"
            else :
                print "Failure parsing Wapoint Index at line", line_no
                return "Error"
            
            match = re.match(".*:a([-0-9]*?):",line) # rmat 0
            if match :
                try:
                    self.rmat0 = int(match.group(1))
                except:
                    print "Corrtup :a value (rmat0) at line", line_no
                    return ("Error")
            else :
                print "Failure parsing rmat0 at line", line_no
                return "Error"
            
            match = re.match(".*:b([-0-9]*?):",line) # rmat 1
            if match :
                try:
                    self.rmat1 = int(match.group(1))
                except:
                    print "Corrupt :b value (rmat1) at line", line_no
                    return "Error"
            else :
                print "Failure parsing rmat1 at line", line_no
                return "Error"
            #Note in this next line, have to be careful to parse the DCM
            # value for rmat2, not the course over the ground
            match = re.match(".*:b[-0-9]*?:c([-0-9]*?):",line) # rmat 2
            if match :
                try:
                    self.rmat2 = int(match.group(1))
                except:
                    print "Corrupt :c value (rmat2) at line", line_no
                    return "Error"
            else :
                print "Failure parsing rmat2 at line", line_no
                return "Error"
            
            match = re.match(".*:d([-0-9]*?):",line) # rmat 3
            
            if match :
                try:
                    self.rmat3 = int(match.group(1))
                except:
                    print "Corrupt :d value (rmat3) at line", line_no
                    return "Error"
            else :
                print "Failure parsing rmat3 at line", line_no
                return "Error"
            
            match = re.match(".*:e([-0-9]*?):",line) # rmat 4
            if match :
                try:
                    self.rmat4 = int(match.group(1))
                except:
                    print "Corrupt :e value (rmat4) at line", line_no
                    return "Error"
            else :
                print "Failure parsing rmat4 at line", line_no
                return "Error"
            
            match = re.match(".*:f([-0-9]*?):",line) # rmat 5
            if match :
                try:
                    self.rmat5 = int(match.group(1))
                except:
                    print "Corrupt :f value (rmat5) at line",line_no
                    return "Error"
            else :
                print "Failure parsing rmat5 at line", line_no
                return "Error"
            
            match = re.match(".*:g([-0-9]*?):",line) # rmat 6
            if match :
                try:
                    self.rmat6 = int(match.group(1))
                except:
                    print "Corrupt :g value (rmat6) at line", line_no
                    return "Error"
            else :
                print "Failure parsing rmat6 at line", line_no
                return "Error"
            
            match = re.match(".*:h([-0-9]*?):",line) # rmat 7
            if match :
                try:
                    self.rmat7 = int(match.group(1))
                except:
                    print "Corrupt :h value (rmat7) at line", line_no
                    return "Error"
            else :
                print "Failure parsing rmat7 at line", line_no
                return "Error"
            
            match = re.match(".*:i([-0-9]*?):",line) # rmat 8
            if match :
                try:
                    self.rmat8 = int(match.group(1))
                except:
                    print "Corrupt :i value (rmat8) at line", line_no
                    return "Error"
            else :
                print "Failure parsing rmat8 at line", line_no
                return "Error"
            # Because of a mistake in sprintf in UDB code,
            # sog and cog have to be swapped over in Rev F1 of telemetry
            match = re.match(".*:s([-0-9]*?):",line) # Speed Over Ground
            if match :
                try:
                    self.sog = int(match.group(1))
                except:
                    print "Corrupt :s value (Speed over ground) at line ",line_no
                    return "Error"
            else :
                print "Failure parsing Speed Over Ground at line", line_no
                return "Error"
            match = re.match(".*:i[-0-9]*?:c([-0-9]*?):",line) # Course Over Ground
            if match :
                try:
                    self.cog = int(match.group(1))
                except:
                    print "Corrupt :c (Course over Ground) value at line", line_no
                    return "Error"
            else :
                print "Failure parsing Course Over Ground at line", line_no
                return "Error"
            match = re.match(".*:cpu([-0-9]*?):",line) # CPU Usage
            if match :
                try:
                    self.cpu = int(match.group(1))
                except:
                    print "Corrupt :cpu (CPU loading percentage) value at ", line_no
                    return "Error"
            else :
                print "Failure parsing CPU Usage at line", line_no
                # not a major error of it is not there.
    
            match = re.match(".*:vd([-0-9]*?):",line) # Vertical Dilution of Precision
            if match :
                try:
                    self.vdop = int(match.group(1))
                except:
                    print "Corrupt :vd (vertical dilution of precision) value  at line",line_no
                    pass
            else :
                pass  # Not a serious error and a late addition to F2

            match = re.match(".*:hd([-0-9]*?):",line) # Horizontal Dilution of Precision
            if match :
                try:
                    self.hdop = int(match.group(1))
                except:
                    print "Corrupt hdop value at line", line_no
                    pass
            else :
                pass # not a serious error

            match = re.match(".*:svs([-0-9]*?):",line) # Sattelites in view
            if match :
                try:
                    self.svs = int(match.group(1))
                except:
                    print "Corrupt svs value (sattelites in view) at line", line_no
                    return "Error"
            else :
                pass # Not a serious error

            match = re.match(".*:as([-0-9]*?):",line) # Estimated Air Speed
            if match :
                try:
                    self.est_airspeed = int(match.group(1))
                except:
                    print "Corrupt :as value (estimated air speed) at line", line_no
                    pass
            else :
                pass # Not a serious error

            match = re.match(".*:wvx([-0-9]*?):",line) # Estimated Wind - x
            if match :
                try:
                    self.est_wind_x = int(match.group(1))
                except:
                    print "Corrupt :wvx value (estimated Wind in X) at line", line_no
                    return "Error"
            else :
                pass # Not a serious error

            match = re.match(".*:wvy([-0-9]*?):",line) # Estimated Wind - y
            if match :
                try:
                    self.est_wind_y = int(match.group(1))
                except:
                    print "Corrupt :wvy value (estimated Wind in Y) at line", line_no
                    return "Error"
            else :
                pass # Not a serious error

            match = re.match(".*:wvz([-0-9]*?):",line) # Estimated Wind - z
            if match :
                try:
                    self.est_wind_z = int(match.group(1))
                except:
                    print "Corrupt :wvz (Estimated Wind in Z) at line", line_no
                    return "Error"
            else :
                pass # Not a serious error

            match = re.match(".*:ma([-0-9]*?):",line) # Earth Ref Magnetic Vector East
            if match :
                try:
                    # The mag vector is actually measured west in UDB, so we negate to make it East
                    self.earth_mag_vec_E = - int(match.group(1))
                except:
                    print "Corrupt :ma (Earth magnetic vector) at line", line_no
                    return "Error"
            else :
                pass # Not a serious error
           
            match = re.match(".*:mb([-0-9]*?):",line) # Earth Ref Magnetic Vector North
            if match :
                try:
                    self.earth_mag_vec_N = int(match.group(1))
                except:
                    print "Corrupt :mb (Earth magnetic vector) at line", line_no
                    return "Error"
            else :
                pass # Not a serious error

            match = re.match(".*:mc([-0-9]*?):",line) # Earth Ref Magnetic Vector up, Z
            if match :
                try:
                    self.earth_mag_vec_Z = int(match.group(1))
                except:
                    print "Corrupt :mc (Earth magnetic vector) at line", line_no
                    return "Error"
            else :
                pass # Not a serious error

            match = re.match(".*:p1i([-0-9]*?):",line) # PWM input 1 to UDB
            if match :
                try:
                    self.pwm_input[1] = int(match.group(1))
                except:
                    print "Corrupt value for pwm_input[1] in line", line_no
                    pass
            else :
                pass # Not a serious error

            match = re.match(".*:p2i([-0-9]*?):",line) # PWM input 2 to UDB
            if match :
                try:
                    self.pwm_input[2] = int(match.group(1))
                except:
                    print "Corrupt value for pwm_input[2] in line", line_no
                    pass
            else :
                pass # Not a serious error

            match = re.match(".*:p3i([-0-9]*?):",line) # PWM input 3 to UDB
            if match :
                try:
                    self.pwm_input[3] = int(match.group(1))
                except:
                    print "Corrupt value for pwm_input[3] in line", line_no
                    pass
            else :
                pass # Not a serious error

            match = re.match(".*:p4i([-0-9]*?):",line) # PWM input 4 to UDB
            if match :
                try:
                    self.pwm_input[4] = int(match.group(1))
                except:
                    print "Corrupt value for pwm_input[4] in line", line_no
                    pass
            else :
                pass # Not a serious error

            match = re.match(".*:p5i([-0-9]*?):",line) # PWM input 5 to UDB
            if match :
                try:
                    self.pwm_input[5] = int(match.group(1))
                except:
                    print "Corrupt value for pwm_input[5] in line", line_no
                    pass
            else :
                pass # Not a serious error

            match = re.match(".*:p6i([-0-9]*?):",line) # PWM input 6 to UDB
            if match :
                try:
                    self.pwm_input[6] = int(match.group(1))
                except:
                    print "Corrupt value for pwm_input[6] in line", line_no
                    pass
            else :
                pass # Not a serious error

            match = re.match(".*:p7i([-0-9]*?):",line) # PWM input 7 to UDB
            if match :
                try:
                    self.pwm_input[7] = int(match.group(1))
                except:
                    print "Corrupt value for pwm_input[7] in line", line_no
                    pass
                
            else :
                pass # Not a serious error

            match = re.match(".*:p8i([-0-9]*?):",line) # PWM input 8 to UDB
            if match :
                try:
                    self.pwm_input[8] = int(match.group(1))
                except:
                    print "Corrupt value for pwm_input[8] in line", line_no
                    pass
            else :
                pass # Not a serious error

            match = re.match(".*:p1o([-0-9]*?):",line) # PWM Output 1 to UDB
            if match :
                try:
                    self.pwm_output[1] = int(match.group(1))
                except:
                    print "Corrupt value for pwm_output[1] in line", line_no
                    pass
            else :
                pass # Not a serious error

            match = re.match(".*:p2o([-0-9]*?):",line) # PWM output 2 to UDB
            if match :
                try:
                    self.pwm_output[2] = int(match.group(1))
                except:
                    print "Corrupt value for pwm_output[2] in line", line_no
                    pass
            else :
                pass # Not a serious error

            match = re.match(".*:p3o([-0-9]*?):",line) # PWM output 3 to UDB
            if match :
                try:
                    self.pwm_output[3] = int(match.group(1))
                except:
                    print "Corrupt value for pwm_output[3] in line", line_no
                    pass
            else :
                pass # Not a serious errors

            match = re.match(".*:p4o([-0-9]*?):",line) # PWM output 4 to UDB
            if match :
                try:
                    self.pwm_output[4] = int(match.group(1))
                except:
                    print "Corrupt value for pwm_output[4] in line", line_no
                    pass
            else :
                pass # Not a serious error

            match = re.match(".*:p5o([-0-9]*?):",line) # PWM output 5 to UDB
            if match :
                try:
                    self.pwm_output[5] = int(match.group(1))
                except:
                    print "Corrupt value for pwm_output[5] in line", line_no
                    pass 
            else :
                pass # Not a serious error

            match = re.match(".*:p6o([-0-9]*?):",line) # PWM output 6 to UDB
            if match :
                try:
                    self.pwm_output[6] = int(match.group(1))
                except:
                    print "Corrupt value for pwm_output[6] in line", line_no
                    pass
            else :
                pass # Not a serious error

            match = re.match(".*:p7o([-0-9]*?):",line) # PWM output 7 to UDB
            if match :
                try:
                    self.pwm_output[7] = int(match.group(1))
                except:
                    print "Corrupt value for pwm_output[7] in line", line_no
                    pass
            else :
                pass # Not a serious error

            match = re.match(".*:p8o([-0-9]*?):",line) # PWM output 8 to UDB
            if match :
                try:
                    self.pwm_output[8] = int(match.group(1))
                except:
                    print "Corrupt value for pwm_output[8] in line", line_no
                    pass
            else :
                pass # Not a serious error
            match = re.match(".*:lex([-0-9]*?):",line) # lex, delta X of Dead Reckoning
            if match :
                try:
                    self.lex = int(match.group(1))
                except:
                    print "Corrupt :lev value in line", line_no
                    pass
            else :
                pass # Not a serious error
            match = re.match(".*:ley([-0-9]*?):",line) # ley, delta Y of Dead Reckoning
            if match :
                try:
                    self.ley = int(match.group(1))
                except:
                    print "Corrtup :ley value in line", line_no
                    pass
            else :
                pass # Not a serious error
            match = re.match(".*:lez([-0-9]*?):",line) # lex, delta Z of Dead Reckoning
            if match :
                try:
                    self.lez = int(match.group(1))
                except:
                    print "Corrupt :lez value in line", line_no
            else :
                pass # Not a serious error
            match = re.match(".*:imx([-0-9]*?):",line) # IMUlocation x. Meters from origin.
            if match :
                try:
                    self.IMUlocationx_W1 = int(match.group(1))
                except:
                    print "Corrupt IMULocationX value in line", line_no
                    return "Error"
            else :
                pass  
            match = re.match(".*:imy([-0-9]*?):",line) # IMUlocation y. Meters from origin
            if match :
                try:
                    self.IMUlocationy_W1 = int(match.group(1))
                except:
                    print "Corrupt IMULocaitonY value in line", line_no
                    return "Error"
            else :
                pass
            match = re.match(".*:imz([-0-9]*?):",line) # IMUlocation z. Meters from origin
            if match :
                try:
                    self.IMUlocationz_W1 = int(match.group(1))
                except:
                    print "Corrupt IMUlocationZ value in line", line_no
                    return "Error"
            else :
                pass

            match = re.match(".*:G([-0-9]*?),([-0-9]*?),([-0-9]*?):",line) # Next waypoint X,Y,Z in meters from origin
            if match :
                try:
                    self.inline_waypoint_x = int(match.group(1))
                    self.inline_waypoint_y = int(match.group(2))
                    self.inline_waypoint_z = int(match.group(3))
                except:
                    print "Corrupt F2: waypoint value in line", line_no
                    pass
            
             # line was parsed without major errors
            return "F2"

        #################################################################
        # Try Another format of telemetry
        
        match = re.match("^F4:",line) # If line starts with F4
        if match :
            # Parse the line for options.h values
            
            match = re.match(".*:R_STAB=(.*?):",line) # ROLL_STABILIZATION
            if match :
                self.roll_stabilization = int (match.group(1))
            else :
                #print "Failure parsing ROLL_STABILIZATION at line", line_no
                # Allow this not to be an error
                pass
                
            match = re.match(".*:P_STAB=(.*?):",line) # PITCH_STABILIZATION
            if match :
                self.pitch_stabilization = int (match.group(1))
            else :
                print "Failure parsing PITCH_STABILIZATION at line", line_no
                return "Error"
            
            match = re.match(".*:Y_STAB_R=(.*?):",line) # YAW_STABILIZATION_RUDDER
            if match :
                self.yaw_stabilization_rudder = int(match.group(1))
            else :
                print "Failure parsing YAW_STABILIZATION_RUDDER at line", line_no
                return "Error"
            
            match = re.match(".*:Y_STAB_A=(.*?):",line) # YAW_STABILIZATION_AILERON
            if match :
                self.yaw_stabilization_aileron = int (match.group(1))
            else :
                print "Failure parsing YAW_STABILIZATION_AILERON at line", line_no
                return "Error"
            
            match = re.match(".*:AIL_NAV=(.*?):",line) # AILERON_NAVIGATION
            if match :
                self.aileron_navigation = int(match.group(1))
            else :
                print "Failure parsing AILERON_NAVIGATION at line", line_no
                return "Error"

            match = re.match(".*:RUD_NAV=(.*?):",line) # RUDDER_NAVIGATION
            if match :
                self.rudder_navigation = int(match.group(1))
            else :
                print "Failure parsing RUDDER_NAVIGATION at line", line_no
                return "Error"
            
            match = re.match(".*:AH_STAB=(.*?):",line) # USE_ALTITUDEHOLD
            if match :
                self.use_altitudehold = int(match.group(1))
            else :
                print "Failure parsing USE_ALTITUDEHOLD at line", line_no
                return "Error"
            
            match = re.match(".*:RACE=(.*?):",line) # RACING_MODE
            if match :
                self.racing_mode = int(match.group(1))
            else :
                print "Failure parsing RACING_MODE at line", line_no
                return "Error"
    
            # line was parsed without Errors
            return "F4"


        #################################################################
        # Try Another format of telemetry
        
        match = re.match("^F5:",line) # If line starts with F5
        if match :
            # Parse the line for options.h values
            
            match = re.match(".*:YAWKP_A=(.*?):",line) # YAWKP_AILERON
            if match :
                self.yawkp_aileron = float (match.group(1))
            else :
                print "Failure parsing YAWKP_AILERON at line", line_no
                return "Error"
                
            match = re.match(".*:YAWKD_A=(.*?):",line) # YAWKD_AILERON
            if match :
                self.yawkd_aileron = float (match.group(1))
            else :
                print "Failure parsing YAWKD_AILERON at line", line_no
                return "Error"
            
            match = re.match(".*:ROLLKP=(.*?):",line) # ROLLKP
            if match :
                self.rollkp = float(match.group(1))
            else :
                print "Failure parsing ROLLKP at line", line_no
                return "Error"
            
            match = re.match(".*:ROLLKD=(.*?):",line) # ROLLKD
            if match :
                self.rollkd = float (match.group(1))
            else :
                print "Failure parsing ROLLKD at line", line_no
                return "Error"
            
            match = re.match(".*:A_BOOST=(.*?):",line) # AILERON_BOOST
            if match :
                self.aileron_boost = float(match.group(1))
            else :
                print "Failure parsing AILERON_BOOST at line", line_no
                return "Error"
    
            # line was parsed without Errors
            return "F5"

        #################################################################
        # Try Another format of telemetry
        
        match = re.match("^F6:",line) # If line starts with F6
        if match :
            # Parse the line for options.h values
          
            match = re.match(".*:P_GAIN=(.*?):",line) # PITCHGAIN
            if match :
                self.pitchgain = float (match.group(1))
            else :
                print "Failure parsing PITCHGAIN at line", line_no
                return "Error" 
            match = re.match(".*:P_KD=(.*?):",line) # PITCHKD
            if match :
                self.pitchkd = float (match.group(1))
            else :
                print "Failure parsing PITCHKD at line", line_no
                return "Error"
            match = re.match(".*:RUD_E_MIX=(.*?):",line) # RUDDER_ELEV_MIX
            if match :
                self.rudder_elev_mix = float(match.group(1))
            else :
                print "Failure parsing RUDDER_ELEV_MIX at line", line_no
                return "Error"
            
            match = re.match(".*:ROL_E_MIX=(.*?):",line) # ROLL_ELEV_MIX
            if match :
                self.roll_elev_mix = float (match.group(1))
            else :
                print "Failure parsing ROLL_ELEV_MIX at line", line_no
                return "Error"
            
            match = re.match(".*:E_BOOST=(.*?):",line) # ELEVATOR_BOOST
            if match :
                self.elevator_boost = float(match.group(1))
            else :
                print "Failure parsing ELEVATOR_BOOST at line", line_no
                return "Error"
    
            # line was parsed without Errors
            return "F6"

        #################################################################
        # Try Another format of telemetry
        
        match = re.match("^F7:",line) # If line starts with F7
        if match :
            # Parse the line for options.h values
            
            match = re.match(".*:Y_KP_R=(.*?):",line) # YAWKP_RUDDER
            if match :
                self.yawkp_rudder = float (match.group(1))
            else :
                print "Failure parsing YAW_KP_RUDDER at line", line_no
                return "Error"  
            match = re.match(".*:Y_KD_R=(.*?):",line) # YAWKD_RUDDER
            if match :
                self.yawkd_rudder = float (match.group(1))
            else :
                print "Failure parsing YAWKD_RUDDER at line", line_no
                return "Error"

            match = re.match(".*:RLKP_RUD=(.*?):",line) # ROLLKP_RUDDER *** ! 
            if match :
                self.rollkp_rudder = float (match.group(1))
            else :
                print "Failure parsing RLKP_RUD at line", line_no
                pass  # May not be in some telemetry

            match = re.match(".*:RLKD_RUD=(.*?):",line) # ROLLKD_RUDDER *** ! 
            if match :
                self.rollkd_rudder = float (match.group(1))
            else :
                print "Failure parsing RLKD_RUD at line", line_no
                pass # May not be in some telemetry
            
            match = re.match(".*:RUD_BOOST=(.*?):",line) # RUDDER_BOOST
            if match :
                self.rudder_boost = float(match.group(1))
            else :
                print "Failure parsing RUDDER_BOOST at line", line_no
                return "Error"
            
            match = re.match(".*:RTL_PITCH_DN=(.*?):",line) # RTL_PITCH_DOWN 
            if match :
                self.rtl_pitch_down = float (match.group(1))
            else :
                print "Failure parsing RTL_PITCH_DOWN at line", line_no
                return "Error"
            # line was parsed without Errors
            return "F7"

        #################################################################
        # Try Another format of telemetry
        
        match = re.match("^F8:",line) # If line starts with F8
        if match :
            # Parse the line for options.h values
           
            match = re.match(".*:H_MAX=(.*?):",line) # HEIGHTMAX
            if match :
                self.heightmax = float (match.group(1))
            else :
                print "Failure parsing HEIGHTMAX at line", line_no
                return "Error"  
            match = re.match(".*:H_MIN=(.*?):",line) # HEIGHTMIN
            if match :
                self.heightmin = float (match.group(1))
            else :
                print "Failure parsing HEIGHTMIN at line", line_no
                return "Error"
            
            match = re.match(".*:MIN_THR=(.*?):",line) # MINIMUMTHROTTLE
            if match :
                self.minimumthrottle = float(match.group(1))
            else :
                print "Failure parsing MINIMUMTHROTTLE at line", line_no
                return "Error"
            
            match = re.match(".*:MAX_THR=(.*?):",line) # MAXIMUMTHROTTLE 
            if match :
                self.maximumthrottle = float (match.group(1))
            else :
                print "Failure parsing MAXIMUMTHROTTLE at line", line_no
                return "Error"
            match = re.match(".*:PITCH_MIN_THR=(.*?):",line) # PITCHATMINTHROTTLE
            if match :
                self.pitchatminthrottle = float (match.group(1))
            else :
                print "Failure parsing PITCHATMINTHROTTLE at line", line_no
                return "Error"
            
            match = re.match(".*:PITCH_MAX_THR=(.*?):",line) # PITCHATMAXTHROTTLE
            if match :
                self.pitchatmaxthrottle = float (match.group(1))
            else :
                print "Failure parsing PITCHATMAXTHROTTLE at line", line_no
                return "Error"
            
            match = re.match(".*:PITCH_ZERO_THR=(.*?):",line) # PITCHATZEROTHROTTLE
            if match :
                self.pitchatzerothrottle = float (match.group(1))
            else :
                print "Failure parsing PITCHATZEROTHROTTLE at line", line_no
                return "Error"


            
            # line was parsed without Errors
            return "F8"

        #################################################################
        # Try Another format of telemetry
        
        match = re.match("^F11:",line) # If line starts with F11
        if match :
            # Parse the line for options.h values
           
            match = re.match(".*:WIND_EST=(.*?):",line) # WIND ESTIMATION ENABLED ?
            if match :
                self.wind_est = int (match.group(1))
            else :
                print "Failure parsing Wind Estimation (enabled / disabled) at line", line_no
            match = re.match(".*:GPS_TYPE=(.*?):",line) # GPS TYPE
            if match :
                self.gps_type = int (match.group(1))
            else :
                print "Failure parsing GPS TYPE at line", line_no
            match = re.match(".*:DR=(.*?):",line) # DEAD RECKONING
            if match :
                self.dead_reckoning = int (match.group(1))
            else :
                print "Failure parsing DEAD RECKONING at line", line_no
            match = re.match(".*:BOARD_TYPE=(.*?):",line) # BOARD TYPE
            if match :
                self.board_type = int(match.group(1))
            else :
                print "Failure parsing BOARD TYPE at line", line_no
            
            match = re.match(".*:AIRFRAME=(.*?):",line) # AIRFRAME TYPE 
            if match :
                self.airframe = int (match.group(1))
            else :
                print "Failure parsing AIRFRAME at line", line_no
            match = re.match(".*:RCON=(.*?):",line) # RCON (Reason for rebooting)
            if match :
                self.rcon = int (match.group(1))
            else :
                print "Failure parsing RCON at line", line_no

            
            # line was parsed without Errors
            return "F11"
        
        

        #################################################################
        # Try Another format of telemetry
        
        match = re.match("^F13:",line) # If line starts with F13
        if match :
            # Parse the line for options.h values
          
            match = re.match(".*:week(.*?):",line) # GPS Week Number
            if match :
                self.gps_week = int (match.group(1))
            else :
                print "Failure parsing GPS Week at line", line_no
                return "Error"  
            match = re.match(".*:origN(.*?):",line) # ORIGIN NORTH
            if match :
                self.origin_north = int (match.group(1))
            else :
                print "Failure parsing Origin North at line", line_no
                return "Error"
            
            match = re.match(".*:origE(.*?):",line) # ORIGIN EAST
            if match :
                self.origin_east = int(match.group(1))
            else :
                print "Failure parsing Origin East at line", line_no
                return "Error"
            
            match = re.match(".*:origA(.*?):",line) # ORIGIN ALTITUDE 
            if match :
                self.origin_altitude = int (match.group(1))
            else :
                print "Failure parsing Origin Altitude at line", line_no
                return "Error"

            
            # line was parsed without Errors
            return "F13"

        #################################################################
        # Try Another format of telemetry
        
        match = re.match("^F14:",line) # If line starts with F14
        if match :
            # Parse the line for options.h values
           
            match = re.match(".*:WIND_EST=(.*?):",line) # WIND ESTIMATION ENABLED ?
            if match :
                self.wind_est = int (match.group(1))
            else :
                print "Failure parsing Wind Estimation (enabled / disabled) at line", line_no
            match = re.match(".*:GPS_TYPE=(.*?):",line) # GPS TYPE
            if match :
                self.gps_type = int (match.group(1))
            else :
                print "Failure parsing GPS TYPE at line", line_no
                
            match = re.match(".*:DR=(.*?):",line) # DEAD RECKONING
            if match :
                self.dead_reckoning = int (match.group(1))
            else :
                print "Failure parsing DEAD RECKONING at line", line_no
            match = re.match(".*:BOARD_TYPE=(.*?):",line) # BOARD TYPE
            if match :
                self.board_type = int(match.group(1))
            else :
                print "Failure parsing BOARD TYPE at line", line_no
            
            match = re.match(".*:AIRFRAME=(.*?):",line) # AIRFRAME TYPE 
            if match :
                self.airframe = int (match.group(1))
            else :
                print "Failure parsing AIRFRAME at line", line_no
            match = re.match(".*:RCON=0x(.*?):",line) # RCON (Reason for rebooting)
            if match :
                self.rcon = int (match.group(1), 16)
            else :
                print "Failure parsing RCON at line", line_no
            match = re.match(".*:TRAP_FLAGS=0x(.*?):",line) # Trap Flags
            if match :
                self.trap_flags = int (match.group(1), 16)
            else :
                print "Failure parsing Trap Flags at line", line_no
            match = re.match(".*:TRAP_SOURCE=0x(.*?):",line) # Trap Source
            if match :
                self.trap_source = int (match.group(1), 16)
            else :
                print "Failure parsing Trap Source at line", line_no
            
            match = re.match(".*:ALARMS=(.*?):",line) # ALARMS
            if match :
                self.alarms = int (match.group(1))
            else :
                print "Failure parsing ALARMS at line", line_no
            match = re.match(".*:CLOCK=(.*?):",line) # CLOCK
            if match :
                self.clock_type = int (match.group(1))
            else :
                print "Failure parsing CLOCK (type) at line", line_no
            match = re.match(".*:FP=(.*?):",line) # FP (Flight Plan Type)
            if match :
                self.flight_plan_type = int (match.group(1))
            else :
                ### There was an error in the telemetry format for some months
                # So we try the error format alternative
                match = re.match(".*:FP=(.*?)$",line) # FP (Flight Plan Type)
                if match :
                    self.flight_plan_type = int (match.group(1))
                else :
                    print "Failure parsing FP (flight plan type) at line", line_no
                

            
            # line was parsed 
            return "F14"


        #################################################################
        # Try Another format of telemetry
        
        match = re.match("^F15:",line) # If line starts with F15
        if match :
            # Parse the line for options.h values
           
            match = re.match(".*:IDA=(.*?):",line) # ID_VEHICLE_MODEL_NAME
            if match :
                self.id_vehicle_model_name =  match.group(1)
            else :
                print "Failure parsing ID_VEHICLE_MODEL_NAME at line", line_no
            match = re.match(".*:IDB=(.*?):",line) # ID_VEHICLE_REGISTRATION
            if match :
                self.id_vehicle_registration = match.group(1)
            else :
                print "Failure parsing ID_VEHICLE_REGISTRATION at line", line_no
            return "F15"

        #################################################################
        # Try Another format of telemetry
        
        match = re.match("^F16:",line) # If line starts with F16
        if match :
            # Parse the line for options.h values
           
            match = re.match(".*:IDC=(.*?):",line) # ID_LEAD_PILOT
            if match :
                self.id_lead_pilot =  match.group(1)
            else :
                print "Failure parsing ID_LEAD_PILOT at line", line_no
            match = re.match(".*:IDD=(.*?:.*?):",line) # ID_DIY_DRONES_URL
            if match :
                self.id_diy_drones_url = match.group(1)
            else :
                print "Failure parsing ID_DIY_DRONES_URL at line", line_no
            return "F16"
 
        #################################################################
        # Try Another format of telemetry
        
        
        match = re.match("^<tm>",line)
        if  match :
            # Parse early PDH file format - PXML
            
            # Deal with ends of lines that lost characters.
            # remove everthing from <f> to the end of the line
            line = re.sub("<f>.*$","", line)
            match = re.match(".*<tm>(.*?)</tm>",line) # Course Over Ground
            if match :
                self.tm = float(match.group(1))
            else :
                print "Failure parsing Time of Week at line", line_no
                return "Error"
        
            match = re.match(".*<st>(.*?)</st>",line) # Status Flags
            if match :
                self.status = match.group(1)
            else :
                print "Failure parsing status flags at line", line_no
                return "Error"
            
            match = re.match(".*<lat>(.*?)</lat>",line) # Lat North
            if match :
                self.latitude = float(match.group(1))
            else :
                print "Failure parsing Lat North at line", line_no
                return "Error"
            
            match = re.match(".*<lon>(.*?)</lon>",line) # Lon East
            if match :
                self.longitude = float (match.group(1))
            else :
                print "Failure parsing Lon East at line", line_no
                return "Error"
            
            match = re.match(".*<alt>(.*?)</alt>",line) # Altitude
            if match :
                self.altitude = float(match.group(1))
            else :
                print "Failure parsing Altitude at line", line_no
                return "Error"
            
            match = re.match(".*<wp>(.*?)</wp>",line) # Waypoint Index
            if match :
                self.waypointIndex = int(match.group(1))
            else :
                print "Failure parsing Wapoint Index at line", line_no
                return "Error"
            
            match = re.match(".*<r1>(.*?)</r1>",line) # rmat 0
            if match :
                self.rmat1 = int(match.group(1))
            else :
                print "Failure parsing rmat1 at line", line_no
                return "Error"
            
            match = re.match(".*<r4>(.*?)</r4>",line) # rmat 1
            if match :
                self.rmat4 = int(match.group(1))
            else :
                print "Failure parsing rmat4 at line", line_no
                return "Error"
            
            # line was parsed without Errors (At this stage not
            #  so many variables were sent down the link
            
            return "<tm>"
            
        
        #################################################################
        # Try another format of telemetry
        #
        # Ardustation Positional Info
        # Sample Telemetry enclosed for future reference
        # from John C. at DiyDrones.com
        # +++ASP:-1,THH:1,RLL:-60,PCH:-9,***
        # +++ASP:0,THH:0,RLL:-60,PCH:-10,***
        #!!!LAT:32259464,LON:-110823520,SPD:13.58,CRT:0.00,ALT:81,ALH:50,CRS:70.13,BER:330,WPN:0,DST:176,BTV:0.00,RSP:-35,***
        #
        # official protocol defined at:-
        #http://diydrones.com/profiles/blogs/ardupilot-telemetry-protocol
        #

        match = re.match("^!!!",line) # If line starts with !!! then Ardustation 
        if match :
            # Parse the line for ardustation format
            
            
            #match = re.match(".*:S(.*?):",line) # Status Flags
            #if match :
            #    self.status = match.group(1)
            #else :
            #    print "Failure parsing status flags at line", line_no
            #    return "Error"
            # JOHNC ADDITIONS
            match = re.match(".*TOW:(.*?),",line) # Time of week
            if match :
                self.tm = float (match.group(1)) # Compatibility
            else :
                pass # If TOW is not there, it is not a major error for now
                # Most Ardupilots do not report TOW of week in telemetry
                # print "Failure parsing Time of Week at line", line_no
                # return "Error"
            
            match = re.match(".*LAT:(.*?),",line) # Latitude
            if match :
                self.latitude = float (match.group(1)) * 10 # Compatibility
            else :
                print "Failure parsing Lat North at line", line_no
                return "Error"
            
            match = re.match(".*LON:(.*?),",line) # Lon East
            if match :
                self.longitude = float (match.group(1)) * 10 # Compatiability
            else :
                print "Failure parsing Lon East at line", line_no
                return "Error"
            
            match = re.match(".*ALT:(.*?),",line) # Altitude
            if match :
                self.altitude = ((float(match.group(1))) * 100.0) # Compatibility with earlier code
            else :
                print "Failure parsing Altitude at line", line_no
                return "Error"
            
            match = re.match(".*WPN:(.*?),",line) # Waypoint Index
            if match :
                self.waypointIndex = int(match.group(1))
            else :
                print "Failure parsing Waypoint Index at line", line_no
                return "Error"
               
            match = re.match(".*SPD:(.*?),",line) # Speed Over Ground
            if match :
                self.sog = (float(match.group(1))) * 100.0 # Backwards Compatibility
            else :
                print "Failure parsing Speed Over Ground at line", line_no
                return "Error"
            
            match = re.match(".*CRS:(.*?),",line) # Course Over Ground
            if match :
                self.cog = (float(match.group(1))) * 100.0 
            else :
                print "Failure parsing Course Over Ground at line", line_no
                return "Error"
            
            # line was parsed without Errors
            return "ARDUSTATION!!!"
        
        match = re.match("^\+\+\+",line) # If line starts with !!! then Ardustation 
        if match :
            # Parse the line for ardustation format
            
            match = re.match(".*RLL:(.*?),",line) # ROLL
            if match :
                self.roll = - int (match.group(1)) 
            else :
                print "Failure parsing Ardustation Roll at ", line_no
                return "Error"
            
            match = re.match(".*PCH:(.*?),",line) # Pitch
            if match :
                self.pitch = - float (match.group(1))
            else :
                print "Failure parsing Ardustation Pitch at", line_no
                return "Error"
            
            match = re.match(".*THH:(.*?),",line) # Throttle
            if match :
                self.throttle = (int (match.group(1)))
            else :
                print "Failure parsing Ardustation Throttle", line_no
                # return "Error" - not an error if no Throttle
            
            match = re.match(".*ASP:(.*?),",line) # Air Speed
            if match :
                self.airspeed = int(match.group(1))
            else :
                print "Failure parsing Ardustation air speed at line", line_no
                # return "Error" - not an error if no Air Speed
            
            # line was parsed without Errors
            return "ARDUSTATION+++"
        
        print "Unknown Telemetry Format at line", line_no
        return "Error"

def matrix_multiply_3x3_3x1(a,b):
    """matrix multiply a 3x1 matrix onto a 3x3 matrix"""
    c =    [(a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]) ,
            (a[3] * b[0]) + (a[4] * b[1]) + (a[5] * b[2]) ,
            (a[6] * b[0]) + (a[7] * b[1]) + (a[8] * b[2]) ]
    return(c)

def matrix_multiply_3x3_3x3(a,b):
    """matrix multiply a 3x3 matrix onto a 3x3 matrix"""
    c =   [(a[0] * b[0])  + (a[1] * b[3]) + (a[2] * b[6]) ,
           (a[0] * b[1])  + (a[1] * b[4]) + (a[2] * b[7]) ,
           (a[0] * b[2])  + (a[1] * b[5]) + (a[2] * b[8]) ,
           (a[3] * b[0])  + (a[4] * b[3]) + (a[5] * b[6]) ,
           (a[3] * b[1])  + (a[4] * b[4]) + (a[5] * b[7]) ,
           (a[3] * b[2])  + (a[4] * b[5]) + (a[5] * b[8]) ,
           (a[6] * b[0])  + (a[7] * b[3]) + (a[8] * b[6]) ,
           (a[6] * b[1])  + (a[7] * b[4]) + (a[8] * b[7]) ,
           (a[6] * b[2])  + (a[7] * b[5]) + (a[8] * b[8]) ]
    return (c)

def matrix_transpose(a) :
    """ Perform the matrix transpose operation on a 3x3 matrix"""
    b = [0,0,0,0,0,0,0,0,0]
    b[0] = a[0]
    b[1] = a[3]
    b[2] = a[6]
    b[3] = a[1]
    b[4] = a[4]
    b[5] = a[7]
    b[6] = a[2]
    b[7] = a[5]
    b[8] = a[8]
    return b
