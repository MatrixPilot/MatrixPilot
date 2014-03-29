
/*******************************************************
* File: MAX21000.h					
*							
* Header file for MAX21000 library. It contains macros and 	
* other definitions					
*
* *Version:* 1.1beta
*
* *Property of* <maxim integrated at 			
* http://www.maximintegrated.com>.			
*							
* *Author*: <Ivo Binda at 				
*  ivo.binda@maximintegrated.com>.			
*	
*******************************************************
*
* _Last change_: Jul/07/2012 				
*                 						
*******************************************************/


signed char getPar(unsigned char device, unsigned char paramBank, unsigned char paramReg, unsigned char paramMask, unsigned char paramPos, unsigned char * paramVal);
signed char setPar(unsigned char device, unsigned char paramBank, unsigned char paramReg, unsigned char paramMask, unsigned char paramPos, unsigned char paramVal);

/********************************
********************************
* Macro: getParameter		
* 				
* Redefine of the getPar function with	
* all the parameter for read a specific	
* parameter value.			
*				
* Parameters:			
*	device - Device address	
*	param - Parameter name	
*	paramValPtr - Pointer to the 
*					return value	
*							
*********************************
********************************/
#define getParameter(device, param, paramValPtr) getPar(device, param##_BNK, param##_REG, param##_MSK, param##_POS, paramValPtr)

/********************************
*********************************
* Macro: setParameter			
* 					
* Redefine of the setPar function with	
* all the parameter for write a specific	
* parameter value.				
*					
* Parameters:				
*		device - Device address	
*		param - Parameter name	
*		paramVal - Parameter value	
*					
*********************************
********************************/
#define setParameter(device, param, paramVal) setPar(device, param##_BNK, param##_REG, param##_MSK, param##_POS, paramVal)

//#define GET_BITSLICE(regvar, param) ((regvar & param##__MSK)>>param##__POS)

/********************************
*********************************
* Macro: changeBank			
* 					
* Redefine the change bank write 	
* command.				
*					
* Parameters:				
*		device - Device address	
*		bank - Bank number to set	
*					
*********************************
********************************/
#define changeBank(device, bank) setParameter(device, BANK_SEL ,bank)


signed char init_max21000(unsigned char powerMode, unsigned char channel, unsigned char bandwidth, unsigned char fullScale, unsigned char odr);
signed char getRatef_max21000(float * rates);
signed char getFifoData_max21000(unsigned char numOfData, unsigned char * dataVect);
unsigned char getFullScale_max21000();
signed char setFullScale_max21000(unsigned char fullscale);
unsigned char getPowerMode_max21000();
signed char setPowerMode_max21000(unsigned char powerMode);	
unsigned char getDataStatus_max21000();
unsigned char getEnabledRate_max21000();
signed char setEnabledRate_max21000(unsigned char enRate);
unsigned char getDataBw_max21000();
signed char setDataBw_max21000(unsigned char bandwidth);
//      unsigned char getMaxFreq_max21000();	
//      signed char setMaxFreq_max21000(unsigned char maxFreq);
//      unsigned char getMaxFreq_max21000();	
signed char setOIS_max21000(unsigned char oisEn);
unsigned char getNumericODR_max21000();	
float getODR_max21000(unsigned int maxOdr);	
unsigned char getDataSource_max21000();	
signed char setDataSource_max21000(unsigned char source);
unsigned char getHpCutoff_max21000();	
signed char setHpCutoff_max21000(unsigned char cutOff);
unsigned char getEndian_max21000();
signed char setEndian_max21000(unsigned char ble);

unsigned char getDataReadyRstMode_max21000();	
signed char setDataReadyRstMode_max21000(unsigned char drrm);
unsigned char getAuxPUD_max21000();	
signed char setAuxPUD_max21000(unsigned char setPUD);
unsigned char getInt1PUD_max21000();	
signed char setInt1PUD_max21000(unsigned char setPUD);	
unsigned char getInt2PUD_max21000();	
signed char setInt2PUD_max21000(unsigned char setPUD);
unsigned char getI2Cmode_max21000();	
signed char setI2Cmode_max21000(unsigned char i2cMode);
unsigned char getI2Coff_max21000();	
signed char setI2Coff_max21000(unsigned char i2cOnOff);
unsigned char getPadCurr_max21000();	
signed char setPadCurr_max21000(unsigned char padCfg);



