/*********************************************************************
  File Information:
    FileName:        usb_function_msd.c
    Dependencies:    See INCLUDES section below
    Processor:       PIC18, PIC24, or PIC32
    Compiler:        C18, C30, or C32
    Company:         Microchip Technology, Inc.

    Software License Agreement

    The software supplied herewith by Microchip Technology Incorporated
    (the "Company") for its PIC(r) Microcontroller is intended and
    supplied to you, the Company's customer, for use solely and
    exclusively on Microchip PICmicro Microcontroller products. The
    software is owned by the Company and/or its supplier, and is
    protected under applicable copyright laws. All rights are reserved.
    Any use in violation of the foregoing restrictions may subject the
    user to criminal sanctions under applicable laws, as well as to
    civil liability for the breach of the terms and conditions of this
    license.

    THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
    WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
    TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
    IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
    CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.

  Summary:
    This file contains functions, macros, definitions, variables,
    datatypes, etc. that are required for use of the MSD function
    driver. This file should be included in projects that use the MSD
    \function driver.
    
    
    
    This file is located in the "\<Install Directory\>\\Microchip\\USB\\MSD
    Device Driver" directory.

  Description:
    USB MSD Function Driver File
    
    This file contains functions, macros, definitions, variables,
    datatypes, etc. that are required for use of the MSD function
    driver. This file should be included in projects that use the MSD
    \function driver.
    
    This file is located in the "\<Install Directory\>\\Microchip\\USB\\MSD
    Device Driver" directory.
    
    When including this file in a new project, this file can either be
    referenced from the directory in which it was installed or copied
    directly into the user application folder. If the first method is
    chosen to keep the file located in the folder in which it is installed
    then include paths need to be added so that the library and the
    application both know where to reference each others files. If the
    application folder is located in the same folder as the Microchip
    folder (like the current demo folders), then the following include
    paths need to be added to the application's project:
    
    ..\\..\\Microchip\\Include
    .
    
    If a different directory structure is used, modify the paths as
    required. An example using absolute paths instead of relative paths
    would be the following:
    
    C:\\Microchip Solutions\\Microchip\\Include
    
    C:\\Microchip Solutions\\My Demo Application

Change History:
  Rev         Description
  ----------  ----------------------------------------------------------
  2.6 - 2.7a  No change
  2.8		  Improved error case checking, error case recovery, and 
			  sense key/status reporting to the host.
  			  Implemented adjustable read/write failure retry feature.
			  Fixed minor bug that would have interfered with supporting 
			  more than 7 LUNs simultaneously.
  2.9         Added additional error case checking and handling so as to
              be able to pass all USB20CV MSC tests. Note: If using the internal
              flash demo, the MSD volume must be sized > 64kB, in order to pass 
              all USB20CV tests. The USB20CV MSC tests attempt to write up to 
              64kB worth of data during the test.  If the MSD volume is <64kB, 
              this portion of the test will fail, but this is intentional and 
              correct behavior, since we obviously can't successfully write more
              data than the entire drive volume size.
              General improvements/improved flows/improved inline code comments.
  2.9d        Added fix to allow MSD devices to be ejected in Mac OSX and so that
              read-only media does not create an error in Mac OSX every time it is
              enumerated.

********************************************************************/
 
/** I N C L U D E S **************************************************/
#include "USB/usb.h"
#include "HardwareProfile.h"
#include "FSconfig.h"

#include "USB/usb_function_msd.h"

#ifdef USB_USE_MSD

#if MAX_LUN == 0
    #define LUN_INDEX 0
#else
    #define LUN_INDEX gblCBW.bCBWLUN
#endif

#if defined(__C30__) || defined(__C32__) || defined __XC16__
    #if defined(USE_INTERNAL_FLASH)
        #include "MDD File System/Internal Flash.h"
    #endif

    #if defined(USE_SD_INTERFACE_WITH_SPI)
        #include "MDD File System/SD-SPI.h"
    #endif

    extern LUN_FUNCTIONS LUN[MAX_LUN + 1];
    #define LUNMediaInitialize()                LUN[LUN_INDEX].MediaInitialize()
    #define LUNReadCapacity()                   LUN[LUN_INDEX].ReadCapacity()
    #define LUNReadSectorSize()                 LUN[LUN_INDEX].ReadSectorSize()
    #define LUNMediaDetect()                    LUN[LUN_INDEX].MediaDetect()
    #define LUNSectorWrite(bLBA,pDest,Write0)   LUN[LUN_INDEX].SectorWrite(bLBA, pDest, Write0)
    #define LUNWriteProtectState()              LUN[LUN_INDEX].WriteProtectState()
    #define LUNSectorRead(bLBA,pSrc)            LUN[LUN_INDEX].SectorRead(bLBA, pSrc)
#else
    #if defined(USE_INTERNAL_FLASH)
        #include "MDD File System/Internal Flash.h"
    #endif

    #if defined(USE_SD_INTERFACE_WITH_SPI)
        #include "MDD File System/SD-SPI.h"
    #endif

    #define LUNMediaInitialize()                MDD_MediaInitialize()
    #define LUNReadCapacity()                   MDD_ReadCapacity()
    #define LUNReadSectorSize()                 MDD_ReadSectorSize()
    #define LUNMediaDetect()                    MDD_MediaDetect()
    #define LUNSectorWrite(bLBA,pDest,Write0)   MDD_SectorWrite(bLBA, pDest, Write0)
    #define LUNWriteProtectState()              MDD_WriteProtectState()
    #define LUNSectorRead(bLBA,pSrc)            MDD_SectorRead(bLBA, pSrc)
#endif

//Adjustable user options
#define MSD_FAILED_READ_MAX_ATTEMPTS  (BYTE)100u    //Used for error case handling
#define MSD_FAILED_WRITE_MAX_ATTEMPTS (BYTE)100u    //Used for error case handling

/** V A R I A B L E S ************************************************/
#if defined(__18CXX)
    #pragma udata
#endif

//State machine variables
BYTE MSD_State;			// Takes values MSD_WAIT, MSD_DATA_IN or MSD_DATA_OUT
BYTE MSDCommandState;
BYTE MSDReadState;
BYTE MSDWriteState;
BYTE MSDRetryAttempt;
//Other variables
USB_MSD_CBW gblCBW;	
BYTE gblCBWLength;
RequestSenseResponse gblSenseData[MAX_LUN + 1];
BYTE *ptrNextData;
USB_HANDLE USBMSDOutHandle;
USB_HANDLE USBMSDInHandle;
WORD MSBBufferIndex;
WORD gblMediaPresent; 
BOOL SoftDetach[MAX_LUN + 1];
BOOL MSDHostNoData;
BOOL MSDCBWValid;

static WORD_VAL TransferLength;
static DWORD_VAL LBA;

/* 
 * Number of Blocks and Block Length are global because 
 * for every READ_10 and WRITE_10 command need to verify if the last LBA 
 * is less than gblNumBLKS	
 */	
DWORD_VAL gblNumBLKS,gblBLKLen;	
extern const ROM InquiryResponse inq_resp;

/** P R I V A T E  P R O T O T Y P E S ***************************************/
BYTE MSDProcessCommand(void);
void MSDProcessCommandMediaAbsent(void);
void MSDProcessCommandMediaPresent(void);
BYTE MSDReadHandler(void);
BYTE MSDWriteHandler(void);
void ResetSenseData(void);
BYTE MSDCheckForErrorCases(DWORD);
void MSDErrorHandler(BYTE);
static void MSDComputeDeviceInAndResidue(WORD);

/** D E C L A R A T I O N S **************************************************/
#if defined(__18CXX)
    #pragma code
#endif

/** C L A S S  S P E C I F I C  R E Q ****************************************/

/******************************************************************************
  Function:
    void USBMSDInit(void)
    
  Summary:
    This routine initializes the MSD class packet handles, prepares to
    receive a MSD packet, and initializes the MSD state machine. This
    \function should be called once after the device is enumerated.

  Description:
    This routine initializes the MSD class packet handles, prepares to
    receive a MSD packet, and initializes the MSD state machine. This
    \function should be called once after the device is enumerated.
    
    Typical Usage:
    <code>
    void USBCBInitEP(void)
    {
        USBEnableEndpoint(MSD_DATA_IN_EP,USB_IN_ENABLED|USB_OUT_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);
        USBMSDInit();
    }
    </code>
  Conditions:
    The device should already be enumerated with a configuration that
    supports MSD before calling this function.
    
  Paramters: None

  Remarks:
    None                                                                                                          
  ****************************************************************************/	
