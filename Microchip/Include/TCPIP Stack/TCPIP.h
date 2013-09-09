/*********************************************************************
 *
 *  Microchip TCP/IP Stack Include File
 *
 *********************************************************************
 * FileName:        TCPIP.h
 * Dependencies:    
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 * Compiler:        Microchip C32 v1.05 or higher
 *					Microchip C30 v3.12 or higher
 *					Microchip C18 v3.30 or higher
 *					HI-TECH PICC-18 PRO 9.63PL2 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright (C) 2002-2009 Microchip Technology Inc.  All rights
 * reserved.
 *
 * Microchip licenses to you the right to use, modify, copy, and
 * distribute:
 * (i)  the Software when embedded on a Microchip microcontroller or
 *      digital signal controller product ("Device") which is
 *      integrated into Licensee's product; or
 * (ii) ONLY the Software driver source files ENC28J60.c, ENC28J60.h,
 *		ENCX24J600.c and ENCX24J600.h ported to a non-Microchip device
 *		used in conjunction with a Microchip ethernet controller for
 *		the sole purpose of interfacing with the ethernet controller.
 *
 * You should refer to the license agreement accompanying this
 * Software for additional information regarding your rights and
 * obligations.
 *
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * MICROCHIP BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 *
 * V5.36 ---- STACK_USE_MPFS has been removed.
 ********************************************************************/
#ifndef __TCPIP_HITECH_WORKAROUND_H
#define __TCPIP_HITECH_WORKAROUND_H

#define TCPIP_STACK_VERSION 		"v5.42"		// TCP/IP stack version

#include <string.h>
#include <stdlib.h>
#include "GenericTypeDefs.h"
#include "Compiler.h"
#include "HardwareProfile.h"

// RESERVED FEATURE -- do not change from current value of 1u as this is not 
// fully implemented yet.
// Defines the number of different network interfaces to support (ex: 2 for 
// Wifi and Ethernet simultaneously).
#define NETWORK_INTERFACES		(1u)	

/*******************************************************************
 * Memory Configuration
 *   The following section sets up the memory types for use by
 *   this application.
 *******************************************************************/
	// Represents data stored in Ethernet buffer RAM
	#define TCP_ETH_RAM	0u
	// The base address for TCP data in Ethernet RAM
	#define TCP_ETH_RAM_BASE_ADDRESS			(BASE_TCB_ADDR)
	// Represents data stored in local PIC RAM
	#define TCP_PIC_RAM	1u
	// The base address for TCP data in PIC RAM
	#define TCP_PIC_RAM_BASE_ADDRESS			((PTR_BASE)&TCPBufferInPIC[0])
	// Represents data stored in external SPI RAM
	#define TCP_SPI_RAM	2u

/*******************************************************************
 * User Configuration
 *   Load the user-specific configuration from TCPIPConfig.h
 *******************************************************************/
#include "TCPIPConfig.h"

/*******************************************************************
 * Configuration Rules Enforcement
 *   The following section enforces requirements for modules based 
 *   on configurations selected in TCPIPConfig.h
 *******************************************************************/

#ifndef STACK_USE_MDD

	
	
	#if defined(STACK_USE_HTTP2_SERVER) || defined(STACK_USE_FTP_SERVER)
		#define STACK_USE_MPFS2
	#endif

	#if defined(STACK_USE_SNMPV3_SERVER) && !defined (STACK_USE_SNMP_SERVER)
		#define STACK_USE_SNMP_SERVER
	#endif

	#if defined(STACK_USE_SNMP_SERVER) //&& !defined(STACK_USE_MPFS) && !defined(STACK_USE_MPFS2)
		#define STACK_USE_MPFS2
	#endif

	#if defined(STACK_USE_SNMP_SERVER) && defined (STACK_USE_SNMPV3_SERVER)
		#define STACK_USE_MD5
		#define STACK_USE_SHA1
	#endif
