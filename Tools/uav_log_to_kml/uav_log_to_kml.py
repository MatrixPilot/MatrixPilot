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

from xml.dom import minidom
from math  import *
import re
import sys
import os
from zipfile import ZipFile,ZIP_DEFLATED


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
                return False
                
            match = re.match(".*:S(.*?):",line) # Status Flags
            if match :
                self.status = match.group(1)
            else :
                print "Failure parsing status flags at line", line_no
                return False
            
            match = re.match(".*:N(.*?):",line) # Lat North
            if match :
                self.latitude = float(match.group(1))
            else :
                print "Failure parsing Lat North at line", line_no
                return False
            
            match = re.match(".*:E(.*?):",line) # Lon East
            if match :
                self.longitude = float (match.group(1))
            else :
                print "Failure parsing Lon East at line", line_no
                return False
            
            match = re.match(".*:A(.*?):",line) # Altitude
            if match :
                self.altitude = float(match.group(1))
            else :
                print "Failure parsing Altitude at line", line_no
                return False
            
            match = re.match(".*:W(.*?):",line) # Waypoint Index
            if match :
                self.waypointIndex = int(match.group(1))
            else :
                print "Failure parsing Wapoint Index at line", line_no
                return False
            
            match = re.match(".*:a(.*?):",line) # rmat 0
            if match :
                self.rmat0 = int(match.group(1))
            else :
                print "Failure parsing rmat0 at line", line_no
                return False
            
            match = re.match(".*:b(.*?):",line) # rmat 1
            if match :
                self.rmat1 = int(match.group(1))
            else :
                print "Failure parsing rmat1 at line", line_no
                return False
            
            match = re.match(".*:c(.*?):",line) # rmat 2
            if match :
                self.rmat2 = int(match.group(1))
            else :
                print "Failure parsing rmat2 at line", line_no
                return False
            
            match = re.match(".*:d(.*?):",line) # rmat 3
            
            if match :
                self.rmat3 = int(match.group(1))
            else :
                print "Failure parsing rmat3 at line", line_no
                return False
            
            match = re.match(".*:e(.*?):",line) # rmat 4
            if match :
                self.rmat4 = int(match.group(1))
            else :
                print "Failure parsing rmat4 at line", line_no
                return False
            
            match = re.match(".*:f(.*?):",line) # rmat 5
            if match :
                self.rmat5 = int(match.group(1))
            else :
                print "Failure parsing rmat5 at line", line_no
                return False
            
            match = re.match(".*:g(.*?):",line) # rmat 6
            if match :
                self.rmat6 = int(match.group(1))
            else :
                print "Failure parsing rmat6 at line", line_no
                return False
            
            match = re.match(".*:h(.*?):",line) # rmat 7
            if match :
                self.rmat7 = int(match.group(1))
            else :
                print "Failure parsing rmat7 at line", line_no
                return False
            
            match = re.match(".*:i(.*?):",line) # rmat 8
            if match :
                self.rmat8 = int(match.group(1))
            else :
                print "Failure parsing rmat8 at line", line_no
                return False
            # Because of a mistake in sprintf in UDB code,
            # sog and cog have to be swapped over in Rev F1 of telemetry
            match = re.match(".*:c(.*?):",line) # Speed Over Ground
            if match :
                self.sog = int(match.group(1))
            else :
                print "Failure parsing Speed Over Ground at line", line_no
                return False
            
            match = re.match(".*:s(.*?):",line) # Course Over Ground
            if match :
                self.cog = int(match.group(1))
            else :
                print "Failure parsing Course Over Ground at line", line_no
                return False
            # line was parsed without Errors
            return True

        #################################################################
        # Try Another format of telemetry
        
        match = re.match("^F2:",line) # If line starts with F1: then Revision 1
        if match :
            # Parse the line for revision 1 format
            if debug : print "Matching a Format Rev 1 line"
            match = re.match(".*:T(.*?):",line) # Time of Week
            if match :
                self.tm = float (match.group(1))
            else :
                print "Failure parsing time of week at line", line_no
                return False
                
            match = re.match(".*:S(.*?):",line) # Status Flags
            if match :
                self.status = match.group(1)
            else :
                print "Failure parsing status flags at line", line_no
                return False
            
            match = re.match(".*:N(.*?):",line) # Lat North
            if match :
                self.latitude = float(match.group(1))
            else :
                print "Failure parsing Lat North at line", line_no
                return False
            
            match = re.match(".*:E(.*?):",line) # Lon East
            if match :
                self.longitude = float (match.group(1))
            else :
                print "Failure parsing Lon East at line", line_no
                return False
            
            match = re.match(".*:A(.*?):",line) # Altitude
            if match :
                self.altitude = float(match.group(1))
            else :
                print "Failure parsing Altitude at line", line_no
                return False
            
            match = re.match(".*:W(.*?):",line) # Waypoint Index
            if match :
                self.waypointIndex = int(match.group(1))
            else :
                print "Failure parsing Wapoint Index at line", line_no
                return False
            
            match = re.match(".*:a(.*?):",line) # rmat 0
            if match :
                self.rmat0 = int(match.group(1))
            else :
                print "Failure parsing rmat0 at line", line_no
                return False
            
            match = re.match(".*:b(.*?):",line) # rmat 1
            if match :
                self.rmat1 = int(match.group(1))
            else :
                print "Failure parsing rmat1 at line", line_no
                return False
            
            match = re.match(".*:c(.*?):",line) # rmat 2
            if match :
                self.rmat2 = int(match.group(1))
            else :
                print "Failure parsing rmat2 at line", line_no
                return False
            
            match = re.match(".*:d(.*?):",line) # rmat 3
            
            if match :
                self.rmat3 = int(match.group(1))
            else :
                print "Failure parsing rmat3 at line", line_no
                return False
            
            match = re.match(".*:e(.*?):",line) # rmat 4
            if match :
                self.rmat4 = int(match.group(1))
            else :
                print "Failure parsing rmat4 at line", line_no
                return False
            
            match = re.match(".*:f(.*?):",line) # rmat 5
            if match :
                self.rmat5 = int(match.group(1))
            else :
                print "Failure parsing rmat5 at line", line_no
                return False
            
            match = re.match(".*:g(.*?):",line) # rmat 6
            if match :
                self.rmat6 = int(match.group(1))
            else :
                print "Failure parsing rmat6 at line", line_no
                return False
            
            match = re.match(".*:h(.*?):",line) # rmat 7
            if match :
                self.rmat7 = int(match.group(1))
            else :
                print "Failure parsing rmat7 at line", line_no
                return False
            
            match = re.match(".*:i(.*?):",line) # rmat 8
            if match :
                self.rmat8 = int(match.group(1))
            else :
                print "Failure parsing rmat8 at line", line_no
                return False
            # Because of a mistake in sprintf in UDB code,
            # sog and cog have to be swapped over in Rev F1 of telemetry
            match = re.match(".*:s(.*?):",line) # Speed Over Ground
            if match :
                self.sog = int(match.group(1))
            else :
                print "Failure parsing Speed Over Ground at line", line_no
                return False
            
            match = re.match(".*:c(.*?):",line) # Course Over Ground
            if match :
                self.cog = int(match.group(1))
            else :
                print "Failure parsing Course Over Ground at line", line_no
                return False
            # line was parsed without Errors
            return True
        
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
                return False
        
            match = re.match(".*<st>(.*?)</st>",line) # Status Flags
            if match :
                self.status = match.group(1)
            else :
                print "Failure parsing status flags at line", line_no
                return False
            
            match = re.match(".*<lat>(.*?)</lat>",line) # Lat North
            if match :
                self.latitude = float(match.group(1))
            else :
                print "Failure parsing Lat North at line", line_no
                return False
            
            match = re.match(".*<lon>(.*?)</lon>",line) # Lon East
            if match :
                self.longitude = float (match.group(1))
            else :
                print "Failure parsing Lon East at line", line_no
                return False
            
            match = re.match(".*<alt>(.*?)</alt>",line) # Altitude
            if match :
                self.altitude = float(match.group(1))
            else :
                print "Failure parsing Altitude at line", line_no
                return False
            
            match = re.match(".*<wp>(.*?)</wp>",line) # Waypoint Index
            if match :
                self.waypointIndex = int(match.group(1))
            else :
                print "Failure parsing Wapoint Index at line", line_no
                return False
            
            match = re.match(".*<r1>(.*?)</r1>",line) # rmat 0
            if match :
                self.rmat1 = int(match.group(1))
            else :
                print "Failure parsing rmat1 at line", line_no
                return False
            
            match = re.match(".*<r4>(.*?)</r4>",line) # rmat 1
            if match :
                self.rmat4 = int(match.group(1))
            else :
                print "Failure parsing rmat4 at line", line_no
                return False
            
            # line was parsed without Errors (At this stage not
            #  so many variables were sent down the link
            
            return True
            
        else:
            print "Unknown Telemetry Format at line", line_no
            return False

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
    for entry in log_book :
        # Calcuate our heading from Rmat readings.
        heading_radians = atan2(- entry.rmat1 , entry.rmat4)
        entry.heading_degrees = (heading_radians / (2 * pi)) * 360
        entry.lon  = entry.longitude / 10000000 # degrees
        entry.lat  = entry.latitude  / 10000000 # degrees
        entry.alt  = entry.altitude / 100       # meters absolute
        entry.pitch = (asin(entry.rmat7 / 16384.0) / (2*pi)) * 360 # degrees
        entry.roll =  (asin(entry.rmat6 / 16385.0) / (2*pi)) * 360

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
    <name>Flight Log """,
    print >> filename, flight_log_name,
    print >> filename, """</name>"""


def write_flight_path_preamble (log_book,filename):
  
    print >> filename, """"  <description>UAV Flight Path</description>"""
    write_style_urls(filename)
    print >> filename, "<Folder><name>Waypoint Routes</name>"
      
