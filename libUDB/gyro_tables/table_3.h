/* 
 * File:   table_1.h
 * Author: bill
 *
 * Created on December 24, 2022, 11:37 AM
 */

#ifndef TABLE_3_H
#define	TABLE_3_H

#define TABLE_ORIGIN -10891

#define LOOKUP_LSB_MASK 0x03FF
#define MSB_SHIFT 10

int16_t residual_offset[] = { 0 , 0 , 0 } ;

struct gyro_offset_table_entry gyro_offset_table[] = {
{ 1064 , -1333 , 164 } , 
{ 1002 , -1150 , 160 } , 
{ 986 , -1023 , 151 } , 
{ 896 , -828 , 140 } , 
{ 852 , -679 , 131 } , 
{ 894 , -586 , 133 } , 
{ 847 , -432 , 118 } , 
{ 826 , -291 , 108 } , 
{ 789 , -141 , 97 } , 
{ 724 , 4 , 83 } , 
{ 659 , 130 , 70 } , 
{ 623 , 243 , 63 } , 
{ 543 , 321 , 53 } , 
{ 461 , 381 , 41 } 
};


#endif	/* TABLE_3_H */

