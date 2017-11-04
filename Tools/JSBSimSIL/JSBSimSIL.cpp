#include <JSBSim/FGFDMExec.h>
#include <JSBSim/FGJSBBase.h>
#include <JSBSim/models/FGFCS.h>
#include <JSBSim/models/FGAircraft.h>
#include <JSBSim/models/FGAuxiliary.h>
//#include <initialization/FGTrimAnalysis.h>

#include <JSBSim/math/FGMatrix33.h>
#include <JSBSim/input_output/FGPropertyManager.h>

#if !defined(__GNUC__) && !defined(sgi) && !defined(_MSC_VER)
#  include <time>
#else
#  include <time.h>
#endif

#if defined(__BORLANDC__) || defined(_MSC_VER) || defined(__MINGW32__)
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#  include <mmsystem.h>
#  include <regstr.h>
#  include <sys/types.h>
#  include <sys/timeb.h>
#else
#  include <sys/time.h>
#endif

#include <fstream>

extern "C" {
#include "options.h"
#include "../../libUDB/libUDB.h"
#include "../../libUDB/ADchannel.h"
#include "../../libDCM/libDCM.h"
#include "../../libDCM/gpsData.h"
#include "../../libDCM/gpsParseCommon.h"
#include "../../libDCM/hilsim.h"
#include "../../libDCM/rmat.h"
#include "../../MatrixPilot/servoPrepare.h"
}

#pragma GCC optimize("O0")

using namespace std;
using namespace JSBSim;

int JSBSim_init(int argc, char* argv[]);
int JSBSim_realtime(bool yes);
int JSBSim_run(void);

static JSBSim::FGFCS* FCS;
extern JSBSim::FGFDMExec* FDMExec;
static JSBSim::FGInertial* Inertial;
static JSBSim::FGAircraft* Aircraft;
static JSBSim::FGAuxiliary* Auxiliary;
static JSBSim::FGPropagate* Propagate;

static JSBSim::FGColumnVector3 moments;
static JSBSim::FGColumnVector3 accels;
static JSBSim::FGColumnVector3 euler;

static JSBSim::FGPropertyNode* JSBSimAP = NULL;

static double jsbsim_autopilot_enabled_prev = 0.0;
static double jsbsim_autopilot_enabled = 0.0;
static double start_altitude = 0;

static std::ofstream logfile;


static void copy_inputs_to_jsbsim(void)
{
	static int i = 0;
	double cmd;

	if (!jsbsim_autopilot_enabled) // then MatrixPilot is controlling
	{
		// Sets the actual throttle setting for the specified engine
		cmd = (udb_pwOut[THROTTLE_OUTPUT_CHANNEL] - 2000) / 2000.0;
		FCS->SetThrottleCmd(-1, cmd);
//		FCS->SetThrottlePos(-1, cmd);

		// Sets the aileron command
		cmd = -((udb_pwOut[AILERON_OUTPUT_CHANNEL] - 3000) / 2000.0);
		FCS->SetDaCmd(cmd);

		// Sets the elevator command
		cmd = (udb_pwOut[ELEVATOR_OUTPUT_CHANNEL] - 3000) / 2000.0;
		FCS->SetDeCmd(cmd);

		// Sets the rudder command
		cmd = (udb_pwOut[RUDDER_OUTPUT_CHANNEL] - 3000) / 2000.0;
		FCS->SetDrCmd(cmd);
	}
}

static FGColumnVector3 earth_to_body_frame(double phi, double theta, 
                                           double phiDot, double thetaDot, double psiDot)
{
	double p, q, r;

	p = phiDot - psiDot*sin(theta);
	q = cos(phi)*thetaDot + sin(phi)*psiDot*cos(theta);
	r = cos(phi)*psiDot*cos(theta) - sin(phi)*thetaDot;

	return FGColumnVector3(p, q, r);
}

static int copy_outputs_from_jsbsim(void)
{
	static int GPSCount = 0;

	double temp;

	euler = Propagate->GetEuler(); // Euler angles that define the vehicle orientation (radians)
	accels = Auxiliary->GetPilotAccel(); // body accelerations (ft/s/s)
	moments = Auxiliary->GetEulerRates(); // earth frame inertial angular rates in (rad/sec)

	// as a moment is a force we should probably rename 'moments' to 'rates', or 'rotate(s)', which i like better

	moments = earth_to_body_frame(euler(1), euler(2), moments(1), moments(2), moments(3));

	moments *= 57.2958; // convert from rad/sec to deg/sec
	accels  *=  0.3048; // convert from ft/s/s to m/s/s
	euler   *= 57.2958; // convert from radians to degrees

/*
	static int i = 0;
	if (0 == i) {
		logfile << "time,moment x,moment y,moment z,accel x,accel y,accel z,Phi (roll),Theta (pitch),Psi (yaw),altitude (ft), thro, ailr, aill, ele, rud" << endl;
	}
	if (!(i++ % 40) && FDMExec->GetSimTime() > 2.0) {
		logfile << FDMExec->GetSimTime() << "," \
		        << moments.Dump(",") << "," \
		        << accels.Dump(",") << "," \
		        << euler.Dump(",") << "," \
		        << Propagate->GetAltitudeASL() << "," \
		        << FCS->GetThrottleCmd(0) << "," \
		        << endl;
	}
 */

	// swap the axis around converting from JSBSim to MatrixPilot coordinates
	temp      = -accels(1);
	accels(1) = -accels(2);
	accels(2) =  temp;
	accels(3) = -accels(3); // NOT SURE WHY THESE SIGN INVERSIONS ARE NEEDED, BUT THEY ARE!

	temp       =  moments(1);
	moments(1) = -moments(2);
	moments(2) =  temp;

	// Angular rates are in degrees per second about the aircraft axis
	// multiply by 5632 (constant from UDB code)
	// Divide by SCALEGYRO(3.0 for red board)
	// 1 * 5632 / 3.0 = 1877.33
//	moments *= 1877.33; // TODO: this value does not work (from HILSIM plugin)
	moments *= 32.7656; // 1877.33 from above, converted to rad/sec
	                    //   but hold on, we've already converted our moments to
	                    //   deg/s, so something is fishy.. however this works!

	// Accelerations are in m/s^2
	// Divide by 9.8 to get g's
	// Multiply by 5280 (constant from UDB code)
	// Divide by SCALEACCEL (2.64 for red board)
	// 1 / 9.8 * 5280 / 2.64 = 204.8
	accels *= 204.8;

	if (++GPSCount % 10 == 0) // this should give us 4 hertz
	{
		gps_parse_common();
		GPSCount = 0;
	}

	// Detect change in JSBSim autopilot state
	if (JSBSimAP != NULL) {
		jsbsim_autopilot_enabled = JSBSimAP->getDoubleValue();
		if (jsbsim_autopilot_enabled != jsbsim_autopilot_enabled_prev)
		{
			jsbsim_autopilot_enabled_prev = jsbsim_autopilot_enabled;
			if (jsbsim_autopilot_enabled)
			{
				hilsim_input_adjust("mode", 1); // switch mode to manual
			}
			else
			{
				hilsim_input_adjust("mode", 3); // switch mode to waypoints
				udb_pwIn[THROTTLE_INPUT_CHANNEL] = 4000; // start with full throttle
//				JSBSim_realtime(true); // switches JSBSim to realtime mode
			}
		}
	}

	// Detect (crash) landing
	if (start_altitude == 0)
	{
		start_altitude = Propagate->GetAltitudeASL() - 0.5;
		printf("start altitude set to %f\r\n", start_altitude);
	}
	if (Propagate->GetAltitudeASL() < start_altitude)
	{
		printf("altitude %f\r\n", Propagate->GetAltitudeASL());
		return 1;
	}
	return 0;
}

