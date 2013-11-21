/* 
 * File:   parameter_macros.h
 * Author: Matt
 *
 * Created on 10 November 2013, 06:23
 */

#ifndef PARAMETER_MACROS_H
#define	PARAMETER_MACROS_H

// NOTE: Function prototypes at bottom

#ifdef	__cplusplus
extern "C" {
#endif

//#include "../MatrixPilot/parameter_table.h"
#include "../MAVLink/include/mavlink_types.h"

#define PARAM_COUNT 0

#define INVALID_PARAMETER_HANDLE 0x7FFF

// callback type for data services user
typedef void (*param_callbackFunc)(boolean);


typedef union
{
    float param_float;
    int32_t param_int32;
    uint32_t param_uint32;
    uint8_t param_uint8;
    uint8_t bytes[4];
} parameter_union_t;



typedef enum
    {
    UDB_PARAM_INT16,
    UDB_PARAM_Q14,
    UDB_PARAM_PWTRIM,
    UDB_PARAM_GYROSCALE_Q14,
//    UDB_PARAM_INT_CIRCULAR,   // Deprecated until further notice
    UDB_PARAM_CM_AIRSPEED_TO_DM,
    UDB_PARAM_M_AIRSPEED_TO_DM,
    UDB_PARAM_M_AIRSPEED_TO_CM,
    UDB_PARAM_FRAME_ANGLERATE,
    UDB_PARAM_DCM_ANGLE,
    } udb_parameter_types_e;


    typedef enum
    {
        PARAM_ENABLE_NONE = 0,
        PARAM_ENABLE_GLOBAL = 1,
        PARAM_ENABLE_MAVLINK = 2,
        PARAM_ENABLE_STORAGE = 4,
    } param_enable_flags_e;

#define PARAM_ENABLE_ALL (PARAM_ENABLE_GLOBAL | PARAM_ENABLE_MAVLINK | PARAM_ENABLE_STORAGE)

///**
// * Parameter types.
// */
//typedef enum param_type_e {
//	/* globally-known parameter types */
//	PARAM_TYPE_INT32 = 0,
//	PARAM_TYPE_FLOAT,
//
//	/* structure parameters; size is encoded in the type value */
//	PARAM_TYPE_STRUCT = 100,
//	PARAM_TYPE_STRUCT_MAX = 16384 + PARAM_TYPE_STRUCT,
//
//	PARAM_TYPE_UNKNOWN = 0xffff
//} param_type_t;

/**
 * Parameter handle.
 *
 * Parameters are represented by parameter handles, which can
 * be obtained by looking up (or creating?) parameters.
 */
//typedef uintptr_t	param_t;

/**
 * Handle returned when a parameter cannot be found.
 */
#define PARAM_INVALID	((uintptr_t)0xffffffff)


/*
 * Macros creating static parameter definitions.
 *
 * Note that these structures are not known by name; they are
 * collected into a section that is iterated by the parameter
 * code.
 *
 * Note that these macros cannot be used in C++ code due to
 * their use of designated initializers.  They should probably
 * be refactored to avoid the use of a union for param_value_u.
 */

/** define an int32 parameter */
#define PARAM_DEFINE_INT(_name, _pvar, _default, _min, _max, _readOnly)		\
	static const					\
	__attribute__((used, section("__param")))	\
	struct param_info_s __param__##_name = {	\
		#_name,					\
		(uint8_t*) _pvar,                       \
		UDB_PARAM_INT16,                        \
                .default_val.param_int32 = _default,    \
		.min.param_int32 = _min,                \
		.max.param_int32 = _max,           	\
                _readOnly                               \
	}

/** define an int32 parameter */
#define PARAM_DEFINE_Q14(_name, _pvar, _default, _min, _max, _readOnly)		\
	static const					\
	__attribute__((used, section("__param")))	\
	struct param_info_s __param__##_name = {	\
		#_name,					\
		(uint8_t*) _pvar,			\
		UDB_PARAM_Q14,                     	\
                .default_val.param_float = _default,    \
		.min.param_float = _min,                \
		.max.param_float = _max,           	\
                _readOnly                               \
	}

/** define a gyroscale parameter */
#define PARAM_DEFINE_GYROSCALE_Q14(_name, _pvar, _default, _min, _max, _readOnly)		\
	static const					\
	__attribute__((used, section("__param")))	\
	struct param_info_s __param__##_name = {	\
		#_name,					\
		(uint8_t*) _pvar,			\
		UDB_PARAM_GYROSCALE_Q14,                \
                .default_val.param_float = _default,    \
		.min.param_float = _min,                \
		.max.param_float = _max,           	\
                _readOnly                               \
	}

    
/** define a float parameter */
#define PARAM_DEFINE_FLOAT(_name, _pvar, _default, _min, _max, _readOnly)       \
	static const					\
	__attribute__((used, section("__param")))	\
	struct param_info_s __param__##_name = {	\
		#_name,					\
                .default_val.param_float = _default,    \
		.min.param_float = _min,                \
		.max.param_float = _max,           	\
                _readOnly                               \
	}

