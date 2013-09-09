/*********************************************************************
 *
 *     MAC Module (Microchip PIC32MX5-7) for Microchip TCP/IP Stack
 *
 *********************************************************************
 * FileName:        ETHPIC32IntMac.c
 * Dependencies:    see the include section below
 * 
 * Processor:       PIC32MX
 *                  
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PICmicro® Microcontroller is intended and
 * supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 * $Id: $
 *
********************************************************************/
#include <string.h>


#include "TCPIP Stack/TCPIP.h"
#include "TCPIP Stack/MAC.h"


// Compile only for PIC32MX with Ethernet MAC interface (must not have external ENCX24J600, ENC28J60, or MRF24WB0M hardware defined)
#if defined(__PIC32MX__) && defined(_ETH) && !defined(ENC100_INTERFACE_MODE) && !defined(ENC_CS_TRIS) && !defined(WF_CS_TRIS)

// running on PIC32MX5-7 family with embedded ETHC

#include <peripheral/eth.h>

#include "TCPIP Stack/ETHPIC32ExtPhy.h"


/** D E F I N I T I O N S ****************************************************/


#define ETHER_IP    (0x00u)
#define ETHER_ARP   (0x06u)


#define	LINK_REFRESH_MS	100		// refresh link status time, ms

typedef struct
{
	int		txBusy;										// busy flag
	unsigned int	dataBuff[(MAC_TX_BUFFER_SIZE+sizeof(ETHER_HEADER)+sizeof(int)-1)/sizeof(int)];	// actual data buffer
}sEthTxDcpt;	// TX buffer descriptor

/******************************************************************************
 * Prototypes
 ******************************************************************************/
static void		_TxAckCallback(void* pPktBuff, int buffIx, void* fParam);			// Eth tx buffer acnowledge function
static int		_LinkReconfigure(void);						// link reconfiguration

static void*    _MacAllocCallback( size_t nitems, size_t size, void* param );


// TX buffers
static volatile sEthTxDcpt	_TxDescriptors[EMAC_TX_DESCRIPTORS];			// the statically allocated TX buffers
static volatile sEthTxDcpt*	_pTxCurrDcpt=0;						// the current TX buffer
static int			_TxLastDcptIx=0;					// the last TX descriptor used
static unsigned short int	_TxCurrSize=0;						// the current TX buffer size


// RX buffers
static unsigned char		_RxBuffers[EMAC_RX_DESCRIPTORS][EMAC_RX_BUFF_SIZE];	// rx buffers for incoming data
static unsigned char*		_pRxCurrBuff=0;						// the current RX buffer
static unsigned short int	_RxCurrSize=0;						// the current RX buffer size



// HTTP +SSL buffers
static unsigned char		_HttpSSlBuffer[RESERVED_HTTP_MEMORY+RESERVED_SSL_MEMORY];


// general stuff
static unsigned char*		_CurrWrPtr=0;						// the current write pointer
static unsigned char*		_CurrRdPtr=0;						// the current read pointer


// timing and link status maintenance
static DWORD			_linkUpdTick;						// last tick value when the link update was started
static eEthLinkStat		_linkPrev;						// last value of the link status
static int			_linkPresent;						// if connection to the PHY properly detected
static int			_linkNegotiation;					// if an auto-negotiation is in effect

// run time statistics
/*static*/ int			_stackMgrRxOkPkts=0;
/*static*/ int			_stackMgrRxBadPkts=0;
/*static*/ int			_stackMgrInGetHdr=0;
/*static*/ int			_stackMgrRxDiscarded=0;
/*static*/ int			_stackMgrTxNotReady=0;


/*
 * interface functions
 *
*/




/****************************************************************************
 * Function:        MACInit
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *                
 * Side Effects:    None
 *
 * Overview:        This function initializes the Eth controller, the MAC and the PHY. It should be called to be able to schedule
 *                  any Eth transmit or receive operation.
 *
 * Note:            None 
 *****************************************************************************/