#endif
	
	// FTP is not supported in MPFS2 or when MPFS is stored in internal program 
	// memory (instead of external EEPROM).
	#if ( (!defined(MPFS_USE_EEPROM) && !defined(MPFS_USE_SPI_FLASH)) || defined(STACK_USE_MPFS2) ) && defined(STACK_USE_FTP)
		#error FTP server is not supported with HTTP2 / MPFS2, or with internal Flash memory storage
	#endif
	
	// When IP Gleaning is enabled, ICMP must also be enabled.
	#if defined(STACK_USE_IP_GLEANING)
	    #if !defined(STACK_USE_ICMP_SERVER)
	        #define STACK_USE_ICMP_SERVER
	    #endif
	#endif
	
	// Include modules required by specific HTTP demos
	#if !defined(STACK_USE_HTTP2_SERVER)
		#undef STACK_USE_HTTP_EMAIL_DEMO
		#undef STACK_USE_HTTP_MD5_DEMO
		#undef STACK_USE_HTTP_APP_RECONFIG
	#endif
	#if defined(STACK_USE_HTTP_EMAIL_DEMO)
		#if !defined(STACK_USE_SMTP_CLIENT)
			#error HTTP E-mail Demo requires SMTP_CLIENT and HTTP2
		#endif
	#endif
	#if defined(STACK_USE_HTTP_MD5_DEMO)
		#if !defined(STACK_USE_MD5)
			#define STACK_USE_MD5
		#endif
	#endif
	
	// Can't do MPFS upload without POST or external memory
	#if defined(HTTP_MPFS_UPLOAD)
		#if !defined(HTTP_USE_POST) || (!defined(MPFS_USE_EEPROM) && !defined(MPFS_USE_SPI_FLASH))
			#undef HTTP_MPFS_UPLOAD
		#endif
	#endif
	
	// Make sure that the DNS client is enabled if services require it
	#if defined(STACK_USE_GENERIC_TCP_CLIENT_EXAMPLE) || \
		defined(STACK_USE_SNTP_CLIENT) || \
		defined(STACK_USE_DYNAMICDNS_CLIENT) || \
		defined(STACK_USE_SMTP_CLIENT) || \
		defined(STACK_USE_AUTOUPDATE_TCPCLIENT)
	    #if !defined(STACK_USE_DNS)
	        #define STACK_USE_DNS
	    #endif
	#endif
	
	// Make sure that STACK_CLIENT_MODE is defined if a service 
	// depends on it
	#if defined(STACK_USE_FTP_SERVER) || \
		defined(STACK_USE_SNMP_SERVER) || \
		defined(STACK_USE_DNS) || \
		defined(STACK_USE_GENERIC_TCP_CLIENT_EXAMPLE) || \
		defined(STACK_USE_TFTP_CLIENT) || \
		defined(STACK_USE_SMTP_CLIENT) || \
		defined(STACK_USE_ICMP_CLIENT) || \
		defined(STACK_USE_DYNAMICDNS_CLIENT) || \
		defined(STACK_USE_SNTP_CLIENT) || \
		defined(STACK_USE_BERKELEY_API) || \
		defined(STACK_USE_SSL_CLIENT) || \
        defined(STACK_USE_AUTO_IP)
		#if !defined(STACK_CLIENT_MODE)
		    #define STACK_CLIENT_MODE
		#endif
	#endif
	
	// Make sure that STACK_USE_TCP is defined if a service 
	// depends on it
	#if defined(STACK_USE_UART2TCP_BRIDGE) || \
		defined(STACK_USE_HTTP2_SERVER) || \
		defined(STACK_USE_FTP_SERVER) || \
		defined(STACK_USE_TELNET_SERVER) || \
		defined(STACK_USE_GENERIC_TCP_CLIENT_EXAMPLE) || \
		defined(STACK_USE_GENERIC_TCP_SERVER_EXAMPLE) || \
		defined(STACK_USE_SMTP_CLIENT) || \
		defined(STACK_USE_TCP_PERFORMANCE_TEST) || \
		defined(STACK_USE_DYNAMICDNS_CLIENT) || \
		defined(STACK_USE_BERKELEY_API) || \
		defined(STACK_USE_SSL_CLIENT) || \
		defined(STACK_USE_SSL_SERVER)
	    #if !defined(STACK_USE_TCP)
	        #define STACK_USE_TCP
	    #endif
	#endif
	
	// If TCP is not enabled, clear all memory allocations
	#if !defined(STACK_USE_TCP)
		#undef TCP_ETH_RAM_SIZE
		#undef TCP_PIC_RAM_SIZE
		#undef TCP_SPI_RAM_SIZE
		#define TCP_ETH_RAM_SIZE 0u
		#define TCP_PIC_RAM_SIZE 0u
		#define TCP_SPI_RAM_SIZE 0u
	#endif
	
	// If PIC RAM is used to store TCP socket FIFOs and TCBs, 
	// let's allocate it so the linker dynamically chooses 
	// where to locate it and prevents other variables from 
	// overlapping with it
	#if defined(__TCP_C) && TCP_PIC_RAM_SIZE > 0u
		#if defined(__18CXX) && !defined(HI_TECH_C)
			#pragma udata TCPSocketMemory
		#endif
		static BYTE TCPBufferInPIC[TCP_PIC_RAM_SIZE] __attribute__((far));
		#if defined(__18CXX) && !defined(HI_TECH_C)
			#pragma udata
		#endif
	#endif
	
	// Make sure that STACK_USE_UDP is defined if a service 
	// depends on it
	#if defined(STACK_USE_DHCP_CLIENT) || \
		defined(STACK_USE_DHCP_SERVER) || \
		defined(STACK_USE_DNS) || \
		defined(STACK_USE_NBNS) || \
		defined(STACK_USE_SNMP_SERVER) || \
		defined(STACK_USE_TFTP_CLIENT) || \
		defined(STACK_USE_ANNOUNCE) || \
		defined(STACK_USE_UDP_PERFORMANCE_TEST) || \
		defined(STACK_USE_SNTP_CLIENT) || \
		defined(STACK_USE_BERKELEY_API)
	    #if !defined(STACK_USE_UDP)
	        #define STACK_USE_UDP
	    #endif
	#endif

	// When using SSL server, enable RSA decryption
	#if defined(STACK_USE_SSL_SERVER)
		#define STACK_USE_RSA_DECRYPT
		#define STACK_USE_SSL
	#endif
	
	// When using SSL client, enable RSA encryption
	#if defined(STACK_USE_SSL_CLIENT)
		#define STACK_USE_RSA_ENCRYPT
		#define STACK_USE_SSL
	#endif

	// If using SSL (either), include the rest of the support modules
	#if defined(STACK_USE_SSL)
		#define STACK_USE_ARCFOUR
		#define STACK_USE_MD5
		#define STACK_USE_SHA1
		#define STACK_USE_RANDOM
	#endif

	// When using either RSA operation, include the RSA module
	#if defined(STACK_USE_RSA_ENCRYPT) || defined(STACK_USE_RSA_DECRYPT)
		#define STACK_USE_RSA
		#define STACK_USE_BIGINT
	#endif

	// Enable the LCD if configured in the hardware profile
	#if defined(LCD_DATA_IO) || defined(LCD_DATA0_IO)
		#define USE_LCD
	#endif
	
	// SPI Flash MPFS images must start on a block boundary
	#if (defined(STACK_USE_MPFS2)) && \
		defined(MPFS_USE_SPI_FLASH) && ((MPFS_RESERVE_BLOCK & 0x0fff) != 0)
		#error MPFS_RESERVE_BLOCK must be a multiple of 4096 for SPI Flash storage
	#endif
	
	// HTTP2 requires 2 MPFS2 handles per connection, plus one spare
	#if defined(STACK_USE_HTTP2_SERVER)
		#if MAX_MPFS_HANDLES < ((MAX_HTTP_CONNECTIONS * 2) + 1)
			#error HTTP2 requires 2 MPFS2 file handles per connection, plus one additional.
		#endif
	#endif