def write_flight_path_postamble(log_book,filename):
    print >> filename, """  </Folder>"""


def write_document_postamble(log_book,filename) :
    print >> filename, """
</Document>
</kml>
 """

def write_placemark_preamble_auto(open_waypoint,current_waypoint,filename):
    waypoints_open = 6  # The no. of waypoints to enable "on" in GE
                        # User can switch on others in places window in GE
                        # Otherwise too many line on screen can be confusing
                        # This feature not working yet.
    print >> filename, """
    <Placemark>
      <name>""",
    print >> filename, "Towards Waypoint: ", current_waypoint,
    print >> filename, """</name>"""
   
    if open_waypoint :
        print >>filename, "     <open>1</open>"
    else:
        print >> filename, "     <open>0</open>"
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
    
def write_placemark_preamble_manual(filename):
    print >> filename, """
    <Placemark>
      <name>Manual Mode</name>
      <description>Manual Mode</description>
          <styleUrl>#red</styleUrl>"
            <LineString>
            <extrude>1</extrude>
            <tessellate>1</tessellate>
            <altitudeMode>absolute</altitudeMode>
        <coordinates>"""

   
def write_T3_waypoints(filename,origin)  :
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
     print >> filename, """<Folder><open>1</open>
    <name>T3 Competition Course</name>
    <description>The T3 Competition Course from DIYDrones.com</description>"""
     for waypoint in waypoint_list :
         print >> filename, """   <Placemark> 
      <name>Waypoint X</name>
      <description>Waypoint</description>
       <Style id="default"></Style>
      <Model>
        <altitudeMode>absolute</altitudeMode>
        <Location>
           <longitude>""",
         print >> filename, waypoint[LON],
         print >> filename, """</longitude>
           <latitude>""",
         print >> filename, waypoint[LAT],
         print >> filename, """</latitude>
           <altitude>""",
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
    write_T3_waypoints(filename,flight_origin)
    first_waypoint = True
   
    open_waypoint = True      # We only open the first few waypoints in GE - to keep graphic clean
    max_waypoints_to_open = 9
    for entry in log_book :
        if entry.status == "1111" : # Auto Mode
            current_waypoint = entry.waypointIndex
            if current_waypoint > 9 : open_waypoint = False
            if first_waypoint :
                write_placemark_preamble_auto(open_waypoint,current_waypoint,filename)
                first_waypoint = False
                last_waypoint = current_waypoint
                last_status_auto = True
            elif last_status_auto == False : # previous entry manual mode
                write_placemark_postamble(filename)
                write_placemark_preamble_auto(open_waypoint,current_waypoint,filename)
                last_waypoint = current_waypoint
                last_status_auto = True
                
            # print intermediary points as we move towards waypoint
            line1 = "%f," % entry.lon
            line2 = "%f," % entry.lat
            line3 = "%f" %  entry.alt
            line = "          " + line1 + line2 + line3
            print >> filename, line
            if  current_waypoint  != last_waypoint :
                write_placemark_postamble(filename)
                write_placemark_preamble_auto(open_waypoint,current_waypoint,filename)
            last_waypoint = current_waypoint
            last_status_auto = True
        else :  # Manual Mode
            if first_waypoint :
                write_placemark_preamble_manual(filename)
                first_waypoint  = False
                last_status_auto = False
            if last_status_auto == True :
                write_placemark_postamble(filename)
                write_placemark_preamble_manual(filename)
                first_waypoint  = False
                last_status_auto = False
            else :
                # print intermediary points 
                line1 = "%f," % entry.lon
                line2 = "%f," % entry.lat
                line3 = "%f" %  entry.alt
                line = "          " + line1 + line2 + line3
                print >> filename, line
            last_status_auto = False
    write_placemark_postamble(filename)
    write_flight_path_postamble(log_book, filename)
 