void MACInit(void)
{
    union
    {
        double      align;              // alignement
        BYTE		addr[6];            // address itself
    }SysMACAddr;        // aligned MAC address

    int		ix;
	eEthRes		ethRes, phyInitRes;
	BYTE		useFactMACAddr[6] = {0x00, 0x04, 0xa3, 0x00, 0x00, 0x00};		// to check if factory programmed MAC address needed
	BYTE		unsetMACAddr[6] =   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};		// not set MAC address

    int		initFail=0;

	_stackMgrRxBadPkts=_stackMgrRxOkPkts=_stackMgrInGetHdr=_stackMgrRxDiscarded=0;
	_CurrWrPtr=_CurrRdPtr=0;

	// set the TX/RX pointers
	for(ix=0; ix<sizeof(_TxDescriptors)/sizeof(*_TxDescriptors); ix++)
	{
		_TxDescriptors[ix].txBusy=0;
	}
	_pTxCurrDcpt=_TxDescriptors+0; _TxLastDcptIx=0; _TxCurrSize=0;

	_pRxCurrBuff=0; _RxCurrSize=0;	

	_linkNegotiation=_linkPresent=0;
	_linkPrev=ETH_LINK_ST_DOWN;
	
		
	while(1)
	{
		eEthLinkStat	linkStat;
		eEthOpenFlags	oFlags, linkFlags;
		eEthMacPauseType pauseType;
		eEthPhyCfgFlags cfgFlags;


		
	#ifdef PHY_RMII
		cfgFlags=ETH_PHY_CFG_RMII;
	#else
		cfgFlags=ETH_PHY_CFG_MII;		
	#endif
		
	#ifdef PHY_CONFIG_ALTERNATE
		cfgFlags|=ETH_PHY_CFG_ALTERNATE;
	#else
		cfgFlags|=ETH_PHY_CFG_DEFAULT;	
	#endif


	#if ETH_CFG_LINK
			oFlags=ETH_CFG_AUTO?ETH_OPEN_AUTO:0;
			oFlags|=ETH_CFG_10?ETH_OPEN_10:0;
			oFlags|=ETH_CFG_100?ETH_OPEN_100:0;
			oFlags|=ETH_CFG_HDUPLEX?ETH_OPEN_HDUPLEX:0;
			oFlags|=ETH_CFG_FDUPLEX?ETH_OPEN_FDUPLEX:0;
			if(ETH_CFG_AUTO_MDIX)
			{
				oFlags|=ETH_OPEN_MDIX_AUTO;
			}
			else
			{
				oFlags|=ETH_CFG_SWAP_MDIX?ETH_OPEN_MDIX_SWAP:ETH_OPEN_MDIX_NORM;
			}			
	#else
		oFlags= ETH_OPEN_DEFAULT;
	#endif // ETH_CFG_LINK

		
		pauseType=(oFlags&ETH_OPEN_FDUPLEX)?ETH_MAC_PAUSE_CPBL_MASK:ETH_MAC_PAUSE_TYPE_NONE;
		
		// start the initialization sequence	
		EthInit();

		phyInitRes=EthPhyInit(oFlags, cfgFlags, &linkFlags);
		
		// let the auto-negotiation (if any) take place
		// continue the initialization
		EthRxFiltersClr(ETH_FILT_ALL_FILTERS);
		EthRxFiltersSet(ETH_FILT_CRC_ERR_REJECT|ETH_FILT_RUNT_REJECT|ETH_FILT_ME_UCAST_ACCEPT|ETH_FILT_MCAST_ACCEPT|ETH_FILT_BCAST_ACCEPT);

		
		// set the MAC address
        memcpy(SysMACAddr.addr, AppConfig.MyMACAddr.v, sizeof(SysMACAddr.addr));
        if(memcmp(SysMACAddr.addr, useFactMACAddr, sizeof(useFactMACAddr))==0 || memcmp(SysMACAddr.addr, unsetMACAddr, sizeof(unsetMACAddr))==0 )
		{	// use the factory programmed address existent in the MAC
            unsigned short* pS=(unsigned short*)SysMACAddr.addr;
            *pS++=EMACxSA2;
            *pS++=EMACxSA1;
            *pS=EMACxSA0;
            memcpy(AppConfig.MyMACAddr.v, SysMACAddr.addr, sizeof(SysMACAddr.addr));
		}
        else
        {   // use the supplied address
			EthMACSetAddress(SysMACAddr.addr);                
        }
				
		if(EthDescriptorsPoolAdd(EMAC_TX_DESCRIPTORS, ETH_DCPT_TYPE_TX, _MacAllocCallback, 0)!=EMAC_TX_DESCRIPTORS)
		{
			initFail++;
		}

		if(EthDescriptorsPoolAdd(EMAC_RX_DESCRIPTORS, ETH_DCPT_TYPE_RX, _MacAllocCallback, 0)!=EMAC_RX_DESCRIPTORS)
		{
			initFail++;
		}

		EthRxSetBufferSize(EMAC_RX_BUFF_SIZE);

		// set the RX buffers as permanent receive buffers
		for(ix=0, ethRes=ETH_RES_OK; ix<EMAC_RX_DESCRIPTORS && ethRes==ETH_RES_OK; ix++)
		{
			void* pRxBuff=_RxBuffers[ix];
			ethRes=EthRxBuffersAppend(&pRxBuff, 1, ETH_BUFF_FLAG_RX_STICKY);
		}

		if(ethRes!=ETH_RES_OK)
		{
			initFail++;
		}


		if(phyInitRes==ETH_RES_OK)
		{	// PHY was detected
			_linkPresent=1;
			if(oFlags&ETH_OPEN_AUTO)
			{	// we'll just wait for the negotiation to be done
				_linkNegotiation=1;	// performing the negotiation
				linkStat=_LinkReconfigure()?ETH_LINK_ST_UP:ETH_LINK_ST_DOWN;	// if negotiation not done yet we need to try it next time
			}
			else
			{	// no need of negotiation results; just update the MAC
				EthMACOpen(linkFlags, pauseType);
				linkStat=EthPhyGetLinkStatus(0);
			}
			
			_linkUpdTick=TickGet();		// the last time we performed the link read
			_linkPrev=linkStat;
		}
		else
		{
			initFail++;
		}

		break;
	}

	
