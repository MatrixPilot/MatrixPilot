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

