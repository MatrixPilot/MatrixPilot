#include "libCAN.h"
#include "../libDCM/libDCM.h"

typedef struct tagIMU_DATA
{
//	char dcm_flags;		// Must not include flags here, handled by different route
	fractional rmat[9];
	fractional omegaAccum[3];
	fractional omegagyro[3];
	struct relative3D GPSlocation;
	struct relative3D GPSvelocity;
	struct relative2D velocity_thru_air;
	int    estimatedWind[3];
	union longww IMUlocationx , IMUlocationy , IMUlocationz ;
	union longww IMUvelocityx , IMUvelocityy , IMUvelocityz ;
	signed char calculated_heading ; // takes into account wind velocity
	int gps_data_age ;
	int velocity_magnitude ;
	int air_speed_magnitude;
	union longbbbb lat_gps , long_gps , alt_sl_gps ;
	union longbbbb lat_origin , long_origin , alt_origin ;
	int  filler;	// to make sure everything is sent and received
} IMU_DATA;


typedef struct tagDCM_DATA
{
	int velocity_previous  ;
	
	fractional dirovergndHRmat[3] ;
	fractional dirovergndHGPS[3] ;
	fractional omega[3] ;
	
	fractional locationErrorEarth[3] ;
		
	union longbbbb tow ;
	union intbb    sog_gps , cog_gps , climb_gps, week_no ;
	union longbbbb xpg , ypg , zpg ;
	union intbb    xvg , yvg , zvg ;
	unsigned char  mode1 , mode2 , svs, hdop ;
	
	int forward_acceleration  ;
	
	unsigned char  	lat_cir ;
	int				cos_lat ;

	char	filler;
} DCM_DATA;


typedef struct tagMAG_DATA
{
//	int udb_magFieldBody[3] ;			// This is transfered from the Pilot to IMU
//	unsigned char 	magreg[6] ;			// used by udb mag drivers
	fractional 		magFieldEarth[3] ;
	int 			udb_magOffset[3] ;	// CRITICAL - MUST TRANSFER
//	int 			magGain[3] ;		// used by udb mag drivers
	int 			offsetDelta[3] ;	// Used in rmat calculation
//	int 			rawMagCalib[3] ;	// used by udb mag drivers
//	int 			magMessage ;		// used by udb mag drivers
} MAG_DATA;


typedef struct tagPILOT_DATA
{
//	union longbbbb lat_origin , long_origin , alt_origin ;
	int  filler;	// to make sure everything is sent and received
}  PILOT_DATA;