//	return !initFail;	// at this point initFail gives some indication of any existent problems
	
}


/****************************************************************************
 * Function:        MACIsLinked
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          TRUE if link is up
 *                  FALSE otherwise
 *
 * Side Effects:    None
 *
 * Overview:        This function checks the link status
 *
 * Note:            None 
 *****************************************************************************/
BOOL MACIsLinked(void)
{
	return (_linkPrev&ETH_LINK_ST_UP)!=0;
}


/****************************************************************************
 * Function:        MACGetTxBaseAddr
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          TX buffer base address
 *                
 * Side Effects:    None
 *
 * Overview:        This function returns the address of the current TX buffer.
 *
 * Note:            The returned value could be 0 if currently there's no available TX buffer. 
 *****************************************************************************/
PTR_BASE MACGetTxBaseAddr(void)
{
	return _pTxCurrDcpt?(PTR_BASE)_pTxCurrDcpt->dataBuff:0;
}

/****************************************************************************
 * Function:        MACGetHttpBaseAddr
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          HTTP buffer base address
 *                
 * Side Effects:    None
 *
 * Overview:        This function returns the address of the HTTP buffer.
 *
 * Note:            The HTTP buffer is a static one, always available. 
 *****************************************************************************/
PTR_BASE MACGetHttpBaseAddr(void)
{
	return (PTR_BASE)_HttpSSlBuffer;
}

/****************************************************************************
 * Function:        MACGetSslBaseAddr
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          SSL buffer base address
 *                
 * Side Effects:    None
 *
 * Overview:        This function returns the address of the SSL buffer.
 *
 * Note:            The SSL buffer is a static one, always available. 
 *****************************************************************************/
PTR_BASE MACGetSslBaseAddr(void)
{
	return (PTR_BASE)(_HttpSSlBuffer+RESERVED_HTTP_MEMORY);
}


/**************************
 * TX functions
 ***********************************************/

/****************************************************************************
 * Function:        MACSetWritePtr
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          old write pointer
 *                
 * Side Effects:    None
 *
 * Overview:        This function sets the new write pointer.
 *
 * Note:            None
 *****************************************************************************/
PTR_BASE MACSetWritePtr(PTR_BASE address)
{
	unsigned char* oldPtr;

	oldPtr=_CurrWrPtr;
	_CurrWrPtr=(unsigned char*)address;
	return (PTR_BASE)oldPtr;
}


/******************************************************************************
 * Function:        BOOL MACIsTxReady(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          TRUE: If data can be inserted in the current TX buffer
 *                  FALSE: there is no free TX buffer
 *
 * Side Effects:    None
 *
 * Overview:        Checks if there is an available current TX buffer
 *
 * Note:            None
 *****************************************************************************/
