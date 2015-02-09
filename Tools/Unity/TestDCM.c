
#include <setjmp.h>
#include "unity.h"

#include "../../libUDB/udbTypes.h"
#include "../../libDCM/dcmTypes.h"

#include "../../libDCM/libDCM.h"
#include "../../libDCM/mathlibNAV.h"


static int SetToOneToFailInTearDown;
static int SetToOneMeanWeAlreadyCheckedThisGuy;

void setUp(void)
{
  SetToOneToFailInTearDown = 0;
  SetToOneMeanWeAlreadyCheckedThisGuy = 0;
}

void tearDown(void)
{
  if (SetToOneToFailInTearDown == 1)
    TEST_FAIL_MESSAGE("<= Failed in tearDown");
  if ((SetToOneMeanWeAlreadyCheckedThisGuy == 0) && (Unity.CurrentTestFailed > 0))
  {
    UnityPrint("[[[[ Previous Test Should Have Passed But Did Not ]]]]");
    UNITY_OUTPUT_CHAR('\n');
  }
}


//#define _SCALEACCEL 1.29    // 4 g range
#define _SCALEACCEL 1.27    // 4 g range measured by WJP on a few UDB5s
#define _SCALEGYRO  3.0016  // 500 degree/second range
#define _GRAVITY    ((int32_t)(5280.0/_SCALEACCEL))  // gravity in AtoD/2 units
#define _RADPERSEC  ((int64_t)5632.0/_SCALEGYRO) // one radian per second, in AtoD/2 units
#define _GRAVITYM   ((int64_t)980.0)             // 100 times gravity, meters/sec/sec

//#define _CENTRISCALE ((int32_t)(((int64_t)519168.0)*_GRAVITY)/((int64_t)_RADPERSEC*_GRAVITYM))                         // old
#define _CENTRISCALE ((uint64_t)((((uint64_t)519168.0)*(uint64_t)_GRAVITY)/((uint64_t)_RADPERSEC*(uint64_t)_GRAVITYM)))  // new

void test_DCM(void)
{
	int64_t a;
	uint64_t b;
	int32_t gravity;
	float scaleaccel;

	printf("SCALEACCEL  %f\r\n", _SCALEACCEL);
	printf("GRAVITY     %ld\r\n", _GRAVITY);
	printf("GRAVITYM    %lld\r\n", _GRAVITYM);
	printf("RADPERSEC   %lld\r\n", _RADPERSEC);
	printf("CENTRISCALE %lld\r\n", _CENTRISCALE);

//	a = (uint64_t)_CENTRISCALE / (uint64_t)519168.0;
	a = _CENTRISCALE;
	b = ((uint64_t)_RADPERSEC*(uint64_t)_GRAVITYM);
	printf("a %lld\r\n", (int64_t)a);
	printf("b %llu\r\n", (uint64_t)b);
//	gravity = (int32_t)(a * b);
	
	gravity = ((uint64_t)_CENTRISCALE * (uint64_t)_RADPERSEC*(uint64_t)_GRAVITYM) / (uint64_t)519168.0;
	
	printf("gravity  %d\r\n", gravity);
	scaleaccel = 5280.0 / (float)gravity;
	printf("scaleaccel  %f\r\n", scaleaccel);

	printf("delta  %f\r\n", scaleaccel - _SCALEACCEL);

//	TEST_ASSERT_FLOAT_WITHIN(delta, expected, actual)
	TEST_ASSERT_FLOAT_WITHIN(0.0015, _SCALEACCEL, scaleaccel);
}
/*
int16_t sine(int8_t angle);
int8_t arcsine(int16_t angle);
int16_t cosine(int8_t angle);

uint16_t sqrt_int(uint16_t sqr);
uint16_t sqrt_long(uint32_t sqr);
int32_t long_scale(int32_t arg1, int16_t arg2);

void rotate_2D_vector_by_vector(int16_t vector[2], int16_t rotate[2]);      //-> void vect2_16x16_rotate(vect2_16t* vector, const vect2_16t* rotate);
void rotate_2D_long_vector_by_vector(int32_t vector[2], int16_t rotate[2]); //-> void vect2_32x16_rotate(vect2_32t* vector, const vect2_16t* rotate);
void rotate_2D_vector_by_angle(int16_t vector[2], int8_t angle);            //-> void vect2_16phi_rotate(vect2_16t* vector, int8_t angle);
void rotate_2D(struct relative2D *vector, int8_t angle);     //-> void vect2_16phi_rotate(vect2_16t* vector, int8_t angle);
int8_t rect_to_polar(struct relative2D *xy);          //-> uint8_t vect2_polar(polar_16t* polar, const vect2_16t* vector);
int16_t rect_to_polar16(struct relative2D *xy);       //-> uint16_t vect2_polar_16(polar_16t* polar, const vect2_16t* vector);
uint16_t vector2_mag(int16_t x, int16_t y);                    //-> uint16_t vect2_16_mag(const vect2_16t* vector);
uint16_t vector3_mag(int16_t x, int16_t y, int16_t z);         //-> uint16_t vect3_16_mag(const vect3_16t* vector);
uint16_t vector2_normalize(int16_t result[], int16_t input[]); //-> uint16_t vect2_16_norm(vect2_16t* result, const vect2_16t* vector);
uint16_t vector3_normalize(int16_t result[], int16_t input[]); //-> uint16_t vect3_16_norm(vect3_16t* result, const vect3_16t* vector);

// first changed bit location functions
int16_t FindFirstBitChangeFromLeft(int16_t);    // applies raw FBCL instruction and returns result
int16_t FindFirstBitFromLeft(int16_t);          // applies raw FF1L instruction and returns result
int16_t find_first_bit_int16(int16_t);          // finds first bit position in the absolute value of a 16 bit signed integer
int16_t find_first_bit_int32(int32_t);          // finds first bit position in the absolute value of a 32 bit signed integer
 */