void USBMSDInit(void)
{
    //Prepare to receive the first CBW
    USBMSDOutHandle = USBRxOnePacket(MSD_DATA_OUT_EP,(BYTE*)&msd_cbw,MSD_OUT_EP_SIZE);
    //Initialize IN handle to point to first available IN MSD bulk endpoint entry
    USBMSDInHandle = USBGetNextHandle(MSD_DATA_IN_EP, IN_TO_HOST);
    MSD_State = MSD_WAIT;
    MSDCommandState = MSD_COMMAND_WAIT;
    MSDReadState = MSD_READ10_WAIT;
    MSDWriteState = MSD_WRITE10_WAIT;
	MSDHostNoData = FALSE;
    gblNumBLKS.Val = 0;
    gblBLKLen.Val = 0;
    MSDCBWValid = TRUE;

    gblMediaPresent = 0;

    //For each of the possible logical units
    for(gblCBW.bCBWLUN=0;gblCBW.bCBWLUN<(MAX_LUN + 1);gblCBW.bCBWLUN++)
    {
        //clear all of the soft detach variables
        SoftDetach[gblCBW.bCBWLUN] =  FALSE;

        //see if the media is attached
        if(LUNMediaDetect())
        {
            //initialize the media
            if(LUNMediaInitialize())
            {
                //if the media was present and successfully initialized
                //  then mark and indicator that the media is ready
                gblMediaPresent |= ((WORD)1<<gblCBW.bCBWLUN);
            }
        }
        ResetSenseData();
    }
}

/******************************************************************************
 	Function:
 		void USBCheckMSDRequest(void)

 	Summary:
 		This routine handles MSD specific request that happen on EP0.  
        This function should be called from the USBCBCheckOtherReq() call back 
        function whenever implementing an MSD device.

 	Description:
 		This routine handles MSD specific request that happen on EP0.  These
        include, but are not limited to, the standard RESET and GET_MAX_LUN 
 		command requests.  This function should be called from the 
        USBCBCheckOtherReq() call back function whenever using an MSD device.	

        Typical Usage:
        <code>
        void USBCBCheckOtherReq(void)
        {
            //Since the stack didn't handle the request I need to check
            //  my class drivers to see if it is for them
            USBCheckMSDRequest();
        }
        </code>

 	PreCondition:
 		None
 		
 	Parameters:
 		None
 	
 	Return Values:
 		None
 		
 	Remarks:
 		None
 
 *****************************************************************************/	
void USBCheckMSDRequest(void)
{
    if(SetupPkt.Recipient != USB_SETUP_RECIPIENT_INTERFACE_BITFIELD) return;
    if(SetupPkt.bIntfID != MSD_INTF_ID) return;

	switch(SetupPkt.bRequest)
    {
	    case MSD_RESET:
	        //First make sure all request parameters are correct:
	        //MSD BOT specs require wValue to be == 0x0000 and wLength == 0x0000
	        if((SetupPkt.wValue != 0) || (SetupPkt.wLength != 0)) 
	        {
    	        return; //Return without handling the request (results in STALL)
    	    }    
	        
	        //Host would typically issue this after a STALL event on an MSD
	        //bulk endpoint.  The MSD reset should re-initialize status
	        //so as to prepare for a new CBW.  Any currently ongoing command
	        //block should be aborted, but the STALL and DTS states need to be
	        //maintained (host will re-initialize these seperately using 
	        //CLEAR_FEATURE, endpoint halt).
            MSD_State = MSD_WAIT;
            MSDCommandState = MSD_COMMAND_WAIT;
            MSDReadState = MSD_READ10_WAIT;
            MSDWriteState = MSD_WRITE10_WAIT;
            MSDCBWValid = TRUE;
            //Need to re-arm MSD bulk OUT endpoint, if it isn't currently armed, 
            //to be able to receive next CBW.  If it is already armed, don't need
            //to do anything, since we can already receive the next CBW (or we are 
            //STALLed, and the host will issue clear halt first).
            if(!USBHandleBusy(USBGetNextHandle(MSD_DATA_OUT_EP, OUT_FROM_HOST)))
            {        
                USBMSDOutHandle = USBRxOnePacket(MSD_DATA_OUT_EP,(BYTE*)&msd_cbw,MSD_OUT_EP_SIZE);
            }                

            //Let USB stack know we took care of handling the EP0 SETUP request.
            //Allow zero byte status stage to proceed normally now.
            USBEP0Transmit(USB_EP0_NO_DATA);    
	    	break;
	    case GET_MAX_LUN:
            //First make sure all request parameters are correct:
            //MSD BOT specs require wValue to be == 0x0000, and wLengh == 1
            if((SetupPkt.wValue != 0) || (SetupPkt.wLength != 1))   
            {
                break;  //Return without handling the request (results in STALL)
            }    
            
            //If the host asks for the maximum number of logical units
            //  then send out a packet with that information
	    	CtrlTrfData[0] = MAX_LUN;
            USBEP0SendRAMPtr((BYTE*)&CtrlTrfData[0],1,USB_EP0_INCLUDE_ZERO);
	    	break;
    }	//end switch(SetupPkt.bRequest)
}

/*********************************************************************************
  Function:
        BYTE MSDTasks(void)
    
  Summary:
    This function runs the MSD class state machines and all of its
    sub-systems. This function should be called periodically once the
    device is in the configured state in order to keep the MSD state
    machine going.
  Description:
    This function runs the MSD class state machines and all of its
    sub-systems. This function should be called periodically once the
    device is in the configured state in order to keep the MSD state
    machine going.
    
    Typical Usage:
    <code>
    void main(void)
    {
        USBDeviceInit();
        while(1)
        {
            USBDeviceTasks();
            if((USBGetDeviceState() \< CONFIGURED_STATE) ||
               (USBIsDeviceSuspended() == TRUE))
            {
                //Either the device is not configured or we are suspended
                //  so we don't want to do execute any application code
                continue;   //go back to the top of the while loop
            }
            else
            {
                //Keep the MSD state machine going
                MSDTasks();
    
                //Run application code.
                UserApplication();
            }
        }
    }
    </code>
  Conditions:
    None
  Return Values:
    BYTE -  the current state of the MSD state machine the valid values are
            defined in MSD.h under the MSDTasks state machine declaration section.
            The possible values are the following\:
            * MSD_WAIT
            * MSD_DATA_IN
            * MSD_DATA_OUT
            * MSD_SEND_CSW
  Remarks:
    None                                                                          
  *********************************************************************************/	