def write_flight_vectors(log_book,origin, filename) :
#    print >> filename , """<?xml version="1.0" encoding="UTF-8"?>
#<kml xmlns="http://earth.google.com/kml/2.1">
#<Document>
    print >> filename, """
      <Folder>
	<name>Yaw Vectors""",
    print >> filename, flight_log_name ,
    print >> filename, "</name>"
    counter = 0
    for entry in log_book :
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
      <Model>
      <altitudeMode>absolute</altitudeMode>
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
            sum_latitude  += log_book[index].latitude
            sum_longitude += log_book[index].longitude
            sum_altitude  += log_book[index].altitude
            if (index  == initial_points) :
                self.latitude =  sum_latitude  /  initial_points
                self.longitude = sum_longitude /  initial_points
                self.altitude =  sum_altitude  /  initial_points
                if debug :
                    print "init_lat: ",self.latitude,"init_lon: ", \
                            self.longitude,"init_alt: ",self.altitude
        
def create_kmz(flight_log_dir,flight_log_name):
    flight_log = flight_log_dir + flight_log_name
    flight_pos = re.sub(".TXT$",".kml", flight_log_name)
    flight_pos_kml = flight_log_dir + flight_pos
    
    f = open(flight_log, 'r')
    f_pos = open(flight_pos_kml, 'w')
    line_no = 0
    log_book = []   # Log book is an emtpy list (of logs entries) initially
    for line in f :
        line_no += 1
        log = telemetry() # Make a new empty log entry
        result = log.parse(line,line_no)
        if result == False :
            print "Error parsing telemetry line ",line_no 
            continue  # Go get the next line  
        if debug : print "lat",log.latitude,"lon",log.longitude,"alt",log.altitude, \
            "wp", log.waypointIndex, "rmat1", log.rmat1
        if (log.latitude == 0 or log.longitude == 0 or log.altitude ==0 ):
            if debug: print "lat or long or alt is 0; ignoring line", line_no
            continue # Get next line of telemetry  - can happen at boot time on plane 
        else :
            # We have a good log entry - put it in the logbook.
            log_book.append(log)
            
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

    # Make up the KML files into KMZ files
    flight_kmz = re.sub(".TXT$",".kmz", flight_log_name)
    flight_pos_kmz = flight_log_dir + flight_kmz
    kmzfile = ZipFile(flight_pos_kmz, "w",ZIP_DEFLATED) # "a" to append, "r" to read
    kmzfile.write(flight_pos_kml)
    kmzfile.write(flight_log_dir + "models/" + "waypoint.dae")
    kmzfile.close()
    print "Program has converted file to ", flight_kmz
    # Remove the temporary kml files, now we have the kmz file
    os.remove(flight_pos_kml)


        
        


########## Start of the Main Program ##########
    
debug = 0 # set this to 1 of you want lot's of debug info to be printed.

instructions = "uav_log_to_kml.py:  Convert Generic UAV telemetry" + \
               "to Google Earth Files (kmz).You must specify the"  + \
               "file to convert by editing the source code of the" + \
               "program - usage:  Edit program using Python I.D.E"  + \
               "and run from there (for now)"

if __name__=="__main__":
    if len(sys.argv) == 1:
        print "instructions"
        ########################################################################
        # CHANGE ME - for your computer. Where are you keeping your flight logs ?
        # Please note the use of forward slashes is required on Windows OS
        flight_log_dir = \
         'C:/Documents and Settings/petholla/Desktop/uav/flight_analysis/flight_logs/'
        flight_log_name = 'flight17.TXT'
        ########################################################################
        mycolors = colors() # get a list of colors to use later
        print "Converting ..."
        create_kmz(flight_log_dir,flight_log_name)
    else:
        print instructions 
    

    
    
