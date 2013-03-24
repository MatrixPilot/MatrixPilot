"""
  MAVProxy vario module
  Uses fluidsynth and soundfonts driving alsa
"""

import mavutil, re, os, sys, threading, time

mpstate = None

try:
    import winsound
except ImportError:
    import os
    def playsound(frequency,duration):
        #apt-get install beep
        os.system('beep -f %s -l %s' % (frequency,duration))
else:
    def playsound(frequency,duration):
        winsound.Beep(frequency,duration)


class status_soundgen(object):
    def __init__(self, mpstate):
        self.mpstate = mpstate
        
        self.unload = threading.Event()
        self.unload.clear()
        
        self.timeout_time = 0
        
        self.play_link_down = threading.Event()
        self.play_link_lost = threading.Event()
        self.play_link_ok = threading.Event()
        
        self.last_play_time = time.time()

        self.sounds_thread = threading.Thread(target=self.announce_sounds_app)
        self.sounds_thread.daemon = True
        self.sounds_thread.start()

            
    def announce_sounds_app(self):
        while(not self.unload.is_set() and (not mpstate.status.exit) ):
            
            if( self.play_link_lost.is_set() ):     #mpstate.status_sounds
                playsound(500,200)
                playsound(300,200)
                self.last_play_time = time.time()
                self.play_link_lost.clear()
            if( self.play_link_ok.is_set()):
                playsound(700,200)
                playsound(1000,200)
                self.last_play_time = time.time()
                self.play_link_ok.clear()
            time.sleep(0.5)
#            playsound(1500,200)
 
        
def name():
    '''return module name'''
    return "vario"

def description():
    '''return module description'''
    return "voice and sound announcement"

def cmd_status_sounds(args):
    '''announce command'''
    return 0

    

#===============================================================================
#    state = mpstate.graph_state
# 
#    if len(args) == 0:
#        # list current graphs
#        for i in range(len(state.graphs)):
#            print("Graph %u: %s" % (i, state.graphs[i].fields))
#        return
# 
#    # start a new graph
#    state.graphs.append(Graph(args[:]))
#===============================================================================


def init(_mpstate):
    '''initialise module'''
    global mpstate
    mpstate = _mpstate

    mpstate.status_sound = status_soundgen(mpstate)
    
    mpstate.command_map['status_sound'] = (cmd_status_sounds, "status sounds settings adjust")


def unload():
    '''unload module'''
    mpstate.status_sound.unload.set()
        
def idle_task():
    now = time.time()
    if(now > mpstate.status_sound.last_play_time + 1):
        if(mpstate.status.heartbeat_error == 1):
            mpstate.status_sound.play_link_lost.set()
    
