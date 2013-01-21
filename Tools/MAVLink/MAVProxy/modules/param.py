"""
  MAVProxy mixer gui module
  
"""

import mavutil, re, os, sys, threading, time

mpstate = None

sys.path.insert(0, os.path.join(os.path.dirname(os.path.realpath(__file__)), 'lib', 'param'))
sys.path.insert(0, os.path.join(os.path.dirname(os.path.realpath(__file__)), 'data', 'param'))

import MAVlinkParameterProcesses as MAVLinkProcesses
 
#import subMAVFunctionSettings as MAVFSettingsAPI

        
def name():
    '''return module name'''
    return "param"

def description():
    '''return module description'''
    return "param settings editor"

def cmd_mixer(args):
    '''param command'''
    return


import wx
from MainFrame import MainFrame

from optparse import OptionParser

class MyOptionParser(OptionParser):
    def error(self, msg):
        pass

class paramApp(wx.App):
#    def __init__( self, mpstate):
#        self.mpstate = mpstate
#        wx.PyApp.__init__(self)

    def set_mpstate(self, mpstate):
        self.mpstate = mpstate
        self.MAVProc.set_mpstate( mpstate )
        
    def OnInit(self):
        #=======================================================================
        # args = sys.argv[1:]
        # print(args)
        # self.aircraft = filter(lambda x: '--aircraft' in x,args)
        #        
        # if(self.aircraft is None):
        #    self.aircraft = ""
        # 
        # self.mix_doc = mixer_document(self.aircraft)
        #=======================================================================
        
#        parser = MyOptionParser("mavproxy.py [options]")
#===============================================================================
#        parser = MyOptionParser()
#        parser.add_option("--aircraft", dest="aircraft", help="aircraft name", default=None)
#        (opts, args) = parser.parse_args()
# 
#        if opts.aircraft is not None:
#            self.mix_doc = mixer_document(opts.aircraft)
#        else:
#            self.mix_doc = mixer_document("")
#===============================================================================
            
        self.m_frame = MainFrame(None)
        self.MAVProc = MAVLinkProcesses.mavlink_parameter_processes(self.m_frame)
        self.m_frame.Show()
        self.SetTopWindow(self.m_frame)
        return True
    
    def stop(self):
         self.MAVProc.stop_services();


class param_app_thread(threading.Thread):
    def __init__(self, mpstate):
        threading.Thread.__init__(self)
        
        self.mpstate = mpstate

    def stopped(self):
        return not self.isAlive()
        
    def run(self):

        print("param app thread starting")
                
        self.param_app = paramApp(0)
#        self.mixer_app.set_mpstate(self.mpstate)
#        app.RedirectStdio()
#        self.mixer_app.SetOutputWindowAttributes("pyFEdit")

        self.param_app.set_mpstate(self.mpstate)
        
        self.mpstate.param_initialised = True
        print("param initialised")

        self.param_app.MainLoop()
        
        print("stopping app")
        self.param_app.stop()

        print("param app thread end")
        self.mpstate.param_initialised = False
        

def init(_mpstate):
    '''initialise module'''
    global mpstate
    mpstate = _mpstate

    mpstate.param_initialised = False

    mpstate.paramapp = param_app_thread(mpstate)
    mpstate.paramapp.start()
    

def unload():
    '''unload module'''
    mpstate.mixer_initialised = False
    mpstate.mixer.stop()
    mpstate.mixer = None
        
def mavlink_packet(msg):
    '''handle an incoming mavlink packet'''

    if(mpstate.mixer_initialised == True):
        mpstate.mixgui.mixer_app.MAVProc.msg_recv(msg)

            
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
        