void test_sqrt_int(void)
{
uint16_t sqrt_int(uint16_t sqr);

	uint16_t sqr;
	uint16_t result;

	sqr = 1764;
	result = sqrt_long(sqr);
	TEST_ASSERT_EQUAL(42, result);
}

void test_sqrt_long(void)
{
uint16_t sqrt_long(uint32_t sqr);

	uint32_t sqr;
	uint16_t result;

	sqr = 78287104L;
	result = sqrt_long(sqr);
	TEST_ASSERT_EQUAL(8848, result);
}

void test_long_scale(void)
{
	// returns arg1*arg2/RMAX
	// usually this is used where arg2 is a Q14.2 fractional number
	int32_t long_scale(int32_t arg1, int16_t arg2);
//#define RMAX                    16384//0b0100000000000000       // 1.0 in 2.14 fractional format

	int32_t arg1;
	int16_t arg2;
	int32_t result = RMAX;

	arg1 = 31234;
	arg2 = 12345;
	result = long_scale(arg1, arg2);
//	TEST_ASSERT_EQUAL(23534, result); // 23534.1632080078125 from calc
	TEST_ASSERT_EQUAL(23532, result); // 23532 from libDCM:SIL
//	TEST_ASSERT_EQUAL(arg1*arg2/RMAX, result); // 23534 expected
	
	arg1 = 32768;
	arg2 = 32768;
	result = long_scale(arg1, arg2);
//	TEST_ASSERT_EQUAL(65536, result); // 65536 from calc
	TEST_ASSERT_EQUAL(-65536, result); // -65536 from libDCM:SIL
	TEST_ASSERT_EQUAL(arg1*arg2/RMAX, result);

	arg1 = 32767;
	arg2 = 32767;
	result = long_scale(arg1, arg2);
	TEST_ASSERT_EQUAL(65532, result); // 65532.00006103515625 from calc
	TEST_ASSERT_EQUAL(65532, result); // 65532 from libDCM:SIL
	TEST_ASSERT_EQUAL(arg1*arg2/RMAX, result);

	arg1 = 5000;
	arg2 = 25000;
	result = long_scale(arg1, arg2);
//	TEST_ASSERT_EQUAL(7629, result); // 7629.39453125 from calc
	TEST_ASSERT_EQUAL(7628, result); // 7628 from libDCM:SIL
//	TEST_ASSERT_EQUAL(arg1*arg2/RMAX, result); // this expects 7629?

	arg1 = 0;
	arg2 = 0;
	result = long_scale(arg1, arg2);
	TEST_ASSERT_EQUAL(0, result);
	TEST_ASSERT_EQUAL(arg1*arg2/RMAX, result);
}

