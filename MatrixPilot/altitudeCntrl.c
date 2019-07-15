// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009-2011 MatrixPilot Team
// See the AUTHORS.TXT file for a list of authors of MatrixPilot.
//
// MatrixPilot is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MatrixPilot is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MatrixPilot.  If not, see <http://www.gnu.org/licenses/>.


#include "defines.h"
#include "navigate.h"
#include "behaviour.h"
#include "servoPrepare.h"
#include "../libUDB/heartbeat.h"
#include "../libUDB/lidar.h"
#include "../libDCM/estAltitude.h"
#include "config.h"
#include "states.h"
#include "altitudeCntrl.h"
#include "sonarCntrl.h"
#include "../libDCM/rmat.h"
#include "../libDCM/gpsData.h"
#include "../libDCM/estWind.h"
#include "../libDCM/deadReckoning.h"
#include "../libUDB/servoOut.h"
#include "options_mavlink.h"
#include "../Config/options_multicopter.h"

#if (ALTITUDE_GAINS_VARIABLE != 1)

union longww throttleFiltered = { 0 };
union longww throttle_error_integral = { 0 };
#define THROTTLEFILTSHIFT   15//gfm 12->15: filter time constant becomes 50ms instead of 600 ms

//#define THROTTLEFILTSHIFT   12

#define DEADBAND            50

#define MAXTHROTTLE         (2.0*SERVORANGE*altit.AltHoldThrottleMax)
#define FIXED_WP_THROTTLE   (2.0*SERVORANGE*RACING_MODE_WP_THROTTLE)

#define THROTTLEHEIGHTGAIN  (((altit.AltHoldThrottleMax - altit.AltHoldThrottleMin)*2.0*SERVORANGE)/(altit.HeightMargin*2.0))

#define PITCHATMAX          (altit.AltHoldPitchMax*(RMAX/57.3))
#define PITCHATMIN          (altit.AltHoldPitchMin*(RMAX/57.3))
#define PITCHATZERO         (altit.AltHoldPitchHigh*(RMAX/57.3))
#define PITCHHEIGHTGAIN     ((PITCHATMAX - PITCHATMIN) / (altit.HeightMargin*2.0))

//gfm 1.5 replaced by 1.35 to adapt HEIGHTTHROTTLEGAIN at the full stick course
#define HEIGHTTHROTTLEGAIN  ((1.35*(altit.HeightTargetMax-altit.HeightTargetMin)* 1024.0) / SERVORANGE)
#define Neutral 700 //Nominal value for SK450 with 3S batterie

static void normalAltitudeCntrl(void);
#if AIRFRAME_TYPE != AIRFRAME_QUAD
static void manualThrottle(int16_t throttleIn);
#endif
static void hoverAltitudeCntrl(void);

int16_t pitchAltitudeAdjust = 0;
boolean filterManual = false;
int16_t desiredHeight;
int32_t desiredHeight_1;//gfm Use for the rate limiter
uint16_t throttleka ;
uint16_t throttlekd;
uint16_t throttlekp;
uint16_t throttleki;
int16_t outeraltitude_control;
typedef  int16_t Filter_Coef[5][3];
typedef  int16_t Memory_Coef[3][3];
Filter_Coef C;
Memory_Coef D={{0,0,0},{0,0,0},{0,0,0}};
#if ( USE_SONAR_ON_PWM_INPUT_7 == 1 )
int sonar_distance ;         // distance to target in centimeters
int sonar_height_to_ground ; // calculated distance to ground in Earth's Z Plane allowing for tilt
fractional cos_pitch_roll ;  // tilt of the plane in UDB fractional units * 2.
void calculate_sonar_height_above_ground();
#endif

// Variables required for mavlink.  Used in AltitudeCntrlVariable and airspeedCntrl
//extern int32_t fusion;
int16_t height_target_min;
int16_t height_target_max;
int16_t height_margin;
fractional alt_hold_throttle_min;
fractional alt_hold_throttle_max;
int16_t alt_hold_pitch_min;
int16_t alt_hold_pitch_max;
int16_t alt_hold_pitch_high;
int16_t rtl_pitch_down;
int16_t desiredSpeed;

