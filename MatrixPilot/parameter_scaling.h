/* 
 * File:   parameter_scaling.h
 * Author: Matt
 *
 * Created on 16 November 2013, 05:37
 */

#ifndef PARAMETER_SCALING_H
#define	PARAMETER_SCALING_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "../libUDB/parameters.h"


// Read an internal variable as a mavlink scaled parameter
extern mavlink_param_union_t param_scale_read( const struct param_info_s* pparam) ;

// Write a mavlink parameter to an internal scaled variable
// Check data range and return false if out of range, true if in range
extern boolean param_scale_write(mavlink_param_union_t setting, const struct param_info_s* pparam) ;



#ifdef	__cplusplus
}
#endif

#endif	/* PARAMETER_SCALING_H */

