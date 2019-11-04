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
static inline int32_t  __builtin_mulss(const  int16_t  p0, const  int16_t  p1) { return (p0 * p1);   }
static inline int32_t  __builtin_mulus(const uint16_t  p0, const  int16_t  p1) { return (p0 * p1);   }
static inline int32_t  __builtin_mulsu(const  int16_t  p0, const uint16_t  p1) { return (p0 * p1);   }
static inline uint32_t __builtin_muluu(const uint16_t  p0, const uint16_t  p1) { return (p0 * p1);   }
static inline uint16_t __builtin_divud(const uint32_t num, const uint16_t den) { return (num / den); }
static inline int16_t  __builtin_divsd(const  int32_t num, const  int16_t den) { return (num / den); }

#endif // _BUILTINS_H_
