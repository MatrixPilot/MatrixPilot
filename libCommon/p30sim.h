/* ------------------------------ */

/* Generic Header File for pic30  */

/* ------------------------------ */


#ifndef __P30SIM_H
#define __P30SIM_H


/* ------------------------- */ 
/* Core Register Definitions */
/* ------------------------- */

/* W registers W0-W15 */
extern volatile unsigned int WREG0 __attribute__((__sfr__,__deprecated__,__unsafe__));
extern volatile unsigned int WREG1 __attribute__((__sfr__,__deprecated__,__unsafe__));
extern volatile unsigned int WREG2 __attribute__((__sfr__,__deprecated__,__unsafe__));
extern volatile unsigned int WREG3 __attribute__((__sfr__,__deprecated__,__unsafe__));
extern volatile unsigned int WREG4 __attribute__((__sfr__,__deprecated__,__unsafe__));
extern volatile unsigned int WREG5 __attribute__((__sfr__,__deprecated__,__unsafe__));
extern volatile unsigned int WREG6 __attribute__((__sfr__,__deprecated__,__unsafe__));
extern volatile unsigned int WREG7 __attribute__((__sfr__,__deprecated__,__unsafe__));
extern volatile unsigned int WREG8 __attribute__((__sfr__,__deprecated__,__unsafe__));
extern volatile unsigned int WREG9 __attribute__((__sfr__,__deprecated__,__unsafe__));
extern volatile unsigned int WREG10 __attribute__((__sfr__,__deprecated__,__unsafe__));
extern volatile unsigned int WREG11 __attribute__((__sfr__,__deprecated__,__unsafe__));
extern volatile unsigned int WREG12 __attribute__((__sfr__,__deprecated__,__unsafe__));
extern volatile unsigned int WREG13 __attribute__((__sfr__,__deprecated__,__unsafe__));
extern volatile unsigned int WREG14 __attribute__((__sfr__,__deprecated__,__unsafe__));
extern volatile unsigned int WREG15 __attribute__((__sfr__,__deprecated__,__unsafe__));

/* SPLIM: Stack Pointer Limit */
extern volatile unsigned int SPLIM __attribute__((__sfr__));

/* Alternative access structure for the 40-bit accumulators */
typedef struct tagACC {
    unsigned int L;
    unsigned int H;
    unsigned char U;
} ACC;

/* Acc A<15:0> */
extern volatile unsigned int ACCAL __attribute__((__sfr__));

/* Acc A<31:16> */
extern volatile unsigned int ACCAH __attribute__((__sfr__));

/* Acc A<39:32> */
extern volatile unsigned char ACCAU __attribute__((__sfr__));

/* Acc A defined as a structure consisting of the 3 parts */
extern volatile ACC ACCA __attribute__((__sfr__));

/* Acc B<15:0> */
extern volatile unsigned int ACCBL __attribute__((__sfr__));

/* Acc B<31:16> */
extern volatile unsigned int ACCBH __attribute__((__sfr__));

/* Acc B<39:32> */
extern volatile unsigned char ACCBU __attribute__((__sfr__));

/* Acc B defined as a structure consisting of the 3 parts */
extern volatile ACC ACCB __attribute__((__sfr__));

/* PCL: Program Counter low word */ 
extern volatile unsigned int PCL __attribute__((__sfr__));

/* PCH: Program Counter high byte */
extern volatile unsigned char PCH __attribute__((__sfr__));

/* TBLPAG: Table Page Register */
extern volatile unsigned char TBLPAG __attribute__((__sfr__));

/* PSVPAG: Program Space Visibility Page Register */
extern volatile unsigned char PSVPAG __attribute__((__sfr__));

/* RCOUNT: REPEAT loop count */
extern volatile unsigned int RCOUNT __attribute__((__sfr__));

/* DCOUNT: DO loop count */
extern volatile unsigned int DCOUNT __attribute__((__sfr__));

