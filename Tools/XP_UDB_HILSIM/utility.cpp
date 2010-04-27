#include "stdafx.h"

double GetHeading(double lat1, double lat2, double lon1, double lon2)
{
	double angle;
	angle = atan2(sin(lon2-lon1)*cos(lat2),cos(lat1)*sin(lat2)-sin(lat1)*cos(lat2)*cos(lon2-lon1));
	return(angle);
}

double GetDistance(double lat1, double lat2, double lon1, double lon2)
{
	double d;
	d = acos((sin(lat1)*sin(lat2))+(cos(lat1)*cos(lat2)*cos(lon2-lon1)))*R_EARTH;
	return d;
}

double GetCTE(double current_lat, double current_lon, double dest_lat, double dest_lon, double track_angle)
{
	double cte, dist, bearing;
	dist = GetDistance(current_lat,dest_lat,current_lon, dest_lon);
	bearing = GetHeading(current_lat,dest_lat,current_lon, dest_lon);
	cte = asin(sin(dist/R_EARTH)*sin(bearing-track_angle)) * R_EARTH;
	return cte;
}
double LimitValue(double &input_val, double limit_mag)
{
	if(input_val < (-1.0 * limit_mag))
	{
		input_val = -1.0 * limit_mag;
	}
	if(input_val > limit_mag)
	{
		input_val = limit_mag;
	}
	return input_val;
}

float LimitValue(float &input_val, float limit_mag)
{
	if(input_val < (-1.0 * limit_mag))
	{
		input_val = (float)-1.0 * limit_mag;
	}
	if(input_val > limit_mag)
	{
		input_val = limit_mag;
	}
	return input_val;
}

void GetPosWHdngDist(double lat1, double lon1, double hdng, double dist, double &new_lat, double &new_long)
{
	new_lat = asin(sin(lat1)*cos(dist/R_EARTH) + cos(lat1)*sin(dist/R_EARTH)*cos(hdng));
 	new_long = lon1 + atan2((sin(hdng)*sin(dist/R_EARTH)*cos(lat1)), (cos(dist/R_EARTH)-sin(lat1)*sin(new_lat)));
}

void LLAtoECEF(double lat, double lon, double alt, double &x, double &y, double &z)
{
	lat = (lat / 180 * PI);
	lon = (lon / 180 * PI);
	x = (alt + R_EARTH) * cos(lat) * cos(lon);
	y = (alt + R_EARTH) * cos(lat) * sin(lon);
	z = (alt + R_EARTH) * sin(lat);
}

void BCBFtoOGL(float &x, float &y, float &z, float phi, float theta, float psi)
{

		float Cr = cos(phi);
		float Cp = cos(theta);
		float Cy = cos(psi);
		float Sr = sin(phi);
		float Sp = sin(theta) ;
		float Sy = sin(psi);
		
		//	c2c3	c3s1s2-c1s3		c1c3s2+s1s3		|	x
		//	c2s3	c1c3+s1s2s3		c1s2s3-c3s1		|	y
		//	-s2		c2s1			c1c2			|	z

		float tempx = ( x * Cp * Cy ) + ( y * (( Cy * Sr * Sp ) - ( Cr * Sy ))) + ( z * (( Cr * Cy * Sp ) + ( Sr * Sy )));
		float tempy = ( x * Cp * Sy ) + ( y * (( Cr * Cy ) + ( Sr * Sp * Sy ))) + ( z * (( Cr * Sp * Sy ) - ( Cy * Sr )));
		float tempz = ( x * Sp * -1.0) + ( y * Cp * Sr ) + ( z * Cr * Cp);

		// tempx, y & z should be in the NED frame, as that is where our roll, pitch, yaw angles are defined.
		// need to convert them to East Up South, which is the OGL frame.

		z = tempx * -1.0;	// tempx points north, z in OGL is +ve south
		x = tempy;			// tempy points east, x in OGL is +ve east
		y = tempz * -1.0;	// tempz points down, y in OGL is +ve up

		return;
}

void OGLtoBCBF(float &x, float &y, float &z, float phi, float theta, float psi)
{
	float x_NED, y_NED, z_NED;
	float Cr, Cp, Cy;
	float Sr, Sp, Sy;
	
	//Accelerations in X-Plane are expressed in the local OpenGL reference frame, for whatever reason. 
	//This coordinate system is defined as follows (taken from the X-Plane SDK Wiki):
	
	//	The origin 0,0,0 is on the surface of the earth at sea level at some "reference point".
	//	The +X axis points east from the reference point.
	//	The +Z axis points south from the reference point.
	//	The +Y axis points straight up away from the center of the earth at the reference point.
	
	// First we shall convert from this East Up South frame, to a more conventional NED (North East Down) frame.
	x_NED = -1.0 * z;
	y_NED = x;
	z_NED = -1.0 * y; 

	// Next calculate cos & sin of angles for use in the transformation matrix.
	// r, p & y subscripts stand for roll pitch and yaw.

	Cr = cos(phi);
	Cp = cos(theta);
	Cy = cos(psi);
	Sr = sin(phi);
	Sp = sin(theta);
	Sy = sin(psi);

	// Next we need to rotate our accelerations from the NED reference frame, into the body fixed reference frame

	// THANKS TO GEORGE M SIOURIS WHOSE "MISSILE GUIDANCE AND CONTROL SYSTEMS" BOOK SEEMS TO BE THE ONLY EASY TO FIND REFERENCE THAT
	// ACTUALLY GETS THE NED TO BODY FRAME ROTATION MATRIX CORRECT!!
	
	// CpCy, CpSy, -Sp					| local_ax
	// SrSpCy-CrSy, SrSpSy+CrCy, SrCp	| local_ay
	// CrSpCy+SrSy, CrSpSy-SrCy, CrCp	| local_az
	
	x = (x_NED * Cp * Cy) + (y_NED * Cp * Sy) - (z_NED * Sp);
	y = (x_NED * ((Sr * Sp * Cy)-(Cr * Sy))) + (y_NED * ((Sr * Sp * Sy)+(Cr * Cy))) + (z_NED * Sr * Cp);
	z = (x_NED * ((Cr * Sp * Cy)+(Sr * Sy))) + (y_NED * ((Cr * Sp * Sy)-(Sr * Cy))) + (z_NED * Cr * Cp);

	return;
}


void FLIGHTtoBCBF(float &x, float &y, float &z, float alpha, float beta)
{
	float Ca = cos(alpha);
    float Cb = cos(beta);
    float Sa = sin(alpha);
    float Sb = sin(beta);

    float X_plane =(x * Ca * Cb) - (z * Sa * Cb) - (y * Sb); 
    float Y_plane =(z * Sa * Sb)- (x * Ca * Sb) - (y * Cb); 
    float Z_plane =(x * Sa) + (z * Ca); 

	x = X_plane;
	y = Y_plane;
	z = Z_plane;
}