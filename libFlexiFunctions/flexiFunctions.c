#include "flexiFunctionTypes.h"
#include "defines.h"

#include <dsp.h>

#define RMAX15 0b0110000000000000	//	1.5 in 2.14 format
#define RMAX   0b0100000000000000	//	1.0 in 2.14 format


uint8_t get_input_register_index_from_directory(uint8_t virtual_index)
{
	return flexiFunction_dataset.inputs_directory[virtual_index];
}

uint8_t get_output_register_index_from_directory(uint8_t virtual_index)
{
	return flexiFunction_dataset.outputs_directory[virtual_index];
}

// Run the flexifunction by stepping through the list of functions one-by-one.
void runFlexiFunctions( void )
{
	int16_t 				functionNo = 0;		// Index of mixer in mixer list
	functionSetting* 	pSetting;
	fractional 			output;
	uint16_t		functionAddr;		// Address of mixer in mixer data

	functionAddr = flexiFunction_dataset.flexiFunction_directory[functionNo];
	pSetting = (functionSetting*) &(flexiFunction_dataset.flexiFunction_data[functionAddr]);

	// Step through each mixer and run it
	while( functionNo < flexiFunction_dataset.flexiFunctionsUsed )
	{	
		flexiFunction_registers[0] = 0;	// reset the NULL register to make sure

		output  =	flexiFunctions[pSetting->functionType] ( pSetting, &flexiFunction_registers[0]);

		// Always limit mixer output to +-RMAX15 or 1.5*full scale
		if(output > RMAX15) output = RMAX15;
		else if(output < -RMAX15) output = -RMAX15;

		if(pSetting->setValue == 0)
		{
			flexiFunction_registers[pSetting->dest] = output;
		}
		else if(pSetting->setValue == 1)
		{
			flexiFunction_registers[pSetting->dest] += output;
			
			// limit final output to +-RMAX15 or 1.5*full scale
			if(flexiFunction_registers[pSetting->dest] > RMAX15) 
				flexiFunction_registers[pSetting->dest] = RMAX15;
			else if (flexiFunction_registers[pSetting->dest] < -RMAX15) 
				flexiFunction_registers[pSetting->dest] = -RMAX15;
		}
		else if(pSetting->setValue == 2)
		{
			flexiFunction_registers[pSetting->dest] = 0;
		}

		functionNo++;
		functionAddr = flexiFunction_dataset.flexiFunction_directory[functionNo];
		pSetting = (functionSetting*) &(flexiFunction_dataset.flexiFunction_data[functionAddr]);
	};
};


fractional null_function(functionSetting* pSetting, fractional* pRegisters)
{
	return 0;
};



fractional gain_offset_function(functionSetting* pSetting, fractional* pRegisters)
{
	union longww 	ltemp;
	fractional 		ftemp;
	ftemp = pRegisters[pSetting->data.gain_offset.src];
	ftemp  -= pSetting->data.gain_offset.Offset;
	if(ftemp > 0)
		ltemp.WW = __builtin_mulss(ftemp, pSetting->data.gain_offset.PosGain);
	else
		ltemp.WW = __builtin_mulss(ftemp, pSetting->data.gain_offset.NegGain);	
	
	ltemp.WW <<= 2;
	ftemp = (fractional) ltemp._.W1;

	ftemp  += pSetting->data.gain_offset.Constant;

	return ftemp;
};

/*

fractional mixDualSum(MixerSetting* pSetting)
{
	return 0;
};

*/

