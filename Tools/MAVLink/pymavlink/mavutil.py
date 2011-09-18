#!/usr/bin/env python
'''
mavlink python utility functions

Copyright Andrew Tridgell 2011
Released under GNU GPL version 3 or later
'''

import socket, math, struct, time, os, fnmatch, array
from math import *
from mavextra import *

def evaluate_expression(expression, vars):
    '''evaluation an expression'''
    try:
        v = eval(expression, globals(), vars)
    except NameError:
        return None
    return v

def evaluate_condition(condition, vars):
    '''evaluation a conditional (boolean) statement'''
    if condition is None:
        return True
    v = evaluate_expression(condition, vars)
    if v is None:
        return False
    return v


class mavfile(object):
    '''a generic mavlink port'''
    def __init__(self, fd, address, source_system=255):
        import mavlink
        self.fd = fd
        self.address = address
        self.messages = { 'MAV' : self,
                          'HOME' : mavlink.MAVLink_gps_raw_message(0,0,0,0,0,0,0,0,0) }
        self.params = {}
        self.mav = None
        self.target_system = 0
        self.target_component = 0
        self.mav = mavlink.MAVLink(self, srcSystem=source_system)
        self.mav.robust_parsing = True
        self.logfile = None
        self.logfile_raw = None
        self.param_fetch_in_progress = False
        self.param_fetch_complete = False
        self.start_time = time.time()
        self.flightmode = "UNKNOWN"
        self.timestamp = 0

    def recv(self):
        '''default recv method'''
        raise RuntimeError('no recv() method supplied')

    def write(self, buf):
        '''default write method'''
        raise RuntimeError('no write() method supplied')

    def pre_message(self):
        '''default pre message call'''
        return

    def post_message(self, msg):
        '''default post message call'''
        msg._timestamp = time.time()
        type = msg.get_type()
        self.messages[type] = msg
        self.timestamp = msg._timestamp
        if type == 'HEARTBEAT':
            self.target_system = msg.get_srcSystem()
            self.target_component = msg.get_srcComponent()
        elif type == 'PARAM_VALUE':
            self.params[str(msg.param_id)] = msg.param_value
            if msg.param_index+1 == msg.param_count:
                self.param_fetch_in_progress = False
                self.param_fetch_complete = True
        elif type == 'SYS_STATUS':
            self.flightmode = mode_string(msg.mode, msg.nav_mode)
        elif type == 'GPS_RAW':
            if self.messages['HOME'].fix_type < 2:
                self.messages['HOME'] = msg


    def recv_msg(self):
        '''message receive routine'''
        self.pre_message()
        while True:
            s = self.recv()
            if len(s) == 0 and len(self.mav.buf) == 0:
                return None
            if self.logfile_raw:
                self.logfile_raw.write(str(s))
            msg = self.mav.parse_char(s)
            if msg:
                self.post_message(msg)
                return msg
                
    def recv_match(self, condition=None, type=None, blocking=False):
        '''recv the next MAVLink message that matches the given condition'''
        while True:
            m = self.recv_msg()
            if m is None:
                if blocking:
                    time.sleep(0.01)
                    continue
                return None
            if type is not None and type != m.get_type():
                continue
            if not evaluate_condition(condition, self.messages):
                continue
            return m

    def setup_logfile(self, logfile, mode='w'):
        '''start logging to the given logfile, with timestamps'''
        self.logfile = open(logfile, mode=mode)

    def setup_logfile_raw(self, logfile, mode='w'):
        '''start logging raw bytes to the given logfile, without timestamps'''
        self.logfile_raw = open(logfile, mode=mode)

    def wait_heartbeat(self, blocking=True):
        '''wait for a heartbeat so we know the target system IDs'''
        return self.recv_match(type='HEARTBEAT', blocking=blocking)

    def param_fetch_all(self):
        '''initiate fetch of all parameters'''
        if time.time() - getattr(self, 'param_fetch_start', 0) < 2.0:
            # don't fetch too often
            return
        self.param_fetch_start = time.time()
        self.param_fetch_in_progress = True
        self.mav.param_request_list_send(self.target_system, self.target_component)

    def time_since(self, mtype):
        '''return the time since the last message of type mtype was received'''
        if not mtype in self.messages:
            return time.time() - self.start_time
        return time.time() - self.messages[mtype]._timestamp

class mavserial(mavfile):
    '''a serial mavlink port'''
    def __init__(self, device, baud=115200, autoreconnect=False, source_system=255):
        import serial
        self.baud = baud
        self.device = device
        self.autoreconnect = autoreconnect
        self.port = serial.Serial(self.device, self.baud, timeout=0,
                                  dsrdtr=False, rtscts=False, xonxoff=False)

        try:
            fd = self.port.fileno()
        except Exception:
            fd = None
        mavfile.__init__(self, fd, device, source_system=source_system)

    def recv(self):
        n = self.mav.bytes_needed()
        if self.fd is None:
            waiting = self.port.inWaiting()
            if waiting < n:
                n = waiting
        return self.port.read(n)

    def write(self, buf):
        try:
            return self.port.write(buf)
        except OSError:
            if self.autoreconnect:
                self.reset()
            return -1
            
    def reset(self):
        import serial
        self.port.close()
        while True:
            try:
                self.port = serial.Serial(self.device, self.baud, timeout=1,
                                          dsrdtr=False, rtscts=False, xonxoff=False)
                try:
                    self.fd = self.port.fileno()
                except Exception:
                    self.fd = None
                return
            except Exception:
                print("Failed to reopen %s" % self.device)
                time.sleep(1)
        

