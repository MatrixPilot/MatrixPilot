#include "defines.h" 

#if (FLIGHT_PLAN_TYPE == FP_LOGO2)

// Do not include the flightplan to prevent double definition of flightplan
#define FLIGHTPLAN_LOGO2_H
#include "logo2.h"

//
const tag_logo_lang_def_t logo_instr_definitions_list[] = {
     {"REPEAT",     LOGO_HCMD_REPEAT, },
     {"FD",         LOGO_HCMD_FD, },
     {"BK",         LOGO_HCMD_BK, },
     {"FD_PARAM",   LOGO_HCMD_FD_PARAM, },
     {"BK_PARAM",   LOGO_HCMD_BK_PARAM, },
     {"RT",         LOGO_HCMD_RT, },
     {"LT",         LOGO_HCMD_LT, },
     {"SET_ANGLE",  LOGO_HCMD_SET_ANGLE, },

    };

const uint16_t count_of_instr_def_list = sizeof(logo_instr_definitions_list) / sizeof(tag_logo_lang_def_t);

//const logo_flightplan_ref_t logo_flightplan_ref_list = {
//     {"FLIGHTPLAN_MAIN", LOGO_FLIGHTPLAN_MAIN, instructions },
//     {"FLIGHTPLAN_RTL", LOGO_FLIGHTPLAN_RTL, rtlInstructions },
//    };
//
//const uint16_t count_of_flightplan_ref_list;


#endif 

