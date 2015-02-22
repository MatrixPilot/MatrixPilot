/* 
 * File:   libCAN_options.h
 * Author: Matt
 *
 * Created on 05 June 2013, 08:27
 */

#ifndef LIBCAN_OPTIONS_H
#define	LIBCAN_OPTIONS_H


/* CAN Baud Rate Configuration 		*/
#define FCAN  	40000000
#define BITRATE 1000000
#define NTQ 	20		// 20 Time Quanta in a Bit Time
#define BRP_VAL		((FCAN/(2*NTQ*BITRATE))-1)


#endif	/* LIBCAN_OPTIONS_H */

