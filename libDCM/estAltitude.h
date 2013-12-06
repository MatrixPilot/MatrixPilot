#ifndef ESTALTITUDE_H
#define ESTALTITUDE_H


void udb_barometer_callback(long pressure, int temperature, char status);
void altimeter_calibrate(void);
void estAGLAltitude(void);

long get_barometer_asl_altitude(void);
long get_barometer_pressure(void);
int get_barometer_temperature(void);
long get_barometer_grd_altitude(void);
long get_barometer_agl_altitude(void);


#endif // ESTALTITUDE_H
