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
		index_lsb = temperature & 0x03FF ;
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
			
			gyro_offset[0] = left_entry[0] + __builtin_divsd(__builtin_mulss(right_minus_left[0],index_lsb),1024);
			gyro_offset[1] = left_entry[1] + __builtin_divsd(__builtin_mulss(right_minus_left[1],index_lsb),1024);
			gyro_offset[2] = left_entry[2] + __builtin_divsd(__builtin_mulss(right_minus_left[2],index_lsb),1024);
		}
	}
}

#define STEP_SIZE 256

int16_t samples = 0 ;
int64_t samples_64t = 0 ;
int32_t samples_32t = 0 ;
int16_t adjusted_temperature = 0 ;
int16_t gyro_offset_entry[] = { 0 , 0 , 0 } ;
int64_t xx_sum = 0 ;
int64_t xy_sum[] = { 0 , 0 , 0  } ;
int32_t x_sum = 0 ;
int32_t y_sum[] = { 0 , 0 , 0  } ;
int32_t xx_bar = 0 ;
int32_t xy_bar[] = { 0 , 0 , 0  } ;
int16_t x_bar = 0 ;
int16_t y_bar[] = { 0 , 0 , 0  } ;
int64_t xx_bar_minus_x_bar_x_bar ;
int16_t offset_left[3] ;
int16_t offset_right[3] ;

extern uint8_t udb_cpu_load(void);
extern void serial_output(const char* format, ...);

void update_offset_table(void)
{
	if (adjusted_temperature < STEP_SIZE)
	{
		gyro_offset_entry[0]= 64*udb_xrate.value ;
		gyro_offset_entry[1]= 64*udb_yrate.value ;
		gyro_offset_entry[2]= 64*udb_zrate.value ;
		
		xx_sum += (uint64_t) __builtin_mulss( adjusted_temperature,adjusted_temperature) ;
		
		xy_sum[0] += (int64_t)	__builtin_mulss( adjusted_temperature,gyro_offset_entry[0]) ;
		xy_sum[1] += (int64_t)	__builtin_mulss( adjusted_temperature,gyro_offset_entry[1]) ;
		xy_sum[2] += (int64_t)	__builtin_mulss( adjusted_temperature,gyro_offset_entry[2]) ;
		
		x_sum += (int32_t ) adjusted_temperature ;
		
		y_sum[0] += (int32_t ) gyro_offset_entry[0] ;
		y_sum[1] += (int32_t ) gyro_offset_entry[1] ;
		y_sum[2] += (int32_t ) gyro_offset_entry[2] ;
		
		samples ++ ;
		adjusted_temperature ++ ;
		
		if ( adjusted_temperature >= STEP_SIZE )
		{
			samples_64t = (int64_t)samples ;
			samples_32t = (int32_t)samples ;	
			
			xx_bar = (int32_t)(xx_sum /samples_64t) ;
			
			xy_bar[0] = (int32_t)(xy_sum[0] /samples_64t) ;
			xy_bar[1] = (int32_t)(xy_sum[1] /samples_64t) ;
			xy_bar[2] = (int32_t)(xy_sum[2] /samples_64t) ;
			
			x_bar = (int16_t)(x_sum/samples_32t);
			
			y_bar[0] = (int16_t)(y_sum[0]/samples_32t);
			y_bar[1] = (int16_t)(y_sum[1]/samples_32t);
			y_bar[2] = (int16_t)(y_sum[2]/samples_32t);
			
			xx_bar_minus_x_bar_x_bar = (int64_t)(xx_bar - x_bar*x_bar) ;
			
			offset_left[0] = (int16_t) ((
					((int64_t)y_bar[0])*((int64_t)xx_bar)
					-((int64_t)x_bar)*((int64_t)xy_bar[0])		
					)/xx_bar_minus_x_bar_x_bar );
			
			offset_left[1] = (int16_t) ((
					((int64_t)y_bar[1])*((int64_t)xx_bar)
					-((int64_t)x_bar)*((int64_t)xy_bar[1])		
					)/xx_bar_minus_x_bar_x_bar );
			
			offset_left[2] = (int16_t) ((
					((int64_t)y_bar[2])*((int64_t)xx_bar)
					-((int64_t)x_bar)*((int64_t)xy_bar[2])		
					)/xx_bar_minus_x_bar_x_bar );
			
			offset_right[0] = (int16_t)((
					
					((int64_t)y_bar[0])*(((int64_t)xx_bar)-(((int64_t)STEP_SIZE)*((int64_t)x_bar)))
					-
					((int64_t)xy_bar[0])*(((int64_t)x_bar)-((int64_t)STEP_SIZE))
					)/xx_bar_minus_x_bar_x_bar);
			
			offset_right[1] = (int16_t)((
					
					((int64_t)y_bar[1])*(((int64_t)xx_bar)-(((int64_t)STEP_SIZE)*((int64_t)x_bar)))
					-
					((int64_t)xy_bar[1])*(((int64_t)x_bar)-((int64_t)STEP_SIZE))
					)/xx_bar_minus_x_bar_x_bar);
			
			offset_right[2] = (int16_t)((
					
					((int64_t)y_bar[2])*(((int64_t)xx_bar)-(((int64_t)STEP_SIZE)*((int64_t)x_bar)))
					-
					((int64_t)xy_bar[2])*(((int64_t)x_bar)-((int64_t)STEP_SIZE))
					)/xx_bar_minus_x_bar_x_bar);
			
			serial_output("%i,%i,%i,%i,%i,%i,%li,%li,%li,%li,%li,%i,%i,%i,%i,%i,%i\r\n",
				udb_cpu_load(),
				samples,
				x_bar,
				y_bar[0],y_bar[1],y_bar[2],
				xx_bar,
				xy_bar[0], xy_bar[1],xy_bar[2],
				(int32_t)xx_bar_minus_x_bar_x_bar ,
				offset_left[0],offset_left[1],offset_left[2],
				offset_right[0],offset_right[1],offset_right[2]);
			
			samples = 0 ;
			adjusted_temperature = 0 ;
			xx_sum = 0 ;
			x_sum = 0 ;
			xy_sum[0] = 0 ;
			xy_sum[1] = 0 ;
			xy_sum[2] = 0 ;
			y_sum[0] = 0 ;
			y_sum[1] = 0 ;
			y_sum[2] = 0 ;
		}
	}		
}
