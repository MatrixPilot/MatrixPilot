
/*******************************************************
* File: MAX21000_map.h					
* 							
* This header contains all the paremeters definition for 	
* MAX21000 registers address and value			
*
* *Version:* 1.1beta
*
* *Property of* <maxim integrated at 			
* http://www.maximintegrated.com>.			
*							
* *Author*: <Ivo Binda at 				
*  ivo.binda@maximintegrated.com>.			
*
********************************************************
*
* _Last change_: Jul/07/2012 				
*                 						
*******************************************************/



/************************************************************
*								
* Constant: SLAVE_ADD_AMU					
* Set the address for slave module. This is a 7-bit address sent in the	
* following format:							
*								
*                      [A6:A5:A4:A3:A2:A1:A0:RS]				
*								
* A zero in the "RS" position of the first byte means that the master		
* transmits (sends) data to the selected slave, and a one in this position	
* means that the master receives data from the slave.			
*								
* *A0* is the value of *SDO_SA0* pin.				
* SDO_SA0_0 means that A0 is 0x00 and the resultant address is *0x58*
*
* SDO_SA0_1 means that A0 is 0x01 and the resultant address is *0x59*
************************************************************/

#define SLAVE_ADD_AMU 	0x58

/************************************************************
*								
* Constant: SINGLE_ADD_BURST					
* This constant must be added to the address if a burst read on a single
* address must be performed					
************************************************************/

#define SINGLE_ADD_BURST 0x40

/*******************************************
* Bank registers description				
*******************************************/
/************************************************************
*									
* Constants: Bank registers address description					
* 									
* REG_CB_32	- 0x20							
* REG_CB_33	- 0x21							
* REG_CB_34	- 0x22							
* REG_CB_35	- 0x23							
* REG_CB_36	- 0x24							
* REG_CB_37	- 0x25							
* REG_CB_38	- 0x26							
* REG_CB_39	- 0x27							
* REG_CB_40	- 0x28							
* REG_CB_41	- 0x29							
* REG_CB_42	- 0x2A							
* REG_CB_43	- 0x2B							
* REG_CB_44	- 0x2C							
* REG_CB_45	- 0x2D							
* REG_CB_46	- 0x2E							
* REG_CB_47	- 0x2F							
* REG_CB_48	- 0x30							
* REG_CB_49	- 0x31							
* REG_CB_50	- 0x32							
* REG_CB_51	- 0x33							
* REG_CB_52	- 0x34							
* REG_CB_53	- 0x35							
* REG_CB_54	- 0x36							
* REG_CB_55	- 0x37							
* REG_CB_56	- 0x38							
* REG_CB_57	- 0x39							
* REG_CB_58	- 0x3A							
* REG_CB_59	- 0x3B							
* REG_CB_60	- 0x3C							
* REG_CB_61	- 0x3D							
* REG_CB_62	- 0x3E							
* 									
* REG_0X_00	- 0x00							
* REG_0X_01	- 0x01							
* REG_0X_02	- 0x02							
* REG_0X_03	- 0x03							
* REG_0X_04	- 0x04							
* REG_0X_05	- 0x05							
* REG_0X_06	- 0x06							
* REG_0X_07	- 0x07							
* REG_0X_08	- 0x08							
* REG_0X_09	- 0x09							
* REG_0X_10	- 0x0A							
* REG_0X_11	- 0x0B							
* REG_0X_12	- 0x0C							
* REG_0X_13	- 0x0D							
* REG_0X_14	- 0x0E							
* REG_0X_15	- 0x0F							
* REG_0X_16	- 0x10							
* REG_0X_17	- 0x11							
* REG_0X_18	- 0x12							
* REG_0X_19	- 0x13							
* REG_0X_20	- 0x14							
* REG_0X_21	- 0x15							
* REG_0X_22	- 0x16							
* REG_0X_23	- 0x17							
* REG_0X_24	- 0x18							
* REG_0X_25	- 0x19							
* REG_0X_26	- 0x1A							
* REG_0X_27	- 0x1B							
* REG_0X_28	- 0x1C							
* REG_0X_29	- 0x1D							
* REG_0X_30	- 0x1E							
* REG_0X_31	- 0x1F							
************************************************************/

#define REG_CB_32	0x20
#define REG_CB_33	0x21
#define REG_CB_34	0x22
#define REG_CB_35	0x23
#define REG_CB_36	0x24
#define REG_CB_37	0x25
#define REG_CB_38	0x26
#define REG_CB_39	0x27
#define REG_CB_40	0x28
#define REG_CB_41	0x29
#define REG_CB_42	0x2A
#define REG_CB_43	0x2B
#define REG_CB_44	0x2C
#define REG_CB_45	0x2D
#define REG_CB_46	0x2E
#define REG_CB_47	0x2F
#define REG_CB_48	0x30
#define REG_CB_49	0x31
#define REG_CB_50	0x32
#define REG_CB_51	0x33
#define REG_CB_52	0x34
#define REG_CB_53	0x35
#define REG_CB_54	0x36
#define REG_CB_55	0x37
#define REG_CB_56	0x38
#define REG_CB_57	0x39
#define REG_CB_58	0x3A
#define REG_CB_59	0x3B
#define REG_CB_60	0x3C
#define REG_CB_61	0x3D
#define REG_CB_62	0x3E