// Scale output, add offset and subtrim then apply full scale limit.
// This output option is good for HILSIM or non-pre mixed RC controls.
fractional scale_trim_limit_function(functionSetting* pSetting, fractional* pRegisters)
{
	union longww 	ltemp;
	fractional 		ftemp;
	ftemp = pRegisters[pSetting->data.scale_trim_limit.src];
	if(ftemp > 0)
		ltemp.WW = __builtin_mulss(ftemp, pSetting->data.scale_trim_limit.scalePos << 2);
	else
		ltemp.WW = __builtin_mulss(ftemp, pSetting->data.scale_trim_limit.scaleNeg << 2);	
	
	ftemp = (fractional) ltemp._.W1;

	ftemp += pSetting->data.scale_trim_limit.offset;
	ftemp += pSetting->data.scale_trim_limit.subTrim;

	if(ftemp > pSetting->data.scale_trim_limit.limitMax) 
		ftemp = pSetting->data.scale_trim_limit.limitMax;
	else if (ftemp < pSetting->data.scale_trim_limit.limitMin) 
		ftemp = pSetting->data.scale_trim_limit.limitMin;

	return ftemp;
};


// Scale output, add referenced offset and subtrim then apply full scale limit.
// referenced offset can be a pwInTrim value or similar.
fractional scale_reftrim_limit_function(functionSetting* pSetting, fractional* pRegisters)
{
	union longww 	ltemp;
	fractional 		ftemp;
	ftemp = pRegisters[pSetting->data.scale_reftrim_limit.src];
	if(ftemp > 0)
		ltemp.WW = __builtin_mulss(ftemp, pSetting->data.scale_reftrim_limit.scalePos << 2);
	else
		ltemp.WW = __builtin_mulss(ftemp, pSetting->data.scale_reftrim_limit.scaleNeg << 2);	
	
	ftemp = (fractional) ltemp._.W1;

	ftemp += pRegisters[pSetting->data.scale_reftrim_limit.refOffset];

	ftemp += pSetting->data.scale_reftrim_limit.subTrim;

	if(ftemp > pSetting->data.scale_reftrim_limit.limitMax) 
		ftemp = pSetting->data.scale_reftrim_limit.limitMax;
	else if (ftemp < pSetting->data.scale_reftrim_limit.limitMin) 
		ftemp = pSetting->data.scale_reftrim_limit.limitMin;

	return ftemp;
};



fractional linear_mux_function(functionSetting* pSetting, fractional* pRegisters)
{
	union longww 	ltemp;
	fractional 		ftemp, ftemp2;
	fractional 		gainTemp, gainTemp2;

	gainTemp =	pRegisters[pSetting->data.linear_mux.Control];

	if(gainTemp < 0)
		gainTemp = 0;
	else if(gainTemp > RMAX)
		gainTemp = RMAX;

	gainTemp2 = RMAX - gainTemp; 

	ftemp = pRegisters[pSetting->data.linear_mux.src2];
	ltemp.WW = __builtin_mulss(ftemp, gainTemp);
	ltemp.WW <<= 2;
	ftemp = (fractional) ltemp._.W1;

	ftemp2 = pRegisters[pSetting->data.linear_mux.src1];
	ltemp.WW = __builtin_mulss(ftemp2, gainTemp2);
	ltemp.WW <<= 2;
	ftemp += (fractional) ltemp._.W1;

	return ftemp;
};


fractional conditional_gain_function(functionSetting* pSetting, fractional* pRegisters)
{
	union longww 	ltemp;
	fractional 		ftemp;
	fractional		fgain;
	boolean			state;

	// collect the reference condition value
	ftemp = pRegisters[pSetting->data.conditional_gain.srcCond];

	state = ( (ftemp >= pSetting->data.conditional_gain.condMin) & 
			(ftemp <= pSetting->data.conditional_gain.condMax) );

	
	ftemp = pRegisters[pSetting->data.conditional_gain.src];

	if(state == 0)
		fgain = pSetting->data.conditional_gain.condInvalidGain;
	else
		fgain = pSetting->data.conditional_gain.condValidGain;

	ltemp.WW = __builtin_mulss(ftemp, fgain);
	ltemp.WW <<= 2;
	ftemp = (fractional) ltemp._.W1;

	return ftemp;

};