BOOL MACIsTxReady(void)
{
	int	ix;

	EthTxAcknowledgeBuffer(0, _TxAckCallback, 0);		// acknowledge everything

	if(_pTxCurrDcpt==0)
	{
		for(ix=_TxLastDcptIx+1; ix<sizeof(_TxDescriptors)/sizeof(*_TxDescriptors); ix++)
		{			
			if(_TxDescriptors[ix].txBusy==0)
			{	// found a non busy descriptor
				_pTxCurrDcpt=_TxDescriptors+ix;
				_TxLastDcptIx=ix;
				break;
			}
		}
		if(_pTxCurrDcpt==0)
		{
			for(ix=0; ix<_TxLastDcptIx; ix++)
			{
				if(_TxDescriptors[ix].txBusy==0)
				{	// found a non busy descriptor
					_pTxCurrDcpt=_TxDescriptors+ix;
					_TxLastDcptIx=ix;
					break;
				}
			}
		}
	}


	if( _pTxCurrDcpt==0)
	{
		_stackMgrTxNotReady++;
	}
	
	return _pTxCurrDcpt!=0;
}

/******************************************************************************
 * Function:        void MACPut(BYTE val)
 *
 * PreCondition:    None
 *
 * Input:           byte to be written
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:       Writes a byte to the current write location and increments the write pointer. 
 *
 * Note:            None
 *****************************************************************************/
void MACPut(BYTE val)
{
	*_CurrWrPtr++=val;
}

/******************************************************************************
 * Function:        void MACPutArray(BYTE* buff, WORD len)
 *
 * PreCondition:    None
 *
 * Input:           buff - buffer to be written
 *                  len - buffer length
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Writes a buffer to the current write location and updates the write pointer. 
 *
 * Note:            None
 *****************************************************************************/
void MACPutArray(BYTE *buff, WORD len)
{
	memcpy(_CurrWrPtr, buff, len);
	_CurrWrPtr+=len;
}


/******************************************************************************
 * Function:        void MACPutHeader(MAC_ADDR *remote, BYTE type, WORD dataLen)
 *
 * PreCondition:    None
 *
 * Input:           remote - Pointer to memory which contains the destination MAC address (6 bytes)
 *                  type - packet type: MAC_IP or ARP
 *                  dataLen - ethernet frame payload
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:       Sets the write pointer at the beginning of the current TX buffer
 *                 and sets the ETH header and the frame length. Updates the write pointer
 *
 * Note:            Assumes there is an available TX buffer, i.e. MACIsTxReady() returned !0
 *****************************************************************************/
void MACPutHeader(MAC_ADDR *remote, BYTE type, WORD dataLen)
{
	_TxCurrSize=dataLen+sizeof(ETHER_HEADER);
	_CurrWrPtr=(unsigned char*)_pTxCurrDcpt->dataBuff;		// point at the beg of the buffer
       	

	memcpy(_CurrWrPtr, remote, sizeof(*remote));
	_CurrWrPtr+=sizeof(*remote);
	memcpy(_CurrWrPtr, &AppConfig.MyMACAddr, sizeof(AppConfig.MyMACAddr));
	_CurrWrPtr+=sizeof(AppConfig.MyMACAddr);

	
	*_CurrWrPtr++=0x08;
	*_CurrWrPtr++=(type == MAC_IP) ? ETHER_IP : ETHER_ARP;
	
}



void MACFlush(void)
{
	if(_pTxCurrDcpt && _TxCurrSize)
	{	// there is a buffer to transmit
		_pTxCurrDcpt->txBusy=1;	
		EthTxSendBuffer((void*)_pTxCurrDcpt->dataBuff, _TxCurrSize);
		// res should be ETH_RES_OK since we made sure we had a descriptor available
		// by the call to MACIsTxReady and the number of the buffers matches the number of descriptors
		_pTxCurrDcpt=0;
		_TxCurrSize=0;
	}
}

/**************************
 * RX functions
 ***********************************************/


/******************************************************************************
 * Function:        void MACDiscardRx(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Marks the last received packet (obtained using 
 *                  MACGetHeader())as being processed and frees the buffer
 *                  memory associated with it.
 *                  It acknowledges the ETHC.
 *
 * Note:            Is is safe to call this function multiple times between
 *                  MACGetHeader() calls.  Extra packets won't be thrown away 
 *                  until MACGetHeader() makes it available.
 *****************************************************************************/
void MACDiscardRx(void)
{
	if(_pRxCurrBuff)
	{	// an already existing packet
		EthRxAcknowledgeBuffer(_pRxCurrBuff, 0, 0);
		_pRxCurrBuff=0;
		_RxCurrSize=0;

		_stackMgrRxDiscarded++;
	}	
}



