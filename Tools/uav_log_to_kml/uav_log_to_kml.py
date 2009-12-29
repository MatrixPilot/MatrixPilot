#  This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 2 of the License, or
#    (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.

#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#  Author: Peter Hollands, Copyright Peter Hollands 2009
#
#  The following 3 lines require keyword insertion to be turned on
#  in the code.google.com subversion repository. 
#  $Rev::               $:  Revision of last commit
#  $Author::            $:  Author of last commit
#  $Date::              $:  Date of last commit


from xml.dom import minidom
from math  import *
import re
import sys
import os
import Tkinter, tkFileDialog, tkMessageBox
from zipfile import ZipFile,ZIP_DEFLATED
from time import sleep


class telemetry :
    def __init__(self) :
        self.tm = float (0)
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
        
    def parse(self,line,line_no) :
        self.line_no = line_no
        # Get the Format Revision No.
        # This allows us to change revisions in the future, and
        # have this code still be able to cope with changes.
        match = re.match("^F1:",line) # If line starts with F1: then Revision 1
        if match :
            # Parse the line for revision 1 format
            if debug : print "Matching a Format Rev 1 line"
            match = re.match(".*:T(.*?):",line) # Time of Week
            if match :
                self.tm = float (match.group(1))
            else :
                print "Failure parsing time of week at line", line_no
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
        
        match = re.match("^F2:",line) # If line starts with F1: then Revision 1
        if match :
            # Parse the line for revision 2 format
            if debug : print "Matching a Format Rev 2 line"
            match = re.match(".*:T(.*?):",line) # Time of Week
            if match :
                self.tm = float (match.group(1))
            else :
                print "Failure parsing time of week at line", line_no
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
            
            match = re.match(".*:a([-0-9]*?):",line) # rmat 0
            if match :
                self.rmat0 = int(match.group(1))
            else :
                print "Failure parsing rmat0 at line", line_no
                return "Error"
            
            match = re.match(".*:b([-0-9]*?):",line) # rmat 1
            if match :
                self.rmat1 = int(match.group(1))
            else :
                print "Failure parsing rmat1 at line", line_no
                return "Error"
            
            match = re.match(".*:c([-0-9]*?):",line) # rmat 2
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
            match = re.match(".*:s(.*?):",line) # Speed Over Ground
            if match :
                self.sog = int(match.group(1))
            else :
                print "Failure parsing Speed Over Ground at line", line_no
                return "Error"
            
            match = re.match(".*:c([-0-9]*?):",line) # Course Over Ground
            if match :
                self.cog = int(match.group(1))
            else :
                print "Failure parsing Course Over Ground at line", line_no
                return "Error"
            # line was parsed without Errors
            return "F2"

        #################################################################
        # Try Another format of telemetry
        
        match = re.match("^F4:",line) # If line starts with F4
        if match :
            # Parse the line for options.h values
            if debug : print "Matching a Format Rev 4 line"
            match = re.match(".*:R_STAB=(.*?):",line) # ROLL_STABILIZATION
            if match :
                self.roll_stabilization = int (match.group(1))
            else :
                print "Failure parsing ROLL_STABILIZATION at line", line_no
                return "Error"
                
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
            
            match = re.match(".*:ALT_HOLD=(.*?):",line) # USE_ALTITUDEHOLD
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
            if debug : print "Matching a Format Rev 4 line"
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
            if debug : print "Matching a Format Rev 6 line"
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
            if debug : print "Matching a Format Rev 7 line" 
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
            if debug : print "Matching a Format Rev 8 line" 
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
            if debug : print "Matching an Ardustation !!! line"
            
            #match = re.match(".*:S(.*?):",line) # Status Flags
            #if match :
            #    self.status = match.group(1)
            #else :
            #    print "Failure parsing status flags at line", line_no
            #    return "Error"
            
            match = re.match(".*LAT:(.*?),",line) # Latitude
            if match :
                self.latitude = float(match.group(1)) * 10 # Compatibility
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
            if debug : print "Matching an Ardustation +++ line "
            
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