fractional multiply_function(functionSetting* pSetting, fractional* pRegisters)
{
	return 0;
};

fractional gain_limit_function(functionSetting* pSetting, fractional* pRegisters)
{
	union longww 	ltemp;
	fractional 		ftemp;
	ftemp = pRegisters[pSetting->data.gain_limit.src];

	if(ftemp > 0)
		ltemp.WW = __builtin_mulss(ftemp, pSetting->data.gain_limit.posGain);
	else
		ltemp.WW = __builtin_mulss(ftemp, pSetting->data.gain_limit.negGain);	
	
	ltemp.WW <<= 2;
	ftemp = (fractional) ltemp._.W1;

	if(ftemp > pSetting->data.gain_limit.limitPos)
		ftemp = pSetting->data.gain_limit.limitPos;
	else if(ftemp < pSetting->data.gain_limit.limitNeg)
		ftemp = pSetting->data.gain_limit.limitNeg;

	return ftemp;
};

fractional conditional_set_ref_function(functionSetting* pSetting, fractional* pRegisters)
{
	return 0;
};



fractional conditional_set_function(functionSetting* pSetting, fractional* pRegisters)
{
	fractional 		ctemp;
	fractional 		ftemp;

	// collect the reference condition value
	ctemp = pRegisters[pSetting->data.conditional_set.srcCond];

	ftemp = pRegisters[pSetting->data.conditional_set.src];

	if(( (ctemp >= pSetting->data.conditional_set.condMin) & 
			(ctemp <= pSetting->data.conditional_set.condMax) ))
		ftemp = pSetting->data.conditional_set.setValue;
	else
		ftemp = pRegisters[pSetting->data.conditional_set.src];

	return ftemp;
};


fractional three_point_function(functionSetting* pSetting, fractional* pRegisters)
{
	fractional output;
	fractional X1;
	fractional X2;
	fractional Y1;
	fractional Y2;
	fractional 		delta;
	union longww 	ltemp;

	int16_t 			gain = 0;

	fractional input = pRegisters[pSetting->data.three_point.src];

	if(input <= pSetting->data.three_point.inputLow)
		return pSetting->data.three_point.outputLow;

	if(input >= pSetting->data.three_point.inputHigh)
		return pSetting->data.three_point.outputHigh;

	if(input >= pSetting->data.three_point.inputMid)
	{
		X1 = pSetting->data.three_point.inputMid;
		X2 = pSetting->data.three_point.inputHigh;
		Y1 = pSetting->data.three_point.outputMid;
		Y2 = pSetting->data.three_point.outputHigh;
	}
	else
	{
		X1 = pSetting->data.three_point.inputLow;
		X2 = pSetting->data.three_point.inputMid;
		Y1 = pSetting->data.three_point.outputLow;
		Y2 = pSetting->data.three_point.outputMid;
	}
	
	input -= X1;
	if(X2 <= X1) return Y1;
	if(Y1 == Y2) return Y1;

	delta = X2 - X1;

	// Find the gain required to increase delta to be in range RMAX to RMAX/2
	while(delta < RMAX/2)
	{
		gain++;
		delta <<= 1;
	}

	if(delta > RMAX)
	{
		gain--;
		delta >>= 1;
	}

	ltemp.WW = 0;
	ltemp._.W1 = (Y2 - Y1);		//  does this need to be inverted???


	// Limit numerator to +-RMAX/4 and adjust gain
	if(ltemp.WW > 0)
	{
		while(ltemp._.W1 > RMAX/4)
		{
			gain++;
			ltemp.WW >>= 1;
		}
	}
	else
	{
		while(ltemp._.W1 < -RMAX/4)
		{
			gain++;
			ltemp.WW >>= 1;
		}
	}

	output = __builtin_divsd( ltemp.WW,  delta ); //(int16_t) (fractional)

	
	ltemp.WW = __builtin_mulss(output, input);	
	ltemp.WW <<= gain;
	output = (fractional) (ltemp._.W1 + Y1);

	return output;
}


