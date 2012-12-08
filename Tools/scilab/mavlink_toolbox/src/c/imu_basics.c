
 
#define RMAX 16384.0

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
#define inline __inline
#endif

#include <stdio.h>

#include "scicos_block4.h"

#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>


enum
{
	INPUT_PORT_RMAT,					// 3*3 double
	INPUT_PORT_GROUNDSPEED,			     // 1*3 double
	INPUT_PORT_WINDSPEED,				// 1*3 double
	INPUT_PORT_ACCELERATION,			// 1*3 double
	INPUT_PORT_ROTATION,				// 1*3 double
	INPUT_PORT_MAGNETOMETER,			// 1*3 double
};


enum
{
	OUTPUT_PORT_WINDSPEED_3DIMU = 1,	// double
	OUTPUT_PORT_GROUNDSPEED_3DIMU,	// double
};


void imu_basics(scicos_block *block, int flag)
{
    double *y = NULL;
    int *piPort = GetIparPtrs(block);
    
    double rmat[3][3];
    double groundspeed[3];
    double windspeed[3];
    
    double temp;

	switch(flag) {
    case DerivativeState:
    {
        printf("[DEBUG] imu_basics :: DerivativeState\n");
    }
    break;
    case OutputUpdate:
    {
        y = GetRealInPortPtrs(block,INPUT_PORT_RMAT);
        memcpy(rmat, y, sizeof(rmat));

        y = GetRealInPortPtrs(block,INPUT_PORT_WINDSPEED);
        memcpy(windspeed, y, sizeof(windspeed));

        y = GetRealInPortPtrs(block,INPUT_PORT_GROUNDSPEED);
        memcpy(groundspeed, y, sizeof(groundspeed));

        // Calculate windspeed 3D magnitude        
        temp =  (windspeed[0] * windspeed[0]) +
                (windspeed[1] * windspeed[1]) +
                (windspeed[2] * windspeed[2]);
        temp = sqrt(temp);
       
        y = GetRealOutPortPtrs(block,OUTPUT_PORT_WINDSPEED_3DIMU);
        y[0] = temp;

        // Calculate groundspeed 3D magnitude        
        temp =  (groundspeed[0] * groundspeed[0]) +
                (groundspeed[1] * groundspeed[1]) +
                (groundspeed[2] * groundspeed[2]);
        temp = sqrt(temp);
       
        y = GetRealOutPortPtrs(block,OUTPUT_PORT_GROUNDSPEED_3DIMU);
        y[0] = temp;
    }
    break;
    case StateUpdate:
    {
        printf("[DEBUG] imu_basics :: StateUpdate\n");
    }
    break;
    case OutputEventTiming:
    {
        printf("[DEBUG] imu_basics :: OutputEventTiming\n");
        // can emit output event (not supported yet)
    }
    break;
    case Initialization:
    {
        printf("[DEBUG] imu_basics :: Initialization\n");
    }
    break;
    case Ending:
    {
    }
    break;
    case ReInitialization:
    {
        printf("[DEBUG] imu_basics :: ReInitialization\n");
    }
    break;
    default:
        printf("[DEBUG] imu_basics :: default ?????\n");
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
// MAIN CODE FOR RECEIVING MAVLINK
//




#ifdef __cplusplus
}
#endif // __cplusplus