class colors :
    def __init__(self) :
        # The following web safe colors taken from
        # http://en.wikipedia.org/wiki/Web_colors
        self.aqua    = "aqua",   "00FFFF"
        self.grey    = "grey",   "808080"
        self.navy    = "navy",   "000080"
        self.silver  = "silver", "C0C0C0"
        self.green   = "green",  "008000"
        self.olive   = "olive",  "808000"
        self.teal    = "teal",   "008080"
        self.blue    = "blue",   "0000FF"
        self.lime    = "lime",   "00FF00"
        self.purple  = "purple", "800080"
        self.white   = "white",  "FFFFFF"
        self.fuchsia = "fuchsia","FF00FF"
        self.maroon  = "maroon", "800000"
        self.red     = "red",    "FF0000"
        self.yellow  = "yellow", "FFFF00"
        self.black   = "black",  "000000"
        self.list = \
            [self.aqua,self.blue,self.fuchsia,self.grey,\
             self.green,self.lime,self.maroon,self.navy,self.olive,self.purple,\
             self.teal, self.yellow]

def calculate_headings_pitch_roll(log_book) :
    for entry in log_book.entries :
        
        entry.lon  = entry.longitude / 10000000 # degrees
        entry.lat  = entry.latitude  / 10000000 # degrees
        entry.alt  = entry.altitude / 100       # meters absolute
        # If using Ardustation, then roll and pitch already set from telemetry
        if log_book.ardustation_pos != "Recorded" : # only calc if using UAV DevBoard
            entry.pitch = (asin(entry.rmat7 / 16384.0) / (2*pi)) * 360 # degrees
            entry.roll =  (asin(entry.rmat6 / 16385.0) / (2*pi)) * 360
            # Calcuate our heading from Rmat readings.
            heading_radians = atan2(- entry.rmat1 , entry.rmat4)
            entry.heading_degrees = (heading_radians / (2 * pi)) * 360
        
def write_style_urls(filename):
    for acolor in mycolors.list :
        temp_line = "      <Style id=\""  + acolor[0] + "\">"
        print >> filename, temp_line
        print >> filename, """        <LineStyle>"""
        temp_line = "          <color>" + "7f" + acolor[1] + "</color>"
        print >> filename, temp_line
        print >> filename, """          <width>4</width>
        </LineStyle>
        <PolyStyle>"""
        temp_line = "          <color>" + "7f" + acolor[1] + "</color>"
        print >> filename, temp_line
        print >> filename, """        </PolyStyle>
      </Style>"""

def write_document_preamble(log_book,filename):
    print >> filename , """<?xml version="1.0" encoding="UTF-8"?>
<kml xmlns="http://www.opengis.net/kml/2.2">
  <Document>
    <open>1</open>
    <name>Flight Log """,
    print >> filename, flight_log_name,
    print >> filename, """</name><description><b>Flight Settings</b><P> </P><TABLE>"""

    if log_book.F4 == "Recorded" :
        print >> filename, "<TR><TD>ROLL_STABILIZATION</TD><TD>", log_book.roll_stabilization, "</TD></TR>"
        print >> filename, "<TR><TD>PITCH_STABILIZATION</TD><TD>",log_book.pitch_stabilization, "</TD></TR>"
        print >> filename, "<TR><TD>YAW_STABILIZATION_RUDDER</TD><TD>",log_book.yaw_stabilization_rudder, "</TD></TR>"
        print >> filename, "<TR><TD>YAW_STABILIZATION_AILERON</TD><TD>", log_book.yaw_stabilization_aileron, "</TD></TR>"
        print >> filename, "<TR><TD>AILERON_NAVIGATION</TD><TD>", log_book.aileron_navigation, "</TD></TR>"
        print >> filename, "<TR><TD>RUDDER_NAVIGATION</TD><TD>",  log_book.rudder_navigation, "</TD></TR>"
        print >> filename, "<TR><TD>USE_ALTITUDEHOLD</TD><TD>", log_book.use_altitudehold, "</TD></TR>"
        print >> filename, "<TR><TD>RACING_MODE</TD><TD>", log_book.racing_mode, "</TD></TR>"
  
    if log_book.F5 == "Recorded" :
        print >> filename, "<TR><TD>YAWKP_AILERON</TD><TD>", log_book.yawkp_aileron, "</TD></TR>"
        print >> filename, "<TR><TD>YAWKD_AILERON</TD><TD>",log_book.yawkd_aileron, "</TD></TR>"
        print >> filename, "<TR><TD>ROLLKP</TD><TD>",log_book.rollkp, "</TD></TR>"
        print >> filename, "<TR><TD>ROLLKD</TD><TD>", log_book.rollkd, "</TD></TR>"
        print >> filename, "<TR><TD>AILERON_BOOST</TD><TD>", log_book.aileron_boost, "</TD></TR>"        
    
    if log_book.F6 == "Recorded" :
        print >> filename, "<TR><TD>PITCHGAIN</TD><TD>", log_book.pitchgain, "</TD></TR>"
        print >> filename, "<TR><TD>PITCHKD</TD><TD>",log_book.pitchkd, "</TD></TR>"
        print >> filename, "<TR><TD>RUDDER_ELEV_MIX</TD><TD>",log_book.rudder_elev_mix, "</TD></TR>"
        print >> filename, "<TR><TD>ROLL_ELEV_MIX</TD><TD>", log_book.roll_elev_mix, "</TD></TR>"
        print >> filename, "<TR><TD>ELEVATOR_BOOST</TD><TD>", log_book.elevator_boost, "</TD></TR>"        
        
    if log_book.F7 == "Recorded" :
        print >> filename, "<TR><TD>YAWKP_RUDDER</TD><TD>", log_book.yawkp_rudder, "</TD></TR>"
        print >> filename, "<TR><TD>YAWKD_RUDDER</TD><TD>",log_book.yawkd_rudder, "</TD></TR>"
        print >> filename, "<TR><TD>RUDDER_BOOST</TD><TD>",log_book.rudder_boost, "</TD></TR>"
        print >> filename, "<TR><TD>RTL_PITCH_DOWN</TD><TD>", log_book.rtl_pitch_down, "</TD></TR>"        
        
    if log_book.F8 == "Recorded" :
        print >> filename, "<TR><TD>HEIGHTMAX</TD><TD>", log_book.heightmax, "</TD></TR>"
        print >> filename, "<TR><TD>HEIGHTMIN</TD><TD>",log_book.heightmin, "</TD></TR>"
        print >> filename, "<TR><TD>MINIMUMTHROTTLE</TD><TD>",log_book.minimumthrottle, "</TD></TR>"
        print >> filename, "<TR><TD>MAXIMUMTHROTTLE</TD><TD>", log_book.maximumthrottle, "</TD></TR>"
        print >> filename, "<TR><TD>PITCHATMINTHROTTLE</TD><TD>", log_book.pitchatminthrottle, "</TD></TR>"
        print >> filename, "<TR><TD>PITCHATMAXTHROTTLE</TD><TD>", log_book.pitchatmaxthrottle, "</TD></TR>"
        print >> filename, "<TR><TD>PITCHATZEROTHROTTLE</TD><TD>",  log_book.pitchatzerothrottle, "</TD></TR>"   
        
    print >> filename, "</TABLE><P> </P>"
    print >> filename, """</description>"""
  