void init_altitudeCntrl(void)
{
	height_target_min     = altit.HeightTargetMin;
	height_target_max     = altit.HeightTargetMax;
	height_margin         = altit.HeightMargin;
	alt_hold_throttle_min = altit.AltHoldThrottleMin * RMAX;
	alt_hold_throttle_max = altit.AltHoldThrottleMax * RMAX;
	alt_hold_pitch_min    = altit.AltHoldPitchMin;
	alt_hold_pitch_max    = altit.AltHoldPitchMax;
	alt_hold_pitch_high   = altit.AltHoldPitchHigh;
	rtl_pitch_down        = gains.RtlPitchDown;
	desiredSpeed          = altit.DesiredSpeed * 10; // Stored in 10ths of meters per second
 // gfm
        throttlekp            = (uint16_t)(gains.ThrottleKP*RMAX*2.0*SERVORANGE/1000.0);
        throttlekd            = (uint16_t)(gains.ThrottleKD*RMAX*2.0*SERVORANGE/1000.0);
        throttleka            = (uint16_t)(gains.ThrottleKA*RMAX*4.0*980.0/GRAVITY*2.0*SERVORANGE/1000.0);
        throttleki            = (uint16_t)(gains.ThrottleKI*RMAX/PID_HZ*2.0*SERVORANGE/1000.0);
        throttle_error_integral.WW = 0; //initialization of the integrator
        desiredHeight_1     = HEIGHT_TARGET_MIN;
        throttle_control = Neutral;
#define DHRATEMAX 2000/PID_HZ //MAximum rate for the desired height = 2m/s thus 50mm per 25 ms (1/PID_HZ)
        //Butterworth filter coefficients for cutoff frequency 30 Hz and TSamp=5 ms
        C[1][1]=(int16_t)(0.672740911*RMAX);C[1][2]=(int16_t)(0.89765794*RMAX);
        C[2][1]=(int16_t)(-0.14453520*RMAX);C[2][2]=(int16_t)(-0.5271869*RMAX);
        C[3][1]=(int16_t)(1.0*RMAX);C[3][2]=(int16_t)(1.0*RMAX);
        C[4][1]=(int16_t)(1.0*RMAX);C[4][2]=(int16_t)(1.0*RMAX);
        C[5][1]=(int16_t)(0.117948572*RMAX);C[5][2]=(int16_t)(0.15738224*RMAX);
}

void save_altitudeCntrl(void)
{
        gains.ThrottleKP = throttlekp / (RMAX*2.0*SERVORANGE/1000.0);
        gains.ThrottleKD = throttlekd / (RMAX*2.0*SERVORANGE/1000.0);
        gains.ThrottleKA = throttleka / (RMAX*4.0*980.0/GRAVITY*2.0*SERVORANGE/1000.0);
        gains.ThrottleKI = throttleki / (RMAX/PID_HZ*2.0*SERVORANGE/1000.0);
//         altit.YawKDAileron = yawkdail / (SCALEGYRO*RMAX);
	altit.HeightTargetMax = height_target_max;
	altit.HeightTargetMin = height_target_min;
	altit.AltHoldThrottleMin = alt_hold_throttle_min / RMAX;
	altit.AltHoldThrottleMax = alt_hold_throttle_max / RMAX;
	altit.AltHoldPitchMin = alt_hold_pitch_min;
	altit.AltHoldPitchMax = alt_hold_pitch_max;
	altit.AltHoldPitchHigh = alt_hold_pitch_high;
//	desiredSpeed / 10;
}

#if (SPEED_CONTROL == 1)  // speed control loop