BYTE MSDTasks(void)
{
    BYTE i;
    
    //Error check to make sure we have are in the CONFIGURED_STATE, prior to
    //performing MSDTasks().  Some of the MSDTasks require that the device be
    //configured first.
    if(USBGetDeviceState() != CONFIGURED_STATE)
    {
        return MSD_WAIT;
    }
    
    //Note: Both the USB stack code (usb_device.c) and this MSD handler code 
    //have the ability to modify the BDT values for the MSD bulk endpoints.  If the 
    //USB stack operates in USB_INTERRUPT mode (user option in usb_config.h), we
    //should temporily disable USB interrupts, to avoid any possibility of both 
    //the USB stack and this MSD handler from modifying the same BDT entry, or
    //MSD state machine variables (ex: in the case of MSD_RESET) at the same time.
    USBMaskInterrupts();
    
    //Main MSD task dispatcher.  Receives MSD Command Block Wrappers (CBW) and
    //dispatches appropriate lower level handlers to service the requests.
    switch(MSD_State)
    {
        case MSD_WAIT: //idle state, when we are waiting for a command from the host
        {
            //Check if we have received a new command block wrapper (CBW)
            if(!USBHandleBusy(USBMSDOutHandle))
            {
        		//If we are in the MSD_WAIT state, and we received an OUT transaction
        		//on the MSD OUT endpoint, then we must have just received an MSD
        		//Command Block Wrapper (CBW).
        		//First copy the the received data to to the gblCBW structure, so
        		//that we keep track of the command, but free up the MSD OUT endpoint
        		//buffer for fulfilling whatever request may have been received.
                //gblCBW = msd_cbw; //we are doing this, but below method can yeild smaller code size
            	for(i = 0; i < MSD_CBW_SIZE; i++)
                {
                	*((BYTE*)&gblCBW.dCBWSignature + i) = *((BYTE*)&msd_cbw.dCBWSignature + i);
                }   	

        	    //If this CBW is valid?
        		if((USBHandleGetLength(USBMSDOutHandle) == MSD_CBW_SIZE) && (gblCBW.dCBWSignature == MSD_VALID_CBW_SIGNATURE)) 
            	{
                	//The CBW was valid, set flag meaning any stalls after this point 
                	//should not be "persistent" (as in the case of non-valid CBWs).
                	MSDCBWValid = TRUE; 

                    //Is this CBW meaningful?	
       				if((gblCBW.bCBWLUN <= MAX_LUN)                                      //Verify the command is addressed to a supported LUN
               		&&(gblCBW.bCBWCBLength <= MSD_MAX_CB_SIZE)                          //Verify the claimed CB length is reasonable/valid
               		&&(gblCBW.bCBWCBLength >= 0x01)                                     //Verify the claimed CB length is reasonable/valid
               		&&((gblCBW.bCBWFlags & MSD_CBWFLAGS_RESERVED_BITS_MASK) == 0x00))   //Verify reserved bits are clear
            		{
                		
                		//The CBW was both valid and meaningful.
                		//Begin preparing a valid Command Status Wrapper (CSW), 
                		//in anticipation of completing the request successfully.
                		//If an error detected is later, we will change the status 
                		//before sending the CSW.
                    	msd_csw.dCSWSignature = MSD_VALID_CSW_SIGNATURE;
                    	msd_csw.dCSWTag = gblCBW.dCBWTag;
                    	msd_csw.dCSWDataResidue = 0x0;
                    	msd_csw.bCSWStatus = MSD_CSW_COMMAND_PASSED;	
                    	
                    	//Since a new CBW just arrived, we should re-init the
                    	//lower level state machines to their default states.  
                    	//Even if the prior operation didn't fully complete 
                    	//normally, we should abandon the prior operation, when
                    	//a new CBW arrives.
                    	MSDCommandState = MSD_COMMAND_WAIT;
                    	MSDReadState = MSD_READ10_WAIT;
                    	MSDWriteState = MSD_WRITE10_WAIT;

                    	//Keep track of retry attempts, in case of temporary 
                    	//failures during read or write of the media.
                    	MSDRetryAttempt = 0;
                    	
                    	//Check the command.  With the exception of the REQUEST_SENSE
                    	//command, we should reset the sense key info for each new command block.
                    	//Assume the command will get processed successfully (and hence "NO SENSE" 
                    	//response, which is used for success cases), unless handler code
                    	//later on detects some kind of error.  If it does, it should
                    	//update the sense keys to reflect the type of error detected,
                    	//prior to sending the CSW.
                    	if(gblCBW.CBWCB[0] != MSD_REQUEST_SENSE)
                    	{
                      		gblSenseData[LUN_INDEX].SenseKey=S_NO_SENSE;
        			        gblSenseData[LUN_INDEX].ASC=ASC_NO_ADDITIONAL_SENSE_INFORMATION;
        			        gblSenseData[LUN_INDEX].ASCQ=ASCQ_NO_ADDITIONAL_SENSE_INFORMATION;
    			        }

                    	//Isolate the data direction bit.  The direction bit is bit 7 of the bCBWFlags byte.
	                   	//Then, based on the direction of the data transfer, prepare the MSD state machine
                    	//so it knows how to proceed with processing the request.
                        //If bit7 = 0, then direction is OUT from host.  If bit7 = 1, direction is IN to host
        				if (gblCBW.bCBWFlags & MSD_CBW_DIRECTION_BITMASK)
        				{
        					    MSD_State = MSD_DATA_IN;
        				}
        				else //else direction must be OUT from host
            			{
            			    MSD_State = MSD_DATA_OUT;
        				}  

        				//Determine if the host is expecting there to be data transfer or not.
        				//Doing this now will make for quicker error checking later.
        				if(gblCBW.dCBWDataTransferLength != 0)		
                    	{
	                    	MSDHostNoData = FALSE;
	                    }
	                    else
	                    {
		                    MSDHostNoData = TRUE;
		                }    	
		                
                        //Copy the received command to the lower level command 
                        //state machine, so it knows what to do.
                        MSDCommandState = gblCBW.CBWCB[0];  
        			}
        			else
        			{
            			//else the CBW wasn't meaningful.  Section 6.4 of BOT specs v1.0 says,
            			//"The response of a device to a CBW that is not meaningful is not specified."
            			//Lets STALL the bulk endpoints, so as to promote the possibility of recovery.
            			USBStallEndpoint(MSD_DATA_IN_EP, IN_TO_HOST);
              			USBStallEndpoint(MSD_DATA_OUT_EP, OUT_FROM_HOST);  
                    } 
        		}//end of: if((USBHandleGetLength(USBMSDOutHandle) == MSD_CBW_SIZE) && (gblCBW.dCBWSignature == MSD_VALID_CBW_SIGNATURE))
        		else  //The CBW was not valid. 
        		{
            		//Section 6.6.1 of the BOT specifications rev. 1.0 says the device shall STALL bulk IN and OUT
	        		//endpoints (or should discard OUT data if not stalled), and should stay in this state until a 
	        		//"Reset Recovery" (MSD Reset + clear endpoint halt commands on EP0, see section 5.3.4)
          			USBStallEndpoint(MSD_DATA_IN_EP, IN_TO_HOST);
          			USBStallEndpoint(MSD_DATA_OUT_EP, OUT_FROM_HOST);  
          			MSDCBWValid = FALSE;    //Flag so as to enable a "persistent" 
          			//stall (cannot be cleared by clear endpoint halt, unless preceded
          			//by an MSD reset).
        		}
            }//if(!USBHandleBusy(USBMSDOutHandle))
            break;
        }//end of: case MSD_WAIT:
        case MSD_DATA_IN:
            if(MSDProcessCommand() == MSD_COMMAND_WAIT)
            {
                // Done processing the command, send the status
                MSD_State = MSD_SEND_CSW;
            }
            break;
        case MSD_DATA_OUT:
            if(MSDProcessCommand() == MSD_COMMAND_WAIT)
            {
    			/* Finished receiving the data prepare and send the status */
    		  	if ((msd_csw.bCSWStatus == MSD_CSW_COMMAND_PASSED)&&(msd_csw.dCSWDataResidue!=0)) 
    		  	{
    		  		msd_csw.bCSWStatus = MSD_CSW_PHASE_ERROR;
    		    }
                MSD_State = MSD_SEND_CSW;
            }
            break;
        case MSD_SEND_CSW:
            //Check to make sure the bulk IN endpoint is available before sending CSW.
            //The endpoint might still be busy sending the last packet on the IN endpoint.
            if(USBHandleBusy(USBGetNextHandle(MSD_DATA_IN_EP, IN_TO_HOST)) == TRUE)
            {
                break;  //Not available yet.  Just stay in this state and try again later.
            }
            
            //Send the Command Status Wrapper (CSW) packet            
            USBMSDInHandle = USBTxOnePacket(MSD_DATA_IN_EP,(BYTE*)&msd_csw,MSD_CSW_SIZE);
            //If the bulk OUT endpoint isn't already armed, make sure to do so 
            //now so we can receive the next CBW packet from the host.
            if(!USBHandleBusy(USBMSDOutHandle))
            {
                USBMSDOutHandle = USBRxOnePacket(MSD_DATA_OUT_EP,(BYTE*)&msd_cbw,MSD_OUT_EP_SIZE);
            }
           	MSD_State=MSD_WAIT;
            break;
        default:
            //Illegal condition that should not happen, but might occur if the
            //device firmware incorrectly calls MSDTasks() prior to calling
            //USBMSDInit() during the set-configuration portion of enumeration.
            MSD_State=MSD_WAIT;
			msd_csw.bCSWStatus = MSD_CSW_PHASE_ERROR;
			USBStallEndpoint(MSD_DATA_OUT_EP, OUT_FROM_HOST);          
    }//switch(MSD_State)
    
    //Safe to re-enable USB interrupts now.
    USBUnmaskInterrupts();
    
    return MSD_State;
}


/******************************************************************************
 	Function:
 		BYTE MSDProcessCommand(void)
 		
 	Description:
 		This funtion processes a command received through the MSD
 		class driver
 		
 	PreCondition:
 		None
 		
 	Paramters:
 		None
 		
 	Return Values:
 		BYTE - the current state of the MSDProcessCommand state
 		machine.  The valid values are defined in MSD.h under the
 		MSDProcessCommand state machine declaration section
 		
 	Remarks:
 		None
 
 *****************************************************************************/	