def write_flight_path_preamble (log_book,filename):
    write_style_urls(filename)
    
      
def write_flight_path_postamble(log_book,filename):
    # print >> filename, """  </Folder>"""
    pass

def write_document_postamble(log_book,filename) :
    print >> filename, """
</Document>
</kml>
 """

def write_placemark_preamble_auto(open_waypoint,current_waypoint,filename):
    waypoints_open = 6  # The no. of waypoints to enable "on" in GE
                        # User can switch on other waypoints in places window of GE
                        # Later
    print >> filename, """
    <Placemark>
      <name>""",
    print >> filename, "Towards Waypoint: ", current_waypoint,
    print >> filename, """</name>"""
   
    if open_waypoint :
        print >>filename, "     <visibility>1</visibility>"
    else:
        print >> filename, "     <visibility>0</visibility>"
    print >> filename, """        <description>waypoint""",
    print >> filename, current_waypoint,
    print >> filename, "</description>"
    temp_line = "     <styleUrl>#" +  \
        mycolors.list[current_waypoint % 12][0] + "</styleUrl>"
    print >> filename, temp_line
    print >> filename, """        <LineString>
        <extrude>1</extrude>
        <tessellate>1</tessellate>
        <altitudeMode>absolute</altitudeMode>
        <coordinates>"""
    
def write_placemark_postamble(filename):
    print >> filename, """        </coordinates>
      </LineString>
    </Placemark>"""
    
def write_placemark_preamble_manual(open_waypoint,filename,log_book):
    print >> filename, """
    <Placemark>
      <name>Manual Mode</name>
      <description>Manual Mode</description>"""
    
    if open_waypoint :
        print >> filename, "      <visibility>1</visibility>"
    else:
        print >> filename, "      <visibility>0</visibility>"
    print >> filename, """      <styleUrl>#red</styleUrl>"
            <LineString>
            <extrude>1</extrude>
            <tessellate>1</tessellate>"""
    if log_book.ardustation_pos == "Recorded" :
        print >> filename, """
            <altitudeMode>relativeToGround</altitudeMode>"""
    else :
        print >> filename, """
            <altitudeMode>absolute</altitudeMode>"""
    print >> filename, """
        <coordinates>"""

   
