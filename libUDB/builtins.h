//
//  builtins.h
//  MatrixPilot
//
//  Created by Robert Dickenson on 15/9/2014.
//  Copyright (c) 2016 MatrixPilot. All rights reserved.
//

#ifndef _BUILTINS_H_
#define _BUILTINS_H_

// Fake dspic builtin ASM calls for Non dspic devices (e.g. Arm devices))
// Microchip dspic controllers use __builtin__ calls to engage special fast instructions in the hardware
// This file is not included in builds for Microchip dspic devices
#define __builtin_mulss(x,y) ((( int32_t)(x))*( int32_t)(y))
#define __builtin_mulus(x,y) (((uint32_t)(x))*( int32_t)(y))
#define __builtin_mulsu(x,y) ((( int32_t)(x))*(uint32_t)(y))
#define __builtin_muluu(x,y) (((uint32_t)(x))*(uint32_t)(y))
#define __builtin_divud(x,y) ((uint16_t)(((uint32_t)(x))/(uint16_t)(y)))
#define __builtin_divsd(x,y) (( int16_t)((( int32_t)(x))/(int16_t)(y)))

#endif // _BUILTINS_H_
