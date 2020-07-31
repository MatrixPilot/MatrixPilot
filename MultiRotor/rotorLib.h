#include "../../libDCM/libDCM.h"

extern void matrix_normalize ( int16_t v[] ) ;
extern void MatrixRotate( int16_t v1[] , int16_t v2[] ) ;
extern int16_t fractional_product( int16_t x  , int16_t y ) ;
extern int16_t multiply_saturate ( int16_t x , int16_t y , int16_t max ) ;

