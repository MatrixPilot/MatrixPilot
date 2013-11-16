#include "defines.h"

#include"parameter_scaling.h"



/* Functions and types internal to parameter_scaling  */
/* Functions and types internal to parameter_scaling  */


typedef struct tag_parameter_scaler
{
	mavlink_param_union_t (*scale_read)(const struct param_info_s*);
	void (*scale_write)(mavlink_param_union_t, const struct param_info_s*);
	const mavlink_message_type_t mavlink_type;
} parameter_scaler;


// Return the scaler for the udb type
extern const parameter_scaler* get_scaler_for_udb_type(uint16_t type);


extern mavlink_param_union_t param_scale_read_int16( const struct param_info_s* pparam) ;
extern void param_scale_write_int16(mavlink_param_union_t setting, const struct param_info_s* pparam ) ;

extern mavlink_param_union_t param_scale_read_Q14( const struct param_info_s* pparam ) ;
extern void param_scale_write_Q14(mavlink_param_union_t setting, const struct param_info_s* pparam) ;

extern mavlink_param_union_t param_scale_read_pwtrim( const struct param_info_s* pparam ) ;
extern void param_scale_write_pwtrim(mavlink_param_union_t setting, const struct param_info_s* pparam) ;

extern mavlink_param_union_t param_scale_read_gyroscale_Q14( const struct param_info_s* pparam ) ;
extern void param_scale_write_gyroscale_Q14(mavlink_param_union_t setting, const struct param_info_s* pparam) ;

//extern mavlink_param_union_t param_scale_read_int_circular( const struct param_info_s* pparam ) ;
//extern void param_scale_write_int_circular(mavlink_param_union_t setting, const struct param_info_s* pparam) ;

extern mavlink_param_union_t param_scale_read_dm_in_cm( const struct param_info_s* pparam ) ;
extern void param_scale_write_dm_from_cm(mavlink_param_union_t setting, const struct param_info_s* pparam) ;

extern mavlink_param_union_t param_scale_read_dm_in_m( const struct param_info_s* pparam ) ;
extern void param_scale_write_dm_from_m(mavlink_param_union_t setting, const struct param_info_s* pparam) ;

extern mavlink_param_union_t param_scale_read_cm_in_m( const struct param_info_s* pparam ) ;
extern void param_scale_write_cm_from_m(mavlink_param_union_t setting, const struct param_info_s* pparam) ;

extern mavlink_param_union_t param_scale_read_frame_anglerate( const struct param_info_s* pparam ) ;
extern void param_scale_write_frame_anglerate(mavlink_param_union_t setting, const struct param_info_s* pparam) ;

extern mavlink_param_union_t param_scale_read_dcm_angle( const struct param_info_s* pparam ) ;
extern void param_scale_write_dcm_angle(mavlink_param_union_t setting, const struct param_info_s* pparam) ;


// Table must match the udb_parameter_types_e enumeration in parameters.h

const parameter_scaler    parameter_scalers[] = {
    { &param_scale_read_int16, &param_scale_write_int16, MAVLINK_TYPE_INT32_T},
    { &param_scale_read_Q14, &param_scale_write_Q14, MAVLINK_TYPE_FLOAT},
    { &param_scale_read_pwtrim, &param_scale_write_pwtrim, MAVLINK_TYPE_FLOAT},
    { &param_scale_read_gyroscale_Q14, &param_scale_write_gyroscale_Q14, MAVLINK_TYPE_FLOAT},
//    { &param_scale_read_int_circular, &param_scale_write_int_circular, MAVLINK_TYPE_INT32_T},
    { &param_scale_read_dm_in_cm, &param_scale_write_dm_from_cm, MAVLINK_TYPE_INT32_T},
    { &param_scale_read_dm_in_m, &param_scale_write_dm_from_m, MAVLINK_TYPE_FLOAT},
    { &param_scale_read_cm_in_m, &param_scale_write_cm_from_m, MAVLINK_TYPE_FLOAT},
    { &param_scale_read_frame_anglerate, &param_scale_write_frame_anglerate, MAVLINK_TYPE_INT32_T},
    { &param_scale_read_dcm_angle, &param_scale_write_dcm_angle, MAVLINK_TYPE_INT32_T},
    };