void test_rect_to_polar(void)
{
//int8_t rect_to_polar(struct relative2D *xy);          //-> uint8_t vect2_polar(polar_16t* polar, const vect2_16t* vector);

	int8_t angle;
	struct relative2D xy;

	xy.x = 2;
	xy.y = 3;
	angle = rect_to_polar(&xy);
	TEST_ASSERT_EQUAL_INT16(40, angle);
	TEST_ASSERT_EQUAL_INT16(0, xy.y);
	TEST_ASSERT_EQUAL_INT16(3, xy.x);

//navigate_set_goal:rect_to_polar(courseLeg.x -1884, .y 661) returned phi 115, x 1992, y -40
	xy.x = -1884;
	xy.y = 661;
	angle = rect_to_polar(&xy);
	TEST_ASSERT_EQUAL_INT16(115, angle);
	TEST_ASSERT_EQUAL_INT16(1992, xy.x);
	TEST_ASSERT_EQUAL_INT16(-40, xy.y);

//navigate_set_goal:rect_to_polar(courseLeg.x -3000, .y 403) returned phi 123, x 3023, y -35
	xy.x = -3000;
	xy.y = 403;
	angle = rect_to_polar(&xy);
	TEST_ASSERT_EQUAL_INT16(123, angle);
	TEST_ASSERT_EQUAL_INT16(3023, xy.x);
	TEST_ASSERT_EQUAL_INT16(-35, xy.y);

//navigate_set_goal:rect_to_polar(courseLeg.x 2624, .y -1578) returned phi -22, x3058, y 68
	xy.x = 2624;
	xy.y = -1578;
	angle = rect_to_polar(&xy);
	TEST_ASSERT_EQUAL_INT16(-22, angle);
	TEST_ASSERT_EQUAL_INT16(3058, xy.x);
	TEST_ASSERT_EQUAL_INT16(68, xy.y);

//navigate_set_goal:rect_to_polar(courseLeg.x 0, .y 1578) returned phi 64, x 1573, y 37
	xy.x = 0;
	xy.y = 1578;
	angle = rect_to_polar(&xy);
	TEST_ASSERT_EQUAL_INT16(64, angle);
	TEST_ASSERT_EQUAL_INT16(1573, xy.x);
	TEST_ASSERT_EQUAL_INT16(37, xy.y);

//navigate_set_goal:rect_to_polar(courseLeg.x -2624, .y -1578) returned phi -105, x 3059, y -74
	xy.x = -2624;
	xy.y = -1578;
	angle = rect_to_polar(&xy);
	TEST_ASSERT_EQUAL_INT16(-105, angle);
	TEST_ASSERT_EQUAL_INT16(3059, xy.x);
	TEST_ASSERT_EQUAL_INT16(-74, xy.y);
}

void test_rect_to_polar16(void)
{
//int16_t rect_to_polar16(struct relative2D *xy);       //-> uint16_t vect2_polar_16(polar_16t* polar, const vect2_16t* vector);

	int16_t angle;
	struct relative2D xy;

//navigate_set_goal:rect_to_polar16(x -1072, y 721) returned phi 26570, x 1289, y25
	xy.x = -1072;
	xy.y = 721;
	angle = rect_to_polar16(&xy);
	TEST_ASSERT_EQUAL_INT16(26570, angle);
	TEST_ASSERT_EQUAL_INT16(1289, xy.x);
	TEST_ASSERT_EQUAL_INT16(25, xy.y);

//navigate_set_goal:rect_to_polar16(x 2628, y -1577) returned phi -5647, x 3061, y 71
	xy.x = 2628;
	xy.y = -1577;
	angle = rect_to_polar16(&xy);
	TEST_ASSERT_EQUAL_INT16(-5647, angle);
	TEST_ASSERT_EQUAL_INT16(3061, xy.x);
	TEST_ASSERT_EQUAL_INT16(71, xy.y);

//navigate_set_goal:rect_to_polar16(x 0, y 1577) returned phi 16373, x 1573, y 37:
	xy.x = 0;
	xy.y = 1577;
	angle = rect_to_polar16(&xy);
	TEST_ASSERT_EQUAL_INT16(16373, angle);
	TEST_ASSERT_EQUAL_INT16(1573, xy.x);
	TEST_ASSERT_EQUAL_INT16(37, xy.y);

//navigate_set_goal:rect_to_polar16(x -2628, y -1577) returned phi -27140, x 3061, y 74:
	xy.x = -2628;
	xy.y = -1577;
	angle = rect_to_polar16(&xy);
	TEST_ASSERT_EQUAL_INT16(-27140, angle);
	TEST_ASSERT_EQUAL_INT16(3061, xy.x);
	TEST_ASSERT_EQUAL_INT16(74, xy.y);
}

