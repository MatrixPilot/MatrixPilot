
#define	ADCON2CONFIG 0b0010010000011000 ; // Vref+ ref, scana ch0, int every 7, 16word, usa A only

#define	yrateBUFF	ADCBUF3 
#define	zrateBUFF	ADCBUF1
#define	xrateBUFF	ADCBUF2
#define	xaccelBUFF	ADCBUF4
#define	yaccelBUFF	ADCBUF5
#define	zaccelBUFF	ADCBUF6

//#define VREF

#define ACCELSHIFT 2 // shift accelerometer output to the right
#define GYROSHIFT 3  // shift gyro output to the right
