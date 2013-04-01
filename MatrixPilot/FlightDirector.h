/* 
 * File:   FlightDirector.h
 * Author: Matt
 *
 * Created on den 3 februari 2013, 18:06
 */


#ifndef FLIGHTDIRECTOR_H
#define	FLIGHTDIRECTOR_H

typedef enum
{
    FLIGHT_DIRECTOR_NONE = 0,
    FLIGHT_DIRECTOR_WAYPOINTS,
    FLIGHT_DIRECTOR_LOGO,
    FLIGHT_DIRECTOR_MAVLINK,
    FLIGHT_DIRECTOR_MAX
} FLIGHT_DIRECTOR;

typedef void (*FD_startDirectorFunc)( int16_t ref );
typedef void (*FD_closeDirectorFunc)(void);
typedef void (*FD_runDirector)(void);

extern void request_flight_director(FLIGHT_DIRECTOR director);
extern FLIGHT_DIRECTOR get_flight_director(void);

typedef struct tag_FlightDirector
{
    const FD_startDirectorFunc start;
    const FD_closeDirectorFunc close;
    const FD_runDirector run;
} FlightDirector;

extern const FlightDirector[FLIGHT_DIRECTOR_MAX];

#endif	/* FLIGHTDIRECTOR_H */