BYTE MSDProcessCommand(void)
{   
  	//Check if the media is either not present, or has been flagged by firmware
  	//to pretend to be non-present (ex: SoftDetached).
  	if((LUNMediaDetect() == FALSE) || (SoftDetach[gblCBW.bCBWLUN] == TRUE))
    {
        //Clear flag so we know the media need initialization, if it becomes 
        //present in the future.
        gblMediaPresent &= ~((WORD)1<<gblCBW.bCBWLUN);
        MSDProcessCommandMediaAbsent();
   	}
    else
    {
        //Check if the media is present and hasn't been already flagged as initialized.
        if((gblMediaPresent & ((WORD)1<<gblCBW.bCBWLUN)) == 0)
        {
            //Try to initialize the media
            if(LUNMediaInitialize())
            {
                //The media initialized successfully.  Set flag letting software
                //know that it doesn't need re-initialization again (unless the 
                //media is removable and is subsequently removed and re-inserted). 
                gblMediaPresent |= ((WORD)1<<gblCBW.bCBWLUN);

                //The media is present and has initialized successfully.  However,
                //we should still notify the host that the media may have changed,
                //from the host's perspective, since we just initialized it for 
                //the first time.         
        		gblSenseData[LUN_INDEX].SenseKey = S_UNIT_ATTENTION;
        		gblSenseData[LUN_INDEX].ASC = ASC_NOT_READY_TO_READY_CHANGE;
        		gblSenseData[LUN_INDEX].ASCQ = ASCQ_MEDIUM_MAY_HAVE_CHANGED;
                //Signify a soft error to the host, so it knows to check the 
                //sense keys to learn that the media just changed.
                msd_csw.bCSWStatus = MSD_CSW_COMMAND_FAILED; //No real "error" per se has occurred
                //Process the command now.
                MSDProcessCommandMediaPresent();
            }
            else
            {
                //The media failed to initialize for some reason.
                MSDProcessCommandMediaAbsent();
            }
        }
        else
        {
            //The media was present and was already initialized/ready to process
            //the host's command.
            MSDProcessCommandMediaPresent();
        }
    }

    return MSDCommandState;
}

/******************************************************************************
 	Function:
 		void MSDProcessCommandMediaAbsent(void)
 		
 	Description:
 		This funtion processes a command received through the MSD
 		class driver, when the removable MSD media (ex: MMC/SD card) is not 
 		present, or has been "soft detached" deliberately by the application
 		firmware.
 		
 	PreCondition:
 		The MSD function should have already been initialized (the media isn't
 		required to be initalized however).  Additionally, a valid MSD Command 
 		Block Wrapper (CBW) should have been received and partially parsed 
 		prior to calling this function.
 		
 	Parameters:
 		None
 	
 	Return Values:
 		BYTE - the current state of the MSDProcessCommand state
 		machine.  The valid values are defined in usb_function_msd.h under the 
 		MSDProcessCommand state machine declaration section
 		
 	Remarks:
 		None
 
  *****************************************************************************/	
void MSDProcessCommandMediaAbsent(void)
{
    //Check what command we are currently processing, to decide how to handle it.
    switch(MSDCommandState)
    {
        case MSD_REQUEST_SENSE:
            //The host sends this request when it wants to check the status of 
            //the device, and/or identify the reason for the last error that was 
            //reported by the device.
            //Set the sense keys to let the host know that the reason the last
            //command failed was because the media was not present.
            ResetSenseData();
            gblSenseData[LUN_INDEX].SenseKey=S_NOT_READY;
    		gblSenseData[LUN_INDEX].ASC=ASC_MEDIUM_NOT_PRESENT;
    		gblSenseData[LUN_INDEX].ASCQ=ASCQ_MEDIUM_NOT_PRESENT;

            //After initializing the sense keys above, the subsequent handling 
            //code for this state is the same with or without media.
            //Therefore, to save code size, we just call the media present handler.
            MSDProcessCommandMediaPresent();
            break;
        case MSD_PREVENT_ALLOW_MEDIUM_REMOVAL:
        case MSD_TEST_UNIT_READY:
            //The host will typically periodically poll the device by sending this
            //request.  Since this is a removable media device, and the media isn't
            //present, we need to indicate an error to let the host know (to 
            //check the sense keys, which will tell it the media isn't present).
    		msd_csw.bCSWStatus = MSD_CSW_COMMAND_FAILED;
            MSDCommandState = MSD_COMMAND_WAIT;
            break;
        case MSD_INQUIRY:
            //The handling code for this state is the same with or without media.
            //Therefore, to save code size, we just call the media present handler.
            MSDProcessCommandMediaPresent();
            break;
        case MSD_COMMAND_RESPONSE:
            //The handling code for this state is the same with or without media.
            //Therefore, to save code size, we just call the media present handler.
            MSDProcessCommandMediaPresent();
            break;            
        default:
		    //An unsupported command was received.  Since we are uncertain how 
		    //many bytes we should send/or receive, we should set sense key data 
		    //and then STALL, to force the host to perform error recovery.
		    MSDErrorHandler(MSD_ERROR_UNSUPPORTED_COMMAND);
            break;
    }
}//void MSDProcessCommandMediaAbsent(void)


/******************************************************************************
 	Function:
 		void MSDProcessCommandMediaPresent(void)
 		
 	Description:
 		This funtion processes a command received through the MSD
 		class driver
 		
 	PreCondition:
 		None
 		
 	Paramters:
 		None
 	
 	Return Values:
 		BYTE - the current state of the MSDProcessCommand state
 		machine.  The valid values are defined in MSD.h under the 
 		MSDProcessCommand state machine declaration section
 		
 	Remarks:
 		None
 
 *****************************************************************************/	
