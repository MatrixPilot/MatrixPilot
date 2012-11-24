#ifndef BAROMETER_H
#define BAROMETER_H


typedef void (*barometer_callback_funcptr)(long pressure, int temperature, char status);

void rxBarometer(barometer_callback_funcptr) ;  // service the barometer
//void rxBarometer(void) ;  // service the barometer
void computeAltitudeAGL(void);

#endif // BAROMETER_H
