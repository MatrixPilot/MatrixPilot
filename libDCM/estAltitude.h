#ifndef ESTALTITUDE_H
#define ESTALTITUDE_H


void udb_barometer_callback(long pressure, int temperature, char status);
void altimeter_calibrate(void);
void estAltitude(void);

inline long get_barometer_altitude(void);
inline long get_barometer_pressure(void);
inline int get_barometer_temperature(void);
inline long get_barometer_agl_altitude(void);


#endif // ESTALTITUDE_H
