#ifndef BAROMETER_H
#define BAROMETER_H


typedef void (*barometer_callback_funcptr)(long pressure, int temperature, char status);

void rxBarometer(barometer_callback_funcptr) ;  // service the barometer

int get_barometer_temperature(void);
long get_barometer_pressure(void);
long get_barometer_altitude(void);
long get_barometer_agl_altitude(void);


#endif // BAROMETER_H
