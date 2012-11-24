#ifndef ESTALTITUDE_H
#define ESTALTITUDE_H


void udb_barometer_callback(long pressure, int temperature, char status);
void altimeter_calibrate(void);
void estAltitudeAGL(void);

long get_barometer_pressure(void);
int get_barometer_temperature(void);
long get_barometer_altitude_asl(void);
long get_barometer_altitude_grd(void);
long get_barometer_altitude_agl(void);


#endif // ESTALTITUDE_H