def write_T3_waypoints(filename,origin,log_book)  :
     # note origin.latitude and origin.longitude are straight from log of telemetry
     # so they are expressed in degrees * 10,000,000
     initLat = origin.latitude / 10000000
     initLon = origin.longitude / 10000000
    
     corner = 100 # easy way to describe location of waypoints, e.g. 100m,100m from origin
     convert = 90.0 /10000000.0
    
     wp_dist_in_lat = (corner * convert)  # in normal degrees
     wp_dist_in_lon = (corner * convert) / (acos(((initLat)/360)*2*pi))
     waypoint_list = \
      [((corner * convert)+initLat,((corner * convert) /(acos(((initLat) / 360)*2*pi))) + initLon), \
       ((corner * convert)+initLat,((-corner * convert) /(acos(((initLat) / 360)*2*pi))) + initLon), \
       ((-corner * convert)+initLat,((corner * convert) /(acos(((initLat) / 360)*2*pi))) + initLon), \
       ((-corner * convert)+initLat,((-corner * convert) /(acos(((initLat) / 360)*2*pi))) + initLon)]
     LAT = 0
     LON = 1
     print >> filename, """<Folder><open>0</open>
    <name>T3 Competition Course</name>
    <description>The T3 Competition Course from DIYDrones.com</description>"""
     for waypoint in waypoint_list :
         print >> filename, """   <Placemark> 
      <name>Waypoint X</name>
      <description>Waypoint</description>
      <visibility>0</visibility>
       <Style id="default"></Style>
      <Model>"""
         if log_book.ardustation_pos == "Recorded" :
             print >> filename, """
            <altitudeMode>relativeToGround</altitudeMode>"""
         else:
             print >> filename, """
            <altitudeMode>absolute</altitudeMode>"""
         print >> filename,"""
        <Location>
           <longitude>""",
         print >> filename, waypoint[LON],
         print >> filename, """</longitude>
           <latitude>""",
         print >> filename, waypoint[LAT],
         print >> filename, """</latitude>
           <altitude>""",
         if log_book.ardustation_pos == "Recorded" :
             print >> filename, 0 ,
         else: 
             print >> filename, origin.altitude / 100.0 ,
         print >> filename, """</altitude>
        </Location>
      <Orientation>
        <heading>0</heading>
        <tilt>0</tilt>
        <roll>0</roll>
      </Orientation>
      <Scale>
        <x>1</x>
        <y>1</y>
        <z>1</z>
      </Scale>
      <Link>
        <href>models/waypoint.dae</href>
      </Link>
      </Model>
      <DocumentSource>Pete Hollands</DocumentSource>
    </Placemark>"""
     print >> filename, "</Folder>"