// Read an internal variable as a mavlink scaled parameter
mavlink_param_union_t param_scale_read( const struct param_info_s* pparam)
{
    parameter_scaler* pscaler = get_scaler_for_udb_type(pparam->type);
    return pscaler->scale_read(pparam);
};

// Write a mavlink parameter to an internal scaled variable
// Check data range and return false if out of range, true if in range and written ok
boolean param_scale_write(mavlink_param_union_t setting, const struct param_info_s* pparam)
{
    parameter_scaler* pscaler = get_scaler_for_udb_type( pparam->type);

    // Check if types match for the mavlink setting and the parameter
    if(pscaler->mavlink_type != setting.type)
        return false;

    switch(pscaler->mavlink_type)
    {
        case MAVLINK_TYPE_UINT32_T:
            if(setting.param_uint32 > pparam->max.param_uint32)
                return false;
            if(setting.param_uint32 < pparam->min.param_uint32)
                return false;
            break;
        case MAVLINK_TYPE_INT32_T:
            if(setting.param_int32 > pparam->max.param_int32)
                return false;
            if(setting.param_int32 < pparam->min.param_int32)
                return false;
            break;
        case MAVLINK_TYPE_FLOAT:
            if(setting.param_float > pparam->max.param_float)
                return false;
            if(setting.param_float < pparam->min.param_float)
                return false;
            break;
        case MAVLINK_TYPE_UINT16_T:
        case MAVLINK_TYPE_INT16_T:
        default:
            return false;
    }

    pscaler->scale_write(setting, pparam);
    return true;
}



// Return the scaler for the udb type
inline const parameter_scaler* get_scaler_for_udb_type(uint16_t type)
{
    return &parameter_scalers[type];
}


mavlink_param_union_t param_scale_read_gyroscale_Q14(const struct param_info_s* pparam)
{
    mavlink_param_union_t param;
    param.type = MAVLINK_TYPE_FLOAT;
    param.param_float =
	    (float)(*((int16_t*) pparam->pvar) / (SCALEGYRO * 16384.0)); // 16384.0 is RMAX defined as a float.
    return param;
}

void param_scale_write_gyroscale_Q14(mavlink_param_union_t setting, const struct param_info_s* pparam)
{
	if (setting.type != MAVLINK_TYPE_FLOAT) return;
	*((int16_t*)pparam->pvar) = (int16_t)(setting.param_float * (SCALEGYRO * 16384.0));
}

mavlink_param_union_t param_scale_read_Q14(const struct param_info_s* pparam)
{
    mavlink_param_union_t param;
    param.type = MAVLINK_TYPE_FLOAT;
    param.param_float = (floor((((float)(*((int16_t*) pparam->pvar) / 16384.0)) * 10000) + 0.5) / 10000.0);
    return param;

//#else
//	mavlink_msg_param_value_send(MAVLINK_COMM_0, mavlink_parameters_list[i].name,
//	    (float)(*((int16_t*) mavlink_parameters_list[i].pparam) / 16384.0),
//	    MAVLINK_TYPE_FLOAT, count_of_parameters_list, i); // 16384.0 is RMAX defined as a float.
//#endif
}

void param_scale_write_Q14(mavlink_param_union_t setting, const struct param_info_s* pparam)
{
	if (setting.type != MAVLINK_TYPE_FLOAT) return;

	*((int16_t*) pparam->pvar) = (int16_t)(setting.param_float * 16384.0);
}

mavlink_param_union_t param_scale_read_pwtrim(const struct param_info_s* pparam)
{
    mavlink_param_union_t param;
    param.type = MAVLINK_TYPE_FLOAT;
    param.param_float = (float)(*((int16_t*) pparam->pvar) / 2.0);
    return param;

//	// Check that the size of the udb_pwtrim array is not exceeded
//	if (mavlink_parameters_list[i].pparam >= (uint8_t*)(&udb_pwTrim[0] + (sizeof(udb_pwTrim[0]) * NUM_INPUTS)))
//		return;
    
//	mavlink_msg_param_value_send(MAVLINK_COMM_0, mavlink_parameters_list[i].name,
//	    (float)(*((int16_t*) mavlink_parameters_list[i].pparam) / 2.0), MAVLINK_TYPE_FLOAT, count_of_parameters_list, i); // 16384.0 is RMAX defined as a float.
}

