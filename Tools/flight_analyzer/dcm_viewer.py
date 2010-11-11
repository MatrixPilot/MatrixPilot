""" A small tool to show the Direction Cosine Matrix Axes in 3D on the screen"""
# Author: Pete Holalnds
# Copyright 2010
# Licenced for reuse under the Gnu Public License V3.
# Note: This program requires the user to install "visual python"

import serial
from time import time
import re
from visual import *
from matrixpilot_lib import telemetry

def matrix_transpose(a) :
    """ Perform the matrix transpose operation on a 3x3 matrix
        and negate the angle terms to change rotation direction"""
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

# Setup the Serial Port Communications
ser = serial.Serial(7)  # open COM 8, Chane his for your setup.
print ser.portstr       # check which port was really used
print ser              
ser.baudrate = 19200    # UDB prints telemetry at 19200 baud.
print                   # print out the serial configuration

telemetry_line = telemetry() # instantiate the telemetry class
max_tm_actual = 0
line_no = 0


z_plane = arrow(pos=(0,0,-5000), axis=(    0,    0, 16384), color = color.white,  shaftwidth=1,length = 16384)
main_wing  = box(pos=(0, 500,    0), axis=(14000,0,0), length=14000, height=500, width=2000) 
tail_plane = box(pos=(0,1000,-5000), axis=(3000 ,0,0), length= 3000, height=500, width=1000)

x_axis = arrow(pos=(0,0,0), axis=(16384,    0,     0), color = color.green, shaftwidth=1,length = 16384)
y_axis = arrow(pos=(0,0,0), axis=(    0,-16384,     0), color = color.red,   shaftwidth=1,length = 16384)
z_axis = arrow(pos=(0,0,0), axis=(    0,    0,16384), color = color.blue,  shaftwidth=1,length = 16384)
vpyth = [0,0,0,0,0,0,0,0,0] # Direction Cosine Matrix in the Visual Python Coordinate Sysetm.
while (1) :
    line_no += 1
    line = ser.readline()
    result = telemetry_line.parse(line, line_no, max_tm_actual)
    #print line
    print telemetry_line.rmat3, telemetry_line.rmat4, telemetry_line.rmat5
    #### conversion from UDB coordinates to V python coordinates.
    # X axis is out of left wing in both coordinate systems.
    # Y is straight up in vpython, and is the negative of the UDB's Z axis,
    #   which points down.
    # Z in vpython coordinates is the Y axis in UDB coordinates.
    
    if result == "F2" :
        # Convert from UDB coordinates, to V Python Coordinates.
        vpyth[0] =   telemetry_line.rmat0
        vpyth[1] =  -telemetry_line.rmat2
        vpyth[2] =   telemetry_line.rmat1
        vpyth[3] =   telemetry_line.rmat6
        vpyth[4] =  -telemetry_line.rmat8
        vpyth[5] =   telemetry_line.rmat7
        vpyth[6] =   telemetry_line.rmat3
        vpyth[7] =  -telemetry_line.rmat5
        vpyth[8] =   telemetry_line.rmat4

        pyth = matrix_transpose(vpyth)
        
        x_axis.axis = (vpyth[0],vpyth[1],vpyth[2])
        y_axis.axis = (vpyth[3],vpyth[4],vpyth[5])
        z_axis.axis = (vpyth[6],vpyth[7],vpyth[8])
    
ser.close()

