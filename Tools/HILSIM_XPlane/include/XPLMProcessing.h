#ifndef _XPLMProcessing_h_
#define _XPLMProcessing_h_

/*
 * Copyright 2005 Sandy Barbour and Ben Supnik
 * 
 * All rights reserved.  See license.txt for usage.
 * 
 * X-Plane SDK Version: 1.0.2                                                  
 *
 */

/*
 * This API allows you to get regular callbacks during the flight loop, the 
 * part of X-Plane where the plane's position calculates the physics of 
 * flight, etc.  Use these APIs to accomplish periodic tasks like logging data 
 * and performing I/O. 
 * 
 * WARNING: Do NOT use these callbacks to draw!  You cannot draw during flight 
 * loop callbacks.  Use the drawing callbacks (see XPLMDisplay for more info) 
 * for graphics.                                                               
 *
 */

#include "XPLMDefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * FLIGHT LOOP CALLBACKS
 ***************************************************************************/
/*
 *                                                                             
 *
 */

/*
 * XPLMFlightLoop_f
 * 
 * This is your flight loop callback.  Each time the flight loop is iterated 
 * through, you receive this call at the end.  You receive a time since you 
 * were last called and a time since the last loop, as well as a loop counter. 
 * The 'phase' parameter is deprecated and should be ignored.				 
 * 
 * Your return value controls when you will next be called. Return 0 to stop 
 * receiving callbacks.  Pass a positive number to specify how many seconds 
 * until the next callback.  (You will be called at or after this time, not 
 * before.)  Pass a negative number to specify how many loops must go by until 
 * you are called.  For example, -1.0 means call me the very next loop.  Try 
 * to run your flight loop as infrequently as is practical, and suspend it 
 * (using return value 0) when you do not need it; lots of flight loop 
 * callbacks that do nothing lowers x-plane's frame rate. 
 * 
 * Your callback will NOT be unregistered if you return 0; it will merely be 
 * inactive. 
 * 
 * The reference constant you passed to your loop is passed back to you.       
 *
 */
typedef float (* XPLMFlightLoop_f)(
                                   float                inElapsedSinceLastCall,    
                                   float                inElapsedTimeSinceLastFlightLoop,    
                                   int                  inCounter,    
                                   void *               inRefcon);    

/*
 * XPLMGetElapsedTime
 * 
 * This routine returns the elapsed time since the sim started up in decimal 
 * seconds.                                                                    
 *
 */
XPLM_API float                XPLMGetElapsedTime(void);

/*
 * XPLMGetCycleNumber
 * 
 * This routine returns a counter starting at zero for each sim cycle 
 * computed/video frame rendered.                                              
 *
 */
XPLM_API int                  XPLMGetCycleNumber(void);

/*
 * XPLMRegisterFlightLoopCallback
 * 
 * This routine registers your flight loop callback.  Pass in a pointer to a 
 * flight loop function and a refcon.  inInterval defines when you will be 
 * called.  Pass in a positive number to specify seconds from registration 
 * time to the next callback. Pass in a negative number to indicate when you 
 * will be called (e.g. pass -1 to be called at the next cylcle).  Pass 0 to 
 * not be called; your callback will be inactive.                              
 *
 */
XPLM_API void                 XPLMRegisterFlightLoopCallback(
                                   XPLMFlightLoop_f     inFlightLoop,    
                                   float                inInterval,    
                                   void *               inRefcon);    

/*
 * XPLMUnregisterFlightLoopCallback
 * 
 * This routine unregisters your flight loop callback.  Do NOT call it from 
 * your  flight loop callback.  Once your flight loop callback is 
 * unregistered, it will not be called again.                                  
 *
 */
XPLM_API void                 XPLMUnregisterFlightLoopCallback(
                                   XPLMFlightLoop_f     inFlightLoop,    
                                   void *               inRefcon);    

/*
 * XPLMSetFlightLoopCallbackInterval
 * 
 * This routine sets when a callback will be called.  Do NOT call it from your 
 * callback; use the return value of the callback to change your callback 
 * interval from inside your callback. 
 * 
 * inInterval is formatted the same way as in XPLMRegisterFlightLoopCallback; 
 * positive for seconds, negative for cycles, and 0 for deactivating the 
 * callback.  If  inRelativeToNow is 1, times are from the time of this call; 
 * otherwise they are from the time the callback was last called (or the time 
 * it was registered if it has never been called.                              
 *
 */
XPLM_API void                 XPLMSetFlightLoopCallbackInterval(
                                   XPLMFlightLoop_f     inFlightLoop,    
                                   float                inInterval,    
                                   int                  inRelativeToNow,    
                                   void *               inRefcon);    

#ifdef __cplusplus
}
#endif

#endif
