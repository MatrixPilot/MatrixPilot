"""
  MAVProxy mixer gui module
  
"""

import mavutil, re, os, sys, threading, time

mpstate = None

sys.path.insert(0, os.path.join(os.path.dirname(os.path.realpath(__file__)), 'lib', 'mixer'))
sys.path.insert(0, os.path.join(os.path.dirname(os.path.realpath(__file__)), 'data', 'mixer'))

 
#import subMAVFunctionSettings as MAVFSettingsAPI

        
def name():
    '''return module name'''
    return "mixer"

def description():
    '''return module description'''
    return "mixer settings editor"

def cmd_mixer(args):
    '''mixer command'''
    return


import wx
from MainFrame import MainFrame

class pyFEdit(wx.App):
    def OnInit(self):
        self.m_frame = MainFrame(None)
        return True

    def start(self):
        self.m_frame.Show()
        self.m_frame.start()
        self.SetTopWindow(self.m_frame)


class mixer_gui_thread(threading.Thread):
    def __init__(self, mpstate):
        threading.Thread.__init__(self)
        
        self.mpstate = mpstate
        self._stop = threading.Event()

    def stop(self):
        print("mixer gui thread request stop")
        self._stop.set()

    def stopped(self):
        return not self.isAlive()
        
    def run(self):

        print("mixer gui thread starting")
        self._stop.clear()
                
        app = pyFEdit(0, self.mpstate)
#        app.RedirectStdio()
        app.SetOutputWindowAttributes("pyFEdit")
        app.start()
        app.MainLoop()

        print("mixer gui thread end")


def init(_mpstate):
    '''initialise module'''
    global mpstate
    mpstate = _mpstate

    mpstate.mixer_initialised = False

    #===========================================================================
    # try:
    #    mpstate.mixer_settings_path = os.path.join(os.path.realpath(__file__)), "data", "mixer", "Settings.xml")
    #    mpstate.Settings = FESettings.parse(mpstate.mixer_settings_path)
    # except:
    #    print("Could not load mixer settings at: " + settings_path)
    #    print("mixer editor not initialised or started")
    # else:
    #===========================================================================


    mpstate.mixgui = mixer_gui_thread(mpstate)
    mpstate.mixgui.start()
    
    mpstate.mixer_initialised = True
    print("mixer initialised")

def unload():
    '''unload module'''
    mpstate.mixer_initialised = False
    mpstate.mixer.stop()
    mpstate.mixer = None
        
def mavlink_packet(msg):
    '''handle an incoming mavlink packet'''

    if msg and msg.get_type() == "GLOBAL_POSITION_INT":
        try:
            vz = msg.vz   # vertical velocity in cm/s
            vz = int(-vz)
        except:
            print("decode global vz message fail")

    return
            
#===============================================================================
# 
# class healthcheck(threading.Thread):
#    def __init__(self, _mpstate):
#        threading.Thread.__init__(self)
#        self._stop = threading.Event()
#        global mpstate
#        mpstate = _mpstate
# 
#        
#    def run(mpstate):
#        while ( (not mpstate.status.exit) and (not self._stop.isSet()) ):
#            time.sleep(0.5)
#===============================================================================
        

