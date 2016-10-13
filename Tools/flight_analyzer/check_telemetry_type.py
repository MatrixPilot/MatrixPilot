#  This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation,  version 3 of the License, or
#    (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.

#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#  Author: Peter Hollands, Copyright Peter Hollands  2012

import re
import sys
import os
import array, struct


try:
    sys.path.insert(0, os.path.join(os.getcwd(), '..', 'MAVLink', 'mavlink'))
    os.environ['MAVLINK10'] = '1'
    import pymavlink.dialects.v10.matrixpilot as mavlink
    import pymavlink.mavutil as mavutil 
except:
    print "Not able to find Python MAVlink libraries"

def check_type_of_telemetry_file(filename):
    """Find out the type of a telemetry file. For example:-
       Is is an Ascii file, a binary raw mavlink file, or a mavlink
       file with timestamps ? The routine returns one of:-
       MAVLINK 1.0 RAW
       MAVLINK 1.0 TIMESTAMPS
       MAVLINK UNKNOWN
       ASCII
       UNKNOWN"""

    try:
        fd = open(filename ,"rb")
    except:
        print  "Error: Could not open", filename
        return "Error opening file"
    ##### Pseudo code
    # Read in 1000 bytes of a file 
    # Check first for presence of 0xfe Mavlink 1.0 marker and then check CRC of message.
    # Note positions of 0xfe, end of valid message, start of next valid message. etc.
    # If all messages back to back, then this is a raw MAVLink file.
    # If all messages back to back with some same space (timestamp), then timestamp file.
    # if No valid MAVLink messages, examine as ASCII. If pure ascii then return Ascii.
    mybuffer = fd.read(1000)
    bytes = array.array('B')
    if isinstance(mybuffer, array.array):
        bytes.extend(mybuffer)
    else:
        bytes.fromstring(mybuffer)

    # Find out if this buffer has valid MAVLink packets
    number_of_valid_mavlink_packets = 0
    mavlink_parser_states = ['looking_for_start_char','found_start_char','valid_mav_packet' \
                             'non_valid_mav_packet']
    state = 'looking_for_start_char'
    parsing_index = 0
    last_end_of_packet_index = 0
    number_of_packet_gaps = 0
    packet_gaps = 0
    while parsing_index < (len(bytes)-(255+8)) : # At least one MAVlink packet left  
        if state == 'looking_for_start_char' :
            if bytes[parsing_index] == 254 :
                state = 'found_start_char'
            else:
                parsing_index += 1
        elif state == "found_start_char" :
            # calculate crc
            crc_is_ok = False
            payload_length = bytes[parsing_index + 1]
            if payload_length > 255 :
                # Nonesensical payload length. Forget it.
                state == 'looking_for_start_char'
                continue
            seq = bytes[parsing_index + 2]
            
            mycrc = mavutil.x25crc()
            if (parsing_index + payload_length + 7) < len(bytes) :
                mycrc.accumulate(bytes[(parsing_index + 1 ):(parsing_index + payload_length + 6)])
                msgId = bytes[parsing_index + 5]
                mymessage = mavlink.mavlink_map[msgId]
                mycrc.accumulate(struct.pack('B',mymessage.crc_extra))
                
                low_byte_sent_crc = bytes[parsing_index + payload_length + 6]
                high_byte_sent_crc = bytes[parsing_index + payload_length + 7]
                total_sent_crc = low_byte_sent_crc + ( high_byte_sent_crc * 256)
                if mycrc.crc == total_sent_crc :
                    state = 'valid_mav_packet'
                    if number_of_valid_mavlink_packets != 0 : # This is not the very first packet
                        packet_gaps +=  parsing_index  - (last_end_of_packet_index + 1)
                        number_of_packet_gaps += 1
                    last_end_of_packet_index = parsing_index + payload_length + 7
                    # Move parser onto expected start of next packet.
                    parsing_index = parsing_index + payload_length + 8
                else :
                    state = 'non_valid_mav_packet'
                    # Look for next valid 0xfe marker for start of packet
            else:
                # We are out of characters in our buffer for testing for packets
                break
        elif state == "valid_mav_packet" :
            number_of_valid_mavlink_packets += 1
            # Do something - like make a note of end of position of end of packet
            state = 'looking_for_start_char'
        elif state == 'non_valid_mav_packet' :
            print "Found a potential non valid mavlink packet"
            state = 'looking_for_start_char'
            parsing_index += 1
        else :
            print "Invalid state when parsing mavlink to check type of telemetry"
            parsing_index += 1
    if number_of_valid_mavlink_packets >= 1 :
        if number_of_packet_gaps > 0 :   
            gap_average = packet_gaps / number_of_packet_gaps
        else :
            gap_average = 0
        #print "Average number of bytes between valid packets is", gap_average
        if (gap_average < 0.5 ) and ( gap_average <= 0 ) :
            return "MAVLINK 1.0 RAW"
        elif (gap_average  > 7.5) and ( gap_average < 8.5 ) :
            return "MAVLINK 1.0 TIMESTAMPS"
        else :
            return "MAVLINK UNKNOWN"
    else :
        parser_index = 0
        number_of_ascii_chars = 0
        while parser_index < len(bytes) :
            this_byte = bytes[parser_index]
            if ( this_byte >= 32 ) and ( this_byte < 128 ) : # space char through to DEL char
                number_of_ascii_chars += 1
            elif this_byte == 13  or this_byte == 10 :  # CR and NL
                number_of_ascii_chars += 1
            else :
                pass
            parser_index += 1
        if len(bytes) > 0 :
            average_ascii_chars = number_of_ascii_chars / float(len(bytes))
        else :
            average_ascii_chars = 0
        #print "This file is", average_ascii_chars * 100, "percent ascii"
        if average_ascii_chars > 0.98 :
            return "ASCII"
        else :
            return "UNKNOWN"
    return "ERROR while checking file types"
    
