
#define	ADCON2CONFIG 0b0000010000011000 ; // AVDD ref, scana ch0, int every 7, 16word, usa A only

#define	yrateBUFF	ADCBUF0 
#define	zrateBUFF	ADCBUF1
#define	xrateBUFF	ADCBUF2
#define	vrefBUFF    ADCBUF3
#define	xaccelBUFF	ADCBUF4
#define	yaccelBUFF	ADCBUF5
#define	zaccelBUFF	ADCBUF6

#define VREF

#define ACCELSHIFT 4 // shift accelerometer output to the right
#define GYROSHIFT 5  // shift gyro output to the right