def write_flight_path(log_book,flight_origin, filename):
    write_flight_path_preamble(log_book,filename)
    write_T3_waypoints(filename,flight_origin,log_book)
    first_waypoint = True
   
    open_waypoint = True      # We only open the first few waypoints in GE - to keep graphic clean
    max_waypoints_to_open = 9
    print >> filename, """     <Folder><open>0</open>
    <name>Paths to Waypoints</name>
    <description>Coloured Coded Paths to Waypoints<p> Manual Mode is in Grey</p></description>"""
    for entry in log_book.entries :
        if entry.status == "1111" : # Auto Mode
            current_waypoint = entry.waypointIndex
            if current_waypoint > 6 : open_waypoint = False
            if first_waypoint :
                write_placemark_preamble_auto(open_waypoint,current_waypoint,filename)
                first_waypoint = False
                last_status_auto = True
            elif last_status_auto == False : # previous entry manual mode
                line1 = "%f," % entry.lon
                line2 = "%f," % entry.lat
                line3 = "%f" %  entry.alt
                line = "          " + line1 + line2 + line3
                print >> filename, line
                write_placemark_postamble(filename)
                write_placemark_preamble_auto(open_waypoint,current_waypoint,filename)
                line1 = "%f," % entry.lon
                line2 = "%f," % entry.lat
                line3 = "%f" %  entry.alt
                line = "          " + line1 + line2 + line3
                print >> filename, line
                last_status_auto = True
            else:       # Previous entry was also Auto Mode
                # Are we still aiming for the same waypoint ?  No  
                if  current_waypoint  != last_waypoint :
                    line1 = "%f," % entry.lon
                    line2 = "%f," % entry.lat
                    line3 = "%f" %  ( entry.alt - 2 )
                    line = "          " + line1 + line2 + line3
                    print >> filename, line
                    write_placemark_postamble(filename)
                    write_placemark_preamble_auto(open_waypoint,current_waypoint,filename)
                    line1 = "%f," % entry.lon
                    line2 = "%f," % entry.lat
                    line3 = "%f" %  ( entry.alt - 2 )
                    line = "          " + line1 + line2 + line3
                    print >> filename, line
                else : # We are still aiming for the same waypoint
                    line1 = "%f," % entry.lon
                    line2 = "%f," % entry.lat
                    line3 = "%f" %  ( entry.alt - 2 )
                    line = "          " + line1 + line2 + line3
                    print >> filename, line                  
                    last_status_auto = True
            last_waypoint = current_waypoint
        else :  # we are currently in Manual Mode
            if first_waypoint :
                write_placemark_preamble_manual(open_waypoint,filename,log_book)
                first_waypoint  = False
                last_status_auto = False
            if last_status_auto == True :  # We've jsut changed from auto to Manual.
                line1 = "%f," % entry.lon
                line2 = "%f," % entry.lat
                line3 = "%f" %  ( entry.alt - 2 )
                line = "          " + line1 + line2 + line3
                print >> filename, line
                write_placemark_postamble(filename)
                write_placemark_preamble_manual(open_waypoint,filename,log_book)
                line1 = "%f," % entry.lon
                line2 = "%f," % entry.lat
                line3 = "%f" %  ( entry.alt - 2 )
                line = "          " + line1 + line2 + line3
                print >> filename, line
                first_waypoint  = False
                last_status_auto = False
            else : # We are still in manual, we were in manual last time.
                # print intermediary points 
                line1 = "%f," % entry.lon
                line2 = "%f," % entry.lat
                line3 = "%f" %  ( entry.alt - 2 )
                line = "          " + line1 + line2 + line3
                print >> filename, line
            last_status_auto = False
    write_placemark_postamble(filename)
    write_flight_path_postamble(log_book, filename)
    print >> filename, """      </Folder>"""   

def write_flight_vectors(log_book,origin, filename) :
    print >> filename, """
      <Folder>
        <open>0</open>
	<name>Pitch/Roll/Yaw""",
    print >> filename, "</name>"
    counter = 0
    print >> filename, "<description>Model plane plotted for each second of flight</description>"
    for entry in log_book.entries :
        counter += 1 
        line1 = "%f," % entry.lon
        line2 = "%f," % entry.lat
        line3 = "%f"  % entry.alt
        line = line1 + line2 + line3

        print >> filename, """   <Placemark> 
      <name>Vector""",
        print >> filename, counter,
        print >> filename, """</name>
      <description>UAV Yaw Vector""",
        print >> filename,  counter ,
        print >> filename,  \
               "<p>GPS Time(Secs)", (entry.tm /100),\
               "</p><p>status",entry.status, "</p>", \
               "<p>Desired waypoint",entry.waypointIndex, "</p>", \
               "<p>Altitude above origin",int((entry.altitude - origin.altitude) / 100.0), "</p>", \
               "<p>Meters North of origin",int(((entry.latitude - origin.latitude) / 90.0)),"</p>", \
               "<p>Meters East of origin", int(((entry.longitude - origin.longitude) /90.0) \
                      * cos(entry.latitude /10000000 * (pi / 180))), "</p>", \
               "<p>GPS SOG", (entry.sog / 100.0),"</p>",\
               "<p>GPS COG",(entry.cog / 100.0),"</p></description>"
        print >> filename, """       <Style id="default"></Style>
      <Model>"""
        if log_book.ardustation_pos == "Recorded" :
            print >> filename, """
      <altitudeMode>relativeToGround</altitudeMode>"""
        else:
            print >> filename, """
      <altitudeMode>absolute</altitudeMode>"""
        print >> filename, """
      <Location>
        <longitude>""",
        print >> filename,  entry.lon,
        print >> filename, """</longitude>
        <latitude>""",
        print >> filename, entry.lat,
        print >> filename, """</latitude>
        <altitude>""",
        print >> filename, entry.alt,
        print >> filename, """</altitude>
      </Location>
      <Orientation>
        <heading>""",
        if log_book.ardustation_pos == "Recorded" :
            print >> filename, entry.cog / 100.0 ,
        else :
            print >> filename, entry.heading_degrees,
        print >> filename, """</heading>
        <tilt>""",
        print >> filename, entry.pitch,
        print >> filename, """</tilt>
        <roll>""",
        print >> filename, entry.roll,
        print >> filename, """</roll>
      </Orientation>
      <Scale>
        <x>2</x>
        <y>2</y>
        <z>2</z>
        </Scale>
      <Link>
        <href>models/block_plane.dae</href>
      </Link>
      </Model>
      <DocumentSource>Pete Hollands</DocumentSource>
    </Placemark>
"""
    # This marks the end of the for loop
    print >> filename, "</Folder>"
    
