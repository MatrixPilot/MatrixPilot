/* 
 * File:   servoOutPins.h
 * Author: phollands
 *
 * Created on 08 February 2017, 11:41
 */

#ifndef SERVOOUTPINS_H
#define	SERVOOUTPINS_H

#if (BOARD_TYPE == UDB4_BOARD || BOARD_TYPE == UDB5_BOARD)

#define SERVO_OUT_PIN_1         _LATD0
#define SERVO_OUT_PIN_2         _LATD1
#define SERVO_OUT_PIN_3         _LATD2
#define SERVO_OUT_PIN_4         _LATD3
#define SERVO_OUT_PIN_5         _LATD4
#define SERVO_OUT_PIN_6         _LATD5
#define SERVO_OUT_PIN_7         _LATD6
#define SERVO_OUT_PIN_8         _LATD7
#define SERVO_OUT_PIN_9         _LATA4
#define SERVO_OUT_PIN_10        _LATA1
#define ACTION_OUT_PIN          SERVO_OUT_PIN_9

#elif (BOARD_TYPE == AUAV3_BOARD)

#define SERVO_OUT_PIN_1         _LATG0
#define SERVO_OUT_PIN_2         _LATE0
#define SERVO_OUT_PIN_3         _LATG13
#define SERVO_OUT_PIN_4         _LATD7
#define SERVO_OUT_PIN_5         _LATG14
#define SERVO_OUT_PIN_6         _LATG1
#define SERVO_OUT_PIN_7         _LATF13
#define SERVO_OUT_PIN_8         _LATF12
#define SERVO_OUT_PIN_9         _LATF12
#define SERVO_OUT_PIN_10        _LATF12
#define ACTION_OUT_PIN          SERVO_OUT_PIN_8

#if (NUM_OUTPUTS > 8)
#error "max of 8 servo outputs currently supported for AUAV3"
#endif

#else
#error Invalid BOARD_TYPE
#endif

#endif	/* SERVOOUTPINS_H */