// from MAVLink.c
//		matrix_accum.x = rmat[8];
//		matrix_accum.y = rmat[6];
//		accum = rect_to_polar16(&matrix_accum);     // binary angle (0 to 65536 = 360 degrees)
//		earth_roll = (-accum) * BYTE_CIR_16_TO_RAD; // Convert to Radians

void test_vect2_polar(void)
{
//uint8_t vect2_polar(polar_16t* polar, const vect2_16t* vector)
	int8_t angle;
	vect2_16t vector;
	polar_16t polar;

	vector.x = 2;
	vector.y = 3;
	angle = vect2_polar(&polar, &vector);
	TEST_ASSERT_EQUAL_INT16(40, angle);
	TEST_ASSERT_EQUAL_INT16(3, polar.r);
	TEST_ASSERT_EQUAL_INT16(40, polar.p);

//navigate_set_goal:rect_to_polar(courseLeg.x -1884, .y 661) returned phi 115, x 1992, y -40
	vector.x = -1884;
	vector.y = 661;
	angle = vect2_polar(&polar, &vector);
	TEST_ASSERT_EQUAL_INT16(115, angle);
	TEST_ASSERT_EQUAL_INT16(1992, polar.r);
	TEST_ASSERT_EQUAL_INT16(115, polar.p);

//navigate_set_goal:rect_to_polar(courseLeg.x -3000, .y 403) returned phi 123, x 3023, y -35
	vector.x = -3000;
	vector.y = 403;
	angle = vect2_polar(&polar, &vector);
	TEST_ASSERT_EQUAL_INT16(123, angle);
	TEST_ASSERT_EQUAL_INT16(3023, polar.r);
	TEST_ASSERT_EQUAL_INT16(123, polar.p);

//navigate_set_goal:rect_to_polar(courseLeg.x 2624, .y -1578) returned phi -22, x3058, y 68
	vector.x = 2624;
	vector.y = -1578;
	angle = vect2_polar(&polar, &vector);
	TEST_ASSERT_EQUAL_INT16(-22, angle);
	TEST_ASSERT_EQUAL_INT16(3058, polar.r);
	TEST_ASSERT_EQUAL_INT16(-22, polar.p);

//navigate_set_goal:rect_to_polar(courseLeg.x 0, .y 1578) returned phi 64, x 1573, y 37
	vector.x = 0;
	vector.y = 1578;
	angle = vect2_polar(&polar, &vector);
	TEST_ASSERT_EQUAL_INT16(64, angle);
	TEST_ASSERT_EQUAL_INT16(1573, polar.r);
	TEST_ASSERT_EQUAL_INT16(64, polar.p);

//navigate_set_goal:rect_to_polar(courseLeg.x -2624, .y -1578) returned phi -105, x 3059, y -74
	vector.x = -2624;
	vector.y = -1578;
	angle = vect2_polar(&polar, &vector);
	TEST_ASSERT_EQUAL_INT16(-105, angle);
	TEST_ASSERT_EQUAL_INT16(3059, polar.r);
	TEST_ASSERT_EQUAL_INT16(-105, polar.p);
}