class origin() : # stores origin of flight 
    def __init__(self):
        self.latitude = 0
        self.longitude = 0
        self.altitude = 0
        
    def average(self,initial_points,log_book) :
        index = 0
        sum_latitude = 0
        sum_longitude = 0
        sum_altitude = 0
        while ( index < initial_points ):
            index += 1
            # Average out first few reading to get our origin
            sum_latitude  += log_book.entries[index].latitude
            sum_longitude += log_book.entries[index].longitude
            sum_altitude  += log_book.entries[index].altitude
            if (index  == initial_points) :
                self.latitude =  sum_latitude  /  initial_points
                self.longitude = sum_longitude /  initial_points
                self.altitude =  sum_altitude  /  initial_points
                if debug :
                    print "init_lat: ",self.latitude,"init_lon: ", \
                            self.longitude,"init_alt: ",self.altitude
class flight_log_book:
    def __init__(self) :
        self.entries = [] # an empty list of entries  at the beginning.
        self.F4 = "Empty"
        self.F5 = "Empty"
        self.F6 = "Empty"
        self.F7 = "Empty"
        self.F8 = "Empty"
        self.ardustation_pos = "Empty"

def calc_average_wind_speed(log_book):
    if log_book.racing_mode == 0 :
        print "Average wind speed is only calculated if racing_mode = 1\n" + \
              "so that the plane is at a known constant throttle setting"
        return
    HAVE_DATA = 0
    AVERAGE = 1
    REMEMBER = 2
    acceptable_pitch = 5  # acceptable climb or descent in degrees that does not
                          #speed up or slow down the plane too much for this analysis
    
    compass_points = ["N  ","NNE","NE ","ENE", \
                      "E  ","ESE","SE ","SSE", \
                      "S  ","SSW","SW ","WSW", \
                      "W  ","WNW","NW ","NNW"  ]
    # We store average wind for 16 points of the compass
    # False means we have not had any value yet for that direction.
    wind_speeds = [ [False,0,[]],[False,0,[]],[False,0,[]],[False,0,[]], \
                        [False,0,[]],[False,0,[]],[False,0,[]],[False,0,[]], \
                        [False,0,[]],[False,0,[]],[False,0,[]],[False,0,[]], \
                        [False,0,[]],[False,0,[]],[False,0,[]],[False,0,[]], ]
    for entry in log_book.entries :
        if (entry.pitch > acceptable_pitch) or (entry.pitch < - acceptable_pitch) :
            continue # Plane is diving or climbing too much           
        cog_degrees = entry.cog / 100.0
        sog_meters = entry.sog / 100.0
        if sog_meters < 1.5 :# meters per second.
                continue # ignore - assume plane has taken off or landed
        else :
            if cog_degrees > 360 or cog_degrees < 0 :
                print "COG is invalid = ", cog_degrees
            direction = int (cog_degrees / 22.5) # 11.25 degrees is 1/16 of compass
            wind_speeds[direction][REMEMBER].append(sog_meters)

    # Ok we have all the value in buckets now create averages
    for wind_direction in wind_speeds :
        sum_of_sog = 0
        no_of_entries = 0
        for sogs in wind_direction[REMEMBER]:
            sum_of_sog = sum_of_sog  + sogs
            no_of_entries += 1
        average = sum_of_sog / no_of_entries
        wind_direction[AVERAGE] = average
    for i in 0,1,2,3,4,5,6,7 :
        print "Average SOG " , compass_points[i], " %4.1f" % wind_speeds[i][AVERAGE] ,\
              "Samples: %4s" % len(wind_speeds[i][REMEMBER]), \
              "  ", compass_points[i+8], " %4.1f" % wind_speeds[i+8][AVERAGE], \
              "Samples: %4s" % len(wind_speeds[i+8][REMEMBER]), \
              "Difference: %4.1f"  % abs(wind_speeds[i][AVERAGE] - wind_speeds[i+8][AVERAGE]) 
              
