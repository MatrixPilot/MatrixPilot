// pyparam generated file - DO NOT EDIT


#include "parameter_table.h"
#include "data_services.h"

#if(USE_NV_MEMORY == 1)

const mavlink_parameter_block    mavlink_parameter_blocks[] = {
    { STORAGE_HANDLE_CONTROL_GAINS , 0 , 12 , STORAGE_FLAG_LOAD_AT_STARTUP | STORAGE_FLAG_LOAD_AT_REBOOT | STORAGE_FLAG_STORE_CALIB , NULL },
    { STORAGE_HANDLE_MAG_CALIB , 12 , 10 , STORAGE_FLAG_LOAD_AT_STARTUP | STORAGE_FLAG_LOAD_AT_REBOOT | STORAGE_FLAG_STORE_CALIB , NULL },
    { STORAGE_HANDLE_RADIO_TRIM , 22 , 15 , STORAGE_FLAG_LOAD_AT_STARTUP | STORAGE_FLAG_LOAD_AT_REBOOT | STORAGE_FLAG_STORE_CALIB , &udb_skip_radio_trim },
    { STORAGE_HANDLE_IMU_CALIB , 37 , 7 , STORAGE_FLAG_LOAD_AT_REBOOT | STORAGE_FLAG_STORE_CALIB , &udb_skip_imu_calibration },
    { STORAGE_HANDLE_THROTTLE_HEIGHT_OPTIONS , 44 , 7 , STORAGE_FLAG_LOAD_AT_STARTUP | STORAGE_FLAG_LOAD_AT_REBOOT , NULL },
    { STORAGE_HANDLE_AIRSPEED_OPTIONS , 51 , 5 , STORAGE_FLAG_LOAD_AT_STARTUP | STORAGE_FLAG_LOAD_AT_REBOOT , NULL },
    };


const unsigned int mavlink_parameter_block_count = sizeof(mavlink_parameter_blocks) / sizeof(mavlink_parameter_block);

#endif    //USE_NV_MEMORY