#include "TCPIP Stack/StackTsk.h"
#include "TCPIP Stack/Helpers.h"
#include "TCPIP Stack/Delay.h"
#include "TCPIP Stack/Tick.h"
#include "TCPIP Stack/MAC.h"
#include "TCPIP Stack/IP.h"
#include "TCPIP Stack/ARP.h"

#if defined(STACK_USE_BIGINT)
	#include "TCPIP Stack/BigInt.h"
#endif

#if defined(STACK_USE_RSA)
	#include "TCPIP Stack/RSA.h"
#endif

#if defined(STACK_USE_ARCFOUR)
	#include "TCPIP Stack/ARCFOUR.h"
#endif

#if defined(STACK_USE_AUTO_IP)
    #include "TCPIP Stack/AutoIP.h"
#endif

#if defined(STACK_USE_RANDOM)
	#include "TCPIP Stack/Random.h"
#endif

#if defined(STACK_USE_MD5) || defined(STACK_USE_SHA1)
	#include "TCPIP Stack/Hashes.h"
#endif

	#include "TCPIP Stack/XEEPROM.h"
	#include "TCPIP Stack/SPIFlash.h"
	#include "TCPIP Stack/SPIRAM.h"

#if defined(STACK_USE_UDP)
	#include "TCPIP Stack/UDP.h"
