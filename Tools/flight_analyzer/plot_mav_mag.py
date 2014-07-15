from __future__ import division, print_function
import re
import sys
import os
from visual import *
import wx
import re

# A program by Peter Hollands, July 2014 to help visualize magnetometer offsets
# You will need to setup the full path to flight analyzer directory below:-
MY_PATH_TO_FLIGHT_ANALYZER = "/home/markw/markw_32/MPLABXprojects/newsvn/MatrixPilot_mw4fbw/Tools/flight_analyzer"
MY_FILE_NAME = "/home/markw/Copy/Cub-auav3-fbw-tests/magnetometer_testing/calDanceBody.RAW"

try:
    sys.path.insert(0, os.path.join(os.getcwd(), '..', 'MAVLink', 'mavlink', 'pymavlink'))
    os.environ['MAVLINK10'] = '1'
    import mavlinkv10 as mavlink
    import mavutil
except:
    print ("Not able to find Python MAVlink libraries")

class raw_mavlink_telemetry_file:
    """Model a mavlink file (one without local time stamps inserted)"""
    def __init__(self, filename, type_of_mavlink):
        if (type_of_mavlink == "SERIAL_MAVLINK_RAW"):
            self.m = mavutil.mavlink_connection(filename, notimestamps = True)
        elif (type_of_mavlink == "SERIAL_MAVLINK_TIMESTAMPS"):
            self.m = mavutil.mavlink_connection(filename, notimestamps = False)    
        else:
            print ("Error: Unknown Mavlink file type (not raw or timestamp).")
        self.total_mavlink_packets_received = 0
        self.dropped_mavlink_packets = 0
        self.first_packet_flag = True
        
    def __iter__(self):
        return(self)
        
    def next(self):
        """return the next good Raw IMU Record"""
        while True :
            self.msg = self.m.recv_match(blocking=False)
            if not self.msg:
                    # Reached end of file
                    print ("Total MAVLink Packets processed:"), self.total_mavlink_packets_received
                    print ("Packets dropped in transmission:"),  self.dropped_mavlink_packets
                    raise StopIteration
            elif self.msg.get_type() == "BAD_DATA":
                pass
            else :  # We have a good mavlink packet
                self.track_dropped_packets(self.msg.get_seq())
                
                if self.msg.get_type() == 'RAW_IMU' :
                    return self.msg
                else :
                        #print "Ignoring non SUE MAVLink message", self.msg.get_type()
                        pass
                    
    def close(self) :
        pass
    
    def track_dropped_packets(self,seq) :
        self.total_mavlink_packets_received += 1
        # print seq
        if (self.first_packet_flag == True) :
            self.last_seq = seq
            return
        else :
            # Mavlink seqence numbers are modulo 255
            if seq > 255 :
                print ("Error: Mavlink packet sequence number greater than 255")
                return
            elif ( seq == 0 )and ( self.last_seq == 255) :
                self.last_seq = seq
                return
            elif (seq == self.last_seq + 1 ) :
                self.last_seq = seq
                return
            else : # We have dropped packets
                if ( seq > self.last_seq ): # most likely case
                    self.dropped_mavlink_packets += seq - self.last_seq
                elif (self.last_seq > seq): # assumes no more than 255 packets drop in one go
                    self.dropped_mavlink_packets += (255 - self.last_seq )+ seq
                elif (self.last_seq == seq) :
                    self.dropped_mavlink_packets += 255 # best guess
            self.last_seq = seq
            return



def stop_plotting(evt) :
    cube.read_mag = False

def start_plotting(evt) :
    if cube.read_mag == False :
        cube.read_mag = True

def set_sphere_size(evt): # called on slider events
    ball.radius = s1.GetValue()

def set_sphere_pos(evt): # called on slider events
    sphere_x = s2.GetValue()
    sphere_y = s3.GetValue()
    sphere_z = s4.GetValue()
    ball.pos = (sphere_x,sphere_y,sphere_z)
    print(ball.pos)
   