void MSDProcessCommandMediaPresent(void)
{
    BYTE i; 
    BYTE NumBytesInPacket;

    //Check what command we are currently processing, to decide how to handle it.
    switch(MSDCommandState)
    {
		case MSD_READ_10:
		    //The host issues a "Read 10" request when it wants to read some number
		    //of 10-bit length blocks (512 byte blocks) of data from the media.
		    //Since this is a common request and is part of the "critical path"
		    //performance wise, we put this at the top of the state machine checks.
        	if(MSDReadHandler() == MSD_READ10_WAIT)
        	{
			    MSDCommandState = MSD_COMMAND_WAIT;
            }
            break;
    	case MSD_WRITE_10:
		    //The host issues a "Write 10" request when it wants to write some number
		    //of 10-bit length blocks (512 byte blocks) of data to the media.
		    //Since this is a common request and is part of the "critical path"
		    //performance wise, we put this near the top of the state machine checks.
        	if(MSDWriteHandler() == MSD_WRITE10_WAIT)
        	{
			    MSDCommandState = MSD_COMMAND_WAIT;
            }
		    break;
    	case MSD_INQUIRY:
    	{
        	//The host wants to learn more about our MSD device (spec version, 
        	//supported abilities, etc.)
        	
            //Error check: If host doesn't want any data, then just advance to CSW phase.
            if(MSDHostNoData == TRUE)
            {
                MSDCommandState = MSD_COMMAND_WAIT;
                break;
            }    
            
            //Get the 16-bit "Allocation Length" (Number of bytes to respond 
            //with.  Note: Value provided in CBWCB is in big endian format)
            TransferLength.byte.HB = gblCBW.CBWCB[3]; //MSB
            TransferLength.byte.LB = gblCBW.CBWCB[4]; //LSB
        	//Check for possible errors.  
            if(MSDCheckForErrorCases(TransferLength.Val) != MSD_ERROR_CASE_NO_ERROR)
            {
                break;
            }

          	//Compute and load proper csw residue and device in number of byte.
            MSDComputeDeviceInAndResidue(sizeof(InquiryResponse));

            //If we get to here, this implies no errors were found and the command is legit.

            //copy the inquiry results from the defined ROM buffer 
            //  into the USB buffer so that it can be transmitted
        	memcpypgm2ram((void *)&msd_buffer[0], (ROM void*)&inq_resp, sizeof(InquiryResponse));   //Inquiry response is 36 bytes total
        	MSDCommandState = MSD_COMMAND_RESPONSE;
            break;
        }
        case MSD_READ_CAPACITY:
        {
            //The host asked for the total capacity of the device.  The response
            //packet is 8-bytes (32-bits for last LBA implemented, 32-bits for block size).
            DWORD_VAL sectorSize;
            DWORD_VAL capacity;

            //get the information from the physical media
            capacity.Val = LUNReadCapacity();
            sectorSize.Val = LUNReadSectorSize();
            
            //Copy the data to the buffer.  Host expects the response in big endian format.
        	msd_buffer[0]=capacity.v[3];
        	msd_buffer[1]=capacity.v[2];
        	msd_buffer[2]=capacity.v[1];
        	msd_buffer[3]=capacity.v[0];
        	
        	msd_buffer[4]=sectorSize.v[3];
        	msd_buffer[5]=sectorSize.v[2];
        	msd_buffer[6]=sectorSize.v[1];
        	msd_buffer[7]=sectorSize.v[0];

          	//Compute and load proper csw residue and device in number of byte.
            TransferLength.Val = 0x08;      //READ_CAPACITY always has an 8-byte response.
            MSDComputeDeviceInAndResidue(0x08);
        
        	MSDCommandState = MSD_COMMAND_RESPONSE;
            break;
        }
        case MSD_REQUEST_SENSE:    
            //The host normally sends this request after a CSW completed, where
            //the device indicated some kind of error on the previous transfer.
            //In this case, the host will typically issue this request, so it can
            //learn more details about the cause/source of the error condition.
            
            //Error check: if the host doesn't want any data, just advance to CSW phase.
            if(MSDHostNoData == TRUE)
            {
                MSDCommandState = MSD_COMMAND_WAIT;
                break;
            }    

          	//Compute and load proper csw residue and device in number of byte.
            TransferLength.Val = sizeof(RequestSenseResponse);      //REQUEST_SENSE has an 18-byte response.
            MSDComputeDeviceInAndResidue(sizeof(RequestSenseResponse));
             
            //Copy the requested response data from flash to the USB ram buffer.
          	for(i=0;i<sizeof(RequestSenseResponse);i++)
          	{
          		msd_buffer[i]=gblSenseData[LUN_INDEX]._byte[i];
            }
          	MSDCommandState = MSD_COMMAND_RESPONSE;
            break;
	    case MSD_MODE_SENSE:
        	msd_buffer[0]=0x03;
        	msd_buffer[1]=0x00;
        	msd_buffer[2]=(LUNWriteProtectState()) ? 0x80 : 0x00;
        	msd_buffer[3]= 0x00;
 
           	//Compute and load proper csw residue and device in number of byte.
            TransferLength.Val = 0x04;      
            MSDComputeDeviceInAndResidue(0x04);
        	MSDCommandState = MSD_COMMAND_RESPONSE;
    	    break;
		case MSD_PREVENT_ALLOW_MEDIUM_REMOVAL:
        	gblSenseData[LUN_INDEX].SenseKey=S_ILLEGAL_REQUEST;
        	gblSenseData[LUN_INDEX].ASC=ASC_INVALID_COMMAND_OPCODE;
        	gblSenseData[LUN_INDEX].ASCQ=ASCQ_INVALID_COMMAND_OPCODE;
        	msd_csw.bCSWStatus = MSD_CSW_COMMAND_FAILED;
        	msd_csw.dCSWDataResidue = 0x00;
			MSDCommandState = MSD_COMMAND_WAIT;
            break;
		case MSD_TEST_UNIT_READY:
            //The host will typically send this command periodically to check if
            //it is ready to be used and to obtain polled notification of changes
            //in status (ex: user removed media from a removable media MSD volume).
            //There is no data stage for this request.  The information we send to
            //the host in response to this request is entirely contained in the CSW.
            
            //First check for possible errors.
            if(MSDCheckForErrorCases(0) != MSD_ERROR_CASE_NO_ERROR)
            {
                break;
            }    
            //The stack sets this condition when the status of the removable media
            //has just changed (ex: the user just plugged in the removable media,
            //in which case we want to notify the host of the changed status, by
            //sending a deliberate "error" notification).  This doesn't mean any 
            //real error has occurred.
            if((gblSenseData[LUN_INDEX].SenseKey==S_UNIT_ATTENTION) && (msd_csw.bCSWStatus==MSD_CSW_COMMAND_FAILED))
            {
                MSDCommandState = MSD_COMMAND_WAIT;
            }
            else
            {
            	ResetSenseData();
            	msd_csw.dCSWDataResidue=0x00;
    			MSDCommandState = MSD_COMMAND_WAIT;
            }
            break;
		case MSD_VERIFY:
            //Fall through to STOP_START
		case MSD_STOP_START:
        	msd_csw.dCSWDataResidue=0x00;
			MSDCommandState = MSD_COMMAND_WAIT;
            break;
        case MSD_COMMAND_RESPONSE:
            //This command state didn't originate from the host.  This state was
            //set by the firmware (for one of the other handlers) when it was 
            //finished preparing the data to send to the host, and it is now time
            //to transmit the data over the bulk IN endpoint.
            if(USBHandleBusy(USBMSDInHandle) == FALSE)
            {
                //We still have more bytes needing to be sent.  Compute how many 
                //bytes should be in the next IN packet that we send.
                if(gblCBW.dCBWDataTransferLength >= MSD_IN_EP_SIZE)
                {
                    NumBytesInPacket = MSD_IN_EP_SIZE;
                    gblCBW.dCBWDataTransferLength -= MSD_IN_EP_SIZE;
                }   
                else
                {
                    //This is a short packet and will be our last IN packet sent
                    //in the transfer.
                    NumBytesInPacket = gblCBW.dCBWDataTransferLength;
                    gblCBW.dCBWDataTransferLength = 0;
                } 
                
                //We still have more bytes needing to be sent.  Check if we have
                //already fulfilled the device input expected quantity of bytes.
                //If so, we need to keep sending IN packets, but pad the extra
                //bytes with value = 0x00 (see error case 5 MSD device BOT v1.0 
                //spec handling).
                if(TransferLength.Val >= NumBytesInPacket)
                {
                    //No problem, just send the requested data and keep track of remaining count.
                    TransferLength.Val -= NumBytesInPacket;
                }    
                else
                {
                    //The host is reading more bytes than the device has to send.
                    //In this case, we still need to send the quantity of bytes requested,
                    //but we have to fill the pad bytes with 0x00.  The below for loop
                    //is execution speed inefficient, but performance isn't important 
                    //since this code only executes in the case of a host error 
                    //anyway (Hi > Di).
                    for(i = 0; i < NumBytesInPacket; i++)
                    {
                        if(TransferLength.Val != 0)
                        {
                            TransferLength.Val--;     
                        }    
                        else
                        {
                            msd_buffer[i] = 0x00;
                        }    
                    }    
                }    
                
                //We are now ready to send the packet to the host.                   
                USBMSDInHandle = USBTxOnePacket(MSD_DATA_IN_EP,(BYTE*)&msd_buffer[0],NumBytesInPacket);
                
                //Check to see if we are done sending all requested bytes of data
                if(gblCBW.dCBWDataTransferLength == 0)
                {
                    //We have sent all the requested bytes.  Go ahead and
                    //advance state so as to send the CSW.
                    MSDCommandState = MSD_COMMAND_WAIT;
                    break;                    
                }                    
            }
            break;
        case MSD_COMMAND_ERROR:
		default:
		    //An unsupported command was received.  Since we are uncertain how many
		    //bytes we should send/or receive, we should set sense key data and then
		    //STALL, to force the host to perform error recovery.
		    MSDErrorHandler(MSD_ERROR_UNSUPPORTED_COMMAND);
 		    break;
	} // end switch	
}//void MSDProcessCommandMediaPresent(void)


/******************************************************************************
 	Function:
 		static void MSDComputeDeviceInAndResidue(WORD DiExpected)
 		
 	Description:
 		This is a private function that performs Hi > Di data size checking
 		and handling.  This function also computes the proper CSW data residue
 		and updates the global variable.
 		
 	PreCondition:
 		Should only be called in the context of the 
 		MSDProcessCommandMediaPresent() handler function, after receiving a new
 		command that needs processing.  Before calling this function, make sure
 		the gblCBW.dCBWDataTransferLength and TransferLength.Val variables have
 		been pre-loaded with the expected host and device data size values.
 		
 	Parameters:
 		WORD DiExpected - Input: Firmware can specify an addional value that 
 		might be smaller than the TransferLength.Val value.  The function will
 		update TransferLength.Val with the smaller of the original value, or
 		DiExpected.
 		
 	Return Values:
 		None
 		
 	Remarks:
 		None
 
  *****************************************************************************/
static void MSDComputeDeviceInAndResidue(WORD DiExpected)
{
  	//Error check number of bytes to send.  Check for Hi < Di
  	if(gblCBW.dCBWDataTransferLength < DiExpected)
  	{
      	//The host has requested less data than the entire reponse.  We
      	//send only the host requested quantity of bytes.
      	msd_csw.dCSWDataResidue = 0;
      	TransferLength.Val = gblCBW.dCBWDataTransferLength;
    }   	
    else
    {
        //The host requested greater than or equal to the number of bytes expected.
        if(DiExpected < TransferLength.Val)
        {
            TransferLength.Val = DiExpected;
        }    
        msd_csw.dCSWDataResidue = gblCBW.dCBWDataTransferLength - TransferLength.Val;
    }     
}    