def create_kmz(flight_log_dir,flight_log_name):
    flight_log = os.path.join(flight_log_dir, flight_log_name)
    #flight telelemetry file must end in .txt or .TXT for this to work
    flight_pos = re.sub(".[tT][xX][tT]$",".kml", flight_log_name)
    flight_pos_kml = os.path.join(flight_log_dir, flight_pos)
    f = open(flight_log, 'r')
    f_pos = open(flight_pos_kml, 'w')
    roll = 0  # only used with ardustation roll
    pitch = 0 # only used with ardustation pitch
    line_no = 0
    log_book = flight_log_book()   
    for line in f :
        line_no += 1
        log = telemetry() # Make a new empty log entry
        log_format  = log.parse(line,line_no)
        if log_format == "Error" :# we had an error
            print "Error parsing telemetry line ",line_no 
            continue  # Go get the next line
        elif log_format == "F1" or log_format == "F2"  or \
               log_format == "ARDUSTATION!!!": # We have a normal telemetry line
            if debug : print "lat",log.latitude,"lon",log.longitude,"alt",log.altitude, \
                "wp", log.waypointIndex, "rmat1", log.rmat1
            if (log.latitude == 0 or log.longitude == 0 or log.altitude ==0 ):
                if debug: print "lat or long or alt is 0; ignoring line", line_no
                continue # Get next line of telemetry  - can happen at boot time on plane 
            else :
                # We have a good log entry - put it in the logbook.
                if log_format == "ARDUSTATION!!!" :
                        log_book.ardustation_pos = "Recorded"
                        log.roll = roll   # add the last roll parsed from Ardustation +++
                        log.pitch = pitch # add the last pitch parsed from Ardustation +++
                log_book.entries.append(log)
        elif log_format == "F4" : # We have a type of options.h line
            log_book.roll_stabilization        = log.roll_stabilization
            log_book.pitch_stabilization       = log.pitch_stabilization
            log_book.yaw_stabilization_rudder  = log.yaw_stabilization_rudder
            log_book.yaw_stabilization_aileron = log.yaw_stabilization_aileron
            log_book.aileron_navigation        = log.aileron_navigation
            log_book.rudder_navigation         = log.rudder_navigation
            log_book.use_altitudehold          = log.use_altitudehold
            log_book.racing_mode               = log.racing_mode
            log_book.F4 = "Recorded"
        elif log_format == "F5" : # We have a type of options.h line
            log_book.yawkp_aileron = log.yawkp_aileron
            log_book.yawkd_aileron = log.yawkd_aileron
            log_book.rollkp = log.rollkp
            log_book.rollkd = log.rollkd
            log_book.aileron_boost = log.aileron_boost
            log_book.F5 = "Recorded"
        elif log_format == "F6" : # We have a type of options.h line
            log_book.pitchgain = log.pitchgain
            log_book.pitchkd = log.pitchkd
            log_book.rudder_elev_mix = log.rudder_elev_mix
            log_book.roll_elev_mix = log.roll_elev_mix
            log_book.elevator_boost = log.elevator_boost
            log_book.F6 = "Recorded"
        elif log_format == "F7" : # We have a type of options.h line
            log_book.yawkp_rudder = log.yawkp_rudder
            log_book.yawkd_rudder = log.yawkd_rudder
            log_book.rudder_boost = log.rudder_boost
            log_book.rtl_pitch_down = log.rtl_pitch_down
            log_book.F7 = "Recorded"
        elif log_format == "F8" : # We have a type of options.h line
            log_book.heightmax = log.heightmax
            log_book.heightmin = log.heightmin
            log_book.minimumthrottle = log.minimumthrottle
            log_book.maximumthrottle = log.maximumthrottle
            log_book.pitchatminthrottle = log.pitchatminthrottle
            log_book.pitchatmaxthrottle = log.pitchatmaxthrottle
            log_book.pitchatzerothrottle = log.pitchatzerothrottle
            log_book.F8 = "Recorded"
        elif log_format == "ARDUSTATION+++" : # Intermediate Ardustation line
            roll = log.roll
            pitch = log.pitch
        else :
            print "Parsed a line format - ,", log_format, \
                  "but don't know what to do with it."
        
    initial_points = 10 # no. log entries to find origin at start        
    flight_origin = origin()        
    flight_origin.average(initial_points,log_book)   
    calculate_headings_pitch_roll(log_book)
    write_document_preamble(log_book,f_pos)
    write_flight_path(log_book,flight_origin,f_pos)
    write_flight_vectors(log_book,flight_origin,f_pos)
    write_document_postamble(log_book,f_pos)
    # create_simulated_dead_reckoning(log_book) # simulate new code for plane
                                                # to do dead reckoning.
    # write_simulated_paths(f_sim,log_book)     # still to be written

    # Close up the open files
    f.close()
    f_pos.close()

    # Make up the KML files into KMZ file 
    flight_kmz = re.sub(".[tT][xX][tT]$",".kmz", flight_log_name)
    flight_pos_kmz = os.path.join(flight_log_dir,flight_kmz)
    # Try to find a models directory nearby to add to zip files....
    model_dir = []
    model_dir.append(os.path.join(flight_log_dir,"models"))
    model_dir.append(os.path.join(os.getcwd(), "models"))  
    if (os.access(os.path.join(model_dir[1],"waypoint.dae"),os.F_OK)) and \
           (os.access(os.path.join(model_dir[1],"block_plane.dae"),os.F_OK)) and \
           (os.access(os.path.join(model_dir[1],"arrow.dae"),os.F_OK)):
        dir_index = 1 
    else: 
        print "Program currently needs the models directory (part of the Tools/uav_log_to_kml download)"
        print "to be placed, with it's internal file contents, in the directory containing"
        print "your flight telemetry. i.e. in the same directory as ", flight_log_name
        print "Exiting Program"
        exit(0) # We exit the program. Note that we did leave a kml file around
    waypoint_model  = os.path.join("models","waypoint.dae")
    block_plane_model = os.path.join("models","block_plane.dae")
    arrow_model = os.path.join("models","arrow.dae")
    kmzfile = ZipFile(flight_pos_kmz, "w",ZIP_DEFLATED) # "a" to append, "r" to read
    kmzfile.write(flight_pos_kml)
    kmzfile.write(waypoint_model)
    kmzfile.write(block_plane_model)
    kmzfile.write(arrow_model)
    kmzfile.close()
    # Remove the temporary kml files, now we have the kmz file
    os.remove(flight_pos_kml)

    # Calculate Average wind speed - Experimental - not accurate
    # as really not enough points to sample from level flight - atleast
    # for the DIY Drones.com T3 Course. But left here for future analysis,
    # for example if we have air speed or a climb / dive performance model.
    # calc_average_wind_speed(log_book)

    # Create a graph of altitude
    ### write out a csv file enabling analysis in Excel or OpenOffice
    flight_csv = re.sub(".[tT][xX][tT]$",".csv", flight_log_name)
    flight_cos_csv = os.path.join(flight_log_dir, flight_csv)
    f_csv = open(flight_cos_csv, 'w')
    print >> f_csv, "Time (secs), Status, Lat, Lon,Waypoint, Altidude, COG, SOG"
    for entry in log_book.entries :
        print >> f_csv, entry.tm / 1000.0, ",", entry.status, "," , \
              entry.latitude / 10000000.0, ",",entry.longitude / 10000000.0,",", \
              entry.waypointIndex, ",", entry.altitude / 100.0 , "," , \
              entry.cog / 100.0 , "," , entry.sog / 100.0
    f_csv.close()
       
    