#define REG_0X_00	0x00
#define REG_0X_01	0x01
#define REG_0X_02	0x02
#define REG_0X_03	0x03
#define REG_0X_04	0x04
#define REG_0X_05	0x05
#define REG_0X_06	0x06
#define REG_0X_07	0x07
#define REG_0X_08	0x08
#define REG_0X_09	0x09
#define REG_0X_10	0x0A
#define REG_0X_11	0x0B
#define REG_0X_12	0x0C
#define REG_0X_13	0x0D
#define REG_0X_14	0x0E
#define REG_0X_15	0x0F
#define REG_0X_16	0x10
#define REG_0X_17	0x11
#define REG_0X_18	0x12
#define REG_0X_19	0x13
#define REG_0X_20	0x14
#define REG_0X_21	0x15
#define REG_0X_22	0x16
#define REG_0X_23	0x17
#define REG_0X_24	0x18
#define REG_0X_25	0x19
#define REG_0X_26	0x1A 
#define REG_0X_27	0x1B 
#define REG_0X_28	0x1C 
#define REG_0X_29	0x1D 
#define REG_0X_30	0x1E
#define REG_0X_31	0x1F

/*******************************************
*  								 
*******************************************/

/*******************************************
* Bank selection value					 
*******************************************/
	
#define BANK_00				0x00
#define BANK_01				0x01
#define BANK_CB				0xFF	// this is the common bank or don't kare

/************************************************************
*							
* Constant: WHO_AM_I					
*							
* WHO_AM_I is the identifier code of MAX21000 and is equal to *0xB1*
*							
************************************************************/
#define WHO_AM_I            0xB1
#define WHO_AM_I_BNK		BANK_CB
#define WHO_AM_I_REG		0x20
#define WHO_AM_I_MSK		0xFF
#define WHO_AM_I_POS		0x00
#define WHO_AM_I_LEN		0x08


#define BANK_SEL_BNK		BANK_CB
#define BANK_SEL_REG		0x21
#define BANK_SEL_MSK		0x07
#define BANK_SEL_POS		0x00
#define BANK_SEL_LEN		0x03

/************************************************************
*									
* Constants: Data status flags						
* 									
*	STATUS_NO_DATA		- 0x00					
*	STATUS_NEW_DATA	- 0x01					
*	STATUS_ERR_DATA	- 0x02					   
*									
************************************************************/
#define STATUS_NO_DATA		0x00
#define STATUS_NEW_DATA		0x01
#define STATUS_ERR_DATA		0x02

#define STATUS
#define STATUS_BNK			BANK_CB
#define STATUS_REG			0x22
#define STATUS_MSK			0x03
#define STATUS_POS			0x00
#define STATUS_LEN			0x02

 
#define GYRO_X_MSB_BNK		BANK_CB
#define GYRO_X_MSB_REG		0x23
#define GYRO_X_MSB_MSK		0xFF
#define GYRO_X_MSB_POS		0x00
#define GYRO_X_MSB_LEN		0x08

 
#define GYRO_X_LSB_BNK		BANK_CB
#define GYRO_X_LSB_REG		0x24
#define GYRO_X_LSB_MSK		0xFF
#define GYRO_X_LSB_POS		0x00
#define GYRO_X_LSB_LEN		0x08
  
#define GYRO_Y_MSB_BNK		BANK_CB
#define GYRO_Y_MSB_REG		0x25
#define GYRO_Y_MSB_MSK		0xFF
#define GYRO_Y_MSB_POS		0x00
#define GYRO_Y_MSB_LEN		0x08
  
#define GYRO_Y_LSB_BNK		BANK_CB
#define GYRO_Y_LSB_REG		0x26
#define GYRO_Y_LSB_MSK		0xFF
#define GYRO_Y_LSB_POS		0x00
#define GYRO_Y_LSB_LEN		0x08
  
#define GYRO_Z_MSB_BNK		BANK_CB
#define GYRO_Z_MSB_REG		0x27
#define GYRO_Z_MSB_MSK		0xFF
#define GYRO_Z_MSB_POS		0x00
#define GYRO_Z_MSB_LEN		0x08
  
#define GYRO_Z_LSB_BNK		BANK_CB
#define GYRO_Z_LSB_REG		0x28
#define GYRO_Z_LSB_MSK		0xFF
#define GYRO_Z_LSB_POS		0x00
#define GYRO_Z_LSB_LEN		0x08
  
#define TEMP_MSB_BNK		BANK_CB
#define TEMP_MSB_REG		0x29
#define TEMP_MSB_MSK		0xFF
#define TEMP_MSB_POS		0x00
#define TEMP_MSB_LEN		0x08

#define TEMP_LSB_BNK		BANK_CB
#define TEMP_LSB_REG		0x2A
#define TEMP_LSB_MSK		0xFF
#define TEMP_LSB_POS		0x00
#define TEMP_LSB_LEN		0x08


#define HP_RESET
#define HP_RESET_BNK		BANK_CB
#define HP_RESET_REG		0x3B
#define HP_RESET_MSK		0xFF
#define HP_RESET_POS		0x00
#define HP_RESET_LEN		0x08

/************************************************************
*												
* Constants: FIFO status flags									
* 												
*       FIFO_EMPTY      - 0x01							
*       FIFO_FULL       - 0x02					        		
*       FIFO_OVER_THS   - 0x04								
*       FIFO_RD_EMPTY   - 0x08								
*       FIFO_DATA_LOST  - 0x10								
*												
************************************************************/
#define FIFO_EMPTY          0x01
#define FIFO_FULL           0x02
#define FIFO_OVER_THS		0x04
#define FIFO_RD_EMPTY		0x08
#define FIFO_DATA_LOST		0x10


