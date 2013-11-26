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


typedef enum
    {
    STORAGE_FLAG_ALL = 1,
    STORAGE_FLAG_LOAD_AT_STARTUP = 2,
    STORAGE_FLAG_LOAD_AT_REBOOT = 4,
    STORAGE_FLAG_SAVE_AFTER_CAL = 8,
    STORAGE_FLAG_STORE_CALIB = 16,
    } storage_flags_e;

typedef void (*storageCallback)(boolean);  // Callback type


    // Initialize storage
    void init_parameter_storage(void);

    // save parameters to storage
    // Runs at low priority with callback
    // return true if request can be serviced
    boolean save_parameters(uint16_t flags, void (*callback) (boolean) );

    // load parameters from storage
    // Runs at low priority with callback
    // return true if request can be serviced
    boolean load_parameters(uint16_t flags, void (*callback) (boolean) );

    // load parameter block name from storage
    // Runs at low priority with callback
    boolean load_parameter_block(char* name, void (*callback) (boolean) );

    // load parameter block name from storage
    // Runs at low priority with callback
    // return true if request can be serviced
    boolean save_parameter_block(char* name, void (*callback) (boolean) );

    // Set all parameters to default value
    // Runs immediately
    void set_parameter_defaults(void);



#ifdef	__cplusplus
}
#endif

#endif	/* PARAMETER_STORAGE_H */