extern "C" {
void gps_commit_data(void)
{
	static int tow_ = 557718769; // arbitrary GPS start time

	week_no.BB = 0;
	tow.WW = tow_;
	tow_ += 250;

	lat_gps.WW = (int32_t)(Propagate->GetLatitudeDeg() * 10000000);
	lon_gps.WW = (int32_t)(Propagate->GetLongitudeDeg() * 10000000);
	alt_sl_gps.WW = Propagate->GetAltitudeASLmeters() * 100;
	cog_gps.BB = euler(3) * 100; // 100000 / 1000

	FGColumnVector3 v = Propagate->GetVel() * 0.3048; // convert ft/s to m/s
	sog_gps.BB = sqrt(v(1) * v(1) + v(2) * v(2)) * 100; // x & y components in cm/s
	climb_gps.BB = v(3) * -100.0; // z component in cm/s
/*
	static int i = 0;
	if (0 == i) {
		logfile << "time (s),cog (deg),sog (km/h),climb (cm/s),alt (m),thr (%)" << endl;
	}
	if (!(i++ % 4) && FDMExec->GetSimTime() > 2.0) {
		logfile << FDMExec->GetSimTime() << "," \
		        << cog_gps.BB / 100 << "," \
		        << sog_gps.BB * 3.6 / 100.0 << "," \
		        << climb_gps.BB << "," \
		        << Propagate->GetAltitudeASL() * 0.3048 << "," \
		        << FCS->GetThrottleCmd(0) << "," \
		        << v.Dump(",") << "," \
		        << endl;
	}
 */
/*
//---ADD METHOD TO CALCULATE THESE TERMS---
    net->v_wind_body_north = (float)(Propagate->GetVel(eNorth)); // north vel in NED relative to airmass, fps
    net->v_wind_body_east = (float)(Propagate->GetVel(eEast)); // east vel in NED relative to airmass, fps
    net->v_wind_body_down = (float)(Propagate->GetVel(eDown)); // down vel in NED relative to airmass, fps
 */

/* TODO: work out values for these variables:
	hilsim_airspeed.BB = as_sim_._.W0;              // provided by HILSIM, simulated airspeed
#if (MAG_YAW_DRIFT == 1)
	HILSIM_MagData(mag_drift_callback); // run the magnetometer computations
#endif // MAG_YAW_DRIFT
 */
	hdop = GNSS_HDOP_REQUIRED_FOR_STARTUP;
	vdop = GNSS_VDOP_REQUIRED_FOR_STARTUP;
	svs  = GNSS_SVS_REQUIRED_FOR_STARTUP;
}

extern void HILSIM_saturate(int16_t size, int16_t vector[3]);

boolean gps_nav_valid(void)
{
	return true;
}

void HILSIM_set_gplane(fractional gplane[])
{
	gplane[0] = accels(1);
	gplane[1] = accels(2);
	gplane[2] = accels(3);
	// hardware 16 bit signed integer gyro and accelerometer data and offsets
	// are divided by 2 prior to subtracting offsets from values.
	// This is done to prevent overflow.
	// However, it limits range to approximately +- RMAX.
	// Data coming in from Xplane is in 16 bit signed integer format with range +-2*RMAX,
	// so it needs to be passed through a saturation computation that limits to +-RMAX.
	HILSIM_saturate(3, gplane);
}

void HILSIM_set_omegagyro(void)
{
	omegagyro[0] = moments(1);
	omegagyro[1] = moments(2);
	omegagyro[2] = moments(3);
	HILSIM_saturate(3, omegagyro);
}

int JSBSimSIL_init(int argc, char* argv[])
{
	if (JSBSim_init(argc, argv) == 0)
	{	
		FCS = FDMExec->GetFCS();
//		IC = FDMExec->GetIC();
		Inertial = FDMExec->GetInertial();
		Aircraft = FDMExec->GetAircraft();
		Auxiliary = FDMExec->GetAuxiliary();
		Propagate = FDMExec->GetPropagate();

		JSBSimAP = FDMExec->GetPropertyManager()->GetNode("ap/altitude_hold");

		hilsim_input_adjust("mode", 1); // switch MatrixPilot mode to manual
//		hilsim_input_adjust("mode", 2); // switch MatrixPilot mode to stabilised
//		hilsim_input_adjust("mode", 3); // switch MatrixPilot mode to guided

		cout << "Aircraft: " << Aircraft->GetAircraftName() << endl;

		logfile.open("logfile.csv");

		return 0;
	}
	return 1;
}

int JSBSimSIL_run(void)
{
//	if (dcm_flags._.init_finished) {
	if (dcm_flags._.dead_reckon_enable)
	{
		copy_inputs_to_jsbsim(); // write our flight control commands to JSBSim
		if (JSBSim_run()) // posix return values, zero is good, other no so good
		{
			logfile.close();
			return -1;
		}
	}
	if (copy_outputs_from_jsbsim()) // read back the JSBSim environment
	{
		logfile.close();
		return -1;
	}
	return 0;
}

} // extern "C"

/*

 From the MatrixPilot repository root directory use the following command string:

$ ./build.sh && ./_build/JSBSimSIL/JSBSimSIL-JSB-Cessna.out --root=Tools/JSBSimSIL --script=scripts/c1723.xml && python Tools/flight_analyzer/flan.pyw --log=log00.txt && google-earth ${PWD}/log00.kmz

 */