#define FIFO_COUNT
#define FIFO_COUNT_BNK		BANK_CB
#define FIFO_COUNT_REG		0x3C
#define FIFO_COUNT_MSK		0xFF
#define FIFO_COUNT_POS		0x00
#define FIFO_COUNT_LEN		0x08

#define FIFO_STATUS
#define FIFO_STATUS_BNK		BANK_CB
#define FIFO_STATUS_REG		0x3D
#define FIFO_STATUS_MSK		0x1F
#define FIFO_STATUS_POS		0x00
#define FIFO_STATUS_LEN		0x05

#define FIFO_DATA
#define FIFO_DATA_BNK		BANK_CB
#define FIFO_DATA_REG		0x3E
#define FIFO_DATA_MSK		0xFF
#define FIFO_DATA_POS		0x00
#define FIFO_DATA_LEN		0x08

#define PAR_RST
#define PAR_RST_BNK		    BANK_CB
#define PAR_RST_REG		    0x3F
#define PAR_RST_MSK		    0xFF
#define PAR_RST_POS		    0x00
#define PAR_RST_LEN		    0x08


/*******************************************
* Constant: Register 0 of BANK 0 content
*
*   SENSE_CFG0  -   0x00			
*******************************************/
#define SENSE_CFG0		0x00

/************************************************************
*										
* Constants: Rate Enable								
*										
*	EN_X_RATE	-	0x01						
*	EN_Y_RATE	-	0x02						
*	EN_Z_RATE	-	0x04
*	EN_ALL_RATE	-	0x07
*										
************************************************************/
#define	EN_X_RATE		0x01
#define	EN_Y_RATE		0x02
#define	EN_Z_RATE		0x04
#define	EN_ALL_RATE		0x07


#define RATE
#define RATE_BNK			BANK_00
#define RATE_REG			SENSE_CFG0
#define RATE_MSK			0x07
#define RATE_POS			0x00
#define RATE_LEN			0x03

/************************************************************
*										
* Constants: Power Mode Constant							
* 										
*	PW_MODE_DEEP_PD	- 0x00						
*	PW_MODE_NORMAL	- 0x08					        	
*	PW_MODE_SLEEP	- 0x18						
*	PW_SL_TO_NM		- 0x30						
*	PW_PD_TO_NM		- 0x38						
*										
************************************************************/

#define	PW_MODE_DEEP_PD	0x00
#define	PW_MODE_NORMAL	0x08
#define	PW_MODE_SLEEP	0x10    //  0x18
#define	PW_MODE_SPOT	0x18
#define	PW_SL_TO_SP	0x20
#define	PW_PD_TO_SP	0x28
#define	PW_SL_TO_NM     0x30
#define	PW_PD_TO_NM     0x38

#define PW
#define PW_BNK			BANK_00
#define PW_REG			SENSE_CFG0
#define PW_MSK			0x38
#define PW_POS			0x03
#define PW_LEN			0x03

/************************************************************
*															
* Constants: Full Scale Constant									
* 	
* If OIS full scale is enabled the FS value is 
* downscaled of a factor 8.
*														
*	FS_2000_DPS - 0x00												
*	FS_1000_DPS - 0x40										
*	FS_500_DPS  - 0x80										
*	FS_250_DPS  - 0xC0										
*															
************************************************************/
#define	FS_2000_DPS		0x00
#define	FS_1000_DPS		0x40
#define	FS_500_DPS		0x80
#define	FS_250_DPS		0xC0

#define	FS
#define FS_BNK			BANK_00
#define FS_REG			SENSE_CFG0
#define FS_MSK			0xC0
#define FS_POS			0x06
#define FS_LEN			0x02


/*******************************************
* Constant: Register 1 of BANK 0 content
*
*   SENSE_CFG1  -   0x01			
*******************************************/
#define SENSE_CFG1		0x01

/************************************************************
*															
* Constants: OIS full scale Constant								
* 															
* If OIS full scale is enabled the FS value is 
* downscaled of a factor 8.	
*															
*	OIS_FS_ON 	-	0x01										
*	OIS_FS_OFF	-	0x00									
*																				
************************************************************/

#define OIS_FS_ON	0x01			
#define OIS_FS_OFF	0x00

#define	OIS_FS
#define OIS_FS_BNK		BANK_00
#define OIS_FS_REG		SENSE_CFG1
#define OIS_FS_MSK		0x01
#define OIS_FS_POS		0x00
#define OIS_FS_LEN		0x01


/************************************************************
*															
* Constants: Max frequency Constant								
* 															
* This is the maximum frequency usable as ODR													
*																					
*   MAX_ODR_10K	-	10000							
*																		
************************************************************/
			
#define SET_MAX_ODR_10K 0x00    // must be removed!!!!
#define MAX_ODR_10K	10000   