static int32_t excess_energy_height(void) // computes (1/2gravity)*(actual_speed^2 - desired_speed^2)
{
	int16_t speedAccum = 6 * desiredSpeed;
	int32_t equivalent_energy_air_speed = -(__builtin_mulss(speedAccum, speedAccum));
	int32_t equivalent_energy_ground_speed = equivalent_energy_air_speed;
	int16_t speed_component;
	union longww accum;

	speed_component = IMUvelocityx._.W1 - estimatedWind[0];
	accum.WW = __builtin_mulsu(speed_component, 37877);
	equivalent_energy_air_speed += __builtin_mulss(accum._.W1, accum._.W1);

	speed_component = IMUvelocityy._.W1 - estimatedWind[1];
	accum.WW = __builtin_mulsu(speed_component, 37877);
	equivalent_energy_air_speed += __builtin_mulss(accum._.W1, accum._.W1);

	speed_component = IMUvelocityz._.W1 - estimatedWind[2];
	accum.WW = __builtin_mulsu(speed_component, 37877);
	equivalent_energy_air_speed += __builtin_mulss(accum._.W1, accum._.W1);

	// if we are going forward, add the energy, otherwise, subract it
	accum.WW = __builtin_mulsu(forward_ground_speed, 37877);
	if (forward_ground_speed > 0)
	{
		equivalent_energy_ground_speed += __builtin_mulss(accum._.W1, accum._.W1);
	}
	else
	{
		equivalent_energy_ground_speed -= __builtin_mulss(accum._.W1, accum._.W1);
	}

	// return the smaller of the energies of ground and air speed
	// to keep both of them from getting too small
	if (equivalent_energy_ground_speed < equivalent_energy_air_speed)
	{
		return equivalent_energy_ground_speed;
	}
	else
	{
		return equivalent_energy_air_speed;
	}
}
#else

static int32_t excess_energy_height(void)
{
	return 0;
}

#if (USE_MAVLINK == 1)
// Initialize to the value from options.h.  Allow updating this value from LOGO/MavLink/etc.
// Stored in 10ths of meters per second
int16_t desiredSpeed = (DESIRED_SPEED*10);
#endif // (USE_MAVLINK == 1)

#endif //(SPEED_CONTROL == 1)  // speed control loop

void altitudeCntrl(void)
{
#if (( USE_SONAR_ON_PWM_INPUT_8 == 1 ) || (USE_LIDAR_ALTITUDE > 0) )
	calculate_sonar_height_above_ground();
#endif
	if (canStabilizeHover() && current_orientation == F_HOVER)
	{
		hoverAltitudeCntrl();
	}
	else
	{
		normalAltitudeCntrl();
	}
}

static void set_throttle_control(int16_t throttle)
{
	int16_t throttleIn;
	int16_t temp;

	if (state_flags._.altitude_hold_throttle || state_flags._.altitude_hold_pitch || filterManual)
	{
		if (udb_flags._.radio_on == 1)
		{
			throttleIn = udb_pwIn[THROTTLE_INPUT_CHANNEL];
		}
		else
		{
			throttleIn = udb_pwTrim[THROTTLE_INPUT_CHANNEL];
		}
		temp = throttleIn + REVERSE_IF_NEEDED(THROTTLE_CHANNEL_REVERSED, throttle);
		if (THROTTLE_CHANNEL_REVERSED)
		{
			if (temp > udb_pwTrim[THROTTLE_INPUT_CHANNEL]) throttle = throttleIn - udb_pwTrim[THROTTLE_INPUT_CHANNEL];
		}
		else
		{
			if (temp < udb_pwTrim[THROTTLE_INPUT_CHANNEL]) throttle = udb_pwTrim[THROTTLE_INPUT_CHANNEL] - throttleIn;
		}
		outeraltitude_control = throttle;
	}
	else
	{
		outeraltitude_control = 0;
	}
}

void setTargetAltitude(int16_t targetAlt)
{
	desiredHeight = targetAlt;
//	printf("setTargetAltitude(%u)\r\n", desiredHeight);
}