/******************************************************************************
 	Function:
 		BYTE MSDReadHandler(void)
 		
 	Description:
 		This funtion processes a read command received through 
 		the MSD class driver
 		
 	PreCondition:
 		None
 		
 	Parameters:
 		None
 		
 	Return Values:
 		BYTE - the current state of the MSDReadHandler state
 		machine.  The valid values are defined in MSD.h under the 
 		MSDReadHandler state machine declaration section
 		
 	Remarks:
 		None
 
  *****************************************************************************/
BYTE MSDReadHandler(void)
{
    switch(MSDReadState)
    {
        case MSD_READ10_WAIT:
            //Extract the LBA from the CBW.  Note: Also need to perform endian 
            //swap, since the multi-byte CBW fields are stored big endian, but 
            //the Microchip C compilers are little endian.
        	LBA.v[3]=gblCBW.CBWCB[2];
        	LBA.v[2]=gblCBW.CBWCB[3];
        	LBA.v[1]=gblCBW.CBWCB[4];
        	LBA.v[0]=gblCBW.CBWCB[5];
        	
        	TransferLength.byte.HB = gblCBW.CBWCB[7];   //MSB of Transfer Length (in number of blocks, not bytes)
        	TransferLength.byte.LB = gblCBW.CBWCB[8];   //LSB of Transfer Length (in number of blocks, not bytes)

            //Check for possible error cases before proceeding
            if(MSDCheckForErrorCases(TransferLength.Val * (DWORD)MEDIA_SECTOR_SIZE) != MSD_ERROR_CASE_NO_ERROR)
            {
                break;
            }    

            MSDReadState = MSD_READ10_BLOCK;
            //Fall through to MSD_READ_BLOCK
        case MSD_READ10_BLOCK:
            if(TransferLength.Val == 0)
            {
                MSDReadState = MSD_READ10_WAIT;
                break;
            }
            
            TransferLength.Val--;					// we have read 1 LBA
            MSDReadState = MSD_READ10_SECTOR;
            //Fall through to MSD_READ10_SECTOR
        case MSD_READ10_SECTOR:
            //if the old data isn't completely sent yet
            if(USBHandleBusy(USBMSDInHandle) != 0)
            {
                break;
            }
            
            //Try to read a sector worth of data from the media, but check for
            //possible errors.
    		if(LUNSectorRead(LBA.Val, (BYTE*)&msd_buffer[0]) != TRUE)
    		{
				if(MSDRetryAttempt < MSD_FAILED_READ_MAX_ATTEMPTS)
				{
				    MSDRetryAttempt++;
                    break;
				}
				else
				{  
    				//Too many consecutive failed reads have occurred.  Need to
    				//give up and abandon the sector read attempt; something must
    				//be wrong and we don't want to get stuck in an infinite loop.
    				//Need to indicate to the host that a device error occurred.
    				//However, we can't send the CSW immediately, since the host
    				//still expects to receive sector read data on the IN endpoint 
    				//first.  Therefore, we still send dummy bytes, before
    				//we send the CSW with the failed status in it.
    				msd_csw.bCSWStatus=0x02;		// Indicate phase error 0x02 
													// (option #1 from BOT section 6.6.2)
                    //Set error status sense keys, so the host can check them later
                    //to determine how to proceed.
                    gblSenseData[LUN_INDEX].SenseKey=S_MEDIUM_ERROR;
			        gblSenseData[LUN_INDEX].ASC=ASC_NO_ADDITIONAL_SENSE_INFORMATION;
			        gblSenseData[LUN_INDEX].ASCQ=ASCQ_NO_ADDITIONAL_SENSE_INFORMATION;
					USBStallEndpoint(MSD_DATA_IN_EP, IN_TO_HOST);
					MSDReadState = MSD_READ10_WAIT;
					break;
                }
            }//else we successfully read a sector worth of data from our media

            LBA.Val++;
			msd_csw.dCSWDataResidue=BLOCKLEN_512;//in order to send the
                                                 //512 bytes of data read
                                                 
            ptrNextData=(BYTE *)&msd_buffer[0];
            
            MSDReadState = MSD_READ10_TX_SECTOR;
            //Fall through to MSD_READ10_TX_SECTOR
        case MSD_READ10_TX_SECTOR:
            if(msd_csw.dCSWDataResidue == 0)
            {
                MSDReadState = MSD_READ10_BLOCK;
                break;
            }
            
            MSDReadState = MSD_READ10_TX_PACKET;
            //Fall through to MSD_READ10_TX_PACKET
            
        case MSD_READ10_TX_PACKET:
    		/* Write next chunk of data to EP Buffer and send */
            
            //Make sure the endpoint is available before using it.
            if(USBHandleBusy(USBMSDInHandle))
            {
                break;
            }
            //Prepare the USB module to send an IN transaction worth of data to the host.
            USBMSDInHandle = USBTxOnePacket(MSD_DATA_IN_EP,ptrNextData,MSD_IN_EP_SIZE);
            
 			MSDReadState = MSD_READ10_TX_SECTOR;

    		gblCBW.dCBWDataTransferLength-=	MSD_IN_EP_SIZE;
    		msd_csw.dCSWDataResidue-=MSD_IN_EP_SIZE;
    		ptrNextData+=MSD_IN_EP_SIZE;
            break;
        
        default:
            //Illegal condition, should never occur.  In the event that it ever
            //did occur anyway, try to notify the host of the error.
            msd_csw.bCSWStatus=0x02;  //indicate "Phase Error"
			USBStallEndpoint(MSD_DATA_IN_EP, IN_TO_HOST);
            //Advance state machine
            MSDReadState = MSD_READ10_WAIT;
    }//switch(MSDReadState)
    
    return MSDReadState;
}


/******************************************************************************
 	Function:
 		BYTE MSDWriteHandler(void)
 		
 	Description:
 		This funtion processes a write command received through 
 		the MSD class driver
 		
 	PreCondition:
 		None
 		
 	Parameters:
 		None
 		
 	Return Values:
 		BYTE - the current state of the MSDWriteHandler state
 		machine.  The valid values are defined in MSD.h under the 
 		MSDWriteHandler state machine declaration section
 		
 	Remarks:
 		None
 
 *****************************************************************************/