/******************************************************************************
 * Function:        BOOL MACGetHeader(MAC_ADDR *remote, BYTE* type)
 *
 * PreCondition:    None
 *
 * Input:           *remote: Location to store the Source MAC address of the
 *                           received frame.
 *                  *type: Location of a BYTE to store the constant
 *                         MAC_UNKNOWN, ETHER_IP, or ETHER_ARP, representing
 *                         the contents of the Ethernet type field.
 *
 * Output:          TRUE: If a packet was waiting in the RX buffer.  The
 *                        remote, and type values are updated.
 *                  FALSE: If a packet was not pending.  remote and type are
 *                         not changed.
 *
 * Side Effects:    Last packet is discarded if MACDiscardRx() hasn't already
 *                  been called.
 *
 * Overview:        None
 *
 * Note:            Sets the read pointer at the beginning of the new packet
 *****************************************************************************/
BOOL MACGetHeader(MAC_ADDR *remote, BYTE* type)
{
	void*			pNewPkt;
	const sEthRxPktStat*	pRxPktStat;
	eEthRes			res;

	_stackMgrInGetHdr++;

	// verify the link status
	// if auto negotiation is enabled we may have to reconfigure the MAC

	while(_linkPresent)
	{
		eEthLinkStat	linkCurr;
		DWORD		currTick=TickGet();
		
		if(currTick-_linkUpdTick< (TICKS_PER_SECOND/1000)*LINK_REFRESH_MS)
		{	// not time to do anything yet
			break;
		}

		linkCurr=EthPhyGetLinkStatus(0);	// read current PHY status
		_linkUpdTick=currTick;			// start a new counting period

		if(_linkNegotiation)
		{	// the auto-negotiation turned on
			if((linkCurr&ETH_LINK_ST_UP) && !(_linkPrev&ETH_LINK_ST_UP))
			{	// we're up after being done. do renegotiate!
				linkCurr=_LinkReconfigure()?ETH_LINK_ST_UP:ETH_LINK_ST_DOWN;	// if negotiation not done yet we need to try it next time
			}
			// else link went/still down; nothing to do yet
		}
		_linkPrev=linkCurr;
	
		break;
	}


	MACDiscardRx();		// discard/acknowledge the old RX buffer, if any
	
	res=EthRxGetBuffer(&pNewPkt, &pRxPktStat);
	
	if(res==ETH_RES_OK)
	{	// available packet; minimum check

		if(pRxPktStat->rxOk && !pRxPktStat->runtPkt && !pRxPktStat->crcError)
		{	// valid packet;
			WORD_VAL newType;
			_RxCurrSize=pRxPktStat->rxBytes;
			_pRxCurrBuff=pNewPkt;
			_CurrRdPtr=_pRxCurrBuff+sizeof(ETHER_HEADER);	// skip the packet header
			// set the packet type
			memcpy(remote, &((ETHER_HEADER*)pNewPkt)->SourceMACAddr, sizeof(*remote));
			*type=MAC_UNKNOWN;
			newType=((ETHER_HEADER*)pNewPkt)->Type;
			if( newType.v[0]==0x08 && (newType.v[1]==ETHER_IP || newType.v[1]==ETHER_ARP) )
			{
				*type=newType.v[1];
			}
			
			_stackMgrRxOkPkts++;
		}
	}

	if(_pRxCurrBuff==0 && pNewPkt)
	{	// failed packet, discard
		EthRxAcknowledgeBuffer(pNewPkt, 0, 0);
		_stackMgrRxBadPkts++;
	}
		
	
	return _pRxCurrBuff!=0;
}



/******************************************************************************
 * Function:        void MACSetReadPtrInRx(WORD offset)
 *
 * PreCondition:    A packet has been obtained by calling MACGetHeader() and
 *                  getting a TRUE result.
 *
 * Input:           offset: WORD specifying how many bytes beyond the Ethernet
 *                          header's type field to relocate the read pointer.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The current read pointer is updated.  All calls to
 *                  MACGet() and MACGetArray() will use these new values.
 *
 * Note:            
 ******************************************************************************/
void MACSetReadPtrInRx(WORD offset)
{
	_CurrRdPtr=_pRxCurrBuff+sizeof(ETHER_HEADER)+offset;
}


/****************************************************************************
 * Function:        MACSetReadPtr
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          old read pointer
 *                
 * Side Effects:    None
 *
 * Overview:        This function sets the new read pointer value.
 *
 * Note:            None
 *****************************************************************************/