void param_scale_write_pwtrim(mavlink_param_union_t setting, const struct param_info_s* pparam)
{
	if (setting.type != MAVLINK_TYPE_FLOAT) return;

        *((int16_t*) pparam->pvar) = (int16_t)(setting.param_float * 2.0);

//	// Check that the size of the ubb_pwtrim array is not exceeded
//	if (mavlink_parameters_list[i].pparam >= (uint8_t*)(&udb_pwTrim[0] + (sizeof(udb_pwTrim[0]) * NUM_INPUTS)))
//		return;
}

mavlink_param_union_t param_scale_read_int16(const struct param_info_s* pparam)
{
    mavlink_param_union_t param;
    param.type = MAVLINK_TYPE_INT32_T;
    param.param_int32 = *((int16_t*) pparam->pvar);
    return param;
}

void param_scale_write_int16(mavlink_param_union_t setting, const struct param_info_s* pparam)
{
	if (setting.type != MAVLINK_TYPE_INT32_T) return;

	*((int16_t*)pparam->pvar) = (int16_t) setting.param_int32;
}

//
//mavlink_param_union_t param_scale_read_int_circular(const struct param_info_s* pparam)
//{
//	param_union_t param;
//	union longww deg_angle;
//
//	deg_angle.WW = 0;
//	deg_angle._.W0 = *((int16_t*) mavlink_parameters_list[i].pparam);
//
//	deg_angle.WW = __builtin_mulss(deg_angle._.W0, (int16_t)(RMAX * 180.0 / 256.0));
//
//	deg_angle.WW >>= 5;
//	if (deg_angle._.W0 > 0x8000) deg_angle._.W1++; // Take care of the rounding error
//
//	param.param_int32 = deg_angle._.W1; // >> 6;
//
//	mavlink_msg_param_value_send(MAVLINK_COMM_0, mavlink_parameters_list[i].name,
//	    param.param_float, MAVLINK_TYPE_INT32_T, count_of_parameters_list, i);
//}
//
//void param_scale_write_int_circular(mavlink_param_union_t setting, const struct param_info_s* pparam)
//{
//	if (setting.type != MAVLINK_TYPE_INT32_T) return;
//
//	union longww dec_angle;
//	dec_angle.WW = __builtin_mulss((int16_t) setting.param_int32, (int16_t)(RMAX * (256.0 / 180.0)));
//	dec_angle.WW <<= 9;
//	if (dec_angle._.W0 > 0x8000) dec_angle.WW += 0x8000; // Take care of the rounding error
//	*((int16_t*)mavlink_parameters_list[i].pparam) = dec_angle._.W1;
//}


mavlink_param_union_t param_scale_read_dm_in_cm(const struct param_info_s* pparam)
{
    mavlink_param_union_t param;
    param.type = MAVLINK_TYPE_INT32_T;


	union longww airspeed;

	airspeed.WW = 0;
	airspeed._.W0 = *((int16_t*) pparam->pvar);

	airspeed.WW = __builtin_mulss(airspeed._.W0, 10.0);

	param.param_int32 = airspeed.WW;

//	mavlink_msg_param_value_send(MAVLINK_COMM_0, mavlink_parameters_list[i].name,
//	    param.param_float, MAVLINK_TYPE_INT32_T, count_of_parameters_list, i);

    param.param_int32 = *((int16_t*) pparam->pvar);
    return param;

}

void param_scale_write_dm_from_cm(mavlink_param_union_t setting, const struct param_info_s* pparam)
{
	if (setting.type != MAVLINK_TYPE_INT32_T) return;

	union longww airspeed;

	airspeed.WW = __builtin_mulss((int16_t) setting.param_int32, (RMAX / 10.0));
	airspeed.WW <<= 2;

	*((int16_t*) pparam->pvar) = airspeed._.W1;
}

mavlink_param_union_t param_scale_read_cm_in_m(const struct param_info_s* pparam)
{
    mavlink_param_union_t param;
    param.type = MAVLINK_TYPE_FLOAT;


    param.param_float = (float) *((int16_t*) pparam->pvar);
    param.param_float *= 0.01;
    return param;
}