fractional four_point_function(functionSetting* pSetting, fractional* pRegisters)
{
	fractional output;
	fractional X1;
	fractional X2;
	fractional Y1;
	fractional Y2;
	fractional 		delta;
	union longww 	ltemp;

	int16_t 			gain = 0;

	fractional input = pRegisters[pSetting->data.four_point.src];

	if(input <= pSetting->data.four_point.input1)
		return pSetting->data.four_point.output1;

	if(input >= pSetting->data.four_point.input4)
		return pSetting->data.four_point.output4;

	if(input >= pSetting->data.four_point.input3)
	{
		X1 = pSetting->data.four_point.input3;
		X2 = pSetting->data.four_point.input4;
		Y1 = pSetting->data.four_point.output3;
		Y2 = pSetting->data.four_point.output4;
	}
	else if(input >= pSetting->data.four_point.input2)
	{
		X1 = pSetting->data.four_point.input2;
		X2 = pSetting->data.four_point.input3;
		Y1 = pSetting->data.four_point.output2;
		Y2 = pSetting->data.four_point.output3;
	}
	else
	{
		X1 = pSetting->data.four_point.input1;
		X2 = pSetting->data.four_point.input2;
		Y1 = pSetting->data.four_point.output1;
		Y2 = pSetting->data.four_point.output2;
	}
	
	input -= X1;
	if(X2 <= X1) return Y1;
	if(Y1 == Y2) return Y1;

	delta = X2 - X1;

	// Find the gain required to increase delta to be in range RMAX to RMAX/2
	while(delta < RMAX/2)
	{
		gain++;
		delta <<= 1;
	}

	if(delta > RMAX)
	{
		gain--;
		delta >>= 1;
	}

	ltemp.WW = 0;
	ltemp._.W1 = (Y2 - Y1);		//  does this need to be inverted???


	// Limit numerator to +-RMAX/4 and adjust gain
	if(ltemp.WW > 0)
	{
		while(ltemp._.W1 > RMAX/4)
		{
			gain++;
			ltemp.WW >>= 1;
		}
	}
	else
	{
		while(ltemp._.W1 < -RMAX/4)
		{
			gain++;
			ltemp.WW >>= 1;
		}
	}

	output = __builtin_divsd( ltemp.WW,  delta ); //(int16_t) (fractional)

	
	ltemp.WW = __builtin_mulss(output, input);
	ltemp.WW <<= gain;
	output = (fractional) (ltemp._.W1 + Y1);

	return output;
}