#endif

#if defined(STACK_USE_TCP)
	#include "TCPIP Stack/TCP.h"
#endif

#if defined(STACK_USE_BERKELEY_API)
	#include "TCPIP Stack/BerkeleyAPI.h"
#endif

#if defined(USE_LCD)
	#include "TCPIP Stack/LCDBlocking.h"
#endif

#if defined(STACK_USE_UART2TCP_BRIDGE)
	#include "TCPIP Stack/UART2TCPBridge.h"
#endif

#if defined(STACK_USE_UART)
	#include "TCPIP Stack/UART.h"
#endif

#if defined(STACK_USE_DHCP_CLIENT) || defined(STACK_USE_DHCP_SERVER)
	#include "TCPIP Stack/DHCP.h"
#endif

#if defined(STACK_USE_DNS) || defined(STACK_USE_DNS_SERVER)
	#include "TCPIP Stack/DNS.h"
#endif

#if defined(STACK_USE_MPFS2)
	#include "TCPIP Stack/MPFS2.h"
#endif

#if defined(STACK_USE_FTP_SERVER)
	#include "TCPIP Stack/FTP.h"
#endif


#if defined(STACK_USE_HTTP2_SERVER)
	#ifdef STACK_USE_MDD
		#include "TCPIP Stack/FileSystem.h"
		#include "TCPIP Stack/_HTTP2.h"
	#else
		#include "TCPIP Stack/HTTP2.h"
	#endif
#endif

#if defined(STACK_USE_ICMP_SERVER) || defined(STACK_USE_ICMP_CLIENT)
	#include "TCPIP Stack/ICMP.h"
#endif

#if defined(STACK_USE_ANNOUNCE)
	#include "TCPIP Stack/Announce.h"
#endif

#if defined(STACK_USE_SNMP_SERVER)
	#include "TCPIP Stack/SNMP.h"
	#include "mib.h"
#endif

#if defined(STACK_USE_NBNS)
	#include "TCPIP Stack/NBNS.h"
#endif

#if defined(STACK_USE_DNS)
	#include "TCPIP Stack/DNS.h"
#endif

#if defined(STACK_USE_DYNAMICDNS_CLIENT)
	#include "TCPIP Stack/DynDNS.h"
#endif

#if defined(STACK_USE_TELNET_SERVER)
	#include "TCPIP Stack/Telnet.h"
#endif

#if defined(STACK_USE_SMTP_CLIENT)
	#include "TCPIP Stack/SMTP.h"
#endif

#if defined(STACK_USE_TFTP_CLIENT)
	#include "TCPIP Stack/TFTPc.h"
#endif

#if defined(STACK_USE_REBOOT_SERVER)
	#include "TCPIP Stack/Reboot.h"
#endif

#if defined(STACK_USE_SNTP_CLIENT)
	#include "TCPIP Stack/SNTP.h"
#endif

#if defined(STACK_USE_UDP_PERFORMANCE_TEST)
	#include "TCPIP Stack/UDPPerformanceTest.h"
#endif

#if defined(STACK_USE_TCP_PERFORMANCE_TEST)
	#include "TCPIP Stack/TCPPerformanceTest.h"
#endif

#if defined(STACK_USE_SSL)
	#include "TCPIP Stack/SSL.h"
#endif

#if defined(WF_CS_TRIS)
    #include "TCPIP Stack/WFMac.h"
#endif
#endif
