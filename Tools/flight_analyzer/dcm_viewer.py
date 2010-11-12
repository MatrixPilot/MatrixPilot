""" A small tool to show the Direction Cosine Matrix Axes in 3D on the screen"""
# Display Direction Cosine Matrix in 3D axis viewer.
# Expects a serial link from UDB2/3 running at 19200 baud
# using telemetry format SERIAL_UDB or SERIAL_UDB_EXTRA.
# Copyright 2010 Pete Hollands
# Licenced for reuse under the Gnu Public License V3.
# Note: This program requires the user to install "visual python"

import serial
from time import time
import re
from visual import *
from matrixpilot_lib import telemetry
from matrixpilot_lib import matrix_multiply_3x3_3x1

def conv_udb_vpython(vector) :
    """Convert a vector from UDB coordinate reference to vpython reference"""
    # conversion from UDB coordinates to V python coordinates.
    # X axis is out of left wing in both coordinate systems.
    # Y is straight up in vpython, and is the negative of the UDB's Z axis,
    #   which points down.
    # Z in vpython coordinates is the Y axis in UDB coordinates.
    # So it turns out that one can covert from UDB to Vpython by means
    # of a 90 degree rotation matrix.
    vpython_rotate = (1,0,0,   0,0,-1,  0,1,0)
    return(matrix_multiply_3x3_3x1(vpython_rotate, vector))
    
# Setup the Serial Port Communications
ser = serial.Serial(7)  # open COM 8, Chane his for your setup.
print ser.portstr       # check which port was really used
print ser              
ser.baudrate = 19200    # UDB prints telemetry at 19200 baud.
print                   # print out the serial configuration

telemetry_line = telemetry() # instantiate the telemetry class
max_tm_actual = 0
line_no = 0

f_length = 50
fuselage_vector_udb = (0, 1, 0) # fuselage along Y axis of UDBcoordinate reference
fuselage_vector_vpython  = conv_udb_vpython(fuselage_vector_udb)
fuselage_plane = arrow(pos=(0, 0, 0), axis = fuselage_vector_vpython,
                       color  = color.white, length = f_length)

w_length = 50
left_wing_vector_udb = (1,0, 0)
left_wing_vector_vpython = conv_udb_vpython(left_wing_vector_udb)
left_wing      = arrow(pos=(0, 0, 0), axis = left_wing_vector_vpython,
                       color = color.orange, length = w_length)

right_wing_vector_udb = (-1,0,0)
right_wing_vector_vpython = conv_udb_vpython(right_wing_vector_udb)
right_wing     = arrow(pos=(0, 0, 0), axis = right_wing_vector_vpython,
                       color = color.orange, length = w_length)

x_axis = arrow(pos=(0,0,0), axis=(1,    0,   0), color = color.green, shaftwidth=1,length = 100)
y_axis = arrow(pos=(0,0,0), axis=( 0,  -1,   0), color = color.red,   shaftwidth=1,length = 100)
z_axis = arrow(pos=(0,0,0), axis=( 0,    0,  1), color = color.blue,  shaftwidth=1,length = 100)
rmat = [0,0,0,0,0,0,0,0,0] 
result = "Not set yet"
while (True) :
    try:
        line_no += 1
        line = ser.readline()
        
        result = telemetry_line.parse(line, line_no, max_tm_actual)
        print line
        
        if result == "F2" : # If received SERIAL_UDB or SERIAL_UDB_EXTRA format telemetry
            rmat[0] =   telemetry_line.rmat0 /16384.0
            rmat[1] =   telemetry_line.rmat1 /16384.0
            rmat[2] =   telemetry_line.rmat2 /16384.0
            rmat[3] =   telemetry_line.rmat3 /16384.0
            rmat[4] =   telemetry_line.rmat4 /16384.0
            rmat[5] =   telemetry_line.rmat5 /16384.0
            rmat[6] =   telemetry_line.rmat6 /16384.0
            rmat[7] =   telemetry_line.rmat7 /16384.0
            rmat[8] =   telemetry_line.rmat8 /16384.0
            fuselage_vector_rotated  = matrix_multiply_3x3_3x1(rmat,fuselage_vector_udb)
            fuselage_plane.axis  = conv_udb_vpython(fuselage_vector_rotated)
            fuselage_plane.length = f_length
            fuselage_plane.pos = (0,0,0)
            left_wing_vector_rotated = matrix_multiply_3x3_3x1(rmat,left_wing_vector_udb)
            left_wing.axis       = conv_udb_vpython(left_wing_vector_rotated)
            left_wing.length = w_length
            left_wing.pos = (0,0,0)
            right_wing_vector_rotated = matrix_multiply_3x3_3x1(rmat,right_wing_vector_udb)
            right_wing.axis       = conv_udb_vpython(right_wing_vector_rotated)
            right_wing.length = w_length
            right_wing.pos = (0,0,0)
    except:
        print "There has been a program exception."
        print "This has been caught by dcm_viewer's own code logic"
        print "That logic will now close the program, and close the serial port"
        ser.close()
        exit()