/* DOSTARTL: DO loop start address bits <15:0> */
extern volatile unsigned int DOSTARTL __attribute__((__sfr__));

/* DOSTARTH: DO loop start address bits <23:16> */
extern volatile unsigned int DOSTARTH __attribute__((__sfr__));

/* DOENDL: DO loop end address bits <15:0> */
extern volatile unsigned int DOENDL __attribute__((__sfr__));

/* DOENDH: DO loop end address bits <23:16> */
extern volatile unsigned int DOENDH __attribute__((__sfr__));

/* SR: Status Register */
extern volatile unsigned int SR __attribute__((__sfr__));
typedef struct tagSRBITS {
        unsigned C      :1;     /* Carry flag                   */
        unsigned Z      :1;     /* Sticky Zero flag             */
        unsigned OV     :1;     /* Overflow flag                */    
        unsigned N      :1;     /* Negative flag                */
        unsigned RA     :1;     /* REPEAT loop active flag      */
        unsigned IPL    :3;     /* CPU Interrupt Priority Level */
        unsigned DC     :1;     /* Digit Carry flag             */
        unsigned DA     :1;     /* DO loop active flag          */
        unsigned SAB    :1;     /* Combined A/B saturation flag */
        unsigned OAB    :1;     /* Combined A/B overflow flag   */    
        unsigned SB     :1;     /* Acc B saturation flag        */
        unsigned SA     :1;     /* Acc A saturation flag        */
        unsigned OB     :1;     /* Acc B overflow flag          */
        unsigned OA     :1;     /* Acc A overflow flag          */
} SRBITS;
extern volatile SRBITS SRbits __attribute__((__sfr__));

/* CORCON: CPU Mode control Register */
extern volatile unsigned int CORCON __attribute__((__sfr__));
typedef struct tagCORCONBITS {
        unsigned IF     :1;     /* Integer/Fractional mode              */    
        unsigned RND    :1;     /* Rounding mode                        */
        unsigned PSV    :1;     /* Program Space Visibility enable      */
        unsigned IPL3   :1;     /* CPU Interrupt Priority Level bit 3   */
        unsigned ACCSAT :1;     /* Acc saturation mode                  */
        unsigned SATDW  :1;     /* Data space write saturation enable   */
        unsigned SATB   :1;     /* Acc B saturation enable              */
        unsigned SATA   :1;     /* Acc A saturation enable              */
        unsigned DL     :3;     /* DO loop nesting level status         */
        unsigned EDT    :1;     /* Early DO loop termination control    */
        unsigned US     :1;     /* Signed/Unsigned mode                 */
        unsigned        :3;
} CORCONBITS;
extern volatile CORCONBITS CORCONbits __attribute__((__sfr__));

/* MODCON: Modulo Addressing Control Register */
extern volatile unsigned int MODCON __attribute__((__sfr__));
typedef struct tagMODCONBITS {
        unsigned XWM    :4;     /* X-RAGU/X-WAGU modulo addressing register select  */
        unsigned YWM    :4;     /* Y-RAGU modulo addressing register select         */
        unsigned BWM    :4;     /* Bit-reversed addressing register select          */
        unsigned        :2;
        unsigned YMODEN :1;     /* Y-RAGU modulo addressing enable                  */
        unsigned XMODEN :1;     /* X-RAGU/X-WAGU modulo addressing enable           */
} MODCONBITS;
extern volatile MODCONBITS MODCONbits __attribute__((__sfr__));

/* XMODSRT: X-RAGU/X-WAGU modulo buffer start address */
extern volatile unsigned int XMODSRT __attribute__((__sfr__));

/* XMODEND: X-RAGU/X-WAGU modulo buffer end address */
extern volatile unsigned int XMODEND __attribute__((__sfr__));

/* YMODSRT: Y-RAGU modulo buffer start address */
extern volatile unsigned int YMODSRT __attribute__((__sfr__));

