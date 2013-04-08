#ifndef FLEXIFUNCTION_TYPES_COMPACT_H
#define FLEXIFUNCTION_TYPES_COMPACT_H

#include <dsp.h>

#include "flexifunction_options.h"


/****************************************************************/
// pyFEdit generated file - DO NOT EDIT


typedef struct tagFuncData_NULL
{
} FuncData_null;


typedef struct tagFuncData_GAIN_OFFSET
{
	unsigned int	src;
	fractional	PosGain;
	fractional	NegGain;
	fractional	Offset;
	fractional	Constant;
} FuncData_gain_offset;


typedef struct tagFuncData_MULTIPLY
{
	unsigned int	src1;
	unsigned int	src2;
} FuncData_multiply;


typedef struct tagFuncData_LINEAR_MUX
{
	unsigned int	src1;
	unsigned int	src2;
	unsigned int	Control;
} FuncData_linear_mux;


typedef struct tagFuncData_CONDITIONAL_SET_REF
{
	unsigned int	src;
	unsigned int	srcCond;
	int	condMax;
	int	condMin;
	fractional	condInvalidOffset;
	fractional	condValidOffset;
} FuncData_conditional_set_ref;


typedef struct tagFuncData_CONDITIONAL_SET
{
	unsigned int	src;
	unsigned int	srcCond;
	int	condMax;
	int	condMin;
	fractional	setValue;
} FuncData_conditional_set;


typedef struct tagFuncData_CONDITIONAL_GAIN
{
	unsigned int	src;
	unsigned int	srcCond;
	int	condMax;
	int	condMin;
	fractional	condInvalidGain;
	fractional	condValidGain;
} FuncData_conditional_gain;


typedef struct tagFuncData_SCALE_TRIM_LIMIT
{
	unsigned int	src;
	int	subTrim;
	int	offset;
	int	scalePos;
	int	scaleNeg;
	int	limitMin;
	int	limitMax;
} FuncData_scale_trim_limit;


typedef struct tagFuncData_SCALE_REFTRIM_LIMIT
{
	unsigned int	src;
	int	subTrim;
	unsigned int	refOffset;
	int	scalePos;
	int	scaleNeg;
	int	limitMin;
	int	limitMax;
} FuncData_scale_reftrim_limit;


typedef struct tagFuncData_GAIN_LIMIT
{
	unsigned int	src;
	fractional	posGain;
	fractional	negGain;
	fractional	limitNeg;
	fractional	limitPos;
} FuncData_gain_limit;


typedef struct tagFuncData_THREE_POINT
{
	unsigned int	src;
	fractional	inputLow;
	fractional	outputLow;
	fractional	inputMid;
	fractional	outputMid;
	fractional	inputHigh;
	fractional	outputHigh;
} FuncData_three_point;


typedef struct tagFuncData_FOUR_POINT
{
	unsigned int	src;
	fractional	input1;
	fractional	output1;
	fractional	input2;
	fractional	output2;
	fractional	input3;
	fractional	output3;
	fractional	input4;
	fractional	output4;
} FuncData_four_point;


typedef struct tagFuncData_FIVE_POINT
{
	unsigned int	src;
	fractional	input1;
	fractional	output1;
	fractional	input2;
	fractional	output2;
	fractional	input3;
	fractional	output3;
	fractional	input4;
	fractional	output4;
	fractional	input5;
	fractional	output5;
} FuncData_five_point;


typedef struct tagFuncData_GAIN
{
	unsigned int	src;
	fractional	posGain;
	fractional	negGain;
} FuncData_gain;


typedef struct tagFuncData_RATE
{
	unsigned int	src;
	fractional	posRate;
	fractional	negRate;
} FuncData_rate;


typedef struct tagFuncData_PCT_COND_GAINS
{
	unsigned int	src;
	unsigned int	srcCond;
	fractional	negRate;
	fractional	condMax;
	fractional	condMin;
	fractional	posGainInvalid;
	fractional	negGainInvalid;
	fractional	posGainValid;
	fractional	negGainValid;
} FuncData_pct_cond_gains;


typedef union
{
	FuncData_null	null;
	FuncData_gain_offset	gain_offset;
	FuncData_multiply	multiply;
	FuncData_linear_mux	linear_mux;
	FuncData_conditional_set_ref	conditional_set_ref;
	FuncData_conditional_set	conditional_set;
	FuncData_conditional_gain	conditional_gain;
	FuncData_scale_trim_limit	scale_trim_limit;
	FuncData_scale_reftrim_limit	scale_reftrim_limit;
	FuncData_gain_limit	gain_limit;
	FuncData_three_point	three_point;
	FuncData_four_point	four_point;
	FuncData_five_point	five_point;
	FuncData_gain	gain;
	FuncData_rate	rate;
	FuncData_pct_cond_gains	pct_cond_gains;
} functionData;


/****************************************************************/
// Function Settings


typedef struct tagFunctionSettings
{
  unsigned int    functionType        : 6 ;   // The type of function
  unsigned int    setValue            : 2 ;   // The destination is set(0) added(1) or cleared (2)
  unsigned int    dest                : 8 ;   // The destination register
  functionData    data;
} functionSetting;


/****************************************************************/
// Mixer functions


typedef int (*pflexFunction)(functionSetting*, fractional*); 


extern fractional null_function(functionSetting* pSetting, fractional* pRegisters);
extern fractional gain_offset_function(functionSetting* pSetting, fractional* pRegisters);
extern fractional multiply_function(functionSetting* pSetting, fractional* pRegisters);
extern fractional linear_mux_function(functionSetting* pSetting, fractional* pRegisters);
extern fractional conditional_set_ref_function(functionSetting* pSetting, fractional* pRegisters);
extern fractional conditional_set_function(functionSetting* pSetting, fractional* pRegisters);
extern fractional conditional_gain_function(functionSetting* pSetting, fractional* pRegisters);
extern fractional scale_trim_limit_function(functionSetting* pSetting, fractional* pRegisters);
extern fractional scale_reftrim_limit_function(functionSetting* pSetting, fractional* pRegisters);
extern fractional gain_limit_function(functionSetting* pSetting, fractional* pRegisters);
extern fractional three_point_function(functionSetting* pSetting, fractional* pRegisters);
extern fractional four_point_function(functionSetting* pSetting, fractional* pRegisters);
extern fractional five_point_function(functionSetting* pSetting, fractional* pRegisters);
extern fractional gain_function(functionSetting* pSetting, fractional* pRegisters);
extern fractional rate_function(functionSetting* pSetting, fractional* pRegisters);
extern fractional pct_cond_gains_function(functionSetting* pSetting, fractional* pRegisters);



extern void runFlexiFunctions( void );

extern const pflexFunction flexiFunctions [];

typedef struct tagFLEXIFUNCTION_DATASET
{
	unsigned char inputs_directory[FLEXIFUNCTION_MAX_DIRECTORY_SIZE];
	unsigned char outputs_directory[FLEXIFUNCTION_MAX_DIRECTORY_SIZE];
	unsigned int flexiFunction_directory[FLEXIFUNCTION_MAX_FUNCS];
	unsigned char flexiFunction_data[FLEXIFUNCTION_MAX_DATA_SIZE];
	unsigned int flexiFunctionsUsed;
} FLEXIFUNCTION_DATASET;

extern FLEXIFUNCTION_DATASET flexiFunction_dataset;

extern fractional flexiFunction_registers[FLEXIFUNCTION_MAX_REGS];

extern unsigned char get_input_register_index_from_directory(unsigned char virtual_index);
extern unsigned char get_output_register_index_from_directory(unsigned char virtual_index);


#endif