/** define a parameter that points to a structure */
#define PARAM_DEFINE_STRUCT(_name, _default)		\
	static const					\
	__attribute__((used, section("__param")))	\
	struct param_info_s __param__##_name = {	\
		#_name,					\
		PARAM_PARAM_STRUCT + sizeof(_default),	\
		.val.p = &_default;			\
	}

/** define a parameter that is the start of the list */
#define PARAM_DEFINE_START()                            \
	static const					\
	__attribute__((used, section("__pstart")))	\
	struct param_info_s __param__the_start = {    	\
		NULL,					\
		NULL,					\
	}

/** define a parameter that that is the end of the list */
#define PARAM_DEFINE_END()                              \
	static const					\
	__attribute__((used, section("__pend")))	\
	struct param_info_s __param__the_end = {    	\
		NULL,					\
		NULL,					\
	}



#define param_value_u param_union_t

/**
 * Static parameter definition structure.
 *
 * This is normally not used by user code; see the PARAM_DEFINE macros
 * instead.
 */
struct param_info_s {
	const char          *name;          // name of parameter
	uint8_t*            pvar;           // Reference to variable
	uint16_t            type;           // Internal UDB type of variable
	parameter_union_t   default_val;    // parameter default
	parameter_union_t   min;            // parameter minimum
	parameter_union_t   max;            // parameter maximum
        boolean             readOnly;       // Is read only
};



/**
 * Static parameter section definition structure.
 *
 * This is normally not used by user code; see the PARAM_DEFINE macros
 * instead.
 */
struct param_section_s {
	const char              *name;
        param_callbackFunc      ploadCallback;
};



/** define a parameter that is the start of the list */
#define PARAM_SECTION_START()                           \
	static const					\
	__attribute__((used, section("__sstart")))	\
	struct param_section_s __section__the_start = { \
		NULL,					\
		NULL,					\
	}

/** define a parameter that that is the end of the list */
#define PARAM_SECTION_END()                             \
	static const					\
	__attribute__((used, section("__send")))	\
	struct param_section_s __section__the_end = {   \
		NULL,					\
		NULL,					\
	}

//#define STR_EXPAND(tok) blah##tok_STORAGE_FLAGS
//#define STR_EXP(tok) STR_EXPAND(tok)

//#define PARAM_SF_NAME_STR(_sectname) #_sectname "_S_FLAGS"
//#define PARAM_EN_NAME_STR(_sectname) #_sectname "_PARAM_EN"


/** define a parameter that is the start of the list */
#define PARAM_SECTION(_name, _flags, _callback)             \
	static const                                        \
	__attribute__((used, section("__section")))         \
	struct param_section_s __section__##name = {        \
		#_name,                                     \
		_callback,                                  \
	};                                                  \
                                                            \
        int16_t storageFlags_##_name = _flags;              \
        int16_t enableFlags_##_name = PARAM_ENABLE_ALL;     \
                                                            \
	static const                                        \
	__attribute__((used, section("__param")))           \
	struct param_info_s __param__##_name_S_FLAGS = {    \
		#_name "_S_FLAGS",                          \
		(int16_t*) &storageFlags_##_name,           \
		UDB_PARAM_INT16,                            \
                .default_val.param_int32 = _flags,          \
		.min.param_int32 = 0,                       \
		.max.param_int32 = 32767,                   \
                false                                       \
        };                                                  \
                                                            \
	static const                                        \
	__attribute__((used, section("__param")))           \
	struct param_info_s __param__##_name_EN_FLAGS = {   \
		#_name "_EN_FLAGS",                         \
		(int16_t*) &enableFlags_##_name,            \
		UDB_PARAM_INT16,                            \
                .default_val.param_int32 = 1,               \
		.min.param_int32 = 0,                       \
		.max.param_int32 = 32767,                   \
                false                                       \
        }



/*
//PARAM_DEFINE_INT( #_name#_PARAM_EN, &parametersEnabled_##_name, 0, 1, false)
//        #undef SECT_FLAGS_NAME                              \
//       #define SECT_FLAGS_NAME PARAM_SECTION_SF_NAME(_name)    \
 */

// Find the handle/index for a parameter with the given name
extern uint16_t get_param_handle(char* name);

// Get a pointer to the parameter with the provided handle
extern const struct param_info_s* get_param(uint16_t handle);

// Get a total count of declared parameters
extern  uint16_t get_param_count(void);

extern uint16_t get_active_param_count(boolean telemetry);

extern parameter_union_t get_param_val(uint16_t handle);
extern uint16_t get_param_udb_type(uint16_t handle);

// Get a handle for the section with given name
extern uint16_t get_section_handle(char* name);

// Return a pointer to the section with given handle
extern const struct param_section_s* get_section(uint16_t handle);

// Return a count of the number of delcared sections
extern uint16_t get_section_count(void);

extern uint16_t get_section_params_count(uint16_t handle);


#ifdef	__cplusplus
}
#endif

#endif	/* PARAMETER_MACROS_H */