/************************************************************
*															
* Constants: Bandwidth Constant								
* 															
* Bandwidth is espressed in Hz @ 10KHz. 																								
*															
*	SNS_BW_2	-	0x00									
*	SNS_BW_4	-	0x04									
*	SNS_BW_6	-	0x08									
*	SNS_BW_8	-	0x0C									
*	SNS_BW_10	-	0x10									
*	SNS_BW_12	-	0x14									
*	SNS_BW_16	-	0x18									
*	SNS_BW_24	-	0x1C									
*	SNS_BW_32	-	0x20									
*	SNS_BW_50	-	0x24									
*	SNS_BW_75	-	0x28									
*	SNS_BW_100	-	0x2C									
*	SNS_BW_150	-	0x30									
*	SNS_BW_200	-	0x34									
*	SNS_BW_250	-	0x38									
*	SNS_BW_300	-	0x3C									
*															
************************************************************/
#define SNS_BW_2		0x00
#define SNS_BW_4		0x04
#define SNS_BW_6		0x08
#define SNS_BW_8		0x0C
#define SNS_BW_10		0x10
#define SNS_BW_12		0x14
#define SNS_BW_16		0x18
#define SNS_BW_24		0x1C
#define SNS_BW_32		0x20
#define SNS_BW_50		0x24
#define SNS_BW_75		0x28
#define SNS_BW_100		0x2C
#define SNS_BW_150		0x30
#define SNS_BW_200		0x34
#define SNS_BW_250		0x38
#define SNS_BW_300		0x3C

#define	BW
#define BW_BNK			BANK_00
#define BW_REG			SENSE_CFG1
#define BW_MSK			0x3C
#define BW_POS			0x02
#define BW_LEN			0x04


/*******************************************
* Constant: Register 2 of BANK 0 content
*
*   SENSE_CFG2  -   0x02			
*******************************************/
#define SENSE_CFG2		0x02

/****************************************************
* Macro: numeric ODR									
*													
* ODR setting is computed as defined in the A.N. and depends	
* on the max frequency setting. 							
*													
* Parameters:											
*		freqVal - Desired ODR frequency in Hz 			
*		maxOdr - Value of the max frequency used in Hz	
*													
****************************************************/

#define numericODR(freqVal, maxOdr) ((freqVal < maxOdr/500) ? ((maxOdr/(20*freqVal))+154) : 	\
                                        (freqVal < maxOdr/10)  ? ((maxOdr/(5*freqVal))+79) 	:	\
                                        ((maxOdr/freqVal)-1)		)

#define ODR_SPOT_200            0x00
#define ODR_SPOT_100            0x01
#define ODR_SPOT_50             0x02
#define ODR_SPOT_25             0x03

#define	ODR
#define ODR_BNK			BANK_00
#define ODR_REG			SENSE_CFG2
#define ODR_MSK			0xFF
#define ODR_POS			0x00
#define ODR_LEN			0x08	

/*******************************************
* Constant: Register 3 of BANK 0 content
*
*   SENSE_CFG3  -   0x03			
*******************************************/
#define SENSE_CFG3		0x03

/************************************************************
*															
* Constants: High pass cut-off constant							
*															
* These are the settings for the possible cut off frequency 	
* for the high pass filter of the digital chain.									
*															
* Cut-off frequency is in the range 0.1Hz (HP_CUT_01) to 				
* 100Hz (HP_CUT_1000)																										
*															
*    HP_CUT_01		    -    0x00 							
*    HP_CUT_02		    -    0x01							
*    HP_CUT_03		    -    0x02							
*    HP_CUT_05		    -    0x03							
*    HP_CUT_07		    -    0x04							
*    HP_CUT_10		    -    0x05							
*    HP_CUT_17		    -    0x06							
*    HP_CUT_30		    -    0x07							
*    HP_CUT_45		    -    0x08							
*    HP_CUT_70		    -    0x09							
*    HP_CUT_110		    -    0x0A							
*    HP_CUT_170		    -    0x0B							
*    HP_CUT_260		    -    0x0C							
*    HP_CUT_400		    -    0x0D							
*    HP_CUT_640		    -    0x0E							
*    HP_CUT_1000		-    0x0F						
*																	
************************************************************/
#define HP_CUT_01		0x00
#define HP_CUT_02		0x01
#define HP_CUT_03		0x02
#define HP_CUT_05		0x03
#define HP_CUT_07		0x04
#define HP_CUT_10		0x05
#define HP_CUT_17		0x06
#define HP_CUT_30		0x07
#define HP_CUT_45		0x08
#define HP_CUT_70		0x09
#define HP_CUT_110		0x0A
#define HP_CUT_170		0x0B
#define HP_CUT_260		0x0C
#define HP_CUT_400		0x0D
#define HP_CUT_640		0x0E
#define HP_CUT_1000		0x0F

#define	HP_CUT
#define HP_CUT_BNK		BANK_00
#define HP_CUT_REG		SENSE_CFG3
#define HP_CUT_MSK		0x0F
#define HP_CUT_POS		0x00
#define HP_CUT_LEN		0x04


/************************************************************
*															
* Constants: Data source constant								
* 															
* These are the settings for the possible chain of the digital elaboration	
*															
* Data source can be: data, data with high pass filter, data at high 		
* frequency, data at high frequency with high pass filter				
*															
*	SNS_OUT 		- 0x00											
*	SNS_OUT_HP		- 0x10									
*	SNS_OUT_HF 		- 0x20											
*	SNS_OUT_HF_HP	- 0x30									
*																					
************************************************************/
#define SNS_OUT			0x00			
#define SNS_OUT_HP		0x10
#define SNS_OUT_HF		0x20
#define SNS_OUT_HF_HP	0x30