fractional five_point_function(functionSetting* pSetting, fractional* pRegisters)
{
	fractional output;
	fractional X1;
	fractional X2;
	fractional Y1;
	fractional Y2;
	fractional 		delta;
	union longww 	ltemp;

	int16_t 			gain = 0;

	fractional input = pRegisters[pSetting->data.five_point.src];

	if(input <= pSetting->data.five_point.input1)
		return pSetting->data.five_point.output1;

	if(input >= pSetting->data.five_point.input5)
		return pSetting->data.five_point.output5;

	if(input >= pSetting->data.five_point.input4)
	{
		X1 = pSetting->data.five_point.input4;
		X2 = pSetting->data.five_point.input5;
		Y1 = pSetting->data.five_point.output4;
		Y2 = pSetting->data.five_point.output5;
	}
	if(input >= pSetting->data.five_point.input3)
	{
		X1 = pSetting->data.five_point.input3;
		X2 = pSetting->data.five_point.input4;
		Y1 = pSetting->data.five_point.output3;
		Y2 = pSetting->data.five_point.output4;
	}
	else if(input >= pSetting->data.five_point.input2)
	{
		X1 = pSetting->data.five_point.input2;
		X2 = pSetting->data.five_point.input3;
		Y1 = pSetting->data.five_point.output2;
		Y2 = pSetting->data.five_point.output3;
	}
	else
	{
		X1 = pSetting->data.five_point.input1;
		X2 = pSetting->data.five_point.input2;
		Y1 = pSetting->data.five_point.output1;
		Y2 = pSetting->data.five_point.output2;
	}
	
	input -= X1;
	if(X2 <= X1) return Y1;
	if(Y1 == Y2) return Y1;

	delta = X2 - X1;

	// Find the gain required to increase delta to be in range RMAX to RMAX/2
	while(delta < RMAX/2)
	{
		gain++;
		delta <<= 1;
	}

	if(delta > RMAX)
	{
		gain--;
		delta >>= 1;
	}

	ltemp.WW = 0;
	ltemp._.W1 = (Y2 - Y1);		//  does this need to be inverted???


	// Limit numerator to +-RMAX/4 and adjust gain
	if(ltemp.WW > 0)
	{
		while(ltemp._.W1 > RMAX/4)
		{
			gain++;
			ltemp.WW >>= 1;
		}
	}
	else
	{
		while(ltemp._.W1 < -RMAX/4)
		{
			gain++;
			ltemp.WW >>= 1;
		}
	}

	output = __builtin_divsd( ltemp.WW,  delta ); //(int16_t) (fractional)

	
	ltemp.WW = __builtin_mulss(output, input);
	ltemp.WW <<= gain;
	output = (fractional) (ltemp._.W1 + Y1);

	return output;
}


fractional gain_function(functionSetting* pSetting, fractional* pRegisters)
{
	union longww 	ltemp;
	fractional 		ftemp;
	ftemp = pRegisters[pSetting->data.gain_limit.src];

	if(ftemp > 0)
		ltemp.WW = __builtin_mulss(ftemp, pSetting->data.gain_limit.posGain);
	else
		ltemp.WW = __builtin_mulss(ftemp, pSetting->data.gain_limit.negGain);	
	
	ltemp.WW <<= 2;
	ftemp = (fractional) ltemp._.W1;

	return ftemp;
}


fractional rate_function(functionSetting* pSetting, fractional* pRegisters)
{
	fractional src = pRegisters[pSetting->data.rate.src];
	fractional dest = pRegisters[pSetting->dest];
	
	if(dest > src)
	{
		src += pSetting->data.rate.posRate;
		if(src > dest) src = dest;
	}
	else if(dest < src)
	{
		src -= pSetting->data.rate.negRate;
		if(src < dest) src = dest;
	}

	// Force output to be set rather than add or clear
	pSetting->setValue = 0;

	// Update the src which is the memory of the last output
	pRegisters[pSetting->data.rate.src] = src;

	return src;
}


fractional pct_cond_gains_function(functionSetting* pSetting, fractional* pRegisters)
{
	union longww 	ltemp;
	fractional 		ftemp;
	fractional		posgain, neggain, gain;
	boolean			state;

	// collect the reference condition value
	ftemp = pRegisters[pSetting->data.pct_cond_gains.srcCond];

	state = ( (ftemp >= pSetting->data.pct_cond_gains.condMin) & 
			(ftemp <= pSetting->data.pct_cond_gains.condMax) );

	
	ftemp = pRegisters[pSetting->data.pct_cond_gains.src];

	if(state == 0)
	{
		posgain = pSetting->data.pct_cond_gains.posGainInvalid;
		neggain = pSetting->data.pct_cond_gains.negGainInvalid;
	}
	else
	{
		posgain = pSetting->data.pct_cond_gains.posGainValid;
		neggain = pSetting->data.pct_cond_gains.negGainValid;
	}

	if(ftemp > 0)
		gain = posgain;
	else
		gain = neggain;

	ltemp.WW = __builtin_mulss(ftemp, gain);
	ltemp.WW <<= 2;
	ftemp = (fractional) ltemp._.W1;

	return ftemp;
}