/* YMODEND: Y-RAGU modulo buffer end address */
extern volatile unsigned int YMODEND __attribute__((__sfr__));

/* XBREV: X-WAGU Bit-reversed Addressing Control Register */
extern volatile unsigned int XBREV __attribute__((__sfr__));
typedef struct tagXBREVBITS {
        unsigned XB     :15;    /* Bit-reversed addressing register select  */
        unsigned BREN   :1;     /* Bit-reversed addressing enable           */
} XBREVBITS;
extern volatile XBREVBITS XBREVbits __attribute__((__sfr__));

/* DISICNT: Disable Interrupt Cycle Count */
extern volatile unsigned int DISICNT __attribute__((__sfr__));
typedef struct tagDISICNTBITS {
        unsigned DISICNT:14;
        unsigned        :2;
} DISICNTBITS;
extern volatile DISICNTBITS DISICNTbits __attribute__((__sfr__));


/* ----------------------------- */
/* Timer2/3 register definitions */
/* ----------------------------- */

/* Generic structure for Timer 2 and Timer 4 Control Registers */
typedef struct tagTCON_EVEN {
        unsigned        :1;
        unsigned TCS    :1;
        unsigned        :1;
        unsigned T32    :1;
        unsigned TCKPS  :2;
        unsigned TGATE  :1;
        unsigned        :6;
        unsigned TSIDL  :1;
        unsigned        :1;
        unsigned TON    :1;
} TCON_EVEN;

/* TMR2: Timer 2 Count Register */
extern volatile unsigned int TMR2 __attribute__((__sfr__));

/* T2CON: Timer 2 Control Register */
extern volatile unsigned int T2CON __attribute__((__sfr__));
extern volatile TCON_EVEN T2CONbits __attribute__((__sfr__));


/* -------------------------- */
/* UART1 register definitions */
/* -------------------------- */

/* Generic structure of entire SFR area for each UART module */
typedef struct tagUART {
        unsigned int uxmode;
        unsigned int uxsta;
        unsigned int uxtxreg;
        unsigned int uxrxreg;
        unsigned int uxbrg;
} UART, *PUART;

/* SFR blocks for each UART module */
extern volatile UART UART1 __attribute__((__sfr__));
extern volatile UART UART2 __attribute__((__sfr__));

/* Generic structure for UART Mode Registers */
typedef struct tagUxMODEBITS {
        unsigned STSEL  :1;
        unsigned PDSEL  :2;
        unsigned        :2;
        unsigned ABAUD  :1;
        unsigned LPBACK :1;
        unsigned WAKE   :1;
        unsigned        :2;
        unsigned ALTIO  :1;
        unsigned        :2;
        unsigned USIDL  :1;
        unsigned        :1;
        unsigned UARTEN :1;
} UxMODEBITS;

/* Generic structure for UART Status and Control Registers */
typedef struct tagUxSTABITS {
        unsigned URXDA  :1;
        unsigned OERR   :1;
        unsigned FERR   :1;
        unsigned PERR   :1;
        unsigned RIDLE  :1;
        unsigned ADDEN  :1;
        unsigned URXISEL:2;
        unsigned TRMT   :1;
        unsigned UTXBF  :1;
        unsigned UTXEN  :1;
        unsigned UTXBRK :1;
        unsigned        :3;
        unsigned UTXISEL:1;
} UxSTABITS; 

/* Generic structure for UART Transmit Registers */
typedef struct tagUxTXREGBITS {
        unsigned UTXREG0:1;
        unsigned UTXREG1:1;
        unsigned UTXREG2:1;
        unsigned UTXREG3:1;
        unsigned UTXREG4:1;
        unsigned UTXREG5:1;
        unsigned UTXREG6:1;
        unsigned UTXREG7:1;
        unsigned UTX8   :1;
        unsigned        :7;
} UxTXREGBITS;