BYTE MSDWriteHandler(void)
{
    switch(MSDWriteState)
    {
        case MSD_WRITE10_WAIT:
         	/* Read the LBA, TransferLength fields from Command Block
               NOTE: CB is Big-Endian */
        	LBA.v[3]=gblCBW.CBWCB[2];
        	LBA.v[2]=gblCBW.CBWCB[3];
        	LBA.v[1]=gblCBW.CBWCB[4];
        	LBA.v[0]=gblCBW.CBWCB[5];
        	TransferLength.v[1]=gblCBW.CBWCB[7];
        	TransferLength.v[0]=gblCBW.CBWCB[8];

            //Do some error case checking.
            if(MSDCheckForErrorCases(TransferLength.Val * (DWORD)MEDIA_SECTOR_SIZE) != MSD_ERROR_CASE_NO_ERROR)
            {
                //An error was detected.  The MSDCheckForErrorCases() function will
                //have taken care of setting the proper states to report the error to the host.
                break;
            }    
      		//Check if the media is write protected before deciding what
      		//to do with the data.
      		if(LUNWriteProtectState()) 
            {
                //The media appears to be write protected.
          	    //Let host know error occurred.  The bCSWStatus flag is also used by
          	    //the write handler, to know not to even attempt the write sequence.
          	    msd_csw.bCSWStatus = MSD_CSW_COMMAND_FAILED;    

                //Set sense keys so the host knows what caused the error.
          	    gblSenseData[LUN_INDEX].SenseKey=S_DATA_PROTECT;
          	    gblSenseData[LUN_INDEX].ASC=ASC_WRITE_PROTECTED;
          	    gblSenseData[LUN_INDEX].ASCQ=ASCQ_WRITE_PROTECTED;

                //Stall the OUT endpoint, so as to promptly inform the host
                //that the data cannot be accepted, due to write protected media.
          		USBStallEndpoint(MSD_DATA_OUT_EP, OUT_FROM_HOST);
          		MSDWriteState = MSD_WRITE10_WAIT;
          	    return MSDWriteState;
          	}
        	
        	MSD_State = MSD_WRITE10_BLOCK;
        	//Fall through to MSD_WRITE10_BLOCK
        case MSD_WRITE10_BLOCK:
            if(TransferLength.Val == 0)
            {
                MSDWriteState = MSD_WRITE10_WAIT;
                break;
            }
            
            MSDWriteState = MSD_WRITE10_RX_SECTOR;
            ptrNextData=(BYTE *)&msd_buffer[0];
              
        	msd_csw.dCSWDataResidue=BLOCKLEN_512;
        	
            //Fall through to MSD_WRITE10_RX_SECTOR
        case MSD_WRITE10_RX_SECTOR:
        {
      		/* Read 512B into msd_buffer*/
      		if(msd_csw.dCSWDataResidue>0) 
      		{
                if(USBHandleBusy(USBMSDOutHandle) == TRUE)
                {
                    break;
                }

                USBMSDOutHandle = USBRxOnePacket(MSD_DATA_OUT_EP,ptrNextData,MSD_OUT_EP_SIZE);
                MSDWriteState = MSD_WRITE10_RX_PACKET;
                //Fall through to MSD_WRITE10_RX_PACKET
      	    }
      	    else
      	    {
          		//We finished receiving a sector worth of data from the host.
          		//Check if the media is write protected before deciding what
          		//to do with the data.
          		if(LUNWriteProtectState()) 
                {
                    //The device appears to be write protected.
              	    //Let host know error occurred.  The bCSWStatus flag is also used by
              	    //the write handler, to know not to even attempt the write sequence.
              	    msd_csw.bCSWStatus=0x01;    
              	    
                    //Set sense keys so the host knows what caused the error.
              	    gblSenseData[LUN_INDEX].SenseKey=S_NOT_READY;
              	    gblSenseData[LUN_INDEX].ASC=ASC_WRITE_PROTECTED;
              	    gblSenseData[LUN_INDEX].ASCQ=ASCQ_WRITE_PROTECTED;
              	}
   			    MSDWriteState = MSD_WRITE10_SECTOR;     
      			break;
          	}
        }
        //Fall through to MSD_WRITE10_RX_PACKET
        case MSD_WRITE10_RX_PACKET:
            if(USBHandleBusy(USBMSDOutHandle) == TRUE)
            {
                break;
            }
            
        	gblCBW.dCBWDataTransferLength-=USBHandleGetLength(USBMSDOutHandle);		// 64B read
        	msd_csw.dCSWDataResidue-=USBHandleGetLength(USBMSDOutHandle);
            ptrNextData += MSD_OUT_EP_SIZE;
            
            MSDWriteState = MSD_WRITE10_RX_SECTOR;
            break;
        case MSD_WRITE10_SECTOR:
        {
            //Make sure that no error has been detected, before performing the write
            //operation.  If there was an error, skip the write operation, but allow
            //the TransferLength to continue decrementing, so that we can eventually
            //receive all OUT bytes that the host is planning on sending us.  Only
            //after that is complete will the host send the IN token for the CSW packet,
            //which will contain the bCSWStatus letting it know an error occurred.
      		if(msd_csw.bCSWStatus == 0x00)
      		{
          		if(LUNSectorWrite(LBA.Val, (BYTE*)&msd_buffer[0], (LBA.Val==0)?TRUE:FALSE) != TRUE)
          		{
              		//The write operation failed for some reason.  Keep track of retry
              		//attempts and abort if repeated write attempts also fail.
    				if(MSDRetryAttempt < MSD_FAILED_WRITE_MAX_ATTEMPTS)
    				{
    				    MSDRetryAttempt++;
                        break;
    				}
    				else
    				{  
        				//Too many consecutive failed write attempts have occurred. 
        				//Need to give up and abandon the write attempt.
        				msd_csw.bCSWStatus = MSD_CSW_COMMAND_FAILED; //Indicate error during CSW phase
						//Set error status sense keys, so the host can check them later
                        //to determine how to proceed.
                        gblSenseData[LUN_INDEX].SenseKey=S_MEDIUM_ERROR;
    			        gblSenseData[LUN_INDEX].ASC=ASC_NO_ADDITIONAL_SENSE_INFORMATION;
    			        gblSenseData[LUN_INDEX].ASCQ=ASCQ_NO_ADDITIONAL_SENSE_INFORMATION;
                    }              		
          		}
      		}
      
            //One LBA is written (unless an error occurred).  Advance state
            //variables so we can eventually finish handling the CBW request.
      		LBA.Val++;				
      		TransferLength.Val--;      
            MSDWriteState = MSD_WRITE10_BLOCK;
            break;
        } 
        
        default:
            //Illegal condition which should not occur.  If for some reason it
            //does, try to let the host know know an error has occurred.
            msd_csw.bCSWStatus=0x02;    //Phase Error
			USBStallEndpoint(MSD_DATA_OUT_EP, OUT_FROM_HOST);
            MSDWriteState = MSD_WRITE10_WAIT;            
    }
    
    return MSDWriteState;
}



/******************************************************************************
 	Function:
 		void ResetSenseData(void)
 		
 	Description:
 		This routine resets the Sense Data, initializing the
 		structure RequestSenseResponse gblSenseData.
 		
 	PreCondition:
 		None 
 		
 	Parameters:
 		None
 		
 	Return Values:
 		None
 		
 	Remarks:
 		None
 			
  *****************************************************************************/
void ResetSenseData(void) 
{
	gblSenseData[LUN_INDEX].ResponseCode=S_CURRENT;
	gblSenseData[LUN_INDEX].VALID=0;			// no data in the information field
	gblSenseData[LUN_INDEX].Obsolete=0x0;
	gblSenseData[LUN_INDEX].SenseKey=S_NO_SENSE;
	//gblSenseData.Resv;
	gblSenseData[LUN_INDEX].ILI=0;
	gblSenseData[LUN_INDEX].EOM=0;
	gblSenseData[LUN_INDEX].FILEMARK=0;
	gblSenseData[LUN_INDEX].InformationB0=0x00;
	gblSenseData[LUN_INDEX].InformationB1=0x00;
	gblSenseData[LUN_INDEX].InformationB2=0x00;
	gblSenseData[LUN_INDEX].InformationB3=0x00;
	gblSenseData[LUN_INDEX].AddSenseLen=0x0a;	// n-7 (n=17 (0..17))
	gblSenseData[LUN_INDEX].CmdSpecificInfo.Val=0x0;
	gblSenseData[LUN_INDEX].ASC=0x0;
	gblSenseData[LUN_INDEX].ASCQ=0x0;
	gblSenseData[LUN_INDEX].FRUC=0x0;
	gblSenseData[LUN_INDEX].SenseKeySpecific[0]=0x0;
	gblSenseData[LUN_INDEX].SenseKeySpecific[1]=0x0;
	gblSenseData[LUN_INDEX].SenseKeySpecific[2]=0x0;
}



/******************************************************************************
 	Function:
 		BYTE MSDCheckForErrorCases(DWORD DeviceBytes)
 		
 	Description:
 	   This function can be called to check for various error cases, primarily 
 	   the "Thirteen Cases" errors described in the MSD BOT v1.0 specs.  If an
 	   error is detected, the function internally calls the MSDErrorHandler()
 	   handler function, to take care of appropriately responding to the host, 
 	   based on the error condition.
 	PreCondition:
 	    None
 	     		
 	Parameters:
 		DWORD DeviceBytes - Input: This is the total number of bytes the MSD 
 		            device firmware is expecting in the MSD transfer.  
 	Return Values:
 		BYTE - Returns a byte containing the error code.  The possible error
 		    cases that can be detected and reported are:
            MSD_ERROR_CASE_NO_ERROR - None of the "Thirteen cases" errors were detected
            MSD_ERROR_CASE_2 	            
            MSD_ERROR_CASE_3 	            
            MSD_ERROR_CASE_4 	            
            MSD_ERROR_CASE_5 	            
            MSD_ERROR_CASE_7 	            
            MSD_ERROR_CASE_8 	            
            MSD_ERROR_CASE_9 	            
            MSD_ERROR_CASE_11               
            MSD_ERROR_CASE_10               
            MSD_ERROR_CASE_13               
 		
 	Remarks:
 		None
 			
  *****************************************************************************/