class mavudp(mavfile):
    '''a UDP mavlink socket'''
    def __init__(self, device, input=True, source_system=255):
        a = device.split(':')
        if len(a) != 2:
            print("UDP ports must be specified as host:port")
            sys.exit(1)
        self.port = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.udp_server = input
        if input:
            self.port.bind((a[0], int(a[1])))
        else:
            self.destination_addr = (a[0], int(a[1]))
        self.port.setblocking(0)
        self.last_address = None
        mavfile.__init__(self, self.port.fileno(), device, source_system=source_system)

    def recv(self):
        try:
            data, self.last_address = self.port.recvfrom(300)
        except socket.error as e:
            if e.errno == 11:
                return ""
            raise
        return data

    def write(self, buf):
        try:
            if self.udp_server:
                if self.last_address:
                    self.port.sendto(buf, self.last_address)
            else:
                self.port.sendto(buf, self.destination_addr)
        except socket.error:
            pass

    def recv_msg(self):
        '''message receive routine for UDP link'''
        self.pre_message()
        s = self.recv()
        if len(s) == 0:
            return None
        msg = self.mav.decode(s)
        if msg:
            self.post_message(msg)
        return msg


class mavlogfile(mavfile):
    '''a MAVLink logfile reader/writer'''
    def __init__(self, filename, planner_format=None,
                 write=False, append=False,
                 robust_parsing=True, notimestamps=False, source_system=255):
        self.filename = filename
        self.writeable = write
        self.robust_parsing = robust_parsing
        self.planner_format = planner_format
        self.notimestamps = notimestamps
        self._two64 = math.pow(2.0, 63)
        if planner_format is None and self.filename.endswith(".tlog"):
            self.planner_format = True
        mode = 'rb'
        if self.writeable:
            if append:
                mode = 'ab'
            else:
                mode = 'wb'
        self.f = open(filename, mode)
        mavfile.__init__(self, None, filename, source_system=source_system)

    def recv(self):
        return self.f.read(self.mav.bytes_needed())

    def pre_message(self):
        '''read timestamp if needed'''
        # read the timestamp
        if self.notimestamps:
            return
        if self.planner_format:
            tbuf = self.f.read(21)
            if len(tbuf) != 21 or tbuf[0] != '-' or tbuf[20] != ':':
                raise RuntimeError('bad planner timestamp %s' % tbuf)
            hnsec = self._two64 + float(tbuf[0:20])
            t = hnsec * 1.0e-7         # convert to seconds
            t -= 719163 * 24 * 60 * 60 # convert to 1970 base
        else:
            tbuf = self.f.read(8)
            if len(tbuf) != 8:
                return
            (tusec,) = struct.unpack('>Q', tbuf)
            t = tusec * 1.0e-6
        self._timestamp = t

    def post_message(self, msg):
        '''add timestamp to message'''
        # read the timestamp
        super(mavlogfile, self).post_message(msg)
        if self.notimestamps:
            msg._timestamp = time.time()
        else:
            msg._timestamp = self._timestamp
        if self.planner_format:
            self.f.read(1) # trailing newline
        self.timestamp = msg._timestamp

def mavlink_connection(device, baud=115200, source_system=255,
                       planner_format=None, write=False, append=False,
                       robust_parsing=True, notimestamps=False, input=True):
    '''make a serial or UDP mavlink connection'''
    if device.find(':') != -1:
        return mavudp(device, source_system=source_system, input=input)
    if os.path.isfile(device):
        return mavlogfile(device, planner_format=planner_format, write=write,
                          append=append, robust_parsing=robust_parsing, notimestamps=notimestamps,
                          source_system=source_system)
    return mavserial(device, baud=baud, source_system=source_system)

class periodic_event(object):
    '''a class for fixed frequency events'''
    def __init__(self, frequency):
        self.frequency = float(frequency)
        self.last_time = time.time()
    def trigger(self):
        '''return True if we should trigger now'''
        tnow = time.time()
        if self.last_time + (1.0/self.frequency) <= tnow:
            self.last_time = tnow
            return True
        return False


try:
    from curses import ascii
    have_ascii = True
except:
    have_ascii = False

def is_printable(c):
    '''see if a character is printable'''
    global have_ascii
    if have_ascii:
        return ascii.isprint(c)
    return ord(c) >= 32 and ord(c) <= 126

def all_printable(buf):
    '''see if a string is all printable'''
    for c in buf:
        if not is_printable(c) and not c in ['\r', '\n', '\t']:
            return False
    return True

