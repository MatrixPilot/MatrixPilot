#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "crc16.h"
#include "stptok.h"
#include "logo_parser.h"

// Channel numbers on the board, mapped to positions in the pulse width arrays.
#define CHANNEL_UNUSED          0   // udb_pwIn[0], udb_pwOut[0], etc. are not used, but allow lazy code everywhere else  :)
#define CHANNEL_1               1
#define CHANNEL_2               2
#define CHANNEL_3               3
#define CHANNEL_4               4
#define CHANNEL_5               5
#define CHANNEL_6               6
#define CHANNEL_7               7
#define CHANNEL_8               8
#define CHANNEL_9               9
#define CHANNEL_10              10
#define CHANNEL_11              11
#define CHANNEL_12              12
#define CHANNEL_13              13
#define CHANNEL_14              14
#define CHANNEL_15              15
#define CHANNEL_16              16

#define THROTTLE_INPUT_CHANNEL              CHANNEL_1
#define AILERON_INPUT_CHANNEL               CHANNEL_2
#define ELEVATOR_INPUT_CHANNEL              CHANNEL_3
#define RUDDER_INPUT_CHANNEL                CHANNEL_4
#define OSD_MODE_SWITCH_INPUT_CHANNEL       CHANNEL_5
#define MODE_SWITCH_INPUT_CHANNEL           CHANNEL_6
#define CAMERA_PITCH_INPUT_CHANNEL          CHANNEL_UNUSED
#define CAMERA_YAW_INPUT_CHANNEL            CHANNEL_UNUSED
#define CAMERA_MODE_INPUT_CHANNEL           CHANNEL_UNUSED
//#define OSD_MODE_SWITCH_INPUT_CHANNEL       CHANNEL_UNUSED
#define PASSTHROUGH_A_INPUT_CHANNEL         CHANNEL_UNUSED
#define PASSTHROUGH_B_INPUT_CHANNEL         CHANNEL_UNUSED
#define PASSTHROUGH_C_INPUT_CHANNEL         CHANNEL_UNUSED
#define PASSTHROUGH_D_INPUT_CHANNEL         CHANNEL_UNUSED


#define F_NORMAL               0
#define F_TAKEOFF              1
#define F_INVERTED             2
#define F_HOVER                4
#define F_ROLL_LEFT            8
#define F_ROLL_RIGHT          16
#define F_TRIGGER             32
#define F_LOITER              64
#define F_LAND               128
#define F_ABSOLUTE           256
#define F_ALTITUDE_GOAL      512
#define F_CROSS_TRACK       1024

#define TRIGGER_TYPE_NONE      0
#define TRIGGER_TYPE_SERVO     1
#define TRIGGER_TYPE_DIGITAL   2

#define TRIGGER_PULSE_HIGH     4
#define TRIGGER_PULSE_LOW      8
#define TRIGGER_TOGGLE        16
#define TRIGGER_REPEATING     32


#define PLANE           0
#define CAMERA          1

// Define the conditional VAL values for IF commands
enum {
	LOGO_VAL_ZERO = 0,
	// XX_INPUT_CHANNEL // leave room for input channels: 1 - NUM_INPUTS (up to 15)
	DIST_TO_HOME = 16,
	DIST_TO_GOAL,
	ALT,
	CURRENT_ANGLE,
	ANGLE_TO_HOME,
	ANGLE_TO_GOAL,
	REL_ANGLE_TO_HOME,
	REL_ANGLE_TO_GOAL,
	GROUND_SPEED,
	AIR_SPEED,
	AIR_SPEED_Z,
	WIND_SPEED,
	WIND_SPEED_X,
	WIND_SPEED_Y,
	WIND_SPEED_Z,
	PARAM
};