/* Generic structure for UART Receive Registers */
typedef struct tagUxRXREGBITS {
        unsigned URXREG0:1;
        unsigned URXREG1:1;
        unsigned URXREG2:1;
        unsigned URXREG3:1;
        unsigned URXREG4:1;
        unsigned URXREG5:1;
        unsigned URXREG6:1;
        unsigned URXREG7:1;
        unsigned URX8   :1;
        unsigned        :7;
} UxRXREGBITS;

/* U1MODE: UART1 Mode Regsiter */
extern volatile unsigned int U1MODE __attribute__((__sfr__));
extern volatile UxMODEBITS U1MODEbits __attribute__((__sfr__));

/* U1STA: UART1 Status and Control Register */
extern volatile unsigned int U1STA __attribute__((__sfr__));
extern volatile UxSTABITS U1STAbits __attribute__((__sfr__));

/* U1TXREG: UART1 Transmit Register */
extern volatile unsigned int U1TXREG __attribute__((__sfr__));
extern volatile UxTXREGBITS U1TXREGbits __attribute__((__sfr__));

/* U1RXREG: UART1 Receive Register */
extern volatile unsigned int U1RXREG __attribute__((__sfr__));
extern volatile UxRXREGBITS U1RXREGbits __attribute__((__sfr__));   

/* U1BRG: UART1 Baud Rate Generator Register */
extern volatile unsigned int U1BRG __attribute__((__sfr__));


/* -------------------------- */
/* UART2 register definitions */
/* -------------------------- */

/* U2MODE: UART2 Mode Regsiter */
extern volatile unsigned int U2MODE __attribute__((__sfr__));
extern volatile UxMODEBITS U2MODEbits __attribute__((__sfr__));

/* U2STA: UART2 Status and Control Register */
extern volatile unsigned int U2STA __attribute__((__sfr__));
extern volatile UxSTABITS U2STAbits __attribute__((__sfr__));

/* U2TXREG: UART2 Transmit Register */
extern volatile unsigned int U2TXREG __attribute__((__sfr__));
extern volatile UxTXREGBITS U2TXREGbits __attribute__((__sfr__));

/* U2RXREG: UART2 Receive Register */
extern volatile unsigned int U2RXREG __attribute__((__sfr__));
extern volatile UxRXREGBITS U2RXREGbits __attribute__((__sfr__));   

/* U2BRG: UART2 Baud Rate Generator Register */
extern volatile unsigned int U2BRG __attribute__((__sfr__));


/* -------------------------------------------- */ 
/* Defines for unique SFR bit names             */
/* -------------------------------------------- */

/* SR */
#define _C SRbits.C
#define _Z SRbits.Z
#define _OV SRbits.OV
#define _N SRbits.N
#define _RA SRbits.RA
#define _IPL SRbits.IPL
#define _DC SRbits.DC
#define _DA SRbits.DA
#define _SAB SRbits.SAB
#define _OAB SRbits.OAB
#define _SB SRbits.SB
#define _SA SRbits.SA
#define _OB SRbits.OB
#define _OA SRbits.OA

/* CORCON */
#define _IF CORCONbits.IF
#define _RND CORCONbits.RND
#define _PSV CORCONbits.PSV
#define _IPL3 CORCONbits.IPL3
#define _ACCSAT CORCONbits.ACCSAT
#define _SATDW CORCONbits.SATDW
#define _SATB CORCONbits.SATB
#define _SATA CORCONbits.SATA
#define _DL CORCONbits.DL
#define _EDT CORCONbits.EDT
#define _US CORCONbits.US

/* MODCON */
#define _XWM MODCONbits.XWM
#define _YWM MODCONbits.YWM
#define _BWM MODCONbits.BWM
#define _YMODEN MODCONbits.YMODEN
#define _XMODEN MODCONbits.XMODEN

/* XBREV */
#define _XB XBREVbits.XB
#define _BREN XBREVbits.BREN

#endif