static void normalAltitudeCntrl(void)
{
	union longww throttleAccum;
	int16_t throttleIn;
	int16_t throttleInOffset;
	union longww heightError = { 0 };
	int32_t speed_height;
    union longww  IntegthrottleAccum;

	speed_height = excess_energy_height(); // equivalent height of the airspeed
	if (udb_flags._.radio_on == 1)
	{
		throttleIn = udb_pwIn[THROTTLE_INPUT_CHANNEL];
		// keep the In and Trim throttle values within 2000-4000 to account for
		// Spektrum receivers using failsafe values below 2000.
		throttleInOffset = udb_servo_pulsesat(udb_pwIn[THROTTLE_INPUT_CHANNEL]) - udb_servo_pulsesat(udb_pwTrim[THROTTLE_INPUT_CHANNEL]);
	}
	else
	{
		throttleIn = udb_pwTrim[THROTTLE_INPUT_CHANNEL];
		throttleInOffset = 0;
	}
#if AIRFRAME_TYPE == AIRFRAME_QUAD
	if (state_flags._.altitude_hold_throttle) 
	{
		if (THROTTLE_CHANNEL_REVERSED)
		{
			throttleInOffset = -throttleInOffset;
		}
		if (state_flags._.GPS_steering)
		{
			desiredHeight = navigate_desired_height();
		}
		else
		{
			//Use the throttle stick value to determine desiredHeight in 1/10.24ème de cm, quasi mm
			desiredHeight = ((__builtin_mulss((int16_t)(HEIGHTTHROTTLEGAIN), throttleInOffset - ((int16_t)(DEADBAND)))) >> 8)
			                + ((int16_t)(HEIGHT_TARGET_MIN*10));

			if (desiredHeight < (int16_t)(HEIGHT_TARGET_MIN*10)) desiredHeight = (int16_t)(HEIGHT_TARGET_MIN*10);
			if (desiredHeight > (int16_t)(HEIGHT_TARGET_MAX*10)) desiredHeight = (int16_t)(HEIGHT_TARGET_MAX*10);
                            //Rate limiter to command no more than 2 m/s
                            if (desiredHeight-desiredHeight_1 > DHRATEMAX) desiredHeight=desiredHeight_1+DHRATEMAX;
                            if (desiredHeight-desiredHeight_1 < -DHRATEMAX) desiredHeight=desiredHeight_1-DHRATEMAX;
                            desiredHeight_1=desiredHeight;
		}
		if (throttleInOffset < (int16_t)(DEADBAND) && udb_flags._.radio_on)
		{
			throttleAccum.WW  = 0;
                            IntegthrottleAccum.WW = 0;
		}
		else
		{
//                            heightError._.W0 = desiredHeight - (IMUlocationz.WW >> 6) ;//error in mm
                            heightError._.W0 = desiredHeight - estimated_altitude ;//error in mm
                            throttleAccum.WW = (__builtin_mulus((int16_t)(throttlekp), heightError._.W0 ))<< 5;
                            //Integral term to take into account misadapted neutral thrust due to battery voltage and other causes
                            throttle_error_integral.WW = throttle_error_integral.WW+(heightError._.W0 << 2);
                            IntegthrottleAccum.WW = (__builtin_mulus((int16_t)(throttleki), throttle_error_integral._.W1)) ;
                            if (IntegthrottleAccum._.W1> Thrust_CLAMP )  IntegthrottleAccum._.W1 = Thrust_CLAMP ;
                            if (IntegthrottleAccum._.W1< -Thrust_CLAMP )  IntegthrottleAccum._.W1 = -Thrust_CLAMP ;
			
                   }
    }
else
   {
	 throttleAccum.WW  = 0;
          IntegthrottleAccum.WW = 0;
          // desiredHeight_1 is initialized at the current throttle stick position during manual mode
	desiredHeight_1 = ((__builtin_mulss((int16_t)(HEIGHTTHROTTLEGAIN), throttleInOffset - ((int16_t)(DEADBAND)))) >> 8)
			                + ((int16_t)(HEIGHT_TARGET_MIN*10));

   }
outeraltitude_control = throttleAccum._.W1+Neutral+IntegthrottleAccum._.W1;
 #else
	union longww pitchAccum;
	if (state_flags._.altitude_hold_throttle || state_flags._.altitude_hold_pitch)
	{
		if (THROTTLE_CHANNEL_REVERSED)
		{
			throttleInOffset = -throttleInOffset;
		}
		if (state_flags._.GPS_steering)
		{
			desiredHeight = navigate_desired_height();
//			if (desired_behavior._.takeoff || desired_behavior._.altitude)
//			{
//				desiredHeight = goal.height;
//			}
//			else
//			{
//				desiredHeight = goal.fromHeight + (((goal.height - goal.fromHeight) * (int32_t)progress_to_goal) >> 12);
//			}
		}
		else
		{
//#if (settings._.AltitudeholdStabilized == AH_PITCH_ONLY)
			if (settings._.AltitudeholdStabilized == AH_PITCH_ONLY)
			{
				// In stabilized mode using pitch-only altitude hold, use desiredHeight as
				// set from the state machine upon entering stabilized mode in ent_stabilizedS().
//#elif (ALTITUDEHOLD_STABILIZED == AH_FULL)
			}
			else if (settings._.AltitudeholdStabilized == AH_FULL)
			{
				// In stabilized mode using full altitude hold, use the throttle stick value to determine desiredHeight,
				desiredHeight = ((__builtin_mulss((int16_t)(HEIGHTTHROTTLEGAIN), throttleInOffset - ((int16_t)(DEADBAND)))) >> 11)
				                + (int16_t)(altit.HeightTargetMin);
}
//#endif
			if (desiredHeight < (int16_t)(altit.HeightTargetMin)) desiredHeight = (int16_t)(altit.HeightTargetMin);
			if (desiredHeight > (int16_t)(altit.HeightTargetMax)) desiredHeight = (int16_t)(altit.HeightTargetMax);
		}
		if (throttleInOffset < (int16_t)(DEADBAND) && udb_flags._.radio_on)
		{
			pitchAltitudeAdjust = 0;
			throttleAccum.WW  = 0;
		}
		else
		{
			heightError._.W1 = -desiredHeight;
			heightError.WW = (heightError.WW + IMUlocationz.WW + speed_height) >> 13;
			if (heightError._.W0 < (-(int16_t)(altit.HeightMargin*8.0)))
			{
				throttleAccum.WW = (int16_t)(MAXTHROTTLE);
			}
			else if (heightError._.W0 > (int16_t)(altit.HeightMargin*8.0))
			{
				throttleAccum.WW = 0;
			}
			else
			{
				throttleAccum.WW = (int16_t)(MAXTHROTTLE) + (__builtin_mulss((int16_t)(THROTTLEHEIGHTGAIN), (-heightError._.W0 - (int16_t)(altit.HeightMargin*8.0))) >> 3);
				if (throttleAccum.WW > (int16_t)(MAXTHROTTLE))throttleAccum.WW = (int16_t)(MAXTHROTTLE);
			}
			heightError._.W1 = - desiredHeight;
			heightError.WW = (heightError.WW + IMUlocationz.WW - speed_height) >> 13;
			if (heightError._.W0 < (- (int16_t)(altit.HeightMargin*8.0)))
			{
				pitchAltitudeAdjust = (int16_t)(PITCHATMAX);
			}
			else if (heightError._.W0 > (int16_t)(altit.HeightMargin*8.0))
			{
				pitchAltitudeAdjust = (int16_t)(PITCHATZERO);
			}
			else
			{
				pitchAccum.WW = __builtin_mulss((int16_t)(PITCHHEIGHTGAIN), - heightError._.W0 - (int16_t)(altit.HeightMargin*8.0)) >> 3;
				pitchAltitudeAdjust = (int16_t)(PITCHATMAX) + pitchAccum._.W0;
			}
//#if (RACING_MODE == 1)
			if (settings._.RacingMode == 1)
			{
				if (state_flags._.GPS_steering)
				{
					throttleAccum.WW = (int32_t)(FIXED_WP_THROTTLE);
				}
			}
//#endif
		}
		if (!state_flags._.altitude_hold_throttle)
		{
			manualThrottle(throttleIn);
		}
		else if (state_flags._.GPS_steering && desired_behavior._.land)
		{
			// place a ceiling, in other words, go down, but not up.
			if (pitchAltitudeAdjust > 0)
			{
				pitchAltitudeAdjust = 0;
			}
			
			throttleFiltered.WW += (((int32_t)(udb_pwTrim[THROTTLE_INPUT_CHANNEL] - throttleFiltered._.W1)) << THROTTLEFILTSHIFT);
			set_throttle_control(throttleFiltered._.W1 - throttleIn);
			filterManual = true;
		}
		else
		{
			// Servo reversing is handled in servoMix.c
			int16_t throttleOut = udb_servo_pulsesat(udb_pwTrim[THROTTLE_INPUT_CHANNEL] + throttleAccum.WW);
			throttleFiltered.WW += (((int32_t)(throttleOut - throttleFiltered._.W1)) << THROTTLEFILTSHIFT);
			set_throttle_control(throttleFiltered._.W1 - throttleIn);
			filterManual = true;
		}
		if (!state_flags._.altitude_hold_pitch)
		{
			pitchAltitudeAdjust = 0;
		}
	}
	else
	{
		pitchAltitudeAdjust = 0;
		manualThrottle(throttleIn);
	}
#endif
}