#define	SRC_CFG
#define SRC_CFG_BNK		BANK_00
#define SRC_CFG_REG		SENSE_CFG3
#define SRC_CFG_MSK		0x30
#define SRC_CFG_POS		0x04
#define SRC_CFG_LEN		0x02

/*******************************************
* Constant: Register 19 of BANK 0 content
*
*   DR_CFG  -   0x13			
*******************************************/
#define DR_CFG		0x13

/************************************************************
*															
* Constants: Data ready reset mode constant						
* 															
* These are the settings for data ready reset mode					
*															
*	DATA_RDY_ALL	- 0x00									
*	DATA_RDY_ANY	- 0x01									
*	DATA_RDY_STA	- 0x02									
*															
************************************************************/
#define DATA_RDY_ALL	0x00
#define DATA_RDY_ANY	0x01
#define DATA_RDY_STA	0x02

#define	DATA_RDY
#define DATA_RDY_BNK	BANK_00
#define DATA_RDY_REG	DR_CFG
#define DATA_RDY_MSK	0x03
#define DATA_RDY_POS	0x00
#define DATA_RDY_LEN	0x02


/*******************************************
* Constant: Register 20 of BANK 0 content
*
*   IO_CFG  -   0x14			
*******************************************/
#define IO_CFG		0x14

/************************************************************
*															
* Constants: Pull Up/Down constant								
* 															
* These are the settings for INT1, INT2, DSYNC pull-up and pull-down					
*															
*	DSYNC_PD_EN	-	0x80									
*	DSYNC_PU_EN	- 	0x40									
*	INT1_PD_EN	- 	0x20									
*	INT1_PU_EN	- 	0x10									
*	INT2_PD_EN	- 	0x08									
*	INT2_PU_EN	- 	0x04									
*															
************************************************************/

#define DSYNC_PD_EN		0x80
#define DSYNC_PU_EN		0x40
#define INT1_PD_EN		0x20
#define INT1_PU_EN		0x10
#define INT2_PD_EN		0x08
#define INT2_PU_EN		0x04

#define	DSYNC_PUD
#define DSYNC_PUD_BNK   BANK_00
#define DSYNC_PUD_REG   IO_CFG
#define DSYNC_PUD_MSK   0xC0
#define DSYNC_PUD_POS   0x06
#define DSYNC_PUD_LEN   0x02

#define	INT1_PUD
#define INT1_PUD_BNK	BANK_00
#define INT1_PUD_REG	IO_CFG
#define INT1_PUD_MSK	0x30
#define INT1_PUD_POS	0x04
#define INT1_PUD_LEN	0x02

#define	INT2_PUD
#define INT2_PUD_BNK	BANK_00
#define INT2_PUD_REG	IO_CFG
#define INT2_PUD_MSK	0x0C
#define INT2_PUD_POS	0x02
#define INT2_PUD_LEN	0x02


/*******************************************
* Constant: Register 21 of BANK 0 content
*
*   I2C_CFG  -   0x15			
*******************************************/
#define I2C_CFG		0x15

/************************************************************
*															
* Constants: I2C mode constant									
* 															
* These are the settings for I2C mode								
*															
*	I2C_FAST_NO_SPIKE	- 	0x00							
*	I2C_FAST_STD		-	0x10							
*	I2C_HIGH_STD		- 	0x20							
*	I2C_HIGH_NO_SPIKE	- 	0x30
*	I2C_FAST_NO_DEL 	- 	0x40							
*	I2C_ONLY_SPI		- 	0x50							
*															
************************************************************/

#define I2C_FAST_NO_SPIKE		0x00
#define I2C_FAST_STD			0x10
#define I2C_HIGH_STD			0x20
#define I2C_HIGH_NO_SPIKE		0x30
#define I2C_ONLY_SPI			0x50

#define	I2C_MODE
#define I2C_MODE_BNK		BANK_00
#define I2C_MODE_REG		I2C_CFG
#define I2C_MODE_MSK		0x70
#define I2C_MODE_POS		0x04
#define I2C_MODE_LEN		0x03

/************************************************************
*													
* Constants: Pads current constant						
* 													
* These are the settings for the pads current				
*													
*	PAD_CURR_3		- 	0x00	3mA driver out		
*	PAD_CURR_6		-	0x04	6mA driver out		
*	PAD_CURR_12	    - 	0x0C	12mA driver out			
*													
************************************************************/

#define PAD_CURR_3		0x00
#define PAD_CURR_6		0x04
#define PAD_CURR_12		0x0C

#define	PAD
#define PAD_BNK		BANK_00
#define PAD_REG		I2C_CFG
#define PAD_MSK		0x0C
#define PAD_POS		0x02
#define PAD_LEN		0x02

/************************************************************
*															
* Constant: I2C_OFF											
*															
* I2C_OFF set to 0x01 turns off the I2C state machine.				
*															
************************************************************/

#define	I2C_OFF			0x01
#define I2C_OFF_BNK		BANK_00
#define I2C_OFF_REG		I2C_CFG
#define I2C_OFF_MSK		0x01
#define I2C_OFF_POS		0x00
#define I2C_OFF_LEN		0x01

/*******************************************
* Constant: Register 22 of BANK 0 content
*
*   ITF_OTP  -   0x16			
*******************************************/
#define ITF_OTP		0x16

/************************************************************
*															
* Constant: PAR_ERR											
*															
* PAR_ERR is set if an error occurs in register address				
*															
************************************************************/

