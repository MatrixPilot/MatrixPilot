// pyparam generated file - DO NOT EDIT


#include "parameter_table.h"
#include "data_services.h"

#if(USE_NV_MEMORY == 1)

const mavlink_parameter_block    mavlink_parameter_blocks[] = {
    { STORAGE_HANDLE_PID , 0 , 12 , STORAGE_FLAG_LOAD_AT_STARTUP | STORAGE_FLAG_LOAD_AT_REBOOT | STORAGE_FLAG_STORE_CALIB , NULL , "PID"  },
     { STORAGE_HANDLE_MAG , 12 , 10 , STORAGE_FLAG_LOAD_AT_STARTUP | STORAGE_FLAG_LOAD_AT_REBOOT | STORAGE_FLAG_STORE_CALIB , NULL , "MAG"  },
     { STORAGE_HANDLE_PWTRIM , 22 , 15 , STORAGE_FLAG_LOAD_AT_STARTUP | STORAGE_FLAG_LOAD_AT_REBOOT | STORAGE_FLAG_STORE_CALIB , &udb_skip_radio_trim , "PWTRIM"  },
     { STORAGE_HANDLE_IMU , 37 , 7 , STORAGE_FLAG_LOAD_AT_REBOOT | STORAGE_FLAG_STORE_CALIB , &udb_skip_imu_calibration , "IMU"  },
     { STORAGE_HANDLE_TH , 44 , 9 , STORAGE_FLAG_LOAD_AT_STARTUP | STORAGE_FLAG_LOAD_AT_REBOOT , NULL , "TH"  },
     { STORAGE_HANDLE_ASPD , 53 , 10 , STORAGE_FLAG_LOAD_AT_STARTUP | STORAGE_FLAG_LOAD_AT_REBOOT , NULL , "ASPD"  },
     };


const uint16_t mavlink_parameter_block_count = sizeof(mavlink_parameter_blocks) / sizeof(mavlink_parameter_block);

#endif    //USE_NV_MEMORY

