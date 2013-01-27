#include "defines.h" 

#if (FLIGHT_PLAN_TYPE == FP_LOGO2)

#include "logo2.h"

const mavlink_parameter mavlink_parameters_list[] = {
     {"REPEAT", LOGO_CMD_REPEAT, },


    };

const int count_of_parameters_list = sizeof(mavlink_parameters_list) / sizeof(mavlink_parameter);


#endif 