#define	PAR_ERR			
#define PAR_ERR_BNK		BANK_00
#define PAR_ERR_REG		ITF_OTP
#define PAR_ERR_MSK		0x40
#define PAR_ERR_POS		0x06
#define PAR_ERR_LEN		0x01

/************************************************************
*															
* Constant: SPI_3_WIRE											
*															
* SPI_3_WIRE is set high for use SPI 3 wires mode				
*															
************************************************************/

#define	SPI_3_WIRE		0x20	
#define SPI_3_WIRE_BNK  BANK_00
#define SPI_3_WIRE_REG  ITF_OTP
#define SPI_3_WIRE_MSK  0x20
#define SPI_3_WIRE_POS  0x05
#define SPI_3_WIRE_LEN  0x01

/************************************************************
*															
* Constants: Autoincrement and parity											
*															
* Used for autoincrement enable/disable or even/odd 
* parity check on register address				
*
*   NO_PARITY_CHECK -   0x00
*   PARITY_EVEN     -   0x80
*   PARITY_ODD      -   0x10
*															
************************************************************/

#define	NO_PARITY_CHECK     0x00
#define PARITY_EVEN         0x80
#define PARITY_ODD          0x10
        
#define	IF_PARITY			
#define IF_PARITY_BNK  BANK_00
#define IF_PARITY_REG  ITF_OTP
#define IF_PARITY_MSK  0x18
#define IF_PARITY_POS  0x03
#define IF_PARITY_LEN  0x02

/************************************************************
*															
* Constants: Endian constant										
* 															
* These are the settings for the endian								
*															
*	BIG_ENDIAN 	- 0x00									
*	LITTLE_ENDIAN	- 0x40									
*															
************************************************************/
#define BIG_ENDIAN	0x00
#define LITTLE_ENDIAN	0x04

#define	ENDIAN
#define ENDIAN_BNK		BANK_00
#define ENDIAN_REG		ITF_OTP
#define ENDIAN_MSK		0x04
#define ENDIAN_POS		0x02
#define ENDIAN_LEN		0x01


/************************************************************
*															
* Constant: MEM_RUN											
*															
* MEM_RUN is set if OTP is being downloaded				
*															
************************************************************/

#define	MEM_RUN			
#define MEM_RUN_BNK		BANK_00
#define MEM_RUN_REG		ITF_OTP
#define MEM_RUN_MSK		0x02
#define MEM_RUN_POS		0x01
#define MEM_RUN_LEN		0x01

/************************************************************
*															
* Constant: RESTART											
*															
* RESTART must be set for OTP trimming download				
*															
************************************************************/

#define	RESTART         0x01			
#define RESTART_BNK     BANK_00
#define RESTART_REG     ITF_OTP
#define RESTART_MSK     0x01
#define RESTART_POS     0x00
#define RESTART_LEN     0x01


/************************************************************
*															
*   Constant: Register 23 of BANK 0 content										
* 															
*	Represent the threshold of the data for the FIFO ths flag			
*	It counts the number of byte (2 byte is one data)				
*																					
************************************************************/

#define	FIFO_THS        0x17
#define FIFO_THS_BNK	BANK_00
#define FIFO_THS_REG	0x17
#define FIFO_THS_MSK	0xFF
#define FIFO_THS_POS	0x00
#define FIFO_THS_LEN	0x08


/*******************************************
* Constant: Register 24 of BANK 0 content
*
*   FIFO_CFG  -   0x18			
*******************************************/
#define FIFO_CFG    0x18

/************************************************************
*															
* Constants: FIFO channels									
* 															
*	FIFO_STORE_X	-	0x01										
*	FIFO_STORE_Y	-	0x02								
*	FIFO_STORE_Z	-	0x04								
*																					
************************************************************/
#define FIFO_STORE_X	0x01
#define FIFO_STORE_Y	0x02
#define FIFO_STORE_Z	0x04

#define	FIFO_STORE
#define FIFO_STORE_BNK	BANK_00
#define FIFO_STORE_REG	FIFO_CFG
#define FIFO_STORE_MSK	0x07
#define FIFO_STORE_POS	0x00
#define FIFO_STORE_LEN	0x03

/************************************************************
*															
* Constants: FIFO overrun mode								
* 															
*	FIFO_OVERRUN_ON	    -   0x10								
*	FIFO_OVERRUN_OFF	-   0x00																
*																			
************************************************************/
#define FIFO_OVERRUN_ON		0x10
#define FIFO_OVERRUN_OFF	0x00

#define	FIFO_OVERRUN
#define FIFO_OVERRUN_BNK	BANK_00
#define FIFO_OVERRUN_REG	FIFO_CFG
#define FIFO_OVERRUN_MSK	0x10
#define FIFO_OVERRUN_POS	0x04
#define FIFO_OVERRUN_LEN	0x01


/************************************************************
*															
* Constants: FIFO interrupt selection
* 															
*	FIFO_INT_OR	    -   0x00								
*	FIFO_INT_AND	-   0x20																
*																			
************************************************************/
#define FIFO_INT_OR     0x00
#define FIFO_INT_AND    0x20
        
#define	FIFO_INT
#define FIFO_INT_BNK	BANK_00
#define FIFO_INT_REG	FIFO_CFG
#define FIFO_INT_MSK	0x20
#define FIFO_INT_POS	0x05
#define FIFO_INT_LEN	0x01