#if AIRFRAME_TYPE != AIRFRAME_QUAD
static void manualThrottle(int16_t throttleIn)
{
//	printf("manualThrottle()\r\n");

	int16_t throttle_control_pre;

	throttleFiltered.WW += (((int32_t)(throttleIn - throttleFiltered._.W1)) << THROTTLEFILTSHIFT);
	if (filterManual)
	{
		// Continue to filter the throttle control value in manual mode to avoid large, instant
		// changes to throttle value, which can burn out a brushed motor.  But after fading over
		// to the new throttle value, stop applying the filter to the throttle out to allow
		// faster control.
		throttle_control_pre = throttleFiltered._.W1 - throttleIn;
		if (throttle_control_pre < 10) filterManual = false;
	}
	else
	{
		throttle_control_pre = 0;
	}
	set_throttle_control(throttle_control_pre);
}

#endif
// For now, hovering does not attempt to control the throttle, and instead
// gives manual throttle control back to the pilot.
static void hoverAltitudeCntrl(void)
{
//	printf("hoverAltitudeCntrl()\r\n");

	int16_t throttle_control_pre;
	int16_t throttleIn = (udb_flags._.radio_on == 1) ? udb_pwIn[THROTTLE_INPUT_CHANNEL] : udb_pwTrim[THROTTLE_INPUT_CHANNEL];

	throttleFiltered.WW += (((int32_t)(throttleIn - throttleFiltered._.W1)) << THROTTLEFILTSHIFT);
	if (filterManual)
	{
		// Continue to filter the throttle control value in manual mode to avoid large, instant
		// changes to throttle value, which can burn out a brushed motor.  But after fading over
		// to the new throttle value, stop applying the filter to the throttle out to allow
		// faster control.
		throttle_control_pre = throttleFiltered._.W1 - throttleIn;
		if (throttle_control_pre < 10) filterManual = false;
	}
	else
	{
		throttle_control_pre = 0;
	}
	set_throttle_control(throttle_control_pre);
}
void Filter( int16_t *Xs,int16_t Xd, int NSections,
 	         Filter_Coef C, Memory_Coef D )  {
  union longww x,y,err;
  uint8_t i;
  x._.W1=Xd;
  for (i=1; i<NSections+1; i++) {
    err.WW=x.WW+((C[1][i]*D[1][i]+C[2][i]*D[2][i])<<2);
    y.WW=C[5][i]*((err.WW +2*C[3][i]*D[1][i]+C[4][i]*D[2][i])>>14);
    D[2][i]=D[1][i];
    D[1][i]=err._.W1;
    x.WW=y.WW<<2;//Why 1 and not 2?
  }
  *Xs=x._.W1;
}