static void logo_defines_write(FILE* fp)
{
fprintf(fp, "#define PLANE\t%u\n", PLANE);
fprintf(fp, "#define CAMERA\t%u\n", CAMERA);
fprintf(fp, "#define LOGO_VAL_ZERO\t%u\n", LOGO_VAL_ZERO);
fprintf(fp, "#define THROTTLE_INPUT_CHANNEL\t%u\n", THROTTLE_INPUT_CHANNEL);
fprintf(fp, "#define AILERON_INPUT_CHANNEL\t%u\n", AILERON_INPUT_CHANNEL);
fprintf(fp, "#define ELEVATOR_INPUT_CHANNEL\t%u\n", ELEVATOR_INPUT_CHANNEL);
fprintf(fp, "#define RUDDER_INPUT_CHANNEL\t%u\n", RUDDER_INPUT_CHANNEL);
fprintf(fp, "#define OSD_MODE_SWITCH_INPUT_CHANNEL\t%u\n", OSD_MODE_SWITCH_INPUT_CHANNEL);
fprintf(fp, "#define MODE_SWITCH_INPUT_CHANNEL\t%u\n", MODE_SWITCH_INPUT_CHANNEL);
fprintf(fp, "#define CAMERA_PITCH_INPUT_CHANNEL\t%u\n", CAMERA_PITCH_INPUT_CHANNEL);
fprintf(fp, "#define CAMERA_YAW_INPUT_CHANNEL\t%u\n", CAMERA_YAW_INPUT_CHANNEL);
fprintf(fp, "#define CAMERA_MODE_INPUT_CHANNEL\t%u\n", CAMERA_MODE_INPUT_CHANNEL);
fprintf(fp, "#define PASSTHROUGH_A_INPUT_CHANNEL\t%u\n", PASSTHROUGH_A_INPUT_CHANNEL);
fprintf(fp, "#define PASSTHROUGH_B_INPUT_CHANNEL\t%u\n", PASSTHROUGH_B_INPUT_CHANNEL);
fprintf(fp, "#define PASSTHROUGH_C_INPUT_CHANNEL\t%u\n", PASSTHROUGH_C_INPUT_CHANNEL);
fprintf(fp, "#define PASSTHROUGH_D_INPUT_CHANNEL\t%u\n", PASSTHROUGH_D_INPUT_CHANNEL);
fprintf(fp, "#define DIST_TO_HOME\t%u\n", DIST_TO_HOME);
fprintf(fp, "#define DIST_TO_GOAL\t%u\n", DIST_TO_GOAL);
fprintf(fp, "#define ALT\t%u\n", ALT);
fprintf(fp, "#define CURRENT_ANGLE\t%u\n", CURRENT_ANGLE);
fprintf(fp, "#define ANGLE_TO_HOME\t%u\n", ANGLE_TO_HOME);
fprintf(fp, "#define ANGLE_TO_GOAL\t%u\n", ANGLE_TO_GOAL);
fprintf(fp, "#define REL_ANGLE_TO_HOME\t%u\n", REL_ANGLE_TO_HOME);
fprintf(fp, "#define REL_ANGLE_TO_GOAL\t%u\n", REL_ANGLE_TO_GOAL);
fprintf(fp, "#define GROUND_SPEED\t%u\n", GROUND_SPEED);
fprintf(fp, "#define AIR_SPEED\t%u\n", AIR_SPEED);
fprintf(fp, "#define AIR_SPEED_Z\t%u\n", AIR_SPEED_Z);
fprintf(fp, "#define WIND_SPEED\t%u\n", WIND_SPEED);
fprintf(fp, "#define WIND_SPEED_X\t%u\n", WIND_SPEED_X);
fprintf(fp, "#define WIND_SPEED_Y\t%u\n", WIND_SPEED_Y);
fprintf(fp, "#define WIND_SPEED_Z\t%u\n", WIND_SPEED_Z);
fprintf(fp, "#define PARAM\t%u\n", PARAM);

fprintf(fp, "#define F_NORMAL\t%u\n", F_NORMAL); //               0
fprintf(fp, "#define F_TAKEOFF\t%u\n", F_TAKEOFF); //               1
fprintf(fp, "#define F_INVERTED\t%u\n", F_INVERTED); //              2
fprintf(fp, "#define F_HOVER\t%u\n", F_HOVER); //                4
fprintf(fp, "#define F_ROLL_LEFT\t%u\n", F_ROLL_LEFT); //            8
fprintf(fp, "#define F_ROLL_RIGHT\t%u\n", F_ROLL_RIGHT); //           16
fprintf(fp, "#define F_TRIGGER\t%u\n", F_TRIGGER); //              32
fprintf(fp, "#define F_LOITER\t%u\n", F_LOITER); //              64
fprintf(fp, "#define F_LAND\t%u\n",  F_LAND); //              128
fprintf(fp, "#define F_ABSOLUTE\t%u\n", F_ABSOLUTE); //          256
fprintf(fp, "#define F_ALTITUDE_GOAL\t%u\n", F_ALTITUDE_GOAL); //      512
fprintf(fp, "#define F_CROSS_TRACK\t%u\n", F_CROSS_TRACK); //        1024

fprintf(fp, "#define TRIGGER_TYPE_NONE\t%u\n", TRIGGER_TYPE_NONE); //       0
fprintf(fp, "#define TRIGGER_TYPE_SERVO\t%u\n", TRIGGER_TYPE_SERVO); //     1
fprintf(fp, "#define TRIGGER_TYPE_DIGITAL\t%u\n", TRIGGER_TYPE_DIGITAL); //    2

fprintf(fp, "#define TRIGGER_PULSE_HIGH\t%u\n", TRIGGER_PULSE_HIGH); //      4
fprintf(fp, "#define TRIGGER_PULSE_LOW\t%u\n", TRIGGER_PULSE_LOW); //       8
fprintf(fp, "#define TRIGGER_TOGGLE\t%u\n", TRIGGER_TOGGLE); //         16
fprintf(fp, "#define TRIGGER_REPEATING\t%u\n", TRIGGER_REPEATING); //      32

fprintf(fp, "\n");
}