void param_scale_write_cm_from_m(mavlink_param_union_t setting, const struct param_info_s* pparam)
{
	if (setting.type != MAVLINK_TYPE_FLOAT) return;

//	union longww airspeed;
//	airspeed.WW = __builtin_mulss((int16_t) setting.param_int32, (RMAX / 10.0));
//	airspeed.WW <<= 2;

	*((int16_t*) pparam->pvar) = (int16_t)(setting.param_float * 100.0);
}

mavlink_param_union_t param_scale_read_dm_in_m(const struct param_info_s* pparam)
{
    mavlink_param_union_t param;
    param.type = MAVLINK_TYPE_FLOAT;

    param.param_float = (float)*((int16_t*) pparam->pvar);
    param.param_float *= 0.1;

    return param;
}

void param_scale_write_dm_from_m(mavlink_param_union_t setting, const struct param_info_s* pparam)
{
	if (setting.type != MAVLINK_TYPE_FLOAT) return;

//	union longww airspeed;
//
//	airspeed.WW = __builtin_mulss((int16_t) setting.param_int32, (RMAX / 10.0));
//	airspeed.WW <<= 2;

	*((int16_t*) pparam->pvar) = (int16_t)(setting.param_float * 10.0);
}

// send angle in dcm units
mavlink_param_union_t param_scale_read_dcm_angle(const struct param_info_s* pparam)
{
    mavlink_param_union_t param;
    param.type = MAVLINK_TYPE_INT32_T;

	union longww deg_angle;

	deg_angle.WW = 0;
	deg_angle._.W0 = *((int16_t*) pparam->pvar);

//	deg_angle.WW = __builtin_mulss(deg_angle._.W0, 40);
	deg_angle.WW = __builtin_mulss(deg_angle._.W0, (int16_t)(57.3 * 16.0)); //(RMAX * 180.0 / 256.0));
	deg_angle.WW >>= 2;
	if (deg_angle._.W0 > 0x8000) deg_angle._.W1++; // Take care of the rounding error

	param.param_int32 = deg_angle._.W1; // >> 6;

        return param;
}

// set angle in dcm units
void param_scale_write_dcm_angle(mavlink_param_union_t setting, const struct param_info_s* pparam)
{
	if (setting.type != MAVLINK_TYPE_INT32_T) return;

	union longww dec_angle;
	dec_angle.WW = __builtin_mulss((int16_t) setting.param_int32, (RMAX * (16.0 / 57.3))); //(int16_t)(RMAX * 64 / 57.3)
	dec_angle.WW <<= 12;
	if (dec_angle._.W0 > 0x8000) dec_angle.WW += 0x8000; // Take care of the rounding error
	*((int16_t*) pparam->pvar) = dec_angle._.W1;
}

// send angle rate in units of angle per frame
mavlink_param_union_t param_scale_read_frame_anglerate(const struct param_info_s* pparam)
{
    mavlink_param_union_t param;
    param.type = MAVLINK_TYPE_INT32_T;

	union longww deg_angle;

	deg_angle.WW = 0;
	deg_angle._.W0 = *((int16_t*) pparam->pvar);

//	deg_angle.WW = __builtin_mulss(deg_angle._.W0, 40);
	deg_angle.WW = __builtin_mulss(deg_angle._.W0, (int16_t)(57.3 * 40.0)); //(RMAX * 180.0 / 256.0));
	deg_angle.WW <<= 2;
	if (deg_angle._.W0 > 0x8000) deg_angle._.W1++; // Take care of the rounding error

	param.param_int32 = deg_angle._.W1; // >> 6;

        return param;
}

// set angle rate in units of angle per frame
void param_scale_write_frame_anglerate(mavlink_param_union_t setting, const struct param_info_s* pparam)
{
	if (setting.type != MAVLINK_TYPE_INT32_T) return;

	union longww dec_angle;
	dec_angle.WW = __builtin_mulss((int16_t) setting.param_int32, (128.0 * 7.15)); //(int16_t)(RMAX * 128 / (57.3 * 40.0))
	dec_angle.WW <<= 9;
	if (dec_angle._.W0 > 0x8000) dec_angle.WW += 0x8000; // Take care of the rounding error
	*((int16_t*) pparam->pvar) = dec_angle._.W1;
}