void InneraltitudeCntrl(void)
{
//	union longww  throttleAccel;
//	union longww  throttleAccel_1 = { 0 };
	union longww speedFeedback;
	union longww AccelFeedback;
//	union longww throttleAccum;
        static int16_t accelEarthFiltered;
        if (state_flags._.altitude_hold_throttle || state_flags._.altitude_hold_pitch)
	{
        Filter(&accelEarthFiltered,accelEarth[2],2,C,D);
        AccelFeedback.WW = __builtin_mulus(throttleka , accelEarthFiltered)<<3 ;
//        AccelFeedback.WW = __builtin_mulus(throttleka , accelEarth[2])<<3 ;
        speedFeedback.WW = __builtin_mulus(throttlekd , IMUvelocityz._.W1) << 5;//IMUvelocityz._.W1 in cm/s, positive upward
        }
        else
        {
	speedFeedback.WW = 0;
        AccelFeedback.WW = 0;
        }
	throttle_control = outeraltitude_control - (int32_t)speedFeedback._.W1 - (int32_t)AccelFeedback._.W1;
    if (throttle_control > SERVORANGE+Thrust_CLAMP*2) throttle_control = SERVORANGE+Thrust_CLAMP*2;
    if (throttle_control < SERVORANGE-Thrust_CLAMP*2) throttle_control = SERVORANGE-Thrust_CLAMP*2;
}
#else

void init_altitudeCntrl(void)
{
}

#endif //(ALTITUDE_GAINS_VARIABLE != 1)