typedef struct tagCmds {
	const char* cmd;
	logoInstructionDef_t opcode;
} cmds_t;


/*
typedef struct logoDef {
	uint16_t cmd        :  6;
	uint16_t do_fly     :  1;
	uint16_t use_param  :  1;
	uint16_t subcmd     :  4;
	int16_t arg         :  4;
} logoDef_t;

Variable arg in logoDef_t can be assigned the following values:
	-1
	 0
	 1
	 P_A
	-P_A
	 P_B

1:	-1
2:	 0
3:	 1
4:	-P_A
5:	 P_A
6:	-P_B
7:	 P_B
8:	-P_A_C2
9:	 P_A_C2
10:	-P_B_C2
11:	 P_B_C2
12:	-P_A_C3
13:	 P_A_C3
14:	-P_B_C3
15:	 P_B_C3
16:
*/

#define P_A 8  // parameter A substitution value (first argument)
#define P_B 9  // parameter B substitution value (second argument)

#define P_A_C2 9  // compound command, 2 opcodes
#define P_B_C3 10 // compound command, 3 opcodes

static const cmds_t cmdslist[] = {
//                           cmd, fly, prm, sub,   x
	{ "FD",                   {3,   1,   0,   0,  P_A}, }, //(x)  _FD(x, 1, 0)
	{ "BK",                   {3,   1,   0,   0, -P_A}, }, //(x)  _FD(-x, 1, 0)
	{ "FD_PARAM",             {3,   1,   1,   0,    1}, }, //     _FD(1, 1, 1)
	{ "BK_PARAM",             {3,   1,   1,   0,   -1}, }, //     _FD(-1, 1, 1)
	{ "RT",                   {4,   0,  -1,   0,  P_A}, }, //(x)  _RT(x, 0)
	{ "LT",                   {4,   0,  -1,   0, -P_A}, }, //(x)  _RT(-x, 0)
	{ "SET_ANGLE",            {4,   0,  -1,   1,  P_A}, }, //(x)  _SET_ANGLE(x, 0)
	{ "RT_PARAM",             {4,   0,  -1,   0,   -1}, }, //     _RT(1, 1)
	{ "LT_PARAM",             {4,   0,  -1,   0,   -1}, }, //	  _RT(-1, 1)
	{ "SET_ANGLE_PARAM",      {4,   0,  -1,   1,   -1}, }, //     _SET_ANGLE(0, 1)
	{ "USE_CURRENT_ANGLE",    {4,   0,   0,   2,    0}, }, //     _USE_CURRENT_ANGLE
	{ "USE_ANGLE_TO_GOAL",    {4,   0,   0,   3,    0}, }, //     _USE_ANGLE_TO_GOAL
	{ "EAST",                 {5,   1,   0,   0,  P_A}, }, //(x)  _MV_X(x, 1, 0)
	{ "WEST",                 {5,   1,   0,   0, -P_A}, }, //(x)  _MV_X(-x, 1, 0)
	{ "SET_X_POS",            {5,   1,   0,   1,  P_A}, }, //(x)  _SET_X(x, 1, 0)
	{ "EAST_PARAM",           {5,   1,   1,   0,    1}, }, //     _MV_X(1, 1, 1)
	{ "WEST_PARAM",           {5,   1,   1,   0,   -1}, }, //     _MV_X(-1, 1, 1)
	{ "SET_X_POS_PARAM",      {5,   1,   1,   1,    1}, }, //     _SET_X(1, 1, 1)
	{ "USE_CURRENT_POS",      {5,   1,   0,   6,    0}, }, //     _USE_CURRENT_POS(1)
	{ "NORTH",                {5,   1,   0,   2,  P_A}, }, //(y)  _MV_Y(y, 1, 0)
	{ "SOUTH",                {5,   1,   0,   2, -P_A}, }, //(y)  _MV_Y(-y, 1, 0)
	{ "SET_Y_POS",            {5,   1,   0,   3,  P_A}, }, //(y)  _SET_Y(y, 1, 0)
	{ "NORTH_PARAM",          {5,   1,   1,   2,    1}, }, //     _MV_Y(1, 1, 1)
	{ "SOUTH_PARAM",          {5,   1,   1,   2,   -1}, }, //     _MV_Y(-1, 1, 1)
	{ "SET_Y_POS_PARAM",      {5,   1,   1,   3,    1}, }, //      _SET_Y(1, 1, 1)
	{ "ALT_UP",               {5,   0,   0,   4,  P_A}, }, //(z)  _MV_Z(z, 0, 0)
	{ "ALT_DOWN",             {5,   0,   0,   4, -P_A}, }, //(z)  _MV_Z(-z, 0, 0)
	{ "SET_ALT",              {5,   0,   0,   5,  P_A}, }, //(z)  _SET_Z(z, 0, 0)
	{ "ALT_UP_PARAM",         {5,   0,   1,   4,    1}, }, //     _MV_Z(1, 0, 1)
	{ "ALT_DOWN_PARAM",       {5,   0,   1,   4,   -1}, }, //     _MV_Z(-1, 0, 1)
	{ "SET_ALT_PARAM",        {5,   0,   1,   5,    1}, }, //     _SET_Z(1, 0, 1)
	{ "SPEED_INCREASE",       {11,  0,   0,   0,  P_A}, }, //(x)  _SPEED_INCREASE(x, 0)
	{ "SPEED_DECREASE",       {11,  0,   0,   0, -P_A}, }, //(x)  _SPEED_INCREASE(-x, 0)
	{ "SET_SPEED",            {11,  0,   0,   1,  P_A}, }, //(x)  _SET_SPEED(x, 0)
	{ "SPEED_INCREASE_PARAM", {11,  0,   1,   0,    1}, }, //     _SPEED_INCREASE(1, 1)
	{ "SPEED_DECREASE_PARAM", {11,  0,   1,   0,   -1}, }, //     _SPEED_INCREASE(-1, 1)
	{ "SET_SPEED_PARAM",      {11,  0,   1,   1,    0}, }, //     _SET_SPEED(0, 1)
	{ "FLAG_ON",              {6,   0,   0,   0,  P_A}, }, //(f)  _FLAG_ON(f)
	{ "FLAG_OFF",             {6,   0,   0,   1,  P_A}, }, //(f)  _FLAG_OFF(f)
	{ "FLAG_TOGGLE",          {6,   0,   0,   2,  P_A}, }, //(f)  _FLAG_TOGGLE(f)
	{ "PEN_UP",               {7,   0,   0,   0,    0}, }, //     _PEN_UP
	{ "PEN_DOWN",             {7,   1,   0,   1,    0}, }, //     _PEN_DOWN
	{ "PEN_TOGGLE",           {7,   0,   0,   2,    0}, }, //     _PEN_TOGGLE
	{ "SET_TURTLE",           {8,   0,   0,   0,  P_A}, }, //(x)  _SET_TURTLE(x)
	{ "REPEAT",               {1,   0,   0,   0,  P_A}, }, //(n)  _REPEAT(n, 0)
	{ "REPEAT_PARAM",         {1,   0,   1,   0,    1}, }, //     _REPEAT(1, 1)
	{ "REPEAT_FOREVER",       {1,   0,   0,   0,   -1}, }, //     _REPEAT(-1, 0)
	{ "END",                  {1,   0,   0,   1,    0}, }, //     _END
	{ "ELSE",                 {1,   0,   0,   3,    0}, }, //     _ELSE
	{ "TO",                   {1,   0,   0,   2,  P_A}, }, //(fn)      _TO(fn)
	{ "DO",                   {2,   0,   0, P_A,    0}, }, //(fn)      _DO(fn, 0, 0)
	{ "DO_ARG",               {2,   0,   0, P_A,  P_B}, }, //(fn, arg) _DO(fn, arg, 0)
	{ "DO_PARAM",             {2,   0,   1, P_A,    1}, }, //(fn)      _DO(fn, 1, 1)
	{ "EXEC",                 {10,  0,   0, P_A,    0}, }, //(fn)      _EXEC(fn, 0, 0)
	{ "EXEC_ARG",             {10,  0,   0, P_A,  P_B}, }, //(fn, arg) _EXEC(fn, arg, 0)
	{ "EXEC_PARAM",           {10,  0,   1, P_A,    1}, }, //(fn)      _EXEC(fn, 1, 1)
	{ "PARAM_SET",            {9,   0,   0,   0,  P_A}, }, //(x)       _PARAM_SET(x)
	{ "PARAM_ADD",            {9,   0,   0,   1,  P_A}, }, //(x)       _PARAM_ADD(x)
	{ "PARAM_SUB",            {9,   0,   0,   1, -P_A}, }, //(x)       _PARAM_ADD(-x)
	{ "PARAM_MUL",            {9,   0,   0,   2,  P_A}, }, //(x)       _PARAM_MUL(x)
	{ "PARAM_DIV",            {9,   0,   0,   3,  P_A}, }, //(x)       _PARAM_DIV(x)
	{ "SET_INTERRUPT",        {12,  0,   0,   1,  P_A}, }, //(fn)      _SET_INTERRUPT(fn)
	{ "CLEAR_INTERRUPT",      {12,  0,   0,   0,    0}, }, //          _CLEAR_INTERRUPT
	{ "LOAD_TO_PARAM",        {13,  0,   0, P_A,    0}, }, //(val)     _LOAD_TO_PARAM(val)
	{ "IF_EQ",                {14,  0,   0, P_A,  P_B}, }, //(val, x)  _IF_EQ(val, x, 0)
	{ "IF_NE",                {15,  0,   0, P_A,  P_B}, }, //(val, x)  _IF_NE(val, x, 0)
	{ "IF_GT",                {16,  0,   0, P_A,  P_B}, }, //(val, x)  _IF_GT(val, x, 0)
	{ "IF_LT",                {17,  0,   0, P_A,  P_B}, }, //(val, x)  _IF_LT(val, x, 0)
	{ "IF_GE",                {18,  0,   0, P_A,  P_B}, }, //(val, x)  _IF_GE(val, x, 0)
	{ "IF_LE",                {19,  0,   0, P_A,  P_B}, }, //(val, x)  _IF_LE(val, x, 0)
	{ "IF_EQ_PARAM",          {14,  0,   1, P_A,    1}, }, //(val)     _IF_EQ(val, 1, 1)
	{ "IF_NE_PARAM",          {15,  0,   1, P_A,    1}, }, //(val)     _IF_NE(val, 1, 1)
	{ "IF_GT_PARAM",          {16,  0,   1, P_A,    1}, }, //(val)     _IF_GT(val, 1, 1)
	{ "IF_LT_PARAM",          {17,  0,   1, P_A,    1}, }, //(val)     _IF_LT(val, 1, 1)
	{ "IF_GE_PARAM",          {18,  0,   1, P_A,    1}, }, //(val)     _IF_GE(val, 1, 1)
	{ "IF_LE_PARAM",          {19,  0,   1, P_A,    1}, }, //(val)     _IF_LE(val, 1, 1)
	{ "HOME",                 {5,   1,   0,   7,    0}, }, //          _HOME(1)

	{ "SET_POS",              {5,   1,   0,   1,  P_A}, }, //          _SET_X()  -- Compound Command
	{ "SET_POS_",             {5,   1,   0,   3,  P_B}, }, //          _SET_Y()
	{ "SET_ABS_POS_X",        {5,   0,   0,   8,  P_A}, }, //          _SET_ABS_VAL_HIGH()
	{ "SET_ABS_POS_X_",       {5,   0,   0,   9,  P_B}, }, //          _SET_ABS_X_LOW()
	{ "SET_ABS_POS_Y",        {5,   0,   0,   8,  P_A}, }, //          _SET_ABS_VAL_HIGH()
	{ "SET_ABS_POS_Y_",       {5,   1,   0,  10,  P_B}, }, //          _SET_ABS_Y_LOW()
/*
#define CC 37
//                           cmd, fly, prm, sub,   x
	{ "SET_POS",              {CC,  1,   0,   1,  P_A}, }, //          _SET_X()  -- Compound Command
	{ "SET_POS-CC1",          {CC,  1,   0,   3,  P_B}, }, //          _SET_Y()
	{ "SET_ABS_POS",          {CC,  0,   0,   8,  P_A}, }, //          _SET_ABS_VAL_HIGH()
	{ "SET_ABS_POS-CC2",      {CC,  0,   0,   9,  P_B}, }, //          _SET_ABS_X_LOW()
	{ "SET_ABS_POS",          {CC,  0,   0,   8,  P_A}, }, //          _SET_ABS_VAL_HIGH()
	{ "SET_ABS_POS-CC3",      {CC,  1,   0,  10,  P_C}, }, //          _SET_ABS_Y_LOW()
 */
};
/* 
	{ "SET_X_POS",            {5,   1,   0,   1,  P_A}, }, //(x)  _SET_X(x, 1, 0)
	{ "SET_Y_POS",            {5,   1,   0,   3,  P_A}, }, //(y)  _SET_Y(y, 1, 0)

#define _SET_X(x, fl, pr)       {5,   fl,  pr,  1,   x},
#define SET_X_POS(x)            _SET_X(x, 1, 0)

#define _SET_Y(y, fl, pr)       {5,   fl,  pr,  3,   y},
#define SET_Y_POS(y)            _SET_Y(y, 1, 0)


TODO: work out how to handle the compound instructions

#define _SET_X(x, fl, pr)       {5,   fl,  pr,  1,   x},
#define _SET_Y(y, fl, pr)       {5,   fl,  pr,  3,   y},

#define SET_POS(x, y)           _SET_X(x, 0, 0) _SET_Y(y, 1, 0)

#define _SET_ABS_VAL_HIGH(x)    {5,   0,   0,   8,   x}, // Set the high and then low words for X and
#define _SET_ABS_X_LOW(x)       {5,   0,   0,   9,   x}, // then Y, as 4 consecutive instructions.
#define _SET_ABS_Y_LOW(y, fl)   {5,   fl,  0,   10,  y}, // (as VAL_HIGH, X_LOW, VAL_HIGH, Y_LOW)

#define SET_ABS_POS(x, y)       _SET_ABS_VAL_HIGH((((uint32_t)(x))>>16)&0xFFFF) _SET_ABS_X_LOW(((uint32_t)(x))&0xFFFF) \
                                _SET_ABS_VAL_HIGH((((uint32_t)(y))>>16)&0xFFFF) _SET_ABS_Y_LOW(((uint32_t)(y))&0xFFFF, 1)

#define SET_ABS_POS_X(x)       _SET_ABS_VAL_HIGH((((uint32_t)(x))>>16)&0xFFFF) _SET_ABS_X_LOW(((uint32_t)(x))&0xFFFF)
#define SET_ABS_POS_Y(y)       _SET_ABS_VAL_HIGH((((uint32_t)(y))>>16)&0xFFFF) _SET_ABS_Y_LOW(((uint32_t)(y))&0xFFFF, 1)
*/

 static void logo_hash_write(FILE* fp)
{
	uint8_t i;
	uint16_t strsize = 0;

	for (i = 0; i < (sizeof(cmdslist)/sizeof(cmdslist[0])); i++) {
		fprintf(fp, "%s\t%u\n", cmdslist[i].cmd, crc16((uint8_t*)cmdslist[i].cmd, strlen(cmdslist[i].cmd)));
		strsize += (strlen(cmdslist[i].cmd) - 1);
	}
	fprintf(fp, "strsize\t%u\n", strsize);
}

