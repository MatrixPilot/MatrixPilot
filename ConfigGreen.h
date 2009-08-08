
#define	ADCON2CONFIG 0b0000010000011000 ; // AVDD ref, scana ch0, int every 7, 16word, usa A only

#define	yrateBUFF	ADCBUF0 
#define	zrateBUFF	ADCBUF1
#define	xrateBUFF	ADCBUF2
#define	vrefBUFF    ADCBUF3
#define	xaccelBUFF	ADCBUF4
#define	yaccelBUFF	ADCBUF5
#define	zaccelBUFF	ADCBUF6

#define XSIGN +
#define YSIGN -
#define ZSIGN +

#define VREF

#define SCALEGYRO 1.0
#define SCALEACCEL 1.0
