/* 
 * File:   parameter_storage.h
 * Author: Matt
 *
 * Created on 15 November 2013, 21:26
 */

#ifndef PARAMETER_STORAGE_H
#define	PARAMETER_STORAGE_H


#ifdef	__cplusplus
extern "C" {
#endif

    // Initialize storage
    void init_parameter_storage(void);

    // save parameters to storage
    void save_parameters(uint16_t flags);

    // load parameters from storage
    void load_parameters(uint16_t flags);


#ifdef	__cplusplus
}
#endif

#endif	/* PARAMETER_STORAGE_H */