//const cmds_t cmdslist[] = {
//                           cmd, fly, prm, sub,   x
//	{ "FD",                   {3,   1,   0,   0,  P_A}, }, //(x)  _FD(x, 1, 0)
//typedef struct logoInstructionDef {
//	uint16_t cmd        :  6;
//	uint16_t do_fly     :  1;
//	uint16_t use_param  :  1;
//	uint16_t subcmd     :  8;
//	int16_t arg         : 16;
//} logoInstructionDef_t;
//typedef struct tagCmds {
//	const char* cmd;
//	logoInstructionDef_t opcode;
//} cmds_t;

char* compound_cmds[] = { "SET_POS", "SET_ABS_POS_X", "SET_ABS_POS_Y" };

void logo_code_write(FILE* fp)
{
	uint8_t i, j;

	fprintf(fp, "#define P_A %u\n", P_A);
	fprintf(fp, "#define P_B %u\n", P_B);
	fprintf(fp, "static const logo_cmd_t logo_cmd_list[] = {\n");
	for (i = 0; i < (sizeof(cmdslist)/sizeof(cmdslist[0])); i++) {
		int8_t compound_cmd_flag = 0;
		for (j = 0; j < (sizeof(compound_cmds)/sizeof(compound_cmds[0])); j++) {
			if (strcmp(cmdslist[i].cmd, compound_cmds[j]) == 0) {
				compound_cmd_flag = 1;
			}
		}
		fprintf(fp, "\t{ %u, { %i, %i, %i, %i, %i, %i }, }, // %s\n", 
			crc16((uint8_t*)cmdslist[i].cmd, strlen(cmdslist[i].cmd)),
			compound_cmd_flag,
			cmdslist[i].opcode.cmd,
			cmdslist[i].opcode.do_fly,
			cmdslist[i].opcode.use_param,
			cmdslist[i].opcode.subcmd,
			cmdslist[i].opcode.arg,
			cmdslist[i].cmd);
	}
	fprintf(fp, "};\n\n");
}

boolean create_logo_file(uint8_t type, char* filename)
{
	FILE* fp;

	fp = fopen(filename, "w+");
	if (fp) {
		if (type == 1) {
			logo_defines_write(fp);
		}
		if (type == 2) {
			logo_hash_write(fp);
		}
		if (type == 3) {
			logo_code_write(fp);
		}
		fclose(fp);
	} else {
		printf("ERROR: failed to open %s\r\n", filename);
		return false;
	}
	return true;
}
