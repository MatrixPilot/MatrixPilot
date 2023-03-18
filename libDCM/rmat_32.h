/* 
 * File:   rmat_32.h
 * Author: bill
 *
 * Created on March 17, 2023, 2:37 PM
 */

#ifndef RMAT_32_H
#define	RMAT_32_H

extern int32_t theta_square ;
extern union longww theta_cross[];
extern union longww theta_cross_theta_cross[];
extern int32_t f1,f2 ;
extern int32_t txtx, txty, txtz, tyty, tytz, tztz ;
extern union longww rupdate_32[] ;
extern int16_t rupdate_16[9];


void rmat_32_update(void) ;


#endif	/* RMAT_32_H */