PTR_BASE MACSetReadPtr(PTR_BASE address)
{
	unsigned char* oldPtr;

	oldPtr=_CurrRdPtr;
	_CurrRdPtr=(unsigned char*)address;
	return (PTR_BASE)oldPtr;
}




/******************************************************************************
 * Function:        BYTE MACGet()
 *
 * PreCondition:    A valid packet should vahe been obtained or the read pointer properly set.
 *
 * Input:           None
 *
 * Output:          Byte read from the current read pointer location
 *
 * Side Effects:    None
 *
 * Overview:        MACGet returns the byte pointed to by the current read pointer location and
 *                  increments the read pointer.
 *
 * Note:            None
 *****************************************************************************/
BYTE MACGet(void)
{
	return *_CurrRdPtr++;
}


/******************************************************************************
 * Function:        WORD MACGetArray(BYTE *address, WORD len)
 *
 * PreCondition:    A valid packet should vahe been obtained or the read pointer properly set.
 *
 * Input:           address: Pointer to storage location
 *                  len:  Number of bytes to read from the data buffer.
 *
 * Output:          number of bytes copied to the data buffer.
 *
 * Side Effects:    None
 *
 * Overview:        Copies data in the supplied buffer.
 *
 * Note:            The read pointer is updated
 *****************************************************************************/
WORD MACGetArray(BYTE *address, WORD len)
{
	if(address)
	{
		memcpy(address, _CurrRdPtr, len);
	}

	_CurrRdPtr+=len;
	return len;
}

/******************************************************************************
 * Function:        WORD MACGetFreeRxSize(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          An estimate of how much RX buffer space is free at the present time.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 *****************************************************************************/
WORD MACGetFreeRxSize(void)
{
	int avlblRxBuffs=sizeof(_RxBuffers)/sizeof(*_RxBuffers)-EthDescriptorsGetRxUnack();	// avlbl=allBuffs-unAck

	return avlblRxBuffs*(sizeof(_RxBuffers[0])/sizeof(*_RxBuffers[0]));	// avlbl* sizeof(buffer)
}


/*****************************************************************************
  Function:
	

  Summary:
	Asynchronously copies data from one address to another within the Ethernet memory.

  Description:


  Precondition:
	SPI bus must be initialized (done in MACInit()).


  Returns:
	None

  Remarks:
  	Call MACIsMemCopyDone() to see when the transfer is complete.
  	
	Copying to a destination region that overlaps with the source address 
	is supported only if the destination start address is at a lower memory 
	address (closer to 0x0000) than the source pointer.  However, if they do 
	overlap there must be at least 2 bytes of non-overlap to ensure correct 
	results due to hardware DMA requirements.  For example, destAddr = 0; 
	sourceAddr = 1; is illegal while destAddr = 0; sourceAddr = 2; is fine.
  
 	If a prior transfer is already in progress prior to calling this function, 
 	this function will block until it can start this transfer.

 	If a negative value is used for the sourceAddr or destAddr parameters, 
 	then that pointer will get updated with the next address after the read or 
 	write.
 *****************************************************************************/


/******************************************************************************
 * Function:        void MACMemCopyAsync(PTR_BASE destAddr, PTR_BASE sourceAddr, WORD len)
 *
 * PreCondition:    Read and write pointers properly set if using the current ponter values
 *
 * Input:           destAddr - Destination address in the memory to copy to.  If it equals -1,
 *                     the current write pointer will be used.
 *                  sourceAddr - Source address to read from.  If it equals -1,
 *                     the current read pointer will be used.
 *                  len - number of bytes to copy
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Copies data from one address to another within the Ethernet memory.
 *                  Overlapped memory regions are allowed only if the destination start address
 *                  is at a lower memory address than the source address.
 *
 * Note:            The addresses do not have to be aligned.
 *****************************************************************************/
void MACMemCopyAsync(PTR_BASE destAddr, PTR_BASE sourceAddr, WORD len)
{
	if(len)
	{
		unsigned char	*pDst, *pSrc;

		pDst=(destAddr==-1)?_CurrWrPtr:(unsigned char*)destAddr;
		pSrc=(sourceAddr==-1)?_CurrRdPtr:(unsigned char*)sourceAddr;
		
		memcpy(pDst, pSrc, len);

	}
}