########## Start of the Main Program ##########
    
debug = 0 # set this to 1 of you want lot's of debug info to be printed.

instructions = "uav_log_to_kml.py:  Convert Generic UAV telemetry " + \
               "to Google Earth Files (kmz). \n You must specify the "  + \
               "file to convert by editing the source code of the " + \
               "program -  edit program using Python I.D.E "  + \
               "and run from there (for now)"

if __name__=="__main__":
    
    if len(sys.argv) == 1:
        root = Tkinter.Tk()
        w = Tkinter.Canvas(root, width=300, height=300)
        w.pack()
        working_dir = os.getcwd()
        image_dir = "images"
        image_file = "fa_banner_300x300.gif"
        image_full_name = os.path.join(image_dir, image_file)
        try:
            imgobj = Tkinter.PhotoImage(file = image_full_name)
            w.create_image(150,150,image = imgobj)
            w.pack()
        except:
            pass
        #sleep(3)
        #root.withdraw()
        filename = tkFileDialog.askopenfilename(parent=root,title='Choose a flight telemetry file')
        if filename != "":
            split_path = os.path.split(filename)
            flight_log_dir  = split_path[0]
            flight_log_name = split_path[1]
            mycolors = colors() # get a list of colors to use later
            print "Converting ..."
            create_kmz(flight_log_dir,flight_log_name)
            message = "Conversion of " + flight_log_name + "\nto kmz file is complete"
            tkMessageBox.showinfo("Finished",message)
            
    else:
        print instructions

root.withdraw() # close the main window
    

    
    
