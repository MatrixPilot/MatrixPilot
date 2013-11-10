/* 
 * File:   parameter_macros.h
 * Author: Matt
 *
 * Created on 10 November 2013, 06:23
 */

#ifndef PARAMETER_MACROS_H
#define	PARAMETER_MACROS_H

#ifdef	__cplusplus
extern "C" {
#endif

#define PARAM_COUNT 0

/**
 * Parameter types.
 */
typedef enum param_type_e {
	/* globally-known parameter types */
	PARAM_TYPE_INT32 = 0,
	PARAM_TYPE_FLOAT,

	/* structure parameters; size is encoded in the type value */
	PARAM_TYPE_STRUCT = 100,
	PARAM_TYPE_STRUCT_MAX = 16384 + PARAM_TYPE_STRUCT,

	PARAM_TYPE_UNKNOWN = 0xffff
} param_type_t;

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
#define PARAM_DEFINE_INT32(_name, _default)		\
	static const					\
	__attribute__((used, section("__param")))	\
	struct param_info_s __param__##_name = {	\
		#_name,					\
		PARAM_TYPE_INT32,			\
		.val.i = _default			\
	}

/** define a float parameter */
#define PARAM_DEFINE_FLOAT(_name, _default)		\
	static const					\
	__attribute__((used, section("__param")))	\
	struct param_info_s __param__##_name = {	\
		#_name,					\
		PARAM_TYPE_FLOAT,			\
		.val.f = _default			\
	}

/** define a parameter that points to a structure */
#define PARAM_DEFINE_STRUCT(_name, _default)		\
	static const					\
	__attribute__((used, section("__param")))	\
	struct param_info_s __param__##_name = {	\
		#_name,					\
		PARAM_TYPE_STRUCT + sizeof(_default),	\
		.val.p = &_default;			\
	}

/** define a parameter that points to a structure */
#define PARAM_DEFINE_END()                              \
	static const					\
	__attribute__((used, section("__paramend")))	\
	struct param_info_s __param__the_end = {    	\
		NULL,					\
		PARAM_TYPE_INT32,                       \
		.val.i = 0                              \
	}

/**
 * Parameter value union.
 */
union param_value_u {
	void		*p;
	int32_t		i;
	float		f;
};

/**
 * Static parameter definition structure.
 *
 * This is normally not used by user code; see the PARAM_DEFINE macros
 * instead.
 */
struct param_info_s {
	const char	*name;
	param_type_t	type;
	union param_value_u val;
};




#ifdef	__cplusplus
}
#endif

#endif	/* PARAMETER_MACROS_H */