/******************************************************************************
 * Function:        void MACIsMemCopyDone(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          TRUE
 *
 * Side Effects:    None
 *
 * Overview:        Since there's no copy initiated by the DMA, the function returns always true for now.
 *
 * Note:            None
 *****************************************************************************/
BOOL MACIsMemCopyDone(void)
{
	return 1;
}


/******************************************************************************
 * Function:        WORD CalcIPBufferChecksum(WORD len)
 *
 * PreCondition:    Read buffer pointer set to starting of checksum data
 *
 * Input:           len: Total number of bytes to calculate the checksum over.
 *
 * Output:          16-bit checksum as defined by RFC 793
 *
 * Side Effects:    None
 *
 * Overview:        This function performs a checksum calculation of the buffer
 *                  pointed by the current value of the read pointer.
 *
 * Note:            None
 *****************************************************************************/
WORD CalcIPBufferChecksum(WORD len)
{
	return CalcIPChecksum(_CurrRdPtr, len);
}


/******************************************************************************
 * Function:        WORD MACCalcRxChecksum(WORD offset, WORD len)
 *
 * PreCondition:    None
 *
 * Input:           offset  - Number of bytes beyond the beginning of the
 *                          Ethernet data (first byte after the type field)
 *                          where the checksum should begin
 *                  len     - Total number of bytes to include in the checksum
 *
 * Output:          16-bit checksum as defined by RFC 793.
 *
 * Side Effects:    None
 *
 * Overview:        This function performs a checksum calculation in the current receive buffer.
 *
 * Note:            None
 *****************************************************************************/
WORD MACCalcRxChecksum(WORD offset, WORD len)
{
	return CalcIPChecksum(_pRxCurrBuff+sizeof(ETHER_HEADER)+offset, len);
}

/******************************************************************************
 * Function:        void SetRXHashTableEntry(MAC_ADDR DestMACAddr)
 *
 * PreCondition:    MACInit() should have been called.
 *
 * Input:           DestMACAddr: 6 byte group destination MAC address to allow 
 *                  through the Hash Table Filter.  If DestMACAddr 
 *                  is set to 00-00-00-00-00-00, then the hash 
 *                  table will be cleared of all entries and the 
 *                  filter will be disabled.
 *
 * Output:          Sets the appropriate bit in the ETHHT0/1 registers to allow 
 *                  packets sent to DestMACAddr to be received and enabled the 
 *                  Hash Table receive filter (if not already).
 *
 * Side Effects:    None
 *
 * Overview:        Calculates a CRC-32 using polynomial 0x4C11DB7 and then,
 *                  using bits 28:23 of the CRC, sets the appropriate bit in 
 *                  the ETHHT0-ETHHT1 registers.
 *
 * Note:            This code is commented out to save code space on systems 
 *                  that do not need this function.  Change the 
 *                  "#if STACK_USE_ZEROCONF_MDNS_SD" line to "#if 1" to 
 *                  uncomment it, assuming you aren't using the Zeroconf module, 
 *                  which requires mutlicast support and enables this function 
 *                  automatically.
 *
 *                  There is no way to individually unset destination MAC 
 *                  addresses from the hash table since it is possible to have 
 *                  a hash collision and therefore multiple MAC addresses 
 *                  relying on the same hash table bit.  The stack would have 
 *                  to individually store each 6 byte MAC address to support 
 *                  this feature, which would waste a lot of RAM and be 
 *                  unnecessary in most applications.  As a simple compromise, 
 *                  you can call SetRXHashTableEntry() using a 
 *                  00-00-00-00-00-00 destination MAC address, which will clear 
 *                  the entire hash table and disable the hash table filter.  
 *                  This will allow you to then readd the necessary destination 
 *                  addresses.
 *****************************************************************************/