BYTE MSDCheckForErrorCases(DWORD DeviceBytes)
{
    BYTE MSDErrorCase;
    BOOL HostMoreDataThanDevice;
    BOOL DeviceNoData;
  
    //Check if device is expecting no data (Dn)
    if(DeviceBytes == 0)
    {
        DeviceNoData = TRUE;
    }    
    else
    {
        DeviceNoData = FALSE;
    }     
    
    //First check for the three good/non-error cases
    
    //Check for good case: Hn = Dn (Case 1)
    if((MSDHostNoData == TRUE) && (DeviceNoData == TRUE))
    {
        return MSD_ERROR_CASE_NO_ERROR;
    }    

    //Check for good cases where the data sizes between host and device match
    if(gblCBW.dCBWDataTransferLength == DeviceBytes)
    {
        //Check for good case: Hi = Di (Case 6)
        if(MSD_State == MSD_DATA_IN)
        {
            //Make sure Hi = Di, instead of Hi = Do
            if(MSDCommandState != MSD_WRITE_10)
            {
                return MSD_ERROR_CASE_NO_ERROR;
            }    
        }
        else //if(MSD_State == MSD_DATA_OUT)  
        {
            //Check for good case: Ho = Do (Case 12)
            //Make sure Ho = Do, instead of Ho = Di
            if(MSDCommandState == MSD_WRITE_10)
            {
                return MSD_ERROR_CASE_NO_ERROR;
            }             
        }      
    }    

    //If we get to here, this implies some kind of error is occuring.  Do some
    //checks to find out which error occurred, so we know how to handle it.

    //Check if the host is expecting to transfer more bytes than the device. (Hx > Dx)
    if(gblCBW.dCBWDataTransferLength > DeviceBytes)
    {
        HostMoreDataThanDevice = TRUE;
    }   
    else
    {
        HostMoreDataThanDevice = FALSE;
    } 
 
    //Check host's expected data direction
	if(MSD_State == MSD_DATA_OUT)
	{
    	//First check for Ho <> Di (Case 10)
    	if((MSDCommandState != MSD_WRITE_10) && (DeviceNoData == FALSE))
    	    MSDErrorCase = MSD_ERROR_CASE_10;
   	   	//Check for Hn < Do  (Case 3)
    	else if(MSDHostNoData == TRUE)  
    	    MSDErrorCase = MSD_ERROR_CASE_3;
    	//Check for Ho > Dn  (Case 9)
    	else if(DeviceNoData == TRUE)
    	    MSDErrorCase = MSD_ERROR_CASE_9;
    	//Check for Ho > Do  (Case 11)
    	else if(HostMoreDataThanDevice == TRUE)
    	    MSDErrorCase = MSD_ERROR_CASE_11;
    	//Check for Ho < Do  (Case 13)
    	else //if(gblCBW.dCBWDataTransferLength < DeviceBytes)
    	    MSDErrorCase = MSD_ERROR_CASE_13;
    }    
    else //else the MSD_State must be == MSD_DATA_IN
    {
    	//First check for Hi <> Do (Case 8)
    	if(MSDCommandState == MSD_WRITE_10)
    	    MSDErrorCase = MSD_ERROR_CASE_8;    	
    	//Check for Hn < Di  (Case 2)
    	else if(MSDHostNoData == TRUE)  
    	    MSDErrorCase = MSD_ERROR_CASE_2;
    	//Check for Hi > Dn  (Case 4)
    	else if(DeviceNoData == TRUE)
    	    MSDErrorCase = MSD_ERROR_CASE_4;
    	//Check for Hi > Di  (Case 5)
    	else if(HostMoreDataThanDevice == TRUE)
    	    MSDErrorCase = MSD_ERROR_CASE_5;
        //Check for Hi < Di  (Case 7)
    	else //if(gblCBW.dCBWDataTransferLength < DeviceBytes)
    	    MSDErrorCase = MSD_ERROR_CASE_7;
    }        
    //Now call the MSDErrorHandler(), based on the error that was detected.
    MSDErrorHandler(MSDErrorCase);
    return MSDErrorCase;
}    


/******************************************************************************
 	Function:
 		void MSDErrorHandler(BYTE ErrorCase)
 		
 	Description:
 	    Once an error condition has been detected, this function can be called
 	    to set the proper states and perform the proper tasks needed to let the
 	    host know about the error.
 	PreCondition:
 		Firmware should have already determined an error occurred, and it should
 		know what the error code was before calling this handler.
 		
 	Parameters:
 		BYTE ErrorCase - Input: This is the error code that the firmware 
 		                    detected.  This error code will determine how the
 		                    handler will behave (ex: what status to send to host,
 		                    what endpoint(s) should be stalled, etc.).
 		                    The implemented error case possibilities are (suffix
 		                    numbers correspond to the "Thirteen cases" numbers 
 		                    described in the MSD BOT specs v1.0):
 		                    
                            MSD_ERROR_CASE_2 	            
                            MSD_ERROR_CASE_3 	            
                            MSD_ERROR_CASE_4 	            
                            MSD_ERROR_CASE_5 	            
                            MSD_ERROR_CASE_7 	            
                            MSD_ERROR_CASE_8 	            
                            MSD_ERROR_CASE_9 	            
                            MSD_ERROR_CASE_11               
                            MSD_ERROR_CASE_10               
                            MSD_ERROR_CASE_13               
                            MSD_ERROR_UNSUPPORTED_COMMAND   

 	Return Values:
 		None
 		
 	Remarks:
 		None
 			
  *****************************************************************************/
void MSDErrorHandler(BYTE ErrorCase)
{
    BYTE OldMSD_State;
    
	//Both MSD bulk IN and OUT endpoints should not be busy when these error cases are detected
	//If for some reason this isn't true, then we should preserve the state machines states for now.
    if((USBHandleBusy(USBMSDInHandle)) || (USBHandleBusy(USBMSDOutHandle)))
    {
    	return;	
    }

    //Save the old state before we change it.  The old state is needed to determine
    //the proper handling behavior in the case of receiving unsupported commands.
    OldMSD_State = MSD_State;

	//Reset main state machines back to idle values.
	MSDCommandState = MSD_COMMAND_WAIT;
	MSDReadState = MSD_READ10_WAIT;
	MSDWriteState = MSD_WRITE10_WAIT;
	//After the conventional 13 test cases failures, the host still expects a valid CSW packet
    msd_csw.dCSWDataResidue = gblCBW.dCBWDataTransferLength; //Indicate the unconsumed/unsent data
   	msd_csw.bCSWStatus = MSD_CSW_COMMAND_FAILED;    //Gets changed later to phase error for errors that user phase error
	MSD_State = MSD_SEND_CSW;	        

    //Now do other error related handling tasks, which depend on the specific 
    //error	type that was detected.
	switch(ErrorCase)
	{
		case MSD_ERROR_CASE_2://Also CASE_3
			msd_csw.bCSWStatus = MSD_CSW_PHASE_ERROR;	
        	break;	
		case MSD_ERROR_CASE_4://Also CASE_5
       		USBStallEndpoint(MSD_DATA_IN_EP, IN_TO_HOST);	//STALL the bulk IN MSD endpoint
			break;
		case MSD_ERROR_CASE_7://Also CASE_8
       		msd_csw.bCSWStatus = MSD_CSW_PHASE_ERROR;	
       		USBStallEndpoint(MSD_DATA_IN_EP, IN_TO_HOST);	//STALL the bulk IN MSD endpoint
       		break;		
		case MSD_ERROR_CASE_9://Also CASE_11
			USBStallEndpoint(MSD_DATA_OUT_EP, OUT_FROM_HOST); //Stall the bulk OUT endpoint
			break;
		case MSD_ERROR_CASE_10://Also CASE_13
	        msd_csw.bCSWStatus = MSD_CSW_PHASE_ERROR;	
			USBStallEndpoint(MSD_DATA_OUT_EP, OUT_FROM_HOST);
			break;
			
        case MSD_ERROR_UNSUPPORTED_COMMAND:
        	ResetSenseData();
			gblSenseData[LUN_INDEX].SenseKey=S_ILLEGAL_REQUEST;
			gblSenseData[LUN_INDEX].ASC=ASC_INVALID_COMMAND_OPCODE;
			gblSenseData[LUN_INDEX].ASCQ=ASCQ_INVALID_COMMAND_OPCODE;
            if((OldMSD_State == MSD_DATA_OUT) && (gblCBW.dCBWDataTransferLength != 0))
            {
			    USBStallEndpoint(MSD_DATA_OUT_EP, OUT_FROM_HOST); 
			}
			else
			{
        	    USBStallEndpoint(MSD_DATA_IN_EP, IN_TO_HOST);
            }     	    
            break;
		default:	//Shouldn't get hit, don't call MSDErrorHandler() if there is no error
			break;
	}//switch(ErrorCase)			
}	



//-----------------------------------------------------------------------------------------
#endif //end of #ifdef USB_USE_MSD
//End of file usb_function_msd.c
