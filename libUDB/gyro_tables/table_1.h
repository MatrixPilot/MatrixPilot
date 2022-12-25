/* 
 * File:   table_1.h
 * Author: bill
 *
 * Created on December 24, 2022, 11:37 AM
 */

#ifndef TABLE_1_H
#define	TABLE_1_H

#define TABLE_ORIGIN -9292

#define LOOKUP_LSB_MASK 0x03FF
#define MSB_SHIFT 10

int16_t residual_offset[] = { -896 , -384 , 2176 } ;
//int16_t residual_offset[] = { 0 , 0 , 0 } ;

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


#endif	/* TABLE_1_H */