#if defined(STACK_USE_ZEROCONF_MDNS_SD)
void SetRXHashTableEntry(MAC_ADDR DestMACAddr)
{
      volatile unsigned int*    pHTSet;
      BYTE                      hVal;
      int                       i, j;
      DWORD_VAL                 crc = {0xFFFFFFFF};
      BYTE                      nullMACAddr[6] =   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

      // Clear the Hash Table bits and disable the Hash Table Filter if a special 
      // 00-00-00-00-00-00 destination MAC address is provided.
      if( memcmp(DestMACAddr.v, nullMACAddr, sizeof(nullMACAddr))==0 )
      {
            // Disable the Hash Table receive filter and clear the hash table
            EthRxFiltersClr(ETH_FILT_HTBL_ACCEPT);
            EthRxFiltersHTSet(0ull);
            return;
      }
 
 
      // Calculate a CRC-32 over the 6 byte MAC address 
      // using polynomial 0x4C11DB7
      for(i = 0; i < sizeof(MAC_ADDR); i++)
      {
            BYTE  crcnext;
      
            // shift in 8 bits
            for(j = 0; j < 8; j++)
            {
                  crcnext = 0;
                  if(((BYTE_VAL*)&(crc.v[3]))->bits.b7)
                        crcnext = 1;
                  crcnext ^= (((BYTE_VAL*)&DestMACAddr.v[i])->bits.b0);
      
                  crc.Val <<= 1;
                  if(crcnext)
                        crc.Val ^= 0x4C11DB7;
                  // next bit
                  DestMACAddr.v[i] >>= 1;
            }
      }
      
      // CRC-32 calculated, now extract bits 28:23
      // Bit 28 defines what HT register is affected: ETHHT0 or ETHHT1
      // Bits 27:23 define the bit offset within the ETHHT register
      pHTSet = (crc.bits.b28)? &ETHHT1SET : &ETHHT0SET;
      hVal = (crc.Val >> 23)&0x1f;
      *pHTSet = 1 << hVal;
      
      // Enable that the Hash Table receive filter
      EthRxFiltersSet(ETH_FILT_HTBL_ACCEPT);
      
}
#endif
 


/**************************
 * local functions and helpers
 ***********************************************/

/*********************************************************************
* Function:        void	_TxAckCallback(void* pPktBuff, int buffIx, void* fParam)
 *
 * PreCondition:    None
 * 
 * Input:           pPktBuff - tx buffer to be acknowledged
 *                  buffIx   - buffer index, when packet spans multiple buffers
 *                  fParam   - optional parameter specified when EthTxAcknowledgeBuffer() called 
 * 
 * Output:          None
 * 
 * Side Effects:    None
 * 
 * Overview:        TX acknowledge call back function.
 *                  Called by the Eth MAC when TX buffers are acknoledged (as a result of a call to EthTxAcknowledgeBuffer).
 * 
 * Note:            None
 ********************************************************************/
static void	_TxAckCallback(void* pPktBuff, int buffIx, void* fParam)
{
	volatile sEthTxDcpt*	pDcpt;

	pDcpt=(sEthTxDcpt*)((char*)pPktBuff-offsetof(sEthTxDcpt, dataBuff));

	pDcpt->txBusy=0;

}

/*********************************************************************
* Function:        void* _MacAllocCallback( size_t nitems, size_t size, void* param )
 *
 * PreCondition:    None
 * 
 * Input:           nitems - number of items to be allocated
 *                  size   - size of each item
 *                  param  - optional parameter specified when EthDescriptorsPoolAdd() called 
 * 
 * Output:          pointer to the allocated memory of NULL if allocation failed
 * 
 * Side Effects:    None
 * 
 * Overview:        Memory allocation callback.
 * 
 * Note:            None
 ********************************************************************/
static void* _MacAllocCallback( size_t nitems, size_t size, void* param )
{
    return calloc(nitems, size);
}

/*********************************************************************
* Function:        int	_LinkReconfigure(void)
 *
 * PreCondition:    None
 * 
 * Input:           None
 * 
 * Output:          TRUE if negotiation succeeded and MAC was updated
 *                  FALSE otherwise
 * 
 * Side Effects:    None
 * 
 * Overview:        Performs re-configuration after auto-negotiation performed.
 * 
 * Note:            None
 ********************************************************************/
static int _LinkReconfigure(void)
{

	eEthOpenFlags	 linkFlags;
	eEthLinkStat	 linkStat;
	eEthMacPauseType pauseType;
	eEthRes		     phyRes;
	int	        	 success=0;


	phyRes=EthPhyNegotiationComplete(0);	// see if negotiation complete
	if(phyRes==ETH_RES_OK)
	{	
		linkStat=EthPhyGetNegotiationResult(&linkFlags, &pauseType);
		if(linkStat&ETH_LINK_ST_UP)
		{	// negotiation succeeded; properly update the MAC
            linkFlags|=(EthPhyGetHwConfigFlags()&ETH_PHY_CFG_RMII)?ETH_OPEN_RMII:ETH_OPEN_MII;                       
			EthMACOpen(linkFlags, pauseType);
			success=1;
		}
	}

	return success;
}



#endif	// defined(__PIC32MX__) && defined(_ETH)