/************************************************************
*															
* Constants: FIFO modes										
* 															
*	FIFO_MODE_OFF		-	0x01							
*	FIFO_MODE_NORMAL	-	0x02							
*	FIFO_MODE_INT		-	0x04							
*	FIFO_MODE_TOGGLE	-	0xC0											
*																			
************************************************************/
#define FIFO_MODE_OFF		0x00
#define FIFO_MODE_NORMAL	0x40
#define FIFO_MODE_INT		0x80
#define FIFO_MODE_TOGGLE	0xC0

#define	FIFO_MODE
#define FIFO_MODE_BNK	BANK_00
#define FIFO_MODE_REG	FIFO_CFG
#define FIFO_MODE_MSK	0xC0
#define FIFO_MODE_POS	0x06
#define FIFO_MODE_LEN	0x02

/*******************************************
* Constant: Register 26 of BANK 0 content
*
*   DSYNC_CFG  -   0x1A			
*******************************************/
#define DSYNC_CFG    0x1A

/************************************************************
*															
* Constants: DATA SYNC configurations								
*
*	DSM_ENB 		-	0x08
*	DSW_LOW 		-	0x10
*	DSW_EDG 		-	0x20 
*	DSQ_ENF 		-	0x40 															
*	DSQ_ENR 		-	0x80																		
*																			
************************************************************/
#define DSM_ENB         0x08
#define DSM_ENB_BNK	    BANK_00
#define DSM_ENB_REG     DSYNC_CFG
#define DSM_ENB_MSK	    0x08
#define DSM_ENB_POS	    0x03
#define DSM_ENB_LEN	    0x01

#define DSW_LOW         0x10
#define DSW_LOW_BNK	    BANK_00
#define DSW_LOW_REG     DSYNC_CFG
#define DSW_LOW_MSK	    0x10
#define DSW_LOW_POS	    0x04
#define DSW_LOW_LEN	    0x01

#define DSW_EDG         0x20
#define DSW_EDG_BNK	    BANK_00
#define DSW_EDG_REG     DSYNC_CFG
#define DSW_EDG_MSK	    0x20
#define DSW_EDG_POS	    0x05
#define DSW_EDG_LEN	    0x01

#define DSQ_ENF         0x40
#define DSQ_ENF_BNK	    BANK_00
#define DSQ_ENF_REG     DSYNC_CFG
#define DSQ_ENF_MSK	    0x40
#define DSQ_ENF_POS	    0x06
#define DSQ_ENF_LEN	    0x01

#define DSQ_ENR         0x80
#define DSQ_ENR_BNK	    BANK_00
#define DSQ_ENR_REG     DSYNC_CFG
#define DSQ_ENR_MSK	    0x80
#define DSQ_ENR_POS	    0x07
#define DSQ_ENR_LEN	    0x01

/*******************************************
* Constant: Register 27 of BANK 0 content
*
* Represents the number of sample to be 
* stored in FIFO upon detecting a DSYNC active edge	
*
*   DSYNC_CNT   -   0x1B		
*******************************************/
#define DSYNC_CNT       0x1B
#define DSYNC_CNT_BNK	BANK_00
#define DSYNC_CNT_REG	DSYNC_CNT
#define DSYNC_CNT_MSK	0xFF
#define DSYNC_CNT_POS	0x00
#define DSYNC_CNT_LEN	0x08


/*******************************************
* Constant: Register 0 of BANK 1 content
*
* Represents the rate interrupt reference for X 
*
*   INT_REF_X   -   0x00		
*******************************************/
#define INT_REF_X       0x00
#define INT_REF_X_BNK	BANK_01
#define INT_REF_X_REG	INT_REF_X
#define INT_REF_X_MSK	0xFF
#define INT_REF_X_POS	0x00
#define INT_REF_X_LEN	0x08

/*******************************************
* Constant: Register 1 of BANK 1 content
*
* Represents the rate interrupt reference for Y 
*
*   INT_REF_Y   -   0x01		
*******************************************/
#define INT_REF_Y       0x01
#define INT_REF_Y_BNK	BANK_01
#define INT_REF_Y_REG	INT_REF_Y
#define INT_REF_Y_MSK	0xFF
#define INT_REF_Y_POS	0x00
#define INT_REF_Y_LEN	0x08

/*******************************************
* Constant: Register 2 of BANK 1 content
*
* Represents the rate interrupt reference for Z 
*
*   INT_REF_Z   -   0x02		
*******************************************/
#define INT_REF_Z       0x02
#define INT_REF_Z_BNK	BANK_01
#define INT_REF_Z_REG	INT_REF_Z
#define INT_REF_Z_MSK	0xFF
#define INT_REF_Z_POS	0x00
#define INT_REF_Z_LEN	0x08

/*******************************************
* Constant: Register 3 of BANK 1 content
*
* Represents the rate interrupt debounce for X 
*
*   INT_DEB_X   -   0x03		
*******************************************/
#define INT_DEB_X       0x03
#define INT_DEB_X_BNK	BANK_01
#define INT_DEB_X_REG	INT_DEB_X
#define INT_DEB_X_MSK	0xFF
#define INT_DEB_X_POS	0x00
#define INT_DEB_X_LEN	0x08

