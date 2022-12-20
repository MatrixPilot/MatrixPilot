/* 
 * File:   gyro_offsets_table.h
 * Author: bill
 *
 * Created on December 20, 2022, 9:35 AM
 */

#include <stdint.h>
#include "../libUDB/ADchannel.h"	

typedef struct gyro_offset_table_entry { int16_t x ; int16_t y ; int16_t z ; } gyro_offset_table_entry ;


#define TABLE_ORIGIN 9292

struct gyro_offset_table_entry gyro_offset_table[] = {
	{3817,-3692,4307},
	{3515,-3092,4082},
	{3201,-2515,3924},
	{2863,-1944,3775},
	{2488,-1531,3670},
	{2129,-1195,3554},
	{1734,-795,3440},
	{1268,-290,3367},
	{857,210,3332},
	{596,689,3356},
	{429,1186,3359},
	{280,1702,3365},
	{169,2209,3382},
	{45,2749,3432},
	{-89,3292,3504}
};

extern struct ADchannel mpu_temp;

int16_t temperature ;

int16_t gyro_offset[3] ;

uint16_t index_msb ;
uint16_t index_lsb ;
int16_t left_entry[3];
int16_t right_minus_left[3];
uint16_t number_entries ;

void lookup_gyro_offsets(void)
{
	temperature = mpu_temp.value + TABLE_ORIGIN ;
	if (temperature < 0)
	{
		gyro_offset[0] = gyro_offset_table[0].x ;
		gyro_offset[1] = gyro_offset_table[0].y ;
		gyro_offset[2] = gyro_offset_table[0].z ;
	}
	else
	{
		index_lsb = temperature && 0x03FF ;
		index_msb = temperature >> 10 ; 
		number_entries = (sizeof (gyro_offset_table))/(sizeof (gyro_offset_table_entry)) ;
		if ( index_msb >= (number_entries - 1 ))
		{
			gyro_offset[0] = gyro_offset_table[number_entries - 1].x ;
			gyro_offset[1] = gyro_offset_table[number_entries - 1].y ;
			gyro_offset[2] = gyro_offset_table[number_entries - 1].z ;
		}
		else
		{
			left_entry[0]= gyro_offset_table[index_msb].x ;
			left_entry[1]= gyro_offset_table[index_msb].y ;
			left_entry[2]= gyro_offset_table[index_msb].z ;
			
			right_minus_left[0]= gyro_offset_table[index_msb+1].x - left_entry[0] ;
			right_minus_left[1]= gyro_offset_table[index_msb+1].y - left_entry[1] ;
			right_minus_left[2]= gyro_offset_table[index_msb+1].z - left_entry[2] ;
			
			gyro_offset[0] = left_entry[0] + __builtin_divsd(__builtin_mulss(right_minus_left[0],index_msb),1024);
			gyro_offset[1] = left_entry[1] + __builtin_divsd(__builtin_mulss(right_minus_left[1],index_msb),1024);
			gyro_offset[2] = left_entry[2] + __builtin_divsd(__builtin_mulss(right_minus_left[2],index_msb),1024);
		}
	}
}