L = 600
# Create a window. Note that w.win is the wxPython "Frame" (the window).
# window.dwidth and window.dheight are the extra width and height of the window
# compared to the display region inside the window. If there is a menu bar,
# there is an additional height taken up, of amount window.menuheight.
w = window(width=2*(L+window.dwidth), height=L+window.dheight+window.menuheight,
           menus=True, title='Widgets')

# Place a 3D display widget in the left half of the window.
d = 20
display(window=w, x=d, y=d, width=L-2*d, height=L-2*d, forward=-vector(0,1,2))
cube = box(color=color.red)
axis_len = 600
x_axis = arrow(pos=(-axis_len,0,0), axis=(2*axis_len,0,0), shaftwidth=1, color = color.red)
y_axis = arrow(pos=(0,-axis_len,0), axis=(0,2*axis_len,0), shaftwidth=1, color = color.green)
z_axis = arrow(pos=(0,0,-axis_len), axis=(0,0,2*axis_len), shaftwidth=1, color = color.yellow)
ball = sphere(pos=(0,0,0), radius=100, opacity=0.5)
# Place buttons, radio buttons, a scrolling text object, and a slider
# in the right half of the window. Positions and sizes are given in
# terms of pixels, and pos(0,0) is the upper left corner of the window.
p = w.panel # Refers to the full region of the window in which to place widgets

wx.StaticText(p, pos=(d,4), size=(L-2*d,d), label='PDH Magnetometer Plot',
              style=wx.ALIGN_CENTRE | wx.ST_NO_AUTORESIZE)

left = wx.Button(p, label='Stop Plotting', pos=(L+10,15))
left.Bind(wx.EVT_BUTTON, stop_plotting)
right = wx.Button(p, label='Start Plotting', pos=(1.5*L+10,15))
right.Bind(wx.EVT_BUTTON, start_plotting)

min_pos = -500
max_pos = +500
s1 = wx.Slider(p, pos=(1.0*L,0.8*L), size=(0.9*L,20), minValue=0, maxValue=1000)
s1.Bind(wx.EVT_SCROLL, set_sphere_size)
wx.StaticText(p, pos=(1.0*L,0.75*L), label='Set sphere size')

s2 = wx.Slider(p, pos=(1.0*L,0.7*L), size=(0.9*L,20), minValue=min_pos, maxValue=max_pos)
s2.Bind(wx.EVT_SCROLL, set_sphere_pos)
wx.StaticText(p, pos=(1.0*L,0.65*L), label='Set sphere on X axis')

s3 = wx.Slider(p, pos=(1.0*L,0.6*L), size=(0.9*L,20), minValue=min_pos, maxValue=max_pos)
s3.Bind(wx.EVT_SCROLL, set_sphere_pos)
wx.StaticText(p, pos=(1.0*L,0.55*L), label='Set sphere on Y axis')

s4 = wx.Slider(p, pos=(1.0*L,0.5*L), size=(0.9*L,20), minValue=min_pos, maxValue=max_pos)
s4.Bind(wx.EVT_SCROLL, set_sphere_pos)
wx.StaticText(p, pos=(1.0*L,0.45*L), label='Set sphere on Z axis')

# A VPython program that uses these wxPython capabilities should always end
# with an infinite loop containing a rate statement, as future developments
# may require this to keep a display active.


cube.read_mag = True
mag_point = [0,0,0]
mag_points = []
       
os.chdir(MY_PATH_TO_FLIGHT_ANALYZER)
mav_stream = raw_mavlink_telemetry_file(MY_FILE_NAME,"SERIAL_MAVLINK_RAW")
print ("hello, setting up MAVLink magnetometer points ...")

for mav_nugget in mav_stream :
    #print (mav_nugget.xmag,",",mav_nugget.ymag,",",mav_nugget.zmag)
    mag_point = (mav_nugget.xmag,mav_nugget.ymag,mav_nugget.zmag)
    mag_points.append(mag_point)

points(pos=mag_points, size=5, color=color.green) 
#ball = sphere(pos=(0,0,0), radius=s1.GetValue(), opacity=0.5)
while True:
    rate(100)
 

        