/*******************************************
* Constant: Register 4 of BANK 1 content
*
* Represents the rate interrupt debounce for Y 
*
*   INT_DEB_Y   -   0x04		
*******************************************/
#define INT_DEB_Y       0x04
#define INT_DEB_Y_BNK	BANK_01
#define INT_DEB_Y_REG	INT_DEB_Y
#define INT_DEB_Y_MSK	0xFF
#define INT_DEB_Y_POS	0x00
#define INT_DEB_Y_LEN	0x08

/*******************************************
* Constant: Register 5 of BANK 1 content
*
* Represents the rate interrupt debounce for Z 
*
*   INT_DEB_Z   -   0x05		
*******************************************/
#define INT_DEB_Z       0x05
#define INT_DEB_Z_BNK	BANK_01
#define INT_DEB_Z_REG	INT_DEB_Z
#define INT_DEB_Z_MSK	0xFF
#define INT_DEB_Z_POS	0x00
#define INT_DEB_Z_LEN	0x08

/*******************************************
* Constant: Register 6 of BANK 1 content
*
* Represents the rate interrupt configuration for X 
*
*   INT_MSK_X   -   0x06		
*******************************************/
#define INT_MSK_X       0x06
#define INT_MSK_X_BNK	BANK_01
#define INT_MSK_X_REG	INT_MSK_X
#define INT_MSK_X_MSK	0xFF
#define INT_MSK_X_POS	0x00
#define INT_MSK_X_LEN	0x08
           
/*******************************************
* Constant: Register 7 of BANK 1 content
*
* Represents the rate interrupt configuration for Y 
*
*   INT_MSK_Y   -   0x07		
*******************************************/
#define INT_MSK_Y       0x07
#define INT_MSK_Y_BNK	BANK_01
#define INT_MSK_Y_REG	INT_MSK_Y
#define INT_MSK_Y_MSK	0xFF
#define INT_MSK_Y_POS	0x00
#define INT_MSK_Y_LEN	0x08
           
/*******************************************
* Constant: Register 8 of BANK 1 content
*
* Represents the rate interrupt configuration for Z 
*
*   INT_MSK_Z   -   0x08		
*******************************************/
#define INT_MSK_Z       0x08
#define INT_MSK_Z_BNK	BANK_01
#define INT_MSK_Z_REG	INT_MSK_Z
#define INT_MSK_Z_MSK	0xFF
#define INT_MSK_Z_POS	0x00
#define INT_MSK_Z_LEN	0x08


/*******************************************
* Constant: Register 25 of BANK 1 content
*
* Revision ID 
*
*   REVISION_ID   -   0x19		
*******************************************/
#define REVISION_ID         0x19
#define REVISION_ID_BNK	    BANK_01
#define REVISION_ID_REG	    REVISION_ID
#define REVISION_ID_MSK	    0xFF
#define REVISION_ID_POS	    0x00
#define REVISION_ID_LEN	    0x08

/*******************************************
* Constant: Register 26 of BANK 1 content
*
* Serial Number 
*
*   SERIAL_0   -   0x1A		
*******************************************/
#define SERIAL_0        0x1A
#define SERIAL_0_BNK	BANK_01
#define SERIAL_0_REG	SERIAL_0
#define SERIAL_0_MSK	0xFF
#define SERIAL_0_POS	0x00
#define SERIAL_0_LEN	0x08

/*******************************************
* Constant: Register 27 of BANK 1 content
*
* Serial Number 
*
*   SERIAL_1   -   0x1B		
*******************************************/
#define SERIAL_1        0x1B
#define SERIAL_1_BNK	BANK_01
#define SERIAL_1_REG	SERIAL_1
#define SERIAL_1_MSK	0xFF
#define SERIAL_1_POS	0x00
#define SERIAL_1_LEN	0x08

/*******************************************
* Constant: Register 28 of BANK 1 content
*
* Serial Number 
*
*   SERIAL_2   -   0x1C		
*******************************************/
#define SERIAL_2        0x1C
#define SERIAL_2_BNK	BANK_01
#define SERIAL_2_REG	SERIAL_2
#define SERIAL_2_MSK	0xFF
#define SERIAL_2_POS	0x00
#define SERIAL_2_LEN	0x08

/*******************************************
* Constant: Register 29 of BANK 1 content
*
* Serial Number 
*
*   SERIAL_3   -   0x1D		
*******************************************/
#define SERIAL_3        0x1D
#define SERIAL_3_BNK	BANK_01
#define SERIAL_3_REG	SERIAL_3
#define SERIAL_3_MSK	0xFF
#define SERIAL_3_POS	0x00
#define SERIAL_3_LEN	0x08

/*******************************************
* Constant: Register 30 of BANK 1 content
*
* Serial Number 
*
*   SERIAL_4   -   0x1E		
*******************************************/
#define SERIAL_4        0x1E
#define SERIAL_4_BNK	BANK_01
#define SERIAL_4_REG	SERIAL_4
#define SERIAL_4_MSK	0xFF
#define SERIAL_4_POS	0x00
#define SERIAL_4_LEN	0x08
    
    /*******************************************
* Constant: Register 31 of BANK 1 content
*
* Serial Number 
*
*   SERIAL_5   -   0x1F		
*******************************************/
#define SERIAL_5        0x1F
#define SERIAL_5_BNK	BANK_01
#define SERIAL_5_REG	SERIAL_5
#define SERIAL_5_MSK	0xFF
#define SERIAL_5_POS	0x00
#define SERIAL_5_LEN	0x08
