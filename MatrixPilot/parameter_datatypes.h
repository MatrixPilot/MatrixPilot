#ifndef PARAMETER_DATATYPES_H
#define PARAMETER_DATATYPES_H
// pyparam generated file - DO NOT EDIT

#include "../MAVlink/include/mavlink_types.h"

typedef enum
    {
    UDB_TYPE_INT,
    UDB_TYPE_Q14,
    UDB_TYPE_PWTRIM,
    UDB_TYPE_GYROSCALE_Q14,
    } udb_internal_type_t;

extern void mavlink_send_param_int16( int16_t i ) ;
extern void mavlink_set_param_int16(mavlink_param_union_t setting, int16_t i ) ;

extern void mavlink_send_param_Q14( int16_t i ) ;
extern void mavlink_set_param_Q14(mavlink_param_union_t setting, int16_t i ) ;

extern void mavlink_send_param_pwtrim( int16_t i ) ;
extern void mavlink_set_param_pwtrim(mavlink_param_union_t setting, int16_t i ) ;

extern void mavlink_send_param_gyroscale_Q14( int16_t i ) ;
extern void mavlink_set_param_gyroscale_Q14(mavlink_param_union_t setting, int16_t i ) ;

#endif    // PARAMETER_DATATYPES_H