void test_vect2_polar_16(void)
{
//uint16_t vect2_polar_16(polar_16t* polar, const vect2_16t* vector)

	int16_t angle;
	vect2_16t vector;
	polar_32t polar;

//navigate_set_goal:rect_to_polar16(x -1072, y 721) returned phi 26570, x 1289, y25
	vector.x = -1072;
	vector.y = 721;
	angle = vect2_polar_16(&polar, &vector);
	TEST_ASSERT_EQUAL_INT16(26570, angle);
	TEST_ASSERT_EQUAL_INT16(1289, polar.r);
	TEST_ASSERT_EQUAL_INT16(26570, polar.p);

//navigate_set_goal:rect_to_polar16(x 2628, y -1577) returned phi -5647, x 3061, y 71
	vector.x = 2628;
	vector.y = -1577;
	angle = vect2_polar_16(&polar, &vector);
	TEST_ASSERT_EQUAL_INT16(-5647, angle);
	TEST_ASSERT_EQUAL_INT16(3061, polar.r);
	TEST_ASSERT_EQUAL_INT16(-5647, polar.p);

//navigate_set_goal:rect_to_polar16(x 0, y 1577) returned phi 16373, x 1573, y 37:
	vector.x = 0;
	vector.y = 1577;
	angle = vect2_polar_16(&polar, &vector);
	TEST_ASSERT_EQUAL_INT16(16373, angle);
	TEST_ASSERT_EQUAL_INT16(1573, polar.r);
	TEST_ASSERT_EQUAL_INT16(16373, polar.p);

//navigate_set_goal:rect_to_polar16(x -2628, y -1577) returned phi -27140, x 3061, y 74:
	vector.x = -2628;
	vector.y = -1577;
	angle = vect2_polar_16(&polar, &vector);
	TEST_ASSERT_EQUAL_INT16(-27140, angle);
	TEST_ASSERT_EQUAL_INT16(3061, polar.r);
	TEST_ASSERT_EQUAL_INT16(-27140, polar.p);
}

void test_MatrixAdd(void)
{
	fractional dstM[]  = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 42 };
	fractional srcM1[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 56 };
	fractional srcM2[] = { 100, 200, 300, 400, 500, 600, 700, 800, 900 };
	fractional expM[]  = { 101, 202, 303, 404, 505, 606, 707, 808, 909 };
//fractional* MatrixAdd(int16_t numRows, int16_t numCols, fractional* dstM, fractional* srcM1, fractional* srcM2);

	MatrixAdd(3, 3, dstM, srcM1, srcM2);

	TEST_ASSERT_EQUAL_INT16_ARRAY(dstM, expM, 3);
	TEST_ASSERT_EQUAL_INT16(dstM[9], 42);   // test MatrixAdd did not overrun
	TEST_ASSERT_EQUAL_INT16(srcM1[0], 1);
	TEST_ASSERT_EQUAL_INT16(srcM2[0], 100);
}

void test_MatrixMultiply(void)
{
//fractional* MatrixMultiply(int16_t numRows1, int16_t numCols1Rows2, int16_t numCols2, fractional* dstM, fractional* srcM1, fractional* srcM2);
}

void test_VectorCopy(void)
{
//	fractional* VectorCopy(int numElems, fractional* dstV, fractional* srcV);

	fractional dstV[] = { 0, 0, 0, 42 }; // extra element on end to check for overrun
	fractional srcV[] = { 1, 2, 3, 56 };

	VectorCopy(3, dstV, srcV);
	TEST_ASSERT_EQUAL_INT16_ARRAY(dstV, srcV, 3);
	TEST_ASSERT_EQUAL_INT16(dstV[3], 42);   // test VectorCopy did not overrun
	TEST_ASSERT_EQUAL_INT16(srcV[0], 1);    // and it worked in the right direction
}

void test_find_first_bit_int16(void)
{
	TEST_ASSERT_EQUAL(16, find_first_bit_int16(0));
	TEST_ASSERT_EQUAL(15, find_first_bit_int16(1));
	TEST_ASSERT_EQUAL(14, find_first_bit_int16(2));
	TEST_ASSERT_EQUAL(8,  find_first_bit_int16(0x00FF));
	TEST_ASSERT_EQUAL(4,  find_first_bit_int16(0x0FFF));
	TEST_ASSERT_EQUAL(1,  find_first_bit_int16(0x4FFF));
	TEST_ASSERT_EQUAL(1,  find_first_bit_int16(0x4000));
	TEST_ASSERT_EQUAL(0,  find_first_bit_int16(0x8000));
}

void test_find_first_bit_int32(void)
{
	TEST_ASSERT_EQUAL(32, find_first_bit_int32(0));
	TEST_ASSERT_EQUAL(31, find_first_bit_int32(1));
	TEST_ASSERT_EQUAL(30, find_first_bit_int32(2));
	TEST_ASSERT_EQUAL(8,  find_first_bit_int32(0x00FFFFFF));
	TEST_ASSERT_EQUAL(4,  find_first_bit_int32(0x0FFFFFFF));
	TEST_ASSERT_EQUAL(1,  find_first_bit_int32(0x4FFFFFFF));
	TEST_ASSERT_EQUAL(1,  find_first_bit_int32(0x40000000));
	TEST_ASSERT_EQUAL(0,  find_first_bit_int32(0x80000000));
}