class SerialPort(object):
    '''auto-detected serial port'''
    def __init__(self, device, description=None, hwid=None):
        self.device = device
        self.description = description
        self.hwid = hwid

    def __str__(self):
        ret = self.device
        if self.description is not None:
            ret += " : " + self.description
        if self.hwid is not None:
            ret += " : " + self.hwid
        return ret

def auto_detect_serial_win32(preferred='*'):
    '''try to auto-detect serial ports on win32'''
    try:
        import scanwin32
        list = sorted(scanwin32.comports())
    except:
        return []
    ret = []
    for order, port, desc, hwid in list:
        if fnmatch.fnmatch(desc, preferred) or fnmatch.fnmatch(hwid, preferred):
            ret.append(SerialPort(port, description=desc, hwid=hwid))
    if len(ret) > 0:
        return ret
    # now the rest
    for order, port, desc, hwid in list:
        ret.append(SerialPort(port, description=desc, hwid=hwid))
    return ret
        

        

def auto_detect_serial_unix(preferred='*'):
    '''try to auto-detect serial ports on win32'''
    import glob
    list = glob.glob('/dev/ttyS*') + glob.glob('/dev/ttyUSB*') + glob.glob('/dev/ttyACM*') + glob.glob('/dev/serial/by-id/*')
    ret = []
    # try preferred ones first
    for d in list:
        if fnmatch.fnmatch(d, preferred):
            ret.append(SerialPort(d))
    if len(ret) > 0:
        return ret
    # now the rest
    for d in list:
        ret.append(SerialPort(d))
    return ret



def auto_detect_serial(preferred='*'):
    '''try to auto-detect serial port'''
    # see if 
    if os.name == 'nt':
        return auto_detect_serial_win32(preferred=preferred)
    return auto_detect_serial_unix(preferred=preferred)

def mode_string(mode, nav_mode):
    '''work out autopilot mode'''
    MAV_MODE_UNINIT = 0
    MAV_MODE_MANUAL = 2
    MAV_MODE_GUIDED = 3
    MAV_MODE_AUTO = 4
    MAV_MODE_TEST1 = 5
    MAV_MODE_TEST2 = 6
    MAV_MODE_TEST3 = 7

    MAV_NAV_GROUNDED = 0
    MAV_NAV_LIFTOFF = 1
    MAV_NAV_HOLD = 2
    MAV_NAV_WAYPOINT = 3
    MAV_NAV_VECTOR = 4
    MAV_NAV_RETURNING = 5
    MAV_NAV_LANDING = 6
    MAV_NAV_LOST = 7
    MAV_NAV_LOITER = 8
    
    cmode = (mode, nav_mode)
    mapping = {
        (MAV_MODE_UNINIT, MAV_NAV_GROUNDED)  : "INITIALISING",
        (MAV_MODE_MANUAL, MAV_NAV_VECTOR)    : "MANUAL",
        (MAV_MODE_TEST3,  MAV_NAV_VECTOR)    : "CIRCLE",
        (MAV_MODE_GUIDED, MAV_NAV_VECTOR)    : "GUIDED",
        (MAV_MODE_TEST1,  MAV_NAV_VECTOR)    : "STABILIZE",
        (MAV_MODE_TEST2,  MAV_NAV_LIFTOFF)   : "FBWA",
        (MAV_MODE_AUTO,   MAV_NAV_WAYPOINT)  : "AUTO",
        (MAV_MODE_AUTO,   MAV_NAV_RETURNING) : "RTL",
        (MAV_MODE_AUTO,   MAV_NAV_LOITER)    : "LOITER",
        (MAV_MODE_AUTO,   MAV_NAV_LIFTOFF)   : "TAKEOFF",
        (MAV_MODE_AUTO,   MAV_NAV_LANDING)   : "LANDING",
        (100,             MAV_NAV_VECTOR)    : "STABILIZE",
        (102,             MAV_NAV_VECTOR)    : "SIMPLE",
        (101,             MAV_NAV_VECTOR)    : "ACRO",
        (103,             MAV_NAV_VECTOR)    : "ALT_HOLD",
        (MAV_MODE_AUTO,   MAV_NAV_HOLD)      : "LOITER",
        }
    if cmode in mapping:
        return mapping[cmode]
    return "Mode(%s,%s)" % cmode

class x25crc(object):
    '''x25 CRC - based on checksum.h from mavlink library'''
    def __init__(self, buf=''):
        self.crc = 0xffff
        self.accumulate(buf)

    def accumulate(self, buf):
        '''add in some more bytes'''
        bytes = array.array('B')
        if isinstance(buf, array.array):
            bytes.extend(buf)
        else:
            bytes.fromstring(buf)
        accum = self.crc
        for b in bytes:
            tmp = b ^ (accum & 0xff)
            tmp = (tmp ^ (tmp<<4)) & 0xFF
            accum = (accum>>8) ^ (tmp<<8) ^ (tmp<<3) ^ (tmp>>4)
            accum = accum & 0xFFFF
        self.crc = accum
