# 1 "../main.c"
# 1 "/home/markw/Dropbox/autopilots/UAVdevboard/UDB4/mplabx_projects/MPQpid400/MatrixPilotQuad/MPQpid400.X//"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "../main.c"
# 22 "../main.c"
# 1 "../../libDCM/libDCM.h" 1
# 26 "../../libDCM/libDCM.h"
# 1 "../../libDCM/../libUDB/libUDB.h" 1
# 26 "../../libDCM/../libUDB/libUDB.h"
# 1 "../../MatrixPilotQuad/options.h" 1
# 27 "../../libDCM/../libUDB/libUDB.h" 2
# 1 "../../libDCM/../libUDB/fixDeps.h" 1
# 28 "../../libDCM/../libUDB/libUDB.h" 2
# 1 "../../libDCM/../libUDB/libUDB_defines.h" 1
# 27 "../../libDCM/../libUDB/libUDB_defines.h"
struct bb { unsigned char B0 ; unsigned char B1 ; } ;
struct bbbb { unsigned char B0 ; unsigned char B1 ; unsigned char B2 ; unsigned char B3 ; } ;
struct ww { int W0 ; int W1 ; } ;

union intbb { int BB ; struct bb _ ; } ;
union longbbbb { long WW ; struct ww _ ; struct bbbb __ ; } ;
union longww { long WW ; struct ww _ ; } ;
# 67 "../../libDCM/../libUDB/libUDB_defines.h"
# 1 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/dsPIC33F/h/p33FJ256GP710A.h" 1 3 4
# 14 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/dsPIC33F/h/p33FJ256GP710A.h" 3 4
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
extern volatile unsigned int SPLIM __attribute__((__sfr__));
extern volatile unsigned int ACCAL __attribute__((__sfr__));
extern volatile unsigned int ACCAH __attribute__((__sfr__));
extern volatile unsigned char ACCAU __attribute__((__sfr__));
extern volatile unsigned int ACCBL __attribute__((__sfr__));
extern volatile unsigned int ACCBH __attribute__((__sfr__));
extern volatile unsigned char ACCBU __attribute__((__sfr__));
extern volatile unsigned int PCL __attribute__((__sfr__));
extern volatile unsigned char PCH __attribute__((__sfr__));
extern volatile unsigned char TBLPAG __attribute__((__sfr__));
extern volatile unsigned char PSVPAG __attribute__((__sfr__));
extern volatile unsigned int RCOUNT __attribute__((__sfr__));
extern volatile unsigned int DCOUNT __attribute__((__sfr__));
extern volatile unsigned int DOSTARTL __attribute__((__sfr__));
extern volatile unsigned int DOSTARTH __attribute__((__sfr__));
extern volatile unsigned int DOENDL __attribute__((__sfr__));
extern volatile unsigned int DOENDH __attribute__((__sfr__));
extern volatile unsigned int SR __attribute__((__sfr__));
__extension__ typedef struct tagSRBITS {
  union {
    struct {
      unsigned C:1;
      unsigned Z:1;
      unsigned OV:1;
      unsigned N:1;
      unsigned RA:1;
      unsigned IPL:3;
      unsigned DC:1;
      unsigned DA:1;
      unsigned SAB:1;
      unsigned OAB:1;
      unsigned SB:1;
      unsigned SA:1;
      unsigned OB:1;
      unsigned OA:1;
    };
    struct {
      unsigned :5;
      unsigned IPL0:1;
      unsigned IPL1:1;
      unsigned IPL2:1;
    };
  };
} SRBITS;
extern volatile SRBITS SRbits __attribute__((__sfr__));

extern volatile unsigned int CORCON __attribute__((__sfr__));
__extension__ typedef struct tagCORCONBITS {
  union {
    struct {
      unsigned IF:1;
      unsigned RND:1;
      unsigned PSV:1;
      unsigned IPL3:1;
      unsigned ACCSAT:1;
      unsigned SATDW:1;
      unsigned SATB:1;
      unsigned SATA:1;
      unsigned DL:3;
      unsigned EDT:1;
      unsigned US:1;
    };
    struct {
      unsigned :8;
      unsigned DL0:1;
      unsigned DL1:1;
      unsigned DL2:1;
    };
  };
} CORCONBITS;
extern volatile CORCONBITS CORCONbits __attribute__((__sfr__));

extern volatile unsigned int MODCON __attribute__((__sfr__));
__extension__ typedef struct tagMODCONBITS {
  union {
    struct {
      unsigned XWM:4;
      unsigned YWM:4;
      unsigned BWM:4;
      unsigned :2;
      unsigned YMODEN:1;
      unsigned XMODEN:1;
    };
    struct {
      unsigned XWM0:1;
      unsigned XWM1:1;
      unsigned XWM2:1;
      unsigned XWM3:1;
      unsigned YWM0:1;
      unsigned YWM1:1;
      unsigned YWM2:1;
      unsigned YWM3:1;
      unsigned BWM0:1;
      unsigned BWM1:1;
      unsigned BWM2:1;
      unsigned BWM3:1;
    };
  };
} MODCONBITS;
extern volatile MODCONBITS MODCONbits __attribute__((__sfr__));

extern volatile unsigned int XMODSRT __attribute__((__sfr__));
extern volatile unsigned int XMODEND __attribute__((__sfr__));
extern volatile unsigned int YMODSRT __attribute__((__sfr__));
extern volatile unsigned int YMODEND __attribute__((__sfr__));
extern volatile unsigned int XBREV __attribute__((__sfr__));
__extension__ typedef struct tagXBREVBITS {
  union {
    struct {
      unsigned XB:15;
      unsigned BREN:1;
    };
    struct {
      unsigned XB0:1;
      unsigned XB1:1;
      unsigned XB2:1;
      unsigned XB3:1;
      unsigned XB4:1;
      unsigned XB5:1;
      unsigned XB6:1;
      unsigned XB7:1;
      unsigned XB8:1;
      unsigned XB9:1;
      unsigned XB10:1;
      unsigned XB11:1;
      unsigned XB12:1;
      unsigned XB13:1;
      unsigned XB14:1;
    };
  };
} XBREVBITS;
extern volatile XBREVBITS XBREVbits __attribute__((__sfr__));

extern volatile unsigned int DISICNT __attribute__((__sfr__));
extern volatile unsigned int CNEN1 __attribute__((__sfr__));
typedef struct tagCNEN1BITS {
  unsigned CN0IE:1;
  unsigned CN1IE:1;
  unsigned CN2IE:1;
  unsigned CN3IE:1;
  unsigned CN4IE:1;
  unsigned CN5IE:1;
  unsigned CN6IE:1;
  unsigned CN7IE:1;
  unsigned CN8IE:1;
  unsigned CN9IE:1;
  unsigned CN10IE:1;
  unsigned CN11IE:1;
  unsigned CN12IE:1;
  unsigned CN13IE:1;
  unsigned CN14IE:1;
  unsigned CN15IE:1;
} CNEN1BITS;
extern volatile CNEN1BITS CNEN1bits __attribute__((__sfr__));

extern volatile unsigned int CNEN2 __attribute__((__sfr__));
typedef struct tagCNEN2BITS {
  unsigned CN16IE:1;
  unsigned CN17IE:1;
  unsigned CN18IE:1;
  unsigned CN19IE:1;
  unsigned CN20IE:1;
  unsigned CN21IE:1;
  unsigned CN22IE:1;
  unsigned CN23IE:1;
} CNEN2BITS;
extern volatile CNEN2BITS CNEN2bits __attribute__((__sfr__));

extern volatile unsigned int CNPU1 __attribute__((__sfr__));
typedef struct tagCNPU1BITS {
  unsigned CN0PUE:1;
  unsigned CN1PUE:1;
  unsigned CN2PUE:1;
  unsigned CN3PUE:1;
  unsigned CN4PUE:1;
  unsigned CN5PUE:1;
  unsigned CN6PUE:1;
  unsigned CN7PUE:1;
  unsigned CN8PUE:1;
  unsigned CN9PUE:1;
  unsigned CN10PUE:1;
  unsigned CN11PUE:1;
  unsigned CN12PUE:1;
  unsigned CN13PUE:1;
  unsigned CN14PUE:1;
  unsigned CN15PUE:1;
} CNPU1BITS;
extern volatile CNPU1BITS CNPU1bits __attribute__((__sfr__));

extern volatile unsigned int CNPU2 __attribute__((__sfr__));
typedef struct tagCNPU2BITS {
  unsigned CN16PUE:1;
  unsigned CN17PUE:1;
  unsigned CN18PUE:1;
  unsigned CN19PUE:1;
  unsigned CN20PUE:1;
  unsigned CN21PUE:1;
  unsigned CN22PUE:1;
  unsigned CN23PUE:1;
} CNPU2BITS;
extern volatile CNPU2BITS CNPU2bits __attribute__((__sfr__));

extern volatile unsigned int INTCON1 __attribute__((__sfr__));
typedef struct tagINTCON1BITS {
  unsigned :1;
  unsigned OSCFAIL:1;
  unsigned STKERR:1;
  unsigned ADDRERR:1;
  unsigned MATHERR:1;
  unsigned DMACERR:1;
  unsigned DIV0ERR:1;
  unsigned SFTACERR:1;
  unsigned COVTE:1;
  unsigned OVBTE:1;
  unsigned OVATE:1;
  unsigned COVBERR:1;
  unsigned COVAERR:1;
  unsigned OVBERR:1;
  unsigned OVAERR:1;
  unsigned NSTDIS:1;
} INTCON1BITS;
extern volatile INTCON1BITS INTCON1bits __attribute__((__sfr__));

extern volatile unsigned int INTCON2 __attribute__((__sfr__));
typedef struct tagINTCON2BITS {
  unsigned INT0EP:1;
  unsigned INT1EP:1;
  unsigned INT2EP:1;
  unsigned INT3EP:1;
  unsigned INT4EP:1;
  unsigned :9;
  unsigned DISI:1;
  unsigned ALTIVT:1;
} INTCON2BITS;
extern volatile INTCON2BITS INTCON2bits __attribute__((__sfr__));

extern volatile unsigned int IFS0 __attribute__((__sfr__));
typedef struct tagIFS0BITS {
  unsigned INT0IF:1;
  unsigned IC1IF:1;
  unsigned OC1IF:1;
  unsigned T1IF:1;
  unsigned DMA0IF:1;
  unsigned IC2IF:1;
  unsigned OC2IF:1;
  unsigned T2IF:1;
  unsigned T3IF:1;
  unsigned SPI1EIF:1;
  unsigned SPI1IF:1;
  unsigned U1RXIF:1;
  unsigned U1TXIF:1;
  unsigned AD1IF:1;
  unsigned DMA1IF:1;
} IFS0BITS;
extern volatile IFS0BITS IFS0bits __attribute__((__sfr__));

extern volatile unsigned int IFS1 __attribute__((__sfr__));
__extension__ typedef struct tagIFS1BITS {
  union {
    struct {
      unsigned SI2C1IF:1;
      unsigned MI2C1IF:1;
      unsigned :1;
      unsigned CNIF:1;
      unsigned INT1IF:1;
      unsigned AD2IF:1;
      unsigned IC7IF:1;
      unsigned IC8IF:1;
      unsigned DMA2IF:1;
      unsigned OC3IF:1;
      unsigned OC4IF:1;
      unsigned T4IF:1;
      unsigned T5IF:1;
      unsigned INT2IF:1;
      unsigned U2RXIF:1;
      unsigned U2TXIF:1;
    };
    struct {
      unsigned SI2CIF:1;
    };
  };
} IFS1BITS;
extern volatile IFS1BITS IFS1bits __attribute__((__sfr__));

extern volatile unsigned int IFS2 __attribute__((__sfr__));
typedef struct tagIFS2BITS {
  unsigned SPI2EIF:1;
  unsigned SPI2IF:1;
  unsigned C1RXIF:1;
  unsigned C1IF:1;
  unsigned DMA3IF:1;
  unsigned IC3IF:1;
  unsigned IC4IF:1;
  unsigned IC5IF:1;
  unsigned IC6IF:1;
  unsigned OC5IF:1;
  unsigned OC6IF:1;
  unsigned OC7IF:1;
  unsigned OC8IF:1;
  unsigned :1;
  unsigned DMA4IF:1;
  unsigned T6IF:1;
} IFS2BITS;
extern volatile IFS2BITS IFS2bits __attribute__((__sfr__));

extern volatile unsigned int IFS3 __attribute__((__sfr__));
typedef struct tagIFS3BITS {
  unsigned T7IF:1;
  unsigned SI2C2IF:1;
  unsigned MI2C2IF:1;
  unsigned T8IF:1;
  unsigned T9IF:1;
  unsigned INT3IF:1;
  unsigned INT4IF:1;
  unsigned C2RXIF:1;
  unsigned C2IF:1;
  unsigned :2;
  unsigned DCIEIF:1;
  unsigned DCIIF:1;
  unsigned DMA5IF:1;
} IFS3BITS;
extern volatile IFS3BITS IFS3bits __attribute__((__sfr__));

extern volatile unsigned int IFS4 __attribute__((__sfr__));
typedef struct tagIFS4BITS {
  unsigned :1;
  unsigned U1EIF:1;
  unsigned U2EIF:1;
  unsigned :1;
  unsigned DMA6IF:1;
  unsigned DMA7IF:1;
  unsigned C1TXIF:1;
  unsigned C2TXIF:1;
} IFS4BITS;
extern volatile IFS4BITS IFS4bits __attribute__((__sfr__));

extern volatile unsigned int IEC0 __attribute__((__sfr__));
typedef struct tagIEC0BITS {
  unsigned INT0IE:1;
  unsigned IC1IE:1;
  unsigned OC1IE:1;
  unsigned T1IE:1;
  unsigned DMA0IE:1;
  unsigned IC2IE:1;
  unsigned OC2IE:1;
  unsigned T2IE:1;
  unsigned T3IE:1;
  unsigned SPI1EIE:1;
  unsigned SPI1IE:1;
  unsigned U1RXIE:1;
  unsigned U1TXIE:1;
  unsigned AD1IE:1;
  unsigned DMA1IE:1;
} IEC0BITS;
extern volatile IEC0BITS IEC0bits __attribute__((__sfr__));

extern volatile unsigned int IEC1 __attribute__((__sfr__));
__extension__ typedef struct tagIEC1BITS {
  union {
    struct {
      unsigned SI2C1IE:1;
      unsigned MI2C1IE:1;
      unsigned :1;
      unsigned CNIE:1;
      unsigned INT1IE:1;
      unsigned AD2IE:1;
      unsigned IC7IE:1;
      unsigned IC8IE:1;
      unsigned DMA2IE:1;
      unsigned OC3IE:1;
      unsigned OC4IE:1;
      unsigned T4IE:1;
      unsigned T5IE:1;
      unsigned INT2IE:1;
      unsigned U2RXIE:1;
      unsigned U2TXIE:1;
    };
    struct {
      unsigned SI2CIE:1;
    };
  };
} IEC1BITS;
extern volatile IEC1BITS IEC1bits __attribute__((__sfr__));

extern volatile unsigned int IEC2 __attribute__((__sfr__));
typedef struct tagIEC2BITS {
  unsigned SPI2EIE:1;
  unsigned SPI2IE:1;
  unsigned C1RXIE:1;
  unsigned C1IE:1;
  unsigned DMA3IE:1;
  unsigned IC3IE:1;
  unsigned IC4IE:1;
  unsigned IC5IE:1;
  unsigned IC6IE:1;
  unsigned OC5IE:1;
  unsigned OC6IE:1;
  unsigned OC7IE:1;
  unsigned OC8IE:1;
  unsigned :1;
  unsigned DMA4IE:1;
  unsigned T6IE:1;
} IEC2BITS;
extern volatile IEC2BITS IEC2bits __attribute__((__sfr__));

extern volatile unsigned int IEC3 __attribute__((__sfr__));
typedef struct tagIEC3BITS {
  unsigned T7IE:1;
  unsigned SI2C2IE:1;
  unsigned MI2C2IE:1;
  unsigned T8IE:1;
  unsigned T9IE:1;
  unsigned INT3IE:1;
  unsigned INT4IE:1;
  unsigned C2RXIE:1;
  unsigned C2IE:1;
  unsigned :2;
  unsigned DCIEIE:1;
  unsigned DCIIE:1;
  unsigned DMA5IE:1;
} IEC3BITS;
extern volatile IEC3BITS IEC3bits __attribute__((__sfr__));

extern volatile unsigned int IEC4 __attribute__((__sfr__));
typedef struct tagIEC4BITS {
  unsigned :1;
  unsigned U1EIE:1;
  unsigned U2EIE:1;
  unsigned :1;
  unsigned DMA6IE:1;
  unsigned DMA7IE:1;
  unsigned C1TXIE:1;
  unsigned C2TXIE:1;
} IEC4BITS;
extern volatile IEC4BITS IEC4bits __attribute__((__sfr__));

extern volatile unsigned int IPC0 __attribute__((__sfr__));
__extension__ typedef struct tagIPC0BITS {
  union {
    struct {
      unsigned INT0IP:3;
      unsigned :1;
      unsigned IC1IP:3;
      unsigned :1;
      unsigned OC1IP:3;
      unsigned :1;
      unsigned T1IP:3;
    };
    struct {
      unsigned INT0IP0:1;
      unsigned INT0IP1:1;
      unsigned INT0IP2:1;
      unsigned :1;
      unsigned IC1IP0:1;
      unsigned IC1IP1:1;
      unsigned IC1IP2:1;
      unsigned :1;
      unsigned OC1IP0:1;
      unsigned OC1IP1:1;
      unsigned OC1IP2:1;
      unsigned :1;
      unsigned T1IP0:1;
      unsigned T1IP1:1;
      unsigned T1IP2:1;
    };
  };
} IPC0BITS;
extern volatile IPC0BITS IPC0bits __attribute__((__sfr__));

extern volatile unsigned int IPC1 __attribute__((__sfr__));
__extension__ typedef struct tagIPC1BITS {
  union {
    struct {
      unsigned DMA0IP:3;
      unsigned :1;
      unsigned IC2IP:3;
      unsigned :1;
      unsigned OC2IP:3;
      unsigned :1;
      unsigned T2IP:3;
    };
    struct {
      unsigned DMA0IP0:1;
      unsigned DMA0IP1:1;
      unsigned DMA0IP2:1;
      unsigned :1;
      unsigned IC2IP0:1;
      unsigned IC2IP1:1;
      unsigned IC2IP2:1;
      unsigned :1;
      unsigned OC2IP0:1;
      unsigned OC2IP1:1;
      unsigned OC2IP2:1;
      unsigned :1;
      unsigned T2IP0:1;
      unsigned T2IP1:1;
      unsigned T2IP2:1;
    };
  };
} IPC1BITS;
extern volatile IPC1BITS IPC1bits __attribute__((__sfr__));

extern volatile unsigned int IPC2 __attribute__((__sfr__));
__extension__ typedef struct tagIPC2BITS {
  union {
    struct {
      unsigned T3IP:3;
      unsigned :1;
      unsigned SPI1EIP:3;
      unsigned :1;
      unsigned SPI1IP:3;
      unsigned :1;
      unsigned U1RXIP:3;
    };
    struct {
      unsigned T3IP0:1;
      unsigned T3IP1:1;
      unsigned T3IP2:1;
      unsigned :1;
      unsigned SPI1EIP0:1;
      unsigned SPI1EIP1:1;
      unsigned SPI1EIP2:1;
      unsigned :1;
      unsigned SPI1IP0:1;
      unsigned SPI1IP1:1;
      unsigned SPI1IP2:1;
      unsigned :1;
      unsigned U1RXIP0:1;
      unsigned U1RXIP1:1;
      unsigned U1RXIP2:1;
    };
  };
} IPC2BITS;
extern volatile IPC2BITS IPC2bits __attribute__((__sfr__));

extern volatile unsigned int IPC3 __attribute__((__sfr__));
__extension__ typedef struct tagIPC3BITS {
  union {
    struct {
      unsigned U1TXIP:3;
      unsigned :1;
      unsigned AD1IP:3;
      unsigned :1;
      unsigned DMA1IP:3;
    };
    struct {
      unsigned U1TXIP0:1;
      unsigned U1TXIP1:1;
      unsigned U1TXIP2:1;
      unsigned :1;
      unsigned AD1IP0:1;
      unsigned AD1IP1:1;
      unsigned AD1IP2:1;
      unsigned :1;
      unsigned DMA1IP0:1;
      unsigned DMA1IP1:1;
      unsigned DMA1IP2:1;
    };
  };
} IPC3BITS;
extern volatile IPC3BITS IPC3bits __attribute__((__sfr__));

extern volatile unsigned int IPC4 __attribute__((__sfr__));
__extension__ typedef struct tagIPC4BITS {
  union {
    struct {
      unsigned SI2C1IP:3;
      unsigned :1;
      unsigned MI2C1IP:3;
      unsigned :5;
      unsigned CNIP:3;
    };
    struct {
      unsigned SI2C1IP0:1;
      unsigned SI2C1IP1:1;
      unsigned SI2C1IP2:1;
      unsigned :1;
      unsigned MI2C1IP0:1;
      unsigned MI2C1IP1:1;
      unsigned MI2C1IP2:1;
      unsigned :5;
      unsigned CNIP0:1;
      unsigned CNIP1:1;
      unsigned CNIP2:1;
    };
  };
} IPC4BITS;
extern volatile IPC4BITS IPC4bits __attribute__((__sfr__));

extern volatile unsigned int IPC5 __attribute__((__sfr__));
__extension__ typedef struct tagIPC5BITS {
  union {
    struct {
      unsigned INT1IP:3;
      unsigned :1;
      unsigned AD2IP:3;
      unsigned :1;
      unsigned IC7IP:3;
      unsigned :1;
      unsigned IC8IP:3;
    };
    struct {
      unsigned INT1IP0:1;
      unsigned INT1IP1:1;
      unsigned INT1IP2:1;
      unsigned :1;
      unsigned AD2IP0:1;
      unsigned AD2IP1:1;
      unsigned AD2IP2:1;
      unsigned :1;
      unsigned IC7IP0:1;
      unsigned IC7IP1:1;
      unsigned IC7IP2:1;
      unsigned :1;
      unsigned IC8IP0:1;
      unsigned IC8IP1:1;
      unsigned IC8IP2:1;
    };
  };
} IPC5BITS;
extern volatile IPC5BITS IPC5bits __attribute__((__sfr__));

extern volatile unsigned int IPC6 __attribute__((__sfr__));
__extension__ typedef struct tagIPC6BITS {
  union {
    struct {
      unsigned DMA2IP:3;
      unsigned :1;
      unsigned OC3IP:3;
      unsigned :1;
      unsigned OC4IP:3;
      unsigned :1;
      unsigned T4IP:3;
    };
    struct {
      unsigned DMA2IP0:1;
      unsigned DMA2IP1:1;
      unsigned DMA2IP2:1;
      unsigned :1;
      unsigned OC3IP0:1;
      unsigned OC3IP1:1;
      unsigned OC3IP2:1;
      unsigned :1;
      unsigned OC4IP0:1;
      unsigned OC4IP1:1;
      unsigned OC4IP2:1;
      unsigned :1;
      unsigned T4IP0:1;
      unsigned T4IP1:1;
      unsigned T4IP2:1;
    };
  };
} IPC6BITS;
extern volatile IPC6BITS IPC6bits __attribute__((__sfr__));

extern volatile unsigned int IPC7 __attribute__((__sfr__));
__extension__ typedef struct tagIPC7BITS {
  union {
    struct {
      unsigned T5IP:3;
      unsigned :1;
      unsigned INT2IP:3;
      unsigned :1;
      unsigned U2RXIP:3;
      unsigned :1;
      unsigned U2TXIP:3;
    };
    struct {
      unsigned T5IP0:1;
      unsigned T5IP1:1;
      unsigned T5IP2:1;
      unsigned :1;
      unsigned INT2IP0:1;
      unsigned INT2IP1:1;
      unsigned INT2IP2:1;
      unsigned :1;
      unsigned U2RXIP0:1;
      unsigned U2RXIP1:1;
      unsigned U2RXIP2:1;
      unsigned :1;
      unsigned U2TXIP0:1;
      unsigned U2TXIP1:1;
      unsigned U2TXIP2:1;
    };
  };
} IPC7BITS;
extern volatile IPC7BITS IPC7bits __attribute__((__sfr__));

extern volatile unsigned int IPC8 __attribute__((__sfr__));
__extension__ typedef struct tagIPC8BITS {
  union {
    struct {
      unsigned SPI2EIP:3;
      unsigned :1;
      unsigned SPI2IP:3;
      unsigned :1;
      unsigned C1RXIP:3;
      unsigned :1;
      unsigned C1IP:3;
    };
    struct {
      unsigned SPI2EIP0:1;
      unsigned SPI2EIP1:1;
      unsigned SPI2EIP2:1;
      unsigned :1;
      unsigned SPI2IP0:1;
      unsigned SPI2IP1:1;
      unsigned SPI2IP2:1;
      unsigned :1;
      unsigned C1RXIP0:1;
      unsigned C1RXIP1:1;
      unsigned C1RXIP2:1;
      unsigned :1;
      unsigned C1IP0:1;
      unsigned C1IP1:1;
      unsigned C1IP2:1;
    };
  };
} IPC8BITS;
extern volatile IPC8BITS IPC8bits __attribute__((__sfr__));

extern volatile unsigned int IPC9 __attribute__((__sfr__));
__extension__ typedef struct tagIPC9BITS {
  union {
    struct {
      unsigned DMA3IP:3;
      unsigned :1;
      unsigned IC3IP:3;
      unsigned :1;
      unsigned IC4IP:3;
      unsigned :1;
      unsigned IC5IP:3;
    };
    struct {
      unsigned DMA3IP0:1;
      unsigned DMA3IP1:1;
      unsigned DMA3IP2:1;
      unsigned :1;
      unsigned IC3IP0:1;
      unsigned IC3IP1:1;
      unsigned IC3IP2:1;
      unsigned :1;
      unsigned IC4IP0:1;
      unsigned IC4IP1:1;
      unsigned IC4IP2:1;
      unsigned :1;
      unsigned IC5IP0:1;
      unsigned IC5IP1:1;
      unsigned IC5IP2:1;
    };
  };
} IPC9BITS;
extern volatile IPC9BITS IPC9bits __attribute__((__sfr__));

extern volatile unsigned int IPC10 __attribute__((__sfr__));
__extension__ typedef struct tagIPC10BITS {
  union {
    struct {
      unsigned IC6IP:3;
      unsigned :1;
      unsigned OC5IP:3;
      unsigned :1;
      unsigned OC6IP:3;
      unsigned :1;
      unsigned OC7IP:3;
    };
    struct {
      unsigned IC6IP0:1;
      unsigned IC6IP1:1;
      unsigned IC6IP2:1;
      unsigned :1;
      unsigned OC5IP0:1;
      unsigned OC5IP1:1;
      unsigned OC5IP2:1;
      unsigned :1;
      unsigned OC6IP0:1;
      unsigned OC6IP1:1;
      unsigned OC6IP2:1;
      unsigned :1;
      unsigned OC7IP0:1;
      unsigned OC7IP1:1;
      unsigned OC7IP2:1;
    };
  };
} IPC10BITS;
extern volatile IPC10BITS IPC10bits __attribute__((__sfr__));

extern volatile unsigned int IPC11 __attribute__((__sfr__));
__extension__ typedef struct tagIPC11BITS {
  union {
    struct {
      unsigned OC8IP:3;
      unsigned :5;
      unsigned DMA4IP:3;
      unsigned :1;
      unsigned T6IP:3;
    };
    struct {
      unsigned OC8IP0:1;
      unsigned OC8IP1:1;
      unsigned OC8IP2:1;
      unsigned :5;
      unsigned DMA4IP0:1;
      unsigned DMA4IP1:1;
      unsigned DMA4IP2:1;
      unsigned :1;
      unsigned T6IP0:1;
      unsigned T6IP1:1;
      unsigned T6IP2:1;
    };
  };
} IPC11BITS;
extern volatile IPC11BITS IPC11bits __attribute__((__sfr__));

extern volatile unsigned int IPC12 __attribute__((__sfr__));
__extension__ typedef struct tagIPC12BITS {
  union {
    struct {
      unsigned T7IP:3;
      unsigned :1;
      unsigned SI2C2IP:3;
      unsigned :1;
      unsigned MI2C2IP:3;
      unsigned :1;
      unsigned T8IP:3;
    };
    struct {
      unsigned T7IP0:1;
      unsigned T7IP1:1;
      unsigned T7IP2:1;
      unsigned :1;
      unsigned SI2C2IP0:1;
      unsigned SI2C2IP1:1;
      unsigned SI2C2IP2:1;
      unsigned :1;
      unsigned MI2C2IP0:1;
      unsigned MI2C2IP1:1;
      unsigned MI2C2IP2:1;
      unsigned :1;
      unsigned T8IP0:1;
      unsigned T8IP1:1;
      unsigned T8IP2:1;
    };
  };
} IPC12BITS;
extern volatile IPC12BITS IPC12bits __attribute__((__sfr__));

extern volatile unsigned int IPC13 __attribute__((__sfr__));
__extension__ typedef struct tagIPC13BITS {
  union {
    struct {
      unsigned T9IP:3;
      unsigned :1;
      unsigned INT3IP:3;
      unsigned :1;
      unsigned INT4IP:3;
      unsigned :1;
      unsigned C2RXIP:3;
    };
    struct {
      unsigned T9IP0:1;
      unsigned T9IP1:1;
      unsigned T9IP2:1;
      unsigned :1;
      unsigned INT3IP0:1;
      unsigned INT3IP1:1;
      unsigned INT3IP2:1;
      unsigned :1;
      unsigned INT4IP0:1;
      unsigned INT4IP1:1;
      unsigned INT4IP2:1;
      unsigned :1;
      unsigned C2RXIP0:1;
      unsigned C2RXIP1:1;
      unsigned C2RXIP2:1;
    };
  };
} IPC13BITS;
extern volatile IPC13BITS IPC13bits __attribute__((__sfr__));

extern volatile unsigned int IPC14 __attribute__((__sfr__));
__extension__ typedef struct tagIPC14BITS {
  union {
    struct {
      unsigned C2IP:3;
      unsigned :9;
      unsigned DCIEIP:3;
    };
    struct {
      unsigned C2IP0:1;
      unsigned C2IP1:1;
      unsigned C2IP2:1;
      unsigned :9;
      unsigned DCIEIP0:1;
      unsigned DCIEIP1:1;
      unsigned DCIEIP2:1;
    };
  };
} IPC14BITS;
extern volatile IPC14BITS IPC14bits __attribute__((__sfr__));

extern volatile unsigned int IPC15 __attribute__((__sfr__));
__extension__ typedef struct tagIPC15BITS {
  union {
    struct {
      unsigned DCIIP:3;
      unsigned :1;
      unsigned DMA5IP:3;
    };
    struct {
      unsigned DCIIP0:1;
      unsigned DCIIP1:1;
      unsigned DCIIP2:1;
      unsigned :1;
      unsigned DMA5IP0:1;
      unsigned DMA5IP1:1;
      unsigned DMA5IP2:1;
    };
  };
} IPC15BITS;
extern volatile IPC15BITS IPC15bits __attribute__((__sfr__));

extern volatile unsigned int IPC16 __attribute__((__sfr__));
__extension__ typedef struct tagIPC16BITS {
  union {
    struct {
      unsigned :4;
      unsigned U1EIP:3;
      unsigned :1;
      unsigned U2EIP:3;
    };
    struct {
      unsigned :4;
      unsigned U1EIP0:1;
      unsigned U1EIP1:1;
      unsigned U1EIP2:1;
      unsigned :1;
      unsigned U2EIP0:1;
      unsigned U2EIP1:1;
      unsigned U2EIP2:1;
    };
  };
} IPC16BITS;
extern volatile IPC16BITS IPC16bits __attribute__((__sfr__));

extern volatile unsigned int IPC17 __attribute__((__sfr__));
__extension__ typedef struct tagIPC17BITS {
  union {
    struct {
      unsigned DMA6IP:3;
      unsigned :1;
      unsigned DMA7IP:3;
      unsigned :1;
      unsigned C1TXIP:3;
      unsigned :1;
      unsigned C2TXIP:3;
    };
    struct {
      unsigned DMA6IP0:1;
      unsigned DMA6IP1:1;
      unsigned DMA6IP2:1;
      unsigned :1;
      unsigned DMA7IP0:1;
      unsigned DMA7IP1:1;
      unsigned DMA7IP2:1;
      unsigned :1;
      unsigned C1TXIP0:1;
      unsigned C1TXIP1:1;
      unsigned C1TXIP2:1;
      unsigned :1;
      unsigned C2TXIP0:1;
      unsigned C2TXIP1:1;
      unsigned C2TXIP2:1;
    };
  };
} IPC17BITS;
extern volatile IPC17BITS IPC17bits __attribute__((__sfr__));

extern volatile unsigned int INTTREG __attribute__((__sfr__));
__extension__ typedef struct tagINTTREGBITS {
  union {
    struct {
      unsigned VECNUM:7;
      unsigned :1;
      unsigned ILR:4;
    };
    struct {
      unsigned VECNUM0:1;
      unsigned VECNUM1:1;
      unsigned VECNUM2:1;
      unsigned VECNUM3:1;
      unsigned VECNUM4:1;
      unsigned VECNUM5:1;
      unsigned VECNUM6:1;
      unsigned :1;
      unsigned ILR0:1;
      unsigned ILR1:1;
      unsigned ILR2:1;
      unsigned ILR3:1;
    };
  };
} INTTREGBITS;
extern volatile INTTREGBITS INTTREGbits __attribute__((__sfr__));

extern volatile unsigned int TMR1 __attribute__((__sfr__));
extern volatile unsigned int PR1 __attribute__((__sfr__));
extern volatile unsigned int T1CON __attribute__((__sfr__));
__extension__ typedef struct tagT1CONBITS {
  union {
    struct {
      unsigned :1;
      unsigned TCS:1;
      unsigned TSYNC:1;
      unsigned :1;
      unsigned TCKPS:2;
      unsigned TGATE:1;
      unsigned :6;
      unsigned TSIDL:1;
      unsigned :1;
      unsigned TON:1;
    };
    struct {
      unsigned :4;
      unsigned TCKPS0:1;
      unsigned TCKPS1:1;
    };
  };
} T1CONBITS;
extern volatile T1CONBITS T1CONbits __attribute__((__sfr__));

extern volatile unsigned int TMR2 __attribute__((__sfr__));
extern volatile unsigned int TMR3HLD __attribute__((__sfr__));
extern volatile unsigned int TMR3 __attribute__((__sfr__));
extern volatile unsigned int PR2 __attribute__((__sfr__));
extern volatile unsigned int PR3 __attribute__((__sfr__));
extern volatile unsigned int T2CON __attribute__((__sfr__));
__extension__ typedef struct tagT2CONBITS {
  union {
    struct {
      unsigned :1;
      unsigned TCS:1;
      unsigned :1;
      unsigned T32:1;
      unsigned TCKPS:2;
      unsigned TGATE:1;
      unsigned :6;
      unsigned TSIDL:1;
      unsigned :1;
      unsigned TON:1;
    };
    struct {
      unsigned :4;
      unsigned TCKPS0:1;
      unsigned TCKPS1:1;
    };
  };
} T2CONBITS;
extern volatile T2CONBITS T2CONbits __attribute__((__sfr__));

extern volatile unsigned int T3CON __attribute__((__sfr__));
__extension__ typedef struct tagT3CONBITS {
  union {
    struct {
      unsigned :1;
      unsigned TCS:1;
      unsigned :2;
      unsigned TCKPS:2;
      unsigned TGATE:1;
      unsigned :6;
      unsigned TSIDL:1;
      unsigned :1;
      unsigned TON:1;
    };
    struct {
      unsigned :4;
      unsigned TCKPS0:1;
      unsigned TCKPS1:1;
    };
  };
} T3CONBITS;
extern volatile T3CONBITS T3CONbits __attribute__((__sfr__));

extern volatile unsigned int TMR4 __attribute__((__sfr__));
extern volatile unsigned int TMR5HLD __attribute__((__sfr__));
extern volatile unsigned int TMR5 __attribute__((__sfr__));
extern volatile unsigned int PR4 __attribute__((__sfr__));
extern volatile unsigned int PR5 __attribute__((__sfr__));
extern volatile unsigned int T4CON __attribute__((__sfr__));
__extension__ typedef struct tagT4CONBITS {
  union {
    struct {
      unsigned :1;
      unsigned TCS:1;
      unsigned :1;
      unsigned T32:1;
      unsigned TCKPS:2;
      unsigned TGATE:1;
      unsigned :6;
      unsigned TSIDL:1;
      unsigned :1;
      unsigned TON:1;
    };
    struct {
      unsigned :4;
      unsigned TCKPS0:1;
      unsigned TCKPS1:1;
    };
  };
} T4CONBITS;
extern volatile T4CONBITS T4CONbits __attribute__((__sfr__));

extern volatile unsigned int T5CON __attribute__((__sfr__));
__extension__ typedef struct tagT5CONBITS {
  union {
    struct {
      unsigned :1;
      unsigned TCS:1;
      unsigned :2;
      unsigned TCKPS:2;
      unsigned TGATE:1;
      unsigned :6;
      unsigned TSIDL:1;
      unsigned :1;
      unsigned TON:1;
    };
    struct {
      unsigned :4;
      unsigned TCKPS0:1;
      unsigned TCKPS1:1;
    };
  };
} T5CONBITS;
extern volatile T5CONBITS T5CONbits __attribute__((__sfr__));

extern volatile unsigned int TMR6 __attribute__((__sfr__));
extern volatile unsigned int TMR7HLD __attribute__((__sfr__));
extern volatile unsigned int TMR7 __attribute__((__sfr__));
extern volatile unsigned int PR6 __attribute__((__sfr__));
extern volatile unsigned int PR7 __attribute__((__sfr__));
extern volatile unsigned int T6CON __attribute__((__sfr__));
__extension__ typedef struct tagT6CONBITS {
  union {
    struct {
      unsigned :1;
      unsigned TCS:1;
      unsigned :1;
      unsigned T32:1;
      unsigned TCKPS:2;
      unsigned TGATE:1;
      unsigned :6;
      unsigned TSIDL:1;
      unsigned :1;
      unsigned TON:1;
    };
    struct {
      unsigned :4;
      unsigned TCKPS0:1;
      unsigned TCKPS1:1;
    };
  };
} T6CONBITS;
extern volatile T6CONBITS T6CONbits __attribute__((__sfr__));

extern volatile unsigned int T7CON __attribute__((__sfr__));
__extension__ typedef struct tagT7CONBITS {
  union {
    struct {
      unsigned :1;
      unsigned TCS:1;
      unsigned :2;
      unsigned TCKPS:2;
      unsigned TGATE:1;
      unsigned :6;
      unsigned TSIDL:1;
      unsigned :1;
      unsigned TON:1;
    };
    struct {
      unsigned :4;
      unsigned TCKPS0:1;
      unsigned TCKPS1:1;
    };
  };
} T7CONBITS;
extern volatile T7CONBITS T7CONbits __attribute__((__sfr__));

extern volatile unsigned int TMR8 __attribute__((__sfr__));
extern volatile unsigned int TMR9HLD __attribute__((__sfr__));
extern volatile unsigned int TMR9 __attribute__((__sfr__));
extern volatile unsigned int PR8 __attribute__((__sfr__));
extern volatile unsigned int PR9 __attribute__((__sfr__));
extern volatile unsigned int T8CON __attribute__((__sfr__));
__extension__ typedef struct tagT8CONBITS {
  union {
    struct {
      unsigned :1;
      unsigned TCS:1;
      unsigned :1;
      unsigned T32:1;
      unsigned TCKPS:2;
      unsigned TGATE:1;
      unsigned :6;
      unsigned TSIDL:1;
      unsigned :1;
      unsigned TON:1;
    };
    struct {
      unsigned :4;
      unsigned TCKPS0:1;
      unsigned TCKPS1:1;
    };
  };
} T8CONBITS;
extern volatile T8CONBITS T8CONbits __attribute__((__sfr__));

extern volatile unsigned int T9CON __attribute__((__sfr__));
__extension__ typedef struct tagT9CONBITS {
  union {
    struct {
      unsigned :1;
      unsigned TCS:1;
      unsigned :2;
      unsigned TCKPS:2;
      unsigned TGATE:1;
      unsigned :6;
      unsigned TSIDL:1;
      unsigned :1;
      unsigned TON:1;
    };
    struct {
      unsigned :4;
      unsigned TCKPS0:1;
      unsigned TCKPS1:1;
    };
  };
} T9CONBITS;
extern volatile T9CONBITS T9CONbits __attribute__((__sfr__));



typedef struct tagIC {
        unsigned int icxbuf;
        unsigned int icxcon;
} IC, *PIC;


extern volatile IC IC1 __attribute__((__sfr__));
extern volatile IC IC2 __attribute__((__sfr__));
extern volatile IC IC3 __attribute__((__sfr__));
extern volatile IC IC4 __attribute__((__sfr__));
extern volatile IC IC5 __attribute__((__sfr__));
extern volatile IC IC6 __attribute__((__sfr__));
extern volatile IC IC7 __attribute__((__sfr__));
extern volatile IC IC8 __attribute__((__sfr__));

extern volatile unsigned int IC1BUF __attribute__((__sfr__));
extern volatile unsigned int IC1CON __attribute__((__sfr__));
__extension__ typedef struct tagIC1CONBITS {
  union {
    struct {
      unsigned ICM:3;
      unsigned ICBNE:1;
      unsigned ICOV:1;
      unsigned ICI:2;
      unsigned ICTMR:1;
      unsigned :5;
      unsigned ICSIDL:1;
    };
    struct {
      unsigned ICM0:1;
      unsigned ICM1:1;
      unsigned ICM2:1;
      unsigned :2;
      unsigned ICI0:1;
      unsigned ICI1:1;
    };
  };
} IC1CONBITS;
extern volatile IC1CONBITS IC1CONbits __attribute__((__sfr__));

extern volatile unsigned int IC2BUF __attribute__((__sfr__));
extern volatile unsigned int IC2CON __attribute__((__sfr__));
__extension__ typedef struct tagIC2CONBITS {
  union {
    struct {
      unsigned ICM:3;
      unsigned ICBNE:1;
      unsigned ICOV:1;
      unsigned ICI:2;
      unsigned ICTMR:1;
      unsigned :5;
      unsigned ICSIDL:1;
    };
    struct {
      unsigned ICM0:1;
      unsigned ICM1:1;
      unsigned ICM2:1;
      unsigned :2;
      unsigned ICI0:1;
      unsigned ICI1:1;
    };
  };
} IC2CONBITS;
extern volatile IC2CONBITS IC2CONbits __attribute__((__sfr__));

extern volatile unsigned int IC3BUF __attribute__((__sfr__));
extern volatile unsigned int IC3CON __attribute__((__sfr__));
__extension__ typedef struct tagIC3CONBITS {
  union {
    struct {
      unsigned ICM:3;
      unsigned ICBNE:1;
      unsigned ICOV:1;
      unsigned ICI:2;
      unsigned ICTMR:1;
      unsigned :5;
      unsigned ICSIDL:1;
    };
    struct {
      unsigned ICM0:1;
      unsigned ICM1:1;
      unsigned ICM2:1;
      unsigned :2;
      unsigned ICI0:1;
      unsigned ICI1:1;
    };
  };
} IC3CONBITS;
extern volatile IC3CONBITS IC3CONbits __attribute__((__sfr__));

extern volatile unsigned int IC4BUF __attribute__((__sfr__));
extern volatile unsigned int IC4CON __attribute__((__sfr__));
__extension__ typedef struct tagIC4CONBITS {
  union {
    struct {
      unsigned ICM:3;
      unsigned ICBNE:1;
      unsigned ICOV:1;
      unsigned ICI:2;
      unsigned ICTMR:1;
      unsigned :5;
      unsigned ICSIDL:1;
    };
    struct {
      unsigned ICM0:1;
      unsigned ICM1:1;
      unsigned ICM2:1;
      unsigned :2;
      unsigned ICI0:1;
      unsigned ICI1:1;
    };
  };
} IC4CONBITS;
extern volatile IC4CONBITS IC4CONbits __attribute__((__sfr__));

extern volatile unsigned int IC5BUF __attribute__((__sfr__));
extern volatile unsigned int IC5CON __attribute__((__sfr__));
__extension__ typedef struct tagIC5CONBITS {
  union {
    struct {
      unsigned ICM:3;
      unsigned ICBNE:1;
      unsigned ICOV:1;
      unsigned ICI:2;
      unsigned ICTMR:1;
      unsigned :5;
      unsigned ICSIDL:1;
    };
    struct {
      unsigned ICM0:1;
      unsigned ICM1:1;
      unsigned ICM2:1;
      unsigned :2;
      unsigned ICI0:1;
      unsigned ICI1:1;
    };
  };
} IC5CONBITS;
extern volatile IC5CONBITS IC5CONbits __attribute__((__sfr__));

extern volatile unsigned int IC6BUF __attribute__((__sfr__));
extern volatile unsigned int IC6CON __attribute__((__sfr__));
__extension__ typedef struct tagIC6CONBITS {
  union {
    struct {
      unsigned ICM:3;
      unsigned ICBNE:1;
      unsigned ICOV:1;
      unsigned ICI:2;
      unsigned ICTMR:1;
      unsigned :5;
      unsigned ICSIDL:1;
    };
    struct {
      unsigned ICM0:1;
      unsigned ICM1:1;
      unsigned ICM2:1;
      unsigned :2;
      unsigned ICI0:1;
      unsigned ICI1:1;
    };
  };
} IC6CONBITS;
extern volatile IC6CONBITS IC6CONbits __attribute__((__sfr__));

extern volatile unsigned int IC7BUF __attribute__((__sfr__));
extern volatile unsigned int IC7CON __attribute__((__sfr__));
__extension__ typedef struct tagIC7CONBITS {
  union {
    struct {
      unsigned ICM:3;
      unsigned ICBNE:1;
      unsigned ICOV:1;
      unsigned ICI:2;
      unsigned ICTMR:1;
      unsigned :5;
      unsigned ICSIDL:1;
    };
    struct {
      unsigned ICM0:1;
      unsigned ICM1:1;
      unsigned ICM2:1;
      unsigned :2;
      unsigned ICI0:1;
      unsigned ICI1:1;
    };
  };
} IC7CONBITS;
extern volatile IC7CONBITS IC7CONbits __attribute__((__sfr__));

extern volatile unsigned int IC8BUF __attribute__((__sfr__));
extern volatile unsigned int IC8CON __attribute__((__sfr__));
__extension__ typedef struct tagIC8CONBITS {
  union {
    struct {
      unsigned ICM:3;
      unsigned ICBNE:1;
      unsigned ICOV:1;
      unsigned ICI:2;
      unsigned ICTMR:1;
      unsigned :5;
      unsigned ICSIDL:1;
    };
    struct {
      unsigned ICM0:1;
      unsigned ICM1:1;
      unsigned ICM2:1;
      unsigned :2;
      unsigned ICI0:1;
      unsigned ICI1:1;
    };
  };
} IC8CONBITS;
extern volatile IC8CONBITS IC8CONbits __attribute__((__sfr__));



typedef struct tagOC {
        unsigned int ocxrs;
        unsigned int ocxr;
        unsigned int ocxcon;
} OC, *POC;


extern volatile OC OC1 __attribute__((__sfr__));
extern volatile OC OC2 __attribute__((__sfr__));
extern volatile OC OC3 __attribute__((__sfr__));
extern volatile OC OC4 __attribute__((__sfr__));
extern volatile OC OC5 __attribute__((__sfr__));
extern volatile OC OC6 __attribute__((__sfr__));
extern volatile OC OC7 __attribute__((__sfr__));
extern volatile OC OC8 __attribute__((__sfr__));

extern volatile unsigned int OC1RS __attribute__((__sfr__));
extern volatile unsigned int OC1R __attribute__((__sfr__));
extern volatile unsigned int OC1CON __attribute__((__sfr__));
__extension__ typedef struct tagOC1CONBITS {
  union {
    struct {
      unsigned OCM:3;
      unsigned OCTSEL:1;
      unsigned OCFLT:1;
      unsigned :8;
      unsigned OCSIDL:1;
    };
    struct {
      unsigned OCM0:1;
      unsigned OCM1:1;
      unsigned OCM2:1;
    };
  };
} OC1CONBITS;
extern volatile OC1CONBITS OC1CONbits __attribute__((__sfr__));

extern volatile unsigned int OC2RS __attribute__((__sfr__));
extern volatile unsigned int OC2R __attribute__((__sfr__));
extern volatile unsigned int OC2CON __attribute__((__sfr__));
__extension__ typedef struct tagOC2CONBITS {
  union {
    struct {
      unsigned OCM:3;
      unsigned OCTSEL:1;
      unsigned OCFLT:1;
      unsigned :8;
      unsigned OCSIDL:1;
    };
    struct {
      unsigned OCM0:1;
      unsigned OCM1:1;
      unsigned OCM2:1;
    };
  };
} OC2CONBITS;
extern volatile OC2CONBITS OC2CONbits __attribute__((__sfr__));

extern volatile unsigned int OC3RS __attribute__((__sfr__));
extern volatile unsigned int OC3R __attribute__((__sfr__));
extern volatile unsigned int OC3CON __attribute__((__sfr__));
__extension__ typedef struct tagOC3CONBITS {
  union {
    struct {
      unsigned OCM:3;
      unsigned OCTSEL:1;
      unsigned OCFLT:1;
      unsigned :8;
      unsigned OCSIDL:1;
    };
    struct {
      unsigned OCM0:1;
      unsigned OCM1:1;
      unsigned OCM2:1;
    };
  };
} OC3CONBITS;
extern volatile OC3CONBITS OC3CONbits __attribute__((__sfr__));

extern volatile unsigned int OC4RS __attribute__((__sfr__));
extern volatile unsigned int OC4R __attribute__((__sfr__));
extern volatile unsigned int OC4CON __attribute__((__sfr__));
__extension__ typedef struct tagOC4CONBITS {
  union {
    struct {
      unsigned OCM:3;
      unsigned OCTSEL:1;
      unsigned OCFLT:1;
      unsigned :8;
      unsigned OCSIDL:1;
    };
    struct {
      unsigned OCM0:1;
      unsigned OCM1:1;
      unsigned OCM2:1;
    };
  };
} OC4CONBITS;
extern volatile OC4CONBITS OC4CONbits __attribute__((__sfr__));

extern volatile unsigned int OC5RS __attribute__((__sfr__));
extern volatile unsigned int OC5R __attribute__((__sfr__));
extern volatile unsigned int OC5CON __attribute__((__sfr__));
__extension__ typedef struct tagOC5CONBITS {
  union {
    struct {
      unsigned OCM:3;
      unsigned OCTSEL:1;
      unsigned OCFLT:1;
      unsigned :8;
      unsigned OCSIDL:1;
    };
    struct {
      unsigned OCM0:1;
      unsigned OCM1:1;
      unsigned OCM2:1;
    };
  };
} OC5CONBITS;
extern volatile OC5CONBITS OC5CONbits __attribute__((__sfr__));

extern volatile unsigned int OC6RS __attribute__((__sfr__));
extern volatile unsigned int OC6R __attribute__((__sfr__));
extern volatile unsigned int OC6CON __attribute__((__sfr__));
__extension__ typedef struct tagOC6CONBITS {
  union {
    struct {
      unsigned OCM:3;
      unsigned OCTSEL:1;
      unsigned OCFLT:1;
      unsigned :8;
      unsigned OCSIDL:1;
    };
    struct {
      unsigned OCM0:1;
      unsigned OCM1:1;
      unsigned OCM2:1;
    };
  };
} OC6CONBITS;
extern volatile OC6CONBITS OC6CONbits __attribute__((__sfr__));

extern volatile unsigned int OC7RS __attribute__((__sfr__));
extern volatile unsigned int OC7R __attribute__((__sfr__));
extern volatile unsigned int OC7CON __attribute__((__sfr__));
__extension__ typedef struct tagOC7CONBITS {
  union {
    struct {
      unsigned OCM:3;
      unsigned OCTSEL:1;
      unsigned OCFLT:1;
      unsigned :8;
      unsigned OCSIDL:1;
    };
    struct {
      unsigned OCM0:1;
      unsigned OCM1:1;
      unsigned OCM2:1;
    };
  };
} OC7CONBITS;
extern volatile OC7CONBITS OC7CONbits __attribute__((__sfr__));

extern volatile unsigned int OC8RS __attribute__((__sfr__));
extern volatile unsigned int OC8R __attribute__((__sfr__));
extern volatile unsigned int OC8CON __attribute__((__sfr__));
__extension__ typedef struct tagOC8CONBITS {
  union {
    struct {
      unsigned OCM:3;
      unsigned OCTSEL:1;
      unsigned OCFLT:1;
      unsigned :8;
      unsigned OCSIDL:1;
    };
    struct {
      unsigned OCM0:1;
      unsigned OCM1:1;
      unsigned OCM2:1;
    };
  };
} OC8CONBITS;
extern volatile OC8CONBITS OC8CONbits __attribute__((__sfr__));

extern volatile unsigned int I2C1RCV __attribute__((__sfr__));
extern volatile unsigned int I2CRCV __attribute__((__sfr__));
extern volatile unsigned int I2C1TRN __attribute__((__sfr__));
extern volatile unsigned int I2CTRN __attribute__((__sfr__));
extern volatile unsigned int I2C1BRG __attribute__((__sfr__));
extern volatile unsigned int I2C1CON __attribute__((__sfr__));
typedef struct tagI2C1CONBITS {
  unsigned SEN:1;
  unsigned RSEN:1;
  unsigned PEN:1;
  unsigned RCEN:1;
  unsigned ACKEN:1;
  unsigned ACKDT:1;
  unsigned STREN:1;
  unsigned GCEN:1;
  unsigned SMEN:1;
  unsigned DISSLW:1;
  unsigned A10M:1;
  unsigned IPMIEN:1;
  unsigned SCLREL:1;
  unsigned I2CSIDL:1;
  unsigned :1;
  unsigned I2CEN:1;
} I2C1CONBITS;
extern volatile I2C1CONBITS I2C1CONbits __attribute__((__sfr__));

extern volatile unsigned int I2CCON __attribute__((__sfr__));
typedef struct tagI2CCONBITS {
  unsigned SEN:1;
  unsigned RSEN:1;
  unsigned PEN:1;
  unsigned RCEN:1;
  unsigned ACKEN:1;
  unsigned ACKDT:1;
  unsigned STREN:1;
  unsigned GCEN:1;
  unsigned SMEN:1;
  unsigned DISSLW:1;
  unsigned A10M:1;
  unsigned IPMIEN:1;
  unsigned SCLREL:1;
  unsigned I2CSIDL:1;
  unsigned :1;
  unsigned I2CEN:1;
} I2CCONBITS;
extern volatile I2CCONBITS I2CCONbits __attribute__((__sfr__));

extern volatile unsigned int I2C1STAT __attribute__((__sfr__));
typedef struct tagI2C1STATBITS {
  unsigned TBF:1;
  unsigned RBF:1;
  unsigned R_W:1;
  unsigned S:1;
  unsigned P:1;
  unsigned D_A:1;
  unsigned I2COV:1;
  unsigned IWCOL:1;
  unsigned ADD10:1;
  unsigned GCSTAT:1;
  unsigned BCL:1;
  unsigned :3;
  unsigned TRSTAT:1;
  unsigned ACKSTAT:1;
} I2C1STATBITS;
extern volatile I2C1STATBITS I2C1STATbits __attribute__((__sfr__));

extern volatile unsigned int I2CSTAT __attribute__((__sfr__));
typedef struct tagI2CSTATBITS {
  unsigned TBF:1;
  unsigned RBF:1;
  unsigned R_W:1;
  unsigned S:1;
  unsigned P:1;
  unsigned D_A:1;
  unsigned I2COV:1;
  unsigned IWCOL:1;
  unsigned ADD10:1;
  unsigned GCSTAT:1;
  unsigned BCL:1;
  unsigned :3;
  unsigned TRSTAT:1;
  unsigned ACKSTAT:1;
} I2CSTATBITS;
extern volatile I2CSTATBITS I2CSTATbits __attribute__((__sfr__));

extern volatile unsigned int I2C1ADD __attribute__((__sfr__));
extern volatile unsigned int I2CADD __attribute__((__sfr__));
extern volatile unsigned int I2C1MSK __attribute__((__sfr__));
extern volatile unsigned int I2C2RCV __attribute__((__sfr__));
extern volatile unsigned int I2C2TRN __attribute__((__sfr__));
extern volatile unsigned int I2C2BRG __attribute__((__sfr__));
extern volatile unsigned int I2C2CON __attribute__((__sfr__));
typedef struct tagI2C2CONBITS {
  unsigned SEN:1;
  unsigned RSEN:1;
  unsigned PEN:1;
  unsigned RCEN:1;
  unsigned ACKEN:1;
  unsigned ACKDT:1;
  unsigned STREN:1;
  unsigned GCEN:1;
  unsigned SMEN:1;
  unsigned DISSLW:1;
  unsigned A10M:1;
  unsigned IPMIEN:1;
  unsigned SCLREL:1;
  unsigned I2CSIDL:1;
  unsigned :1;
  unsigned I2CEN:1;
} I2C2CONBITS;
extern volatile I2C2CONBITS I2C2CONbits __attribute__((__sfr__));

extern volatile unsigned int I2C2STAT __attribute__((__sfr__));
typedef struct tagI2C2STATBITS {
  unsigned TBF:1;
  unsigned RBF:1;
  unsigned R_W:1;
  unsigned S:1;
  unsigned P:1;
  unsigned D_A:1;
  unsigned I2COV:1;
  unsigned IWCOL:1;
  unsigned ADD10:1;
  unsigned GCSTAT:1;
  unsigned BCL:1;
  unsigned :3;
  unsigned TRSTAT:1;
  unsigned ACKSTAT:1;
} I2C2STATBITS;
extern volatile I2C2STATBITS I2C2STATbits __attribute__((__sfr__));

extern volatile unsigned int I2C2ADD __attribute__((__sfr__));
extern volatile unsigned int I2C2MSK __attribute__((__sfr__));


typedef struct tagUART {
        unsigned int uxmode;
        unsigned int uxsta;
        unsigned int uxtxreg;
        unsigned int uxrxreg;
        unsigned int uxbrg;
} UART, *PUART;
# 1821 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/dsPIC33F/h/p33FJ256GP710A.h" 3 4
extern volatile UART UART1 __attribute__((__sfr__));
extern volatile UART UART2 __attribute__((__sfr__));

extern volatile unsigned int U1MODE __attribute__((__sfr__));
__extension__ typedef struct tagU1MODEBITS {
  union {
    struct {
      unsigned STSEL:1;
      unsigned PDSEL:2;
      unsigned BRGH:1;
      unsigned URXINV:1;
      unsigned ABAUD:1;
      unsigned LPBACK:1;
      unsigned WAKE:1;
      unsigned UEN:2;
      unsigned :1;
      unsigned RTSMD:1;
      unsigned IREN:1;
      unsigned USIDL:1;
      unsigned :1;
      unsigned UARTEN:1;
    };
    struct {
      unsigned :1;
      unsigned PDSEL0:1;
      unsigned PDSEL1:1;
      unsigned :5;
      unsigned UEN0:1;
      unsigned UEN1:1;
    };
  };
} U1MODEBITS;
extern volatile U1MODEBITS U1MODEbits __attribute__((__sfr__));

extern volatile unsigned int U1STA __attribute__((__sfr__));
__extension__ typedef struct tagU1STABITS {
  union {
    struct {
      unsigned URXDA:1;
      unsigned OERR:1;
      unsigned FERR:1;
      unsigned PERR:1;
      unsigned RIDLE:1;
      unsigned ADDEN:1;
      unsigned URXISEL:2;
      unsigned TRMT:1;
      unsigned UTXBF:1;
      unsigned UTXEN:1;
      unsigned UTXBRK:1;
      unsigned :1;
      unsigned UTXISEL0:1;
      unsigned UTXINV:1;
      unsigned UTXISEL1:1;
    };
    struct {
      unsigned :6;
      unsigned URXISEL0:1;
      unsigned URXISEL1:1;
    };
  };
} U1STABITS;
extern volatile U1STABITS U1STAbits __attribute__((__sfr__));

extern volatile unsigned int U1TXREG __attribute__((__sfr__));
typedef struct tagU1TXREGBITS {
  unsigned UTXREG0:1;
  unsigned UTXREG1:1;
  unsigned UTXREG2:1;
  unsigned UTXREG3:1;
  unsigned UTXREG4:1;
  unsigned UTXREG5:1;
  unsigned UTXREG6:1;
  unsigned UTXREG7:1;
  unsigned UTXREG8:1;
} U1TXREGBITS;
extern volatile U1TXREGBITS U1TXREGbits __attribute__((__sfr__));

extern volatile unsigned int U1RXREG __attribute__((__sfr__));
typedef struct tagU1RXREGBITS {
  unsigned URXREG0:1;
  unsigned URXREG1:1;
  unsigned URXREG2:1;
  unsigned URXREG3:1;
  unsigned URXREG4:1;
  unsigned URXREG5:1;
  unsigned URXREG6:1;
  unsigned URXREG7:1;
  unsigned URXREG8:1;
} U1RXREGBITS;
extern volatile U1RXREGBITS U1RXREGbits __attribute__((__sfr__));

extern volatile unsigned int U1BRG __attribute__((__sfr__));
extern volatile unsigned int U2MODE __attribute__((__sfr__));
__extension__ typedef struct tagU2MODEBITS {
  union {
    struct {
      unsigned STSEL:1;
      unsigned PDSEL:2;
      unsigned BRGH:1;
      unsigned URXINV:1;
      unsigned ABAUD:1;
      unsigned LPBACK:1;
      unsigned WAKE:1;
      unsigned UEN:2;
      unsigned :1;
      unsigned RTSMD:1;
      unsigned IREN:1;
      unsigned USIDL:1;
      unsigned :1;
      unsigned UARTEN:1;
    };
    struct {
      unsigned :1;
      unsigned PDSEL0:1;
      unsigned PDSEL1:1;
      unsigned :5;
      unsigned UEN0:1;
      unsigned UEN1:1;
    };
  };
} U2MODEBITS;
extern volatile U2MODEBITS U2MODEbits __attribute__((__sfr__));

extern volatile unsigned int U2STA __attribute__((__sfr__));
__extension__ typedef struct tagU2STABITS {
  union {
    struct {
      unsigned URXDA:1;
      unsigned OERR:1;
      unsigned FERR:1;
      unsigned PERR:1;
      unsigned RIDLE:1;
      unsigned ADDEN:1;
      unsigned URXISEL:2;
      unsigned TRMT:1;
      unsigned UTXBF:1;
      unsigned UTXEN:1;
      unsigned UTXBRK:1;
      unsigned :1;
      unsigned UTXISEL0:1;
      unsigned UTXINV:1;
      unsigned UTXISEL1:1;
    };
    struct {
      unsigned :6;
      unsigned URXISEL0:1;
      unsigned URXISEL1:1;
    };
  };
} U2STABITS;
extern volatile U2STABITS U2STAbits __attribute__((__sfr__));

extern volatile unsigned int U2TXREG __attribute__((__sfr__));
typedef struct tagU2TXREGBITS {
  unsigned UTXREG0:1;
  unsigned UTXREG1:1;
  unsigned UTXREG2:1;
  unsigned UTXREG3:1;
  unsigned UTXREG4:1;
  unsigned UTXREG5:1;
  unsigned UTXREG6:1;
  unsigned UTXREG7:1;
  unsigned UTXREG8:1;
} U2TXREGBITS;
extern volatile U2TXREGBITS U2TXREGbits __attribute__((__sfr__));

extern volatile unsigned int U2RXREG __attribute__((__sfr__));
typedef struct tagU2RXREGBITS {
  unsigned URXREG0:1;
  unsigned URXREG1:1;
  unsigned URXREG2:1;
  unsigned URXREG3:1;
  unsigned URXREG4:1;
  unsigned URXREG5:1;
  unsigned URXREG6:1;
  unsigned URXREG7:1;
  unsigned URXREG8:1;
} U2RXREGBITS;
extern volatile U2RXREGBITS U2RXREGbits __attribute__((__sfr__));

extern volatile unsigned int U2BRG __attribute__((__sfr__));


typedef struct tagSPI {
        unsigned int spixstat;
        unsigned int spixcon1;
        unsigned int spixcon2;
        unsigned int unused;
        unsigned int spixbuf;
} SPI, *PSPI;





extern volatile SPI SPI1 __attribute__((__sfr__));
extern volatile SPI SPI2 __attribute__((__sfr__));

extern volatile unsigned int SPI1STAT __attribute__((__sfr__));
typedef struct tagSPI1STATBITS {
  unsigned SPIRBF:1;
  unsigned SPITBF:1;
  unsigned :4;
  unsigned SPIROV:1;
  unsigned :6;
  unsigned SPISIDL:1;
  unsigned :1;
  unsigned SPIEN:1;
} SPI1STATBITS;
extern volatile SPI1STATBITS SPI1STATbits __attribute__((__sfr__));

extern volatile unsigned int SPI1CON1 __attribute__((__sfr__));
__extension__ typedef struct tagSPI1CON1BITS {
  union {
    struct {
      unsigned PPRE:2;
      unsigned SPRE:3;
      unsigned MSTEN:1;
      unsigned CKP:1;
      unsigned SSEN:1;
      unsigned CKE:1;
      unsigned SMP:1;
      unsigned MODE16:1;
      unsigned DISSDO:1;
      unsigned DISSCK:1;
    };
    struct {
      unsigned PPRE0:1;
      unsigned PPRE1:1;
      unsigned SPRE0:1;
      unsigned SPRE1:1;
      unsigned SPRE2:1;
    };
  };
} SPI1CON1BITS;
extern volatile SPI1CON1BITS SPI1CON1bits __attribute__((__sfr__));

extern volatile unsigned int SPI1CON2 __attribute__((__sfr__));
typedef struct tagSPI1CON2BITS {
  unsigned :1;
  unsigned FRMDLY:1;
  unsigned :11;
  unsigned FRMPOL:1;
  unsigned SPIFSD:1;
  unsigned FRMEN:1;
} SPI1CON2BITS;
extern volatile SPI1CON2BITS SPI1CON2bits __attribute__((__sfr__));

extern volatile unsigned int SPI1BUF __attribute__((__sfr__));
extern volatile unsigned int SPI2STAT __attribute__((__sfr__));
typedef struct tagSPI2STATBITS {
  unsigned SPIRBF:1;
  unsigned SPITBF:1;
  unsigned :4;
  unsigned SPIROV:1;
  unsigned :6;
  unsigned SPISIDL:1;
  unsigned :1;
  unsigned SPIEN:1;
} SPI2STATBITS;
extern volatile SPI2STATBITS SPI2STATbits __attribute__((__sfr__));

extern volatile unsigned int SPI2CON1 __attribute__((__sfr__));
__extension__ typedef struct tagSPI2CON1BITS {
  union {
    struct {
      unsigned PPRE:2;
      unsigned SPRE:3;
      unsigned MSTEN:1;
      unsigned CKP:1;
      unsigned SSEN:1;
      unsigned CKE:1;
      unsigned SMP:1;
      unsigned MODE16:1;
      unsigned DISSDO:1;
      unsigned DISSCK:1;
    };
    struct {
      unsigned PPRE0:1;
      unsigned PPRE1:1;
      unsigned SPRE0:1;
      unsigned SPRE1:1;
      unsigned SPRE2:1;
    };
  };
} SPI2CON1BITS;
extern volatile SPI2CON1BITS SPI2CON1bits __attribute__((__sfr__));

extern volatile unsigned int SPI2CON2 __attribute__((__sfr__));
typedef struct tagSPI2CON2BITS {
  unsigned :1;
  unsigned FRMDLY:1;
  unsigned :11;
  unsigned FRMPOL:1;
  unsigned SPIFSD:1;
  unsigned FRMEN:1;
} SPI2CON2BITS;
extern volatile SPI2CON2BITS SPI2CON2bits __attribute__((__sfr__));

extern volatile unsigned int SPI2BUF __attribute__((__sfr__));
extern volatile unsigned int DCICON1 __attribute__((__sfr__));
__extension__ typedef struct tagDCICON1BITS {
  union {
    struct {
      unsigned COFSM:2;
      unsigned :3;
      unsigned DJST:1;
      unsigned CSDOM:1;
      unsigned UNFM:1;
      unsigned COFSD:1;
      unsigned CSCKE:1;
      unsigned CSCKD:1;
      unsigned DLOOP:1;
      unsigned :1;
      unsigned DCISIDL:1;
      unsigned :1;
      unsigned DCIEN:1;
    };
    struct {
      unsigned COFSM0:1;
      unsigned COFSM1:1;
    };
  };
} DCICON1BITS;
extern volatile DCICON1BITS DCICON1bits __attribute__((__sfr__));

extern volatile unsigned int DCICON2 __attribute__((__sfr__));
__extension__ typedef struct tagDCICON2BITS {
  union {
    struct {
      unsigned WS:4;
      unsigned :1;
      unsigned COFSG:4;
      unsigned :1;
      unsigned BLEN:2;
    };
    struct {
      unsigned WS0:1;
      unsigned WS1:1;
      unsigned WS2:1;
      unsigned WS3:1;
      unsigned :1;
      unsigned COFSG0:1;
      unsigned COFSG1:1;
      unsigned COFSG2:1;
      unsigned COFSG3:1;
      unsigned :1;
      unsigned BLEN0:1;
      unsigned BLEN1:1;
    };
  };
} DCICON2BITS;
extern volatile DCICON2BITS DCICON2bits __attribute__((__sfr__));

extern volatile unsigned int DCICON3 __attribute__((__sfr__));
typedef struct tagDCICON3BITS {
  unsigned BCG:12;
} DCICON3BITS;
extern volatile DCICON3BITS DCICON3bits __attribute__((__sfr__));

extern volatile unsigned int DCISTAT __attribute__((__sfr__));
__extension__ typedef struct tagDCISTATBITS {
  union {
    struct {
      unsigned TMPTY:1;
      unsigned TUNF:1;
      unsigned RFUL:1;
      unsigned ROV:1;
      unsigned :4;
      unsigned SLOT:4;
    };
    struct {
      unsigned :8;
      unsigned SLOT0:1;
      unsigned SLOT1:1;
      unsigned SLOT2:1;
      unsigned SLOT3:1;
    };
  };
} DCISTATBITS;
extern volatile DCISTATBITS DCISTATbits __attribute__((__sfr__));

extern volatile unsigned int TSCON __attribute__((__sfr__));
typedef struct tagTSCONBITS {
  unsigned TSE0:1;
  unsigned TSE1:1;
  unsigned TSE2:1;
  unsigned TSE3:1;
  unsigned TSE4:1;
  unsigned TSE5:1;
  unsigned TSE6:1;
  unsigned TSE7:1;
  unsigned TSE8:1;
  unsigned TSE9:1;
  unsigned TSE10:1;
  unsigned TSE11:1;
  unsigned TSE12:1;
  unsigned TSE13:1;
  unsigned TSE14:1;
  unsigned TSE15:1;
} TSCONBITS;
extern volatile TSCONBITS TSCONbits __attribute__((__sfr__));

extern volatile unsigned int RSCON __attribute__((__sfr__));
typedef struct tagRSCONBITS {
  unsigned RSE0:1;
  unsigned RSE1:1;
  unsigned RSE2:1;
  unsigned RSE3:1;
  unsigned RSE4:1;
  unsigned RSE5:1;
  unsigned RSE6:1;
  unsigned RSE7:1;
  unsigned RSE8:1;
  unsigned RSE9:1;
  unsigned RSE10:1;
  unsigned RSE11:1;
  unsigned RSE12:1;
  unsigned RSE13:1;
  unsigned RSE14:1;
  unsigned RSE15:1;
} RSCONBITS;
extern volatile RSCONBITS RSCONbits __attribute__((__sfr__));

extern volatile unsigned int RXBUF0 __attribute__((__sfr__));
extern volatile unsigned int RXBUF1 __attribute__((__sfr__));
extern volatile unsigned int RXBUF2 __attribute__((__sfr__));
extern volatile unsigned int RXBUF3 __attribute__((__sfr__));
extern volatile unsigned int TXBUF0 __attribute__((__sfr__));
extern volatile unsigned int TXBUF1 __attribute__((__sfr__));
extern volatile unsigned int TXBUF2 __attribute__((__sfr__));
extern volatile unsigned int TXBUF3 __attribute__((__sfr__));
extern volatile unsigned int TRISA __attribute__((__sfr__));
typedef struct tagTRISABITS {
  unsigned TRISA0:1;
  unsigned TRISA1:1;
  unsigned TRISA2:1;
  unsigned TRISA3:1;
  unsigned TRISA4:1;
  unsigned TRISA5:1;
  unsigned TRISA6:1;
  unsigned TRISA7:1;
  unsigned :1;
  unsigned TRISA9:1;
  unsigned TRISA10:1;
  unsigned :1;
  unsigned TRISA12:1;
  unsigned TRISA13:1;
  unsigned TRISA14:1;
  unsigned TRISA15:1;
} TRISABITS;
extern volatile TRISABITS TRISAbits __attribute__((__sfr__));

extern volatile unsigned int PORTA __attribute__((__sfr__));
typedef struct tagPORTABITS {
  unsigned RA0:1;
  unsigned RA1:1;
  unsigned RA2:1;
  unsigned RA3:1;
  unsigned RA4:1;
  unsigned RA5:1;
  unsigned RA6:1;
  unsigned RA7:1;
  unsigned :1;
  unsigned RA9:1;
  unsigned RA10:1;
  unsigned :1;
  unsigned RA12:1;
  unsigned RA13:1;
  unsigned RA14:1;
  unsigned RA15:1;
} PORTABITS;
extern volatile PORTABITS PORTAbits __attribute__((__sfr__));

extern volatile unsigned int LATA __attribute__((__sfr__));
typedef struct tagLATABITS {
  unsigned LATA0:1;
  unsigned LATA1:1;
  unsigned LATA2:1;
  unsigned LATA3:1;
  unsigned LATA4:1;
  unsigned LATA5:1;
  unsigned LATA6:1;
  unsigned LATA7:1;
  unsigned :1;
  unsigned LATA9:1;
  unsigned LATA10:1;
  unsigned :1;
  unsigned LATA12:1;
  unsigned LATA13:1;
  unsigned LATA14:1;
  unsigned LATA15:1;
} LATABITS;
extern volatile LATABITS LATAbits __attribute__((__sfr__));

extern volatile unsigned int TRISB __attribute__((__sfr__));
typedef struct tagTRISBBITS {
  unsigned TRISB0:1;
  unsigned TRISB1:1;
  unsigned TRISB2:1;
  unsigned TRISB3:1;
  unsigned TRISB4:1;
  unsigned TRISB5:1;
  unsigned TRISB6:1;
  unsigned TRISB7:1;
  unsigned TRISB8:1;
  unsigned TRISB9:1;
  unsigned TRISB10:1;
  unsigned TRISB11:1;
  unsigned TRISB12:1;
  unsigned TRISB13:1;
  unsigned TRISB14:1;
  unsigned TRISB15:1;
} TRISBBITS;
extern volatile TRISBBITS TRISBbits __attribute__((__sfr__));

extern volatile unsigned int PORTB __attribute__((__sfr__));
typedef struct tagPORTBBITS {
  unsigned RB0:1;
  unsigned RB1:1;
  unsigned RB2:1;
  unsigned RB3:1;
  unsigned RB4:1;
  unsigned RB5:1;
  unsigned RB6:1;
  unsigned RB7:1;
  unsigned RB8:1;
  unsigned RB9:1;
  unsigned RB10:1;
  unsigned RB11:1;
  unsigned RB12:1;
  unsigned RB13:1;
  unsigned RB14:1;
  unsigned RB15:1;
} PORTBBITS;
extern volatile PORTBBITS PORTBbits __attribute__((__sfr__));

extern volatile unsigned int LATB __attribute__((__sfr__));
typedef struct tagLATBBITS {
  unsigned LATB0:1;
  unsigned LATB1:1;
  unsigned LATB2:1;
  unsigned LATB3:1;
  unsigned LATB4:1;
  unsigned LATB5:1;
  unsigned LATB6:1;
  unsigned LATB7:1;
  unsigned LATB8:1;
  unsigned LATB9:1;
  unsigned LATB10:1;
  unsigned LATB11:1;
  unsigned LATB12:1;
  unsigned LATB13:1;
  unsigned LATB14:1;
  unsigned LATB15:1;
} LATBBITS;
extern volatile LATBBITS LATBbits __attribute__((__sfr__));

extern volatile unsigned int TRISC __attribute__((__sfr__));
typedef struct tagTRISCBITS {
  unsigned :1;
  unsigned TRISC1:1;
  unsigned TRISC2:1;
  unsigned TRISC3:1;
  unsigned TRISC4:1;
  unsigned :7;
  unsigned TRISC12:1;
  unsigned TRISC13:1;
  unsigned TRISC14:1;
  unsigned TRISC15:1;
} TRISCBITS;
extern volatile TRISCBITS TRISCbits __attribute__((__sfr__));

extern volatile unsigned int PORTC __attribute__((__sfr__));
typedef struct tagPORTCBITS {
  unsigned :1;
  unsigned RC1:1;
  unsigned RC2:1;
  unsigned RC3:1;
  unsigned RC4:1;
  unsigned :7;
  unsigned RC12:1;
  unsigned RC13:1;
  unsigned RC14:1;
  unsigned RC15:1;
} PORTCBITS;
extern volatile PORTCBITS PORTCbits __attribute__((__sfr__));

extern volatile unsigned int LATC __attribute__((__sfr__));
typedef struct tagLATCBITS {
  unsigned :1;
  unsigned LATC1:1;
  unsigned LATC2:1;
  unsigned LATC3:1;
  unsigned LATC4:1;
  unsigned :7;
  unsigned LATC12:1;
  unsigned LATC13:1;
  unsigned LATC14:1;
  unsigned LATC15:1;
} LATCBITS;
extern volatile LATCBITS LATCbits __attribute__((__sfr__));

extern volatile unsigned int TRISD __attribute__((__sfr__));
typedef struct tagTRISDBITS {
  unsigned TRISD0:1;
  unsigned TRISD1:1;
  unsigned TRISD2:1;
  unsigned TRISD3:1;
  unsigned TRISD4:1;
  unsigned TRISD5:1;
  unsigned TRISD6:1;
  unsigned TRISD7:1;
  unsigned TRISD8:1;
  unsigned TRISD9:1;
  unsigned TRISD10:1;
  unsigned TRISD11:1;
  unsigned TRISD12:1;
  unsigned TRISD13:1;
  unsigned TRISD14:1;
  unsigned TRISD15:1;
} TRISDBITS;
extern volatile TRISDBITS TRISDbits __attribute__((__sfr__));

extern volatile unsigned int PORTD __attribute__((__sfr__));
typedef struct tagPORTDBITS {
  unsigned RD0:1;
  unsigned RD1:1;
  unsigned RD2:1;
  unsigned RD3:1;
  unsigned RD4:1;
  unsigned RD5:1;
  unsigned RD6:1;
  unsigned RD7:1;
  unsigned RD8:1;
  unsigned RD9:1;
  unsigned RD10:1;
  unsigned RD11:1;
  unsigned RD12:1;
  unsigned RD13:1;
  unsigned RD14:1;
  unsigned RD15:1;
} PORTDBITS;
extern volatile PORTDBITS PORTDbits __attribute__((__sfr__));

extern volatile unsigned int LATD __attribute__((__sfr__));
typedef struct tagLATDBITS {
  unsigned LATD0:1;
  unsigned LATD1:1;
  unsigned LATD2:1;
  unsigned LATD3:1;
  unsigned LATD4:1;
  unsigned LATD5:1;
  unsigned LATD6:1;
  unsigned LATD7:1;
  unsigned LATD8:1;
  unsigned LATD9:1;
  unsigned LATD10:1;
  unsigned LATD11:1;
  unsigned LATD12:1;
  unsigned LATD13:1;
  unsigned LATD14:1;
  unsigned LATD15:1;
} LATDBITS;
extern volatile LATDBITS LATDbits __attribute__((__sfr__));

extern volatile unsigned int TRISE __attribute__((__sfr__));
typedef struct tagTRISEBITS {
  unsigned TRISE0:1;
  unsigned TRISE1:1;
  unsigned TRISE2:1;
  unsigned TRISE3:1;
  unsigned TRISE4:1;
  unsigned TRISE5:1;
  unsigned TRISE6:1;
  unsigned TRISE7:1;
} TRISEBITS;
extern volatile TRISEBITS TRISEbits __attribute__((__sfr__));

extern volatile unsigned int PORTE __attribute__((__sfr__));
typedef struct tagPORTEBITS {
  unsigned RE0:1;
  unsigned RE1:1;
  unsigned RE2:1;
  unsigned RE3:1;
  unsigned RE4:1;
  unsigned RE5:1;
  unsigned RE6:1;
  unsigned RE7:1;
} PORTEBITS;
extern volatile PORTEBITS PORTEbits __attribute__((__sfr__));

extern volatile unsigned int LATE __attribute__((__sfr__));
typedef struct tagLATEBITS {
  unsigned LATE0:1;
  unsigned LATE1:1;
  unsigned LATE2:1;
  unsigned LATE3:1;
  unsigned LATE4:1;
  unsigned LATE5:1;
  unsigned LATE6:1;
  unsigned LATE7:1;
} LATEBITS;
extern volatile LATEBITS LATEbits __attribute__((__sfr__));

extern volatile unsigned int TRISF __attribute__((__sfr__));
typedef struct tagTRISFBITS {
  unsigned TRISF0:1;
  unsigned TRISF1:1;
  unsigned TRISF2:1;
  unsigned TRISF3:1;
  unsigned TRISF4:1;
  unsigned TRISF5:1;
  unsigned TRISF6:1;
  unsigned TRISF7:1;
  unsigned TRISF8:1;
  unsigned :3;
  unsigned TRISF12:1;
  unsigned TRISF13:1;
} TRISFBITS;
extern volatile TRISFBITS TRISFbits __attribute__((__sfr__));

extern volatile unsigned int PORTF __attribute__((__sfr__));
typedef struct tagPORTFBITS {
  unsigned RF0:1;
  unsigned RF1:1;
  unsigned RF2:1;
  unsigned RF3:1;
  unsigned RF4:1;
  unsigned RF5:1;
  unsigned RF6:1;
  unsigned RF7:1;
  unsigned RF8:1;
  unsigned :3;
  unsigned RF12:1;
  unsigned RF13:1;
} PORTFBITS;
extern volatile PORTFBITS PORTFbits __attribute__((__sfr__));

extern volatile unsigned int LATF __attribute__((__sfr__));
typedef struct tagLATFBITS {
  unsigned LATF0:1;
  unsigned LATF1:1;
  unsigned LATF2:1;
  unsigned LATF3:1;
  unsigned LATF4:1;
  unsigned LATF5:1;
  unsigned LATF6:1;
  unsigned LATF7:1;
  unsigned LATF8:1;
  unsigned :3;
  unsigned LATF12:1;
  unsigned LATF13:1;
} LATFBITS;
extern volatile LATFBITS LATFbits __attribute__((__sfr__));

extern volatile unsigned int TRISG __attribute__((__sfr__));
typedef struct tagTRISGBITS {
  unsigned TRISG0:1;
  unsigned TRISG1:1;
  unsigned TRISG2:1;
  unsigned TRISG3:1;
  unsigned :2;
  unsigned TRISG6:1;
  unsigned TRISG7:1;
  unsigned TRISG8:1;
  unsigned TRISG9:1;
  unsigned :2;
  unsigned TRISG12:1;
  unsigned TRISG13:1;
  unsigned TRISG14:1;
  unsigned TRISG15:1;
} TRISGBITS;
extern volatile TRISGBITS TRISGbits __attribute__((__sfr__));

extern volatile unsigned int PORTG __attribute__((__sfr__));
typedef struct tagPORTGBITS {
  unsigned RG0:1;
  unsigned RG1:1;
  unsigned RG2:1;
  unsigned RG3:1;
  unsigned :2;
  unsigned RG6:1;
  unsigned RG7:1;
  unsigned RG8:1;
  unsigned RG9:1;
  unsigned :2;
  unsigned RG12:1;
  unsigned RG13:1;
  unsigned RG14:1;
  unsigned RG15:1;
} PORTGBITS;
extern volatile PORTGBITS PORTGbits __attribute__((__sfr__));

extern volatile unsigned int LATG __attribute__((__sfr__));
typedef struct tagLATGBITS {
  unsigned LATG0:1;
  unsigned LATG1:1;
  unsigned LATG2:1;
  unsigned LATG3:1;
  unsigned :2;
  unsigned LATG6:1;
  unsigned LATG7:1;
  unsigned LATG8:1;
  unsigned LATG9:1;
  unsigned :2;
  unsigned LATG12:1;
  unsigned LATG13:1;
  unsigned LATG14:1;
  unsigned LATG15:1;
} LATGBITS;
extern volatile LATGBITS LATGbits __attribute__((__sfr__));

extern volatile unsigned int ADC1BUF0 __attribute__((__sfr__));
extern volatile unsigned int ADCBUF0 __attribute__((__sfr__));
extern volatile unsigned int AD1CON1 __attribute__((__sfr__));
__extension__ typedef struct tagAD1CON1BITS {
  union {
    struct {
      unsigned DONE:1;
      unsigned SAMP:1;
      unsigned ASAM:1;
      unsigned SIMSAM:1;
      unsigned :1;
      unsigned SSRC:3;
      unsigned FORM:2;
      unsigned AD12B:1;
      unsigned :1;
      unsigned ADDMABM:1;
      unsigned ADSIDL:1;
      unsigned :1;
      unsigned ADON:1;
    };
    struct {
      unsigned :5;
      unsigned SSRC0:1;
      unsigned SSRC1:1;
      unsigned SSRC2:1;
      unsigned FORM0:1;
      unsigned FORM1:1;
    };
  };
} AD1CON1BITS;
extern volatile AD1CON1BITS AD1CON1bits __attribute__((__sfr__));

extern volatile unsigned int AD1CON2 __attribute__((__sfr__));
__extension__ typedef struct tagAD1CON2BITS {
  union {
    struct {
      unsigned ALTS:1;
      unsigned BUFM:1;
      unsigned SMPI:4;
      unsigned :1;
      unsigned BUFS:1;
      unsigned CHPS:2;
      unsigned CSCNA:1;
      unsigned :2;
      unsigned VCFG:3;
    };
    struct {
      unsigned :2;
      unsigned SMPI0:1;
      unsigned SMPI1:1;
      unsigned SMPI2:1;
      unsigned SMPI3:1;
      unsigned :2;
      unsigned CHPS0:1;
      unsigned CHPS1:1;
      unsigned :3;
      unsigned VCFG0:1;
      unsigned VCFG1:1;
      unsigned VCFG2:1;
    };
  };
} AD1CON2BITS;
extern volatile AD1CON2BITS AD1CON2bits __attribute__((__sfr__));

extern volatile unsigned int AD1CON3 __attribute__((__sfr__));
__extension__ typedef struct tagAD1CON3BITS {
  union {
    struct {
      unsigned ADCS:8;
      unsigned SAMC:5;
      unsigned :2;
      unsigned ADRC:1;
    };
    struct {
      unsigned ADCS0:1;
      unsigned ADCS1:1;
      unsigned ADCS2:1;
      unsigned ADCS3:1;
      unsigned ADCS4:1;
      unsigned ADCS5:1;
      unsigned ADCS6:1;
      unsigned ADCS7:1;
      unsigned SAMC0:1;
      unsigned SAMC1:1;
      unsigned SAMC2:1;
      unsigned SAMC3:1;
      unsigned SAMC4:1;
    };
  };
} AD1CON3BITS;
extern volatile AD1CON3BITS AD1CON3bits __attribute__((__sfr__));

extern volatile unsigned int AD1CHS123 __attribute__((__sfr__));
__extension__ typedef struct tagAD1CHS123BITS {
  union {
    struct {
      unsigned CH123SA:1;
      unsigned CH123NA:2;
      unsigned :5;
      unsigned CH123SB:1;
      unsigned CH123NB:2;
    };
    struct {
      unsigned :1;
      unsigned CH123NA0:1;
      unsigned CH123NA1:1;
      unsigned :6;
      unsigned CH123NB0:1;
      unsigned CH123NB1:1;
    };
  };
} AD1CHS123BITS;
extern volatile AD1CHS123BITS AD1CHS123bits __attribute__((__sfr__));

extern volatile unsigned int AD1CHS0 __attribute__((__sfr__));
__extension__ typedef struct tagAD1CHS0BITS {
  union {
    struct {
      unsigned CH0SA:5;
      unsigned :2;
      unsigned CH0NA:1;
      unsigned CH0SB:5;
      unsigned :2;
      unsigned CH0NB:1;
    };
    struct {
      unsigned CH0SA0:1;
      unsigned CH0SA1:1;
      unsigned CH0SA2:1;
      unsigned CH0SA3:1;
      unsigned CH0SA4:1;
      unsigned :3;
      unsigned CH0SB0:1;
      unsigned CH0SB1:1;
      unsigned CH0SB2:1;
      unsigned CH0SB3:1;
      unsigned CH0SB4:1;
    };
  };
} AD1CHS0BITS;
extern volatile AD1CHS0BITS AD1CHS0bits __attribute__((__sfr__));

extern volatile unsigned int AD1PCFGH __attribute__((__sfr__));
typedef struct tagAD1PCFGHBITS {
  unsigned PCFG16:1;
  unsigned PCFG17:1;
  unsigned PCFG18:1;
  unsigned PCFG19:1;
  unsigned PCFG20:1;
  unsigned PCFG21:1;
  unsigned PCFG22:1;
  unsigned PCFG23:1;
  unsigned PCFG24:1;
  unsigned PCFG25:1;
  unsigned PCFG26:1;
  unsigned PCFG27:1;
  unsigned PCFG28:1;
  unsigned PCFG29:1;
  unsigned PCFG30:1;
  unsigned PCFG31:1;
} AD1PCFGHBITS;
extern volatile AD1PCFGHBITS AD1PCFGHbits __attribute__((__sfr__));

extern volatile unsigned int AD1PCFGL __attribute__((__sfr__));
typedef struct tagAD1PCFGLBITS {
  unsigned PCFG0:1;
  unsigned PCFG1:1;
  unsigned PCFG2:1;
  unsigned PCFG3:1;
  unsigned PCFG4:1;
  unsigned PCFG5:1;
  unsigned PCFG6:1;
  unsigned PCFG7:1;
  unsigned PCFG8:1;
  unsigned PCFG9:1;
  unsigned PCFG10:1;
  unsigned PCFG11:1;
  unsigned PCFG12:1;
  unsigned PCFG13:1;
  unsigned PCFG14:1;
  unsigned PCFG15:1;
} AD1PCFGLBITS;
extern volatile AD1PCFGLBITS AD1PCFGLbits __attribute__((__sfr__));

extern volatile unsigned int ADPCFG __attribute__((__sfr__));
typedef struct tagADPCFGBITS {
  unsigned PCFG0:1;
  unsigned PCFG1:1;
  unsigned PCFG2:1;
  unsigned PCFG3:1;
  unsigned PCFG4:1;
  unsigned PCFG5:1;
  unsigned PCFG6:1;
  unsigned PCFG7:1;
  unsigned PCFG8:1;
  unsigned PCFG9:1;
  unsigned PCFG10:1;
  unsigned PCFG11:1;
  unsigned PCFG12:1;
  unsigned PCFG13:1;
  unsigned PCFG14:1;
  unsigned PCFG15:1;
} ADPCFGBITS;
extern volatile ADPCFGBITS ADPCFGbits __attribute__((__sfr__));

extern volatile unsigned int AD1CSSH __attribute__((__sfr__));
typedef struct tagAD1CSSHBITS {
  unsigned CSS16:1;
  unsigned CSS17:1;
  unsigned CSS18:1;
  unsigned CSS19:1;
  unsigned CSS20:1;
  unsigned CSS21:1;
  unsigned CSS22:1;
  unsigned CSS23:1;
  unsigned CSS24:1;
  unsigned CSS25:1;
  unsigned CSS26:1;
  unsigned CSS27:1;
  unsigned CSS28:1;
  unsigned CSS29:1;
  unsigned CSS30:1;
  unsigned CSS31:1;
} AD1CSSHBITS;
extern volatile AD1CSSHBITS AD1CSSHbits __attribute__((__sfr__));

extern volatile unsigned int AD1CSSL __attribute__((__sfr__));
typedef struct tagAD1CSSLBITS {
  unsigned CSS0:1;
  unsigned CSS1:1;
  unsigned CSS2:1;
  unsigned CSS3:1;
  unsigned CSS4:1;
  unsigned CSS5:1;
  unsigned CSS6:1;
  unsigned CSS7:1;
  unsigned CSS8:1;
  unsigned CSS9:1;
  unsigned CSS10:1;
  unsigned CSS11:1;
  unsigned CSS12:1;
  unsigned CSS13:1;
  unsigned CSS14:1;
  unsigned CSS15:1;
} AD1CSSLBITS;
extern volatile AD1CSSLBITS AD1CSSLbits __attribute__((__sfr__));

extern volatile unsigned int AD1CON4 __attribute__((__sfr__));
__extension__ typedef struct tagAD1CON4BITS {
  union {
    struct {
      unsigned DMABL:3;
    };
    struct {
      unsigned DMABL0:1;
      unsigned DMABL1:1;
      unsigned DMABL2:1;
    };
  };
} AD1CON4BITS;
extern volatile AD1CON4BITS AD1CON4bits __attribute__((__sfr__));

extern volatile unsigned int ADC2BUF0 __attribute__((__sfr__));
extern volatile unsigned int AD2CON1 __attribute__((__sfr__));
__extension__ typedef struct tagAD2CON1BITS {
  union {
    struct {
      unsigned DONE:1;
      unsigned SAMP:1;
      unsigned ASAM:1;
      unsigned SIMSAM:1;
      unsigned :1;
      unsigned SSRC:3;
      unsigned FORM:2;
      unsigned AD12B:1;
      unsigned :1;
      unsigned ADDMABM:1;
      unsigned ADSIDL:1;
      unsigned :1;
      unsigned ADON:1;
    };
    struct {
      unsigned :5;
      unsigned SSRC0:1;
      unsigned SSRC1:1;
      unsigned SSRC2:1;
      unsigned FORM0:1;
      unsigned FORM1:1;
    };
  };
} AD2CON1BITS;
extern volatile AD2CON1BITS AD2CON1bits __attribute__((__sfr__));

extern volatile unsigned int AD2CON2 __attribute__((__sfr__));
__extension__ typedef struct tagAD2CON2BITS {
  union {
    struct {
      unsigned ALTS:1;
      unsigned BUFM:1;
      unsigned SMPI:4;
      unsigned :1;
      unsigned BUFS:1;
      unsigned CHPS:2;
      unsigned CSCNA:1;
      unsigned :2;
      unsigned VCFG:3;
    };
    struct {
      unsigned :2;
      unsigned SMPI0:1;
      unsigned SMPI1:1;
      unsigned SMPI2:1;
      unsigned SMPI3:1;
      unsigned :2;
      unsigned CHPS0:1;
      unsigned CHPS1:1;
      unsigned :3;
      unsigned VCFG0:1;
      unsigned VCFG1:1;
      unsigned VCFG2:1;
    };
  };
} AD2CON2BITS;
extern volatile AD2CON2BITS AD2CON2bits __attribute__((__sfr__));

extern volatile unsigned int AD2CON3 __attribute__((__sfr__));
__extension__ typedef struct tagAD2CON3BITS {
  union {
    struct {
      unsigned ADCS:8;
      unsigned SAMC:5;
      unsigned :2;
      unsigned ADRC:1;
    };
    struct {
      unsigned ADCS0:1;
      unsigned ADCS1:1;
      unsigned ADCS2:1;
      unsigned ADCS3:1;
      unsigned ADCS4:1;
      unsigned ADCS5:1;
      unsigned ADCS6:1;
      unsigned ADCS7:1;
      unsigned SAMC0:1;
      unsigned SAMC1:1;
      unsigned SAMC2:1;
      unsigned SAMC3:1;
      unsigned SAMC4:1;
    };
  };
} AD2CON3BITS;
extern volatile AD2CON3BITS AD2CON3bits __attribute__((__sfr__));

extern volatile unsigned int AD2CHS123 __attribute__((__sfr__));
__extension__ typedef struct tagAD2CHS123BITS {
  union {
    struct {
      unsigned CH123SA:1;
      unsigned CH123NA:2;
      unsigned :5;
      unsigned CH123SB:1;
      unsigned CH123NB:2;
    };
    struct {
      unsigned :1;
      unsigned CH123NA0:1;
      unsigned CH123NA1:1;
      unsigned :6;
      unsigned CH123NB0:1;
      unsigned CH123NB1:1;
    };
  };
} AD2CHS123BITS;
extern volatile AD2CHS123BITS AD2CHS123bits __attribute__((__sfr__));

extern volatile unsigned int AD2CHS0 __attribute__((__sfr__));
__extension__ typedef struct tagAD2CHS0BITS {
  union {
    struct {
      unsigned CH0SA:5;
      unsigned :2;
      unsigned CH0NA:1;
      unsigned CH0SB:5;
      unsigned :2;
      unsigned CH0NB:1;
    };
    struct {
      unsigned CH0SA0:1;
      unsigned CH0SA1:1;
      unsigned CH0SA2:1;
      unsigned CH0SA3:1;
      unsigned CH0SA4:1;
      unsigned :3;
      unsigned CH0SB0:1;
      unsigned CH0SB1:1;
      unsigned CH0SB2:1;
      unsigned CH0SB3:1;
      unsigned CH0SB4:1;
    };
  };
} AD2CHS0BITS;
extern volatile AD2CHS0BITS AD2CHS0bits __attribute__((__sfr__));

extern volatile unsigned int AD2PCFGL __attribute__((__sfr__));
typedef struct tagAD2PCFGLBITS {
  unsigned PCFG0:1;
  unsigned PCFG1:1;
  unsigned PCFG2:1;
  unsigned PCFG3:1;
  unsigned PCFG4:1;
  unsigned PCFG5:1;
  unsigned PCFG6:1;
  unsigned PCFG7:1;
  unsigned PCFG8:1;
  unsigned PCFG9:1;
  unsigned PCFG10:1;
  unsigned PCFG11:1;
  unsigned PCFG12:1;
  unsigned PCFG13:1;
  unsigned PCFG14:1;
  unsigned PCFG15:1;
} AD2PCFGLBITS;
extern volatile AD2PCFGLBITS AD2PCFGLbits __attribute__((__sfr__));

extern volatile unsigned int AD2CSSL __attribute__((__sfr__));
typedef struct tagAD2CSSLBITS {
  unsigned CSS0:1;
  unsigned CSS1:1;
  unsigned CSS2:1;
  unsigned CSS3:1;
  unsigned CSS4:1;
  unsigned CSS5:1;
  unsigned CSS6:1;
  unsigned CSS7:1;
  unsigned CSS8:1;
  unsigned CSS9:1;
  unsigned CSS10:1;
  unsigned CSS11:1;
  unsigned CSS12:1;
  unsigned CSS13:1;
  unsigned CSS14:1;
  unsigned CSS15:1;
} AD2CSSLBITS;
extern volatile AD2CSSLBITS AD2CSSLbits __attribute__((__sfr__));

extern volatile unsigned int AD2CON4 __attribute__((__sfr__));
__extension__ typedef struct tagAD2CON4BITS {
  union {
    struct {
      unsigned DMABL:3;
    };
    struct {
      unsigned DMABL0:1;
      unsigned DMABL1:1;
      unsigned DMABL2:1;
    };
  };
} AD2CON4BITS;
extern volatile AD2CON4BITS AD2CON4bits __attribute__((__sfr__));

extern volatile unsigned int DMA0CON __attribute__((__sfr__));
__extension__ typedef struct tagDMA0CONBITS {
  union {
    struct {
      unsigned MODE:2;
      unsigned :2;
      unsigned AMODE:2;
      unsigned :5;
      unsigned NULLW:1;
      unsigned HALF:1;
      unsigned DIR:1;
      unsigned SIZE:1;
      unsigned CHEN:1;
    };
    struct {
      unsigned MODE0:1;
      unsigned MODE1:1;
      unsigned :2;
      unsigned AMODE0:1;
      unsigned AMODE1:1;
    };
  };
} DMA0CONBITS;
extern volatile DMA0CONBITS DMA0CONbits __attribute__((__sfr__));

extern volatile unsigned int DMA0REQ __attribute__((__sfr__));
__extension__ typedef struct tagDMA0REQBITS {
  union {
    struct {
      unsigned IRQSEL:7;
      unsigned :8;
      unsigned FORCE:1;
    };
    struct {
      unsigned IRQSEL0:1;
      unsigned IRQSEL1:1;
      unsigned IRQSEL2:1;
      unsigned IRQSEL3:1;
      unsigned IRQSEL4:1;
      unsigned IRQSEL5:1;
      unsigned IRQSEL6:1;
    };
  };
} DMA0REQBITS;
extern volatile DMA0REQBITS DMA0REQbits __attribute__((__sfr__));

extern volatile unsigned int DMA0STA __attribute__((__sfr__));
extern volatile unsigned int DMA0STB __attribute__((__sfr__));
extern volatile unsigned int DMA0PAD __attribute__((__sfr__));
extern volatile unsigned int DMA0CNT __attribute__((__sfr__));
extern volatile unsigned int DMA1CON __attribute__((__sfr__));
__extension__ typedef struct tagDMA1CONBITS {
  union {
    struct {
      unsigned MODE:2;
      unsigned :2;
      unsigned AMODE:2;
      unsigned :5;
      unsigned NULLW:1;
      unsigned HALF:1;
      unsigned DIR:1;
      unsigned SIZE:1;
      unsigned CHEN:1;
    };
    struct {
      unsigned MODE0:1;
      unsigned MODE1:1;
      unsigned :2;
      unsigned AMODE0:1;
      unsigned AMODE1:1;
    };
  };
} DMA1CONBITS;
extern volatile DMA1CONBITS DMA1CONbits __attribute__((__sfr__));

extern volatile unsigned int DMA1REQ __attribute__((__sfr__));
__extension__ typedef struct tagDMA1REQBITS {
  union {
    struct {
      unsigned IRQSEL:7;
      unsigned :8;
      unsigned FORCE:1;
    };
    struct {
      unsigned IRQSEL0:1;
      unsigned IRQSEL1:1;
      unsigned IRQSEL2:1;
      unsigned IRQSEL3:1;
      unsigned IRQSEL4:1;
      unsigned IRQSEL5:1;
      unsigned IRQSEL6:1;
    };
  };
} DMA1REQBITS;
extern volatile DMA1REQBITS DMA1REQbits __attribute__((__sfr__));

extern volatile unsigned int DMA1STA __attribute__((__sfr__));
extern volatile unsigned int DMA1STB __attribute__((__sfr__));
extern volatile unsigned int DMA1PAD __attribute__((__sfr__));
extern volatile unsigned int DMA1CNT __attribute__((__sfr__));
extern volatile unsigned int DMA2CON __attribute__((__sfr__));
__extension__ typedef struct tagDMA2CONBITS {
  union {
    struct {
      unsigned MODE:2;
      unsigned :2;
      unsigned AMODE:2;
      unsigned :5;
      unsigned NULLW:1;
      unsigned HALF:1;
      unsigned DIR:1;
      unsigned SIZE:1;
      unsigned CHEN:1;
    };
    struct {
      unsigned MODE0:1;
      unsigned MODE1:1;
      unsigned :2;
      unsigned AMODE0:1;
      unsigned AMODE1:1;
    };
  };
} DMA2CONBITS;
extern volatile DMA2CONBITS DMA2CONbits __attribute__((__sfr__));

extern volatile unsigned int DMA2REQ __attribute__((__sfr__));
__extension__ typedef struct tagDMA2REQBITS {
  union {
    struct {
      unsigned IRQSEL:7;
      unsigned :8;
      unsigned FORCE:1;
    };
    struct {
      unsigned IRQSEL0:1;
      unsigned IRQSEL1:1;
      unsigned IRQSEL2:1;
      unsigned IRQSEL3:1;
      unsigned IRQSEL4:1;
      unsigned IRQSEL5:1;
      unsigned IRQSEL6:1;
    };
  };
} DMA2REQBITS;
extern volatile DMA2REQBITS DMA2REQbits __attribute__((__sfr__));

extern volatile unsigned int DMA2STA __attribute__((__sfr__));
extern volatile unsigned int DMA2STB __attribute__((__sfr__));
extern volatile unsigned int DMA2PAD __attribute__((__sfr__));
extern volatile unsigned int DMA2CNT __attribute__((__sfr__));
extern volatile unsigned int DMA3CON __attribute__((__sfr__));
__extension__ typedef struct tagDMA3CONBITS {
  union {
    struct {
      unsigned MODE:2;
      unsigned :2;
      unsigned AMODE:2;
      unsigned :5;
      unsigned NULLW:1;
      unsigned HALF:1;
      unsigned DIR:1;
      unsigned SIZE:1;
      unsigned CHEN:1;
    };
    struct {
      unsigned MODE0:1;
      unsigned MODE1:1;
      unsigned :2;
      unsigned AMODE0:1;
      unsigned AMODE1:1;
    };
  };
} DMA3CONBITS;
extern volatile DMA3CONBITS DMA3CONbits __attribute__((__sfr__));

extern volatile unsigned int DMA3REQ __attribute__((__sfr__));
__extension__ typedef struct tagDMA3REQBITS {
  union {
    struct {
      unsigned IRQSEL:7;
      unsigned :8;
      unsigned FORCE:1;
    };
    struct {
      unsigned IRQSEL0:1;
      unsigned IRQSEL1:1;
      unsigned IRQSEL2:1;
      unsigned IRQSEL3:1;
      unsigned IRQSEL4:1;
      unsigned IRQSEL5:1;
      unsigned IRQSEL6:1;
    };
  };
} DMA3REQBITS;
extern volatile DMA3REQBITS DMA3REQbits __attribute__((__sfr__));

extern volatile unsigned int DMA3STA __attribute__((__sfr__));
extern volatile unsigned int DMA3STB __attribute__((__sfr__));
extern volatile unsigned int DMA3PAD __attribute__((__sfr__));
extern volatile unsigned int DMA3CNT __attribute__((__sfr__));
extern volatile unsigned int DMA4CON __attribute__((__sfr__));
__extension__ typedef struct tagDMA4CONBITS {
  union {
    struct {
      unsigned MODE:2;
      unsigned :2;
      unsigned AMODE:2;
      unsigned :5;
      unsigned NULLW:1;
      unsigned HALF:1;
      unsigned DIR:1;
      unsigned SIZE:1;
      unsigned CHEN:1;
    };
    struct {
      unsigned MODE0:1;
      unsigned MODE1:1;
      unsigned :2;
      unsigned AMODE0:1;
      unsigned AMODE1:1;
    };
  };
} DMA4CONBITS;
extern volatile DMA4CONBITS DMA4CONbits __attribute__((__sfr__));

extern volatile unsigned int DMA4REQ __attribute__((__sfr__));
__extension__ typedef struct tagDMA4REQBITS {
  union {
    struct {
      unsigned IRQSEL:7;
      unsigned :8;
      unsigned FORCE:1;
    };
    struct {
      unsigned IRQSEL0:1;
      unsigned IRQSEL1:1;
      unsigned IRQSEL2:1;
      unsigned IRQSEL3:1;
      unsigned IRQSEL4:1;
      unsigned IRQSEL5:1;
      unsigned IRQSEL6:1;
    };
  };
} DMA4REQBITS;
extern volatile DMA4REQBITS DMA4REQbits __attribute__((__sfr__));

extern volatile unsigned int DMA4STA __attribute__((__sfr__));
extern volatile unsigned int DMA4STB __attribute__((__sfr__));
extern volatile unsigned int DMA4PAD __attribute__((__sfr__));
extern volatile unsigned int DMA4CNT __attribute__((__sfr__));
extern volatile unsigned int DMA5CON __attribute__((__sfr__));
__extension__ typedef struct tagDMA5CONBITS {
  union {
    struct {
      unsigned MODE:2;
      unsigned :2;
      unsigned AMODE:2;
      unsigned :5;
      unsigned NULLW:1;
      unsigned HALF:1;
      unsigned DIR:1;
      unsigned SIZE:1;
      unsigned CHEN:1;
    };
    struct {
      unsigned MODE0:1;
      unsigned MODE1:1;
      unsigned :2;
      unsigned AMODE0:1;
      unsigned AMODE1:1;
    };
  };
} DMA5CONBITS;
extern volatile DMA5CONBITS DMA5CONbits __attribute__((__sfr__));

extern volatile unsigned int DMA5REQ __attribute__((__sfr__));
__extension__ typedef struct tagDMA5REQBITS {
  union {
    struct {
      unsigned IRQSEL:7;
      unsigned :8;
      unsigned FORCE:1;
    };
    struct {
      unsigned IRQSEL0:1;
      unsigned IRQSEL1:1;
      unsigned IRQSEL2:1;
      unsigned IRQSEL3:1;
      unsigned IRQSEL4:1;
      unsigned IRQSEL5:1;
      unsigned IRQSEL6:1;
    };
  };
} DMA5REQBITS;
extern volatile DMA5REQBITS DMA5REQbits __attribute__((__sfr__));

extern volatile unsigned int DMA5STA __attribute__((__sfr__));
extern volatile unsigned int DMA5STB __attribute__((__sfr__));
extern volatile unsigned int DMA5PAD __attribute__((__sfr__));
extern volatile unsigned int DMA5CNT __attribute__((__sfr__));
extern volatile unsigned int DMA6CON __attribute__((__sfr__));
__extension__ typedef struct tagDMA6CONBITS {
  union {
    struct {
      unsigned MODE:2;
      unsigned :2;
      unsigned AMODE:2;
      unsigned :5;
      unsigned NULLW:1;
      unsigned HALF:1;
      unsigned DIR:1;
      unsigned SIZE:1;
      unsigned CHEN:1;
    };
    struct {
      unsigned MODE0:1;
      unsigned MODE1:1;
      unsigned :2;
      unsigned AMODE0:1;
      unsigned AMODE1:1;
    };
  };
} DMA6CONBITS;
extern volatile DMA6CONBITS DMA6CONbits __attribute__((__sfr__));

extern volatile unsigned int DMA6REQ __attribute__((__sfr__));
__extension__ typedef struct tagDMA6REQBITS {
  union {
    struct {
      unsigned IRQSEL:7;
      unsigned :8;
      unsigned FORCE:1;
    };
    struct {
      unsigned IRQSEL0:1;
      unsigned IRQSEL1:1;
      unsigned IRQSEL2:1;
      unsigned IRQSEL3:1;
      unsigned IRQSEL4:1;
      unsigned IRQSEL5:1;
      unsigned IRQSEL6:1;
    };
  };
} DMA6REQBITS;
extern volatile DMA6REQBITS DMA6REQbits __attribute__((__sfr__));

extern volatile unsigned int DMA6STA __attribute__((__sfr__));
extern volatile unsigned int DMA6STB __attribute__((__sfr__));
extern volatile unsigned int DMA6PAD __attribute__((__sfr__));
extern volatile unsigned int DMA6CNT __attribute__((__sfr__));
extern volatile unsigned int DMA7CON __attribute__((__sfr__));
__extension__ typedef struct tagDMA7CONBITS {
  union {
    struct {
      unsigned MODE:2;
      unsigned :2;
      unsigned AMODE:2;
      unsigned :5;
      unsigned NULLW:1;
      unsigned HALF:1;
      unsigned DIR:1;
      unsigned SIZE:1;
      unsigned CHEN:1;
    };
    struct {
      unsigned MODE0:1;
      unsigned MODE1:1;
      unsigned :2;
      unsigned AMODE0:1;
      unsigned AMODE1:1;
    };
  };
} DMA7CONBITS;
extern volatile DMA7CONBITS DMA7CONbits __attribute__((__sfr__));

extern volatile unsigned int DMA7REQ __attribute__((__sfr__));
__extension__ typedef struct tagDMA7REQBITS {
  union {
    struct {
      unsigned IRQSEL:7;
      unsigned :8;
      unsigned FORCE:1;
    };
    struct {
      unsigned IRQSEL0:1;
      unsigned IRQSEL1:1;
      unsigned IRQSEL2:1;
      unsigned IRQSEL3:1;
      unsigned IRQSEL4:1;
      unsigned IRQSEL5:1;
      unsigned IRQSEL6:1;
    };
  };
} DMA7REQBITS;
extern volatile DMA7REQBITS DMA7REQbits __attribute__((__sfr__));

extern volatile unsigned int DMA7STA __attribute__((__sfr__));
extern volatile unsigned int DMA7STB __attribute__((__sfr__));
extern volatile unsigned int DMA7PAD __attribute__((__sfr__));
extern volatile unsigned int DMA7CNT __attribute__((__sfr__));
extern volatile unsigned int DMACS0 __attribute__((__sfr__));
__extension__ typedef struct tagDMACS0BITS {
  union {
    struct {
      unsigned XWCOL0:1;
      unsigned XWCOL1:1;
      unsigned XWCOL2:1;
      unsigned XWCOL3:1;
      unsigned XWCOL4:1;
      unsigned XWCOL5:1;
      unsigned XWCOL6:1;
      unsigned XWCOL7:1;
      unsigned PWCOL0:1;
      unsigned PWCOL1:1;
      unsigned PWCOL2:1;
      unsigned PWCOL3:1;
      unsigned PWCOL4:1;
      unsigned PWCOL5:1;
      unsigned PWCOL6:1;
      unsigned PWCOL7:1;
    };
    struct {
      unsigned XWCOL:8;
      unsigned PWCOL:8;
    };
  };
} DMACS0BITS;
extern volatile DMACS0BITS DMACS0bits __attribute__((__sfr__));

extern volatile unsigned int DMACS1 __attribute__((__sfr__));
__extension__ typedef struct tagDMACS1BITS {
  union {
    struct {
      unsigned PPST0:1;
      unsigned PPST1:1;
      unsigned PPST2:1;
      unsigned PPST3:1;
      unsigned PPST4:1;
      unsigned PPST5:1;
      unsigned PPST6:1;
      unsigned PPST7:1;
      unsigned LSTCH:4;
    };
    struct {
      unsigned :8;
      unsigned LSTCH0:1;
      unsigned LSTCH1:1;
      unsigned LSTCH2:1;
      unsigned LSTCH3:1;
    };
    struct {
      unsigned PPST:8;
    };
  };
} DMACS1BITS;
extern volatile DMACS1BITS DMACS1bits __attribute__((__sfr__));

extern volatile unsigned int DSADR __attribute__((__sfr__));
extern volatile unsigned int C1CTRL1 __attribute__((__sfr__));
__extension__ typedef struct tagC1CTRL1BITS {
  union {
    struct {
      unsigned WIN:1;
      unsigned :2;
      unsigned CANCAP:1;
      unsigned :1;
      unsigned OPMODE:3;
      unsigned REQOP:3;
      unsigned CANCKS:1;
      unsigned ABAT:1;
      unsigned CSIDL:1;
    };
    struct {
      unsigned :5;
      unsigned OPMODE0:1;
      unsigned OPMODE1:1;
      unsigned OPMODE2:1;
      unsigned REQOP0:1;
      unsigned REQOP1:1;
      unsigned REQOP2:1;
    };
  };
} C1CTRL1BITS;
extern volatile C1CTRL1BITS C1CTRL1bits __attribute__((__sfr__));

extern volatile unsigned int C1CTRL2 __attribute__((__sfr__));
__extension__ typedef struct tagC1CTRL2BITS {
  union {
    struct {
      unsigned DNCNT:5;
    };
    struct {
      unsigned DNCNT0:1;
      unsigned DNCNT1:1;
      unsigned DNCNT2:1;
      unsigned DNCNT3:1;
      unsigned DNCNT4:1;
    };
  };
} C1CTRL2BITS;
extern volatile C1CTRL2BITS C1CTRL2bits __attribute__((__sfr__));

extern volatile unsigned int C1VEC __attribute__((__sfr__));
__extension__ typedef struct tagC1VECBITS {
  union {
    struct {
      unsigned ICODE:7;
      unsigned :1;
      unsigned FILHIT:5;
    };
    struct {
      unsigned ICODE0:1;
      unsigned ICODE1:1;
      unsigned ICODE2:1;
      unsigned ICODE3:1;
      unsigned ICODE4:1;
      unsigned ICODE5:1;
      unsigned ICODE6:1;
      unsigned :1;
      unsigned FILHIT0:1;
      unsigned FILHIT1:1;
      unsigned FILHIT2:1;
      unsigned FILHIT3:1;
      unsigned FILHIT4:1;
    };
  };
} C1VECBITS;
extern volatile C1VECBITS C1VECbits __attribute__((__sfr__));

extern volatile unsigned int C1FCTRL __attribute__((__sfr__));
__extension__ typedef struct tagC1FCTRLBITS {
  union {
    struct {
      unsigned FSA:5;
      unsigned :8;
      unsigned DMABS:3;
    };
    struct {
      unsigned FSA0:1;
      unsigned FSA1:1;
      unsigned FSA2:1;
      unsigned FSA3:1;
      unsigned FSA4:1;
      unsigned :8;
      unsigned DMABS0:1;
      unsigned DMABS1:1;
      unsigned DMABS2:1;
    };
  };
} C1FCTRLBITS;
extern volatile C1FCTRLBITS C1FCTRLbits __attribute__((__sfr__));

extern volatile unsigned int C1FIFO __attribute__((__sfr__));
__extension__ typedef struct tagC1FIFOBITS {
  union {
    struct {
      unsigned FNRB:6;
      unsigned :2;
      unsigned FBP:6;
    };
    struct {
      unsigned FNRB0:1;
      unsigned FNRB1:1;
      unsigned FNRB2:1;
      unsigned FNRB3:1;
      unsigned FNRB4:1;
      unsigned FNRB5:1;
      unsigned :2;
      unsigned FBP0:1;
      unsigned FBP1:1;
      unsigned FBP2:1;
      unsigned FBP3:1;
      unsigned FBP4:1;
      unsigned FBP5:1;
    };
  };
} C1FIFOBITS;
extern volatile C1FIFOBITS C1FIFObits __attribute__((__sfr__));

extern volatile unsigned int C1INTF __attribute__((__sfr__));
typedef struct tagC1INTFBITS {
  unsigned TBIF:1;
  unsigned RBIF:1;
  unsigned RBOVIF:1;
  unsigned FIFOIF:1;
  unsigned :1;
  unsigned ERRIF:1;
  unsigned WAKIF:1;
  unsigned IVRIF:1;
  unsigned EWARN:1;
  unsigned RXWAR:1;
  unsigned TXWAR:1;
  unsigned RXBP:1;
  unsigned TXBP:1;
  unsigned TXBO:1;
} C1INTFBITS;
extern volatile C1INTFBITS C1INTFbits __attribute__((__sfr__));

extern volatile unsigned int C1INTE __attribute__((__sfr__));
typedef struct tagC1INTEBITS {
  unsigned TBIE:1;
  unsigned RBIE:1;
  unsigned RBOVIE:1;
  unsigned FIFOIE:1;
  unsigned :1;
  unsigned ERRIE:1;
  unsigned WAKIE:1;
  unsigned IVRIE:1;
} C1INTEBITS;
extern volatile C1INTEBITS C1INTEbits __attribute__((__sfr__));

extern volatile unsigned int C1EC __attribute__((__sfr__));
__extension__ typedef struct tagC1ECBITS {
  union {
    struct {
      unsigned RERRCNT:8;
      unsigned TERRCNT:8;
    };
    struct {
      unsigned RERRCNT0:1;
      unsigned RERRCNT1:1;
      unsigned RERRCNT2:1;
      unsigned RERRCNT3:1;
      unsigned RERRCNT4:1;
      unsigned RERRCNT5:1;
      unsigned RERRCNT6:1;
      unsigned RERRCNT7:1;
      unsigned TERRCNT0:1;
      unsigned TERRCNT1:1;
      unsigned TERRCNT2:1;
      unsigned TERRCNT3:1;
      unsigned TERRCNT4:1;
      unsigned TERRCNT5:1;
      unsigned TERRCNT6:1;
      unsigned TERRCNT7:1;
    };
  };
} C1ECBITS;
extern volatile C1ECBITS C1ECbits __attribute__((__sfr__));

extern volatile unsigned char C1RERRCNT __attribute__((__sfr__));
extern volatile unsigned char C1TERRCNT __attribute__((__sfr__));
extern volatile unsigned int C1CFG1 __attribute__((__sfr__));
__extension__ typedef struct tagC1CFG1BITS {
  union {
    struct {
      unsigned BRP:6;
      unsigned SJW:2;
    };
    struct {
      unsigned BRP0:1;
      unsigned BRP1:1;
      unsigned BRP2:1;
      unsigned BRP3:1;
      unsigned BRP4:1;
      unsigned BRP5:1;
      unsigned SJW0:1;
      unsigned SJW1:1;
    };
  };
} C1CFG1BITS;
extern volatile C1CFG1BITS C1CFG1bits __attribute__((__sfr__));

extern volatile unsigned int C1CFG2 __attribute__((__sfr__));
__extension__ typedef struct tagC1CFG2BITS {
  union {
    struct {
      unsigned PRSEG:3;
      unsigned SEG1PH:3;
      unsigned SAM:1;
      unsigned SEG2PHTS:1;
      unsigned SEG2PH:3;
      unsigned :3;
      unsigned WAKFIL:1;
    };
    struct {
      unsigned PRSEG0:1;
      unsigned PRSEG1:1;
      unsigned PRSEG2:1;
      unsigned SEG1PH0:1;
      unsigned SEG1PH1:1;
      unsigned SEG1PH2:1;
      unsigned :2;
      unsigned SEG2PH0:1;
      unsigned SEG2PH1:1;
      unsigned SEG2PH2:1;
    };
  };
} C1CFG2BITS;
extern volatile C1CFG2BITS C1CFG2bits __attribute__((__sfr__));

extern volatile unsigned int C1FEN1 __attribute__((__sfr__));
__extension__ typedef struct tagC1FEN1BITS {
  union {
    struct {
      unsigned FLTEN0:1;
      unsigned FLTEN1:1;
      unsigned FLTEN2:1;
      unsigned FLTEN3:1;
      unsigned FLTEN4:1;
      unsigned FLTEN5:1;
      unsigned FLTEN6:1;
      unsigned FLTEN7:1;
      unsigned FLTEN8:1;
      unsigned FLTEN9:1;
      unsigned FLTEN10:1;
      unsigned FLTEN11:1;
      unsigned FLTEN12:1;
      unsigned FLTEN13:1;
      unsigned FLTEN14:1;
      unsigned FLTEN15:1;
    };
    struct {
      unsigned FLTEN:16;
    };
  };
} C1FEN1BITS;
extern volatile C1FEN1BITS C1FEN1bits __attribute__((__sfr__));

extern volatile unsigned int C1FMSKSEL1 __attribute__((__sfr__));
__extension__ typedef struct tagC1FMSKSEL1BITS {
  union {
    struct {
      unsigned F0MSK:2;
      unsigned F1MSK:2;
      unsigned F2MSK:2;
      unsigned F3MSK:2;
      unsigned F4MSK:2;
      unsigned F5MSK:2;
      unsigned F6MSK:2;
      unsigned F7MSK:2;
    };
    struct {
      unsigned F0MSK0:1;
      unsigned F0MSK1:1;
      unsigned F1MSK0:1;
      unsigned F1MSK1:1;
      unsigned F2MSK0:1;
      unsigned F2MSK1:1;
      unsigned F3MSK0:1;
      unsigned F3MSK1:1;
      unsigned F4MSK0:1;
      unsigned F4MSK1:1;
      unsigned F5MSK0:1;
      unsigned F5MSK1:1;
      unsigned F6MSK0:1;
      unsigned F6MSK1:1;
      unsigned F7MSK0:1;
      unsigned F7MSK1:1;
    };
  };
} C1FMSKSEL1BITS;
extern volatile C1FMSKSEL1BITS C1FMSKSEL1bits __attribute__((__sfr__));

extern volatile unsigned int C1FMSKSEL2 __attribute__((__sfr__));
__extension__ typedef struct tagC1FMSKSEL2BITS {
  union {
    struct {
      unsigned F8MSK:2;
      unsigned F9MSK:2;
      unsigned F10MSK:2;
      unsigned F11MSK:2;
      unsigned F12MSK:2;
      unsigned F13MSK:2;
      unsigned F14MSK:2;
      unsigned F15MSK:2;
    };
    struct {
      unsigned F8MSK0:1;
      unsigned F8MSK1:1;
      unsigned F9MSK0:1;
      unsigned F9MSK1:1;
      unsigned F10MSK0:1;
      unsigned F10MSK1:1;
      unsigned F11MSK0:1;
      unsigned F11MSK1:1;
      unsigned F12MSK0:1;
      unsigned F12MSK1:1;
      unsigned F13MSK0:1;
      unsigned F13MSK1:1;
      unsigned F14MSK0:1;
      unsigned F14MSK1:1;
      unsigned F15MSK0:1;
      unsigned F15MSK1:1;
    };
  };
} C1FMSKSEL2BITS;
extern volatile C1FMSKSEL2BITS C1FMSKSEL2bits __attribute__((__sfr__));

extern volatile unsigned int C1BUFPNT1 __attribute__((__sfr__));
__extension__ typedef struct tagC1BUFPNT1BITS {
  union {
    struct {
      unsigned F0BP:4;
      unsigned F1BP:4;
      unsigned F2BP:4;
      unsigned F3BP:4;
    };
    struct {
      unsigned F0BP0:1;
      unsigned F0BP1:1;
      unsigned F0BP2:1;
      unsigned F0BP3:1;
      unsigned F1BP0:1;
      unsigned F1BP1:1;
      unsigned F1BP2:1;
      unsigned F1BP3:1;
      unsigned F2BP0:1;
      unsigned F2BP1:1;
      unsigned F2BP2:1;
      unsigned F2BP3:1;
      unsigned F3BP0:1;
      unsigned F3BP1:1;
      unsigned F3BP2:1;
      unsigned F3BP3:1;
    };
  };
} C1BUFPNT1BITS;
extern volatile C1BUFPNT1BITS C1BUFPNT1bits __attribute__((__sfr__));

extern volatile unsigned int C1RXFUL1 __attribute__((__sfr__));
typedef struct tagC1RXFUL1BITS {
  unsigned RXFUL0:1;
  unsigned RXFUL1:1;
  unsigned RXFUL2:1;
  unsigned RXFUL3:1;
  unsigned RXFUL4:1;
  unsigned RXFUL5:1;
  unsigned RXFUL6:1;
  unsigned RXFUL7:1;
  unsigned RXFUL8:1;
  unsigned RXFUL9:1;
  unsigned RXFUL10:1;
  unsigned RXFUL11:1;
  unsigned RXFUL12:1;
  unsigned RXFUL13:1;
  unsigned RXFUL14:1;
  unsigned RXFUL15:1;
} C1RXFUL1BITS;
extern volatile C1RXFUL1BITS C1RXFUL1bits __attribute__((__sfr__));

extern volatile unsigned int C1BUFPNT2 __attribute__((__sfr__));
__extension__ typedef struct tagC1BUFPNT2BITS {
  union {
    struct {
      unsigned F4BP:4;
      unsigned F5BP:4;
      unsigned F6BP:4;
      unsigned F7BP:4;
    };
    struct {
      unsigned F4BP0:1;
      unsigned F4BP1:1;
      unsigned F4BP2:1;
      unsigned F4BP3:1;
      unsigned F5BP0:1;
      unsigned F5BP1:1;
      unsigned F5BP2:1;
      unsigned F5BP3:1;
      unsigned F6BP0:1;
      unsigned F6BP1:1;
      unsigned F6BP2:1;
      unsigned F6BP3:1;
      unsigned F7BP0:1;
      unsigned F7BP1:1;
      unsigned F7BP2:1;
      unsigned F7BP3:1;
    };
  };
} C1BUFPNT2BITS;
extern volatile C1BUFPNT2BITS C1BUFPNT2bits __attribute__((__sfr__));

extern volatile unsigned int C1RXFUL2 __attribute__((__sfr__));
typedef struct tagC1RXFUL2BITS {
  unsigned RXFUL16:1;
  unsigned RXFUL17:1;
  unsigned RXFUL18:1;
  unsigned RXFUL19:1;
  unsigned RXFUL20:1;
  unsigned RXFUL21:1;
  unsigned RXFUL22:1;
  unsigned RXFUL23:1;
  unsigned RXFUL24:1;
  unsigned RXFUL25:1;
  unsigned RXFUL26:1;
  unsigned RXFUL27:1;
  unsigned RXFUL28:1;
  unsigned RXFUL29:1;
  unsigned RXFUL30:1;
  unsigned RXFUL31:1;
} C1RXFUL2BITS;
extern volatile C1RXFUL2BITS C1RXFUL2bits __attribute__((__sfr__));

extern volatile unsigned int C1BUFPNT3 __attribute__((__sfr__));
__extension__ typedef struct tagC1BUFPNT3BITS {
  union {
    struct {
      unsigned F8BP:4;
      unsigned F9BP:4;
      unsigned F10BP:4;
      unsigned F11BP:4;
    };
    struct {
      unsigned F8BP0:1;
      unsigned F8BP1:1;
      unsigned F8BP2:1;
      unsigned F8BP3:1;
      unsigned F9BP0:1;
      unsigned F9BP1:1;
      unsigned F9BP2:1;
      unsigned F9BP3:1;
      unsigned F10BP0:1;
      unsigned F10BP1:1;
      unsigned F10BP2:1;
      unsigned F10BP3:1;
      unsigned F11BP0:1;
      unsigned F11BP1:1;
      unsigned F11BP2:1;
      unsigned F11BP3:1;
    };
  };
} C1BUFPNT3BITS;
extern volatile C1BUFPNT3BITS C1BUFPNT3bits __attribute__((__sfr__));

extern volatile unsigned int C1BUFPNT4 __attribute__((__sfr__));
__extension__ typedef struct tagC1BUFPNT4BITS {
  union {
    struct {
      unsigned F12BP:4;
      unsigned F13BP:4;
      unsigned F14BP:4;
      unsigned F15BP:4;
    };
    struct {
      unsigned F12BP0:1;
      unsigned F12BP1:1;
      unsigned F12BP2:1;
      unsigned F12BP3:1;
      unsigned F13BP0:1;
      unsigned F13BP1:1;
      unsigned F13BP2:1;
      unsigned F13BP3:1;
      unsigned F14BP0:1;
      unsigned F14BP1:1;
      unsigned F14BP2:1;
      unsigned F14BP3:1;
      unsigned F15BP0:1;
      unsigned F15BP1:1;
      unsigned F15BP2:1;
      unsigned F15BP3:1;
    };
  };
} C1BUFPNT4BITS;
extern volatile C1BUFPNT4BITS C1BUFPNT4bits __attribute__((__sfr__));

extern volatile unsigned int C1RXOVF1 __attribute__((__sfr__));
typedef struct tagC1RXOVF1BITS {
  unsigned RXOVF0:1;
  unsigned RXOVF1:1;
  unsigned RXOVF2:1;
  unsigned RXOVF3:1;
  unsigned RXOVF4:1;
  unsigned RXOVF5:1;
  unsigned RXOVF6:1;
  unsigned RXOVF7:1;
  unsigned RXOVF8:1;
  unsigned RXOVF9:1;
  unsigned RXOVF10:1;
  unsigned RXOVF11:1;
  unsigned RXOVF12:1;
  unsigned RXOVF13:1;
  unsigned RXOVF14:1;
  unsigned RXOVF15:1;
} C1RXOVF1BITS;
extern volatile C1RXOVF1BITS C1RXOVF1bits __attribute__((__sfr__));

extern volatile unsigned int C1RXOVF2 __attribute__((__sfr__));
typedef struct tagC1RXOVF2BITS {
  unsigned RXOVF16:1;
  unsigned RXOVF17:1;
  unsigned RXOVF18:1;
  unsigned RXOVF19:1;
  unsigned RXOVF20:1;
  unsigned RXOVF21:1;
  unsigned RXOVF22:1;
  unsigned RXOVF23:1;
  unsigned RXOVF24:1;
  unsigned RXOVF25:1;
  unsigned RXOVF26:1;
  unsigned RXOVF27:1;
  unsigned RXOVF28:1;
  unsigned RXOVF29:1;
  unsigned RXOVF30:1;
  unsigned RXOVF31:1;
} C1RXOVF2BITS;
extern volatile C1RXOVF2BITS C1RXOVF2bits __attribute__((__sfr__));

extern volatile unsigned int C1RXM0SID __attribute__((__sfr__));
__extension__ typedef struct tagC1RXM0SIDBITS {
  union {
    struct {
      unsigned EID16:1;
      unsigned EID17:1;
      unsigned :1;
      unsigned MIDE:1;
      unsigned :1;
      unsigned SID0:1;
      unsigned SID1:1;
      unsigned SID2:1;
      unsigned SID3:1;
      unsigned SID4:1;
      unsigned SID5:1;
      unsigned SID6:1;
      unsigned SID7:1;
      unsigned SID8:1;
      unsigned SID9:1;
      unsigned SID10:1;
    };
    struct {
      unsigned EID:2;
      unsigned :3;
      unsigned SID:11;
    };
  };
} C1RXM0SIDBITS;
extern volatile C1RXM0SIDBITS C1RXM0SIDbits __attribute__((__sfr__));

extern volatile unsigned int C1TR01CON __attribute__((__sfr__));
__extension__ typedef struct tagC1TR01CONBITS {
  union {
    struct {
      unsigned TX0PRI:2;
      unsigned RTREN0:1;
      unsigned TXREQ0:1;
      unsigned TXERR0:1;
      unsigned TXLARB0:1;
      unsigned TXABT0:1;
      unsigned TXEN0:1;
      unsigned TX1PRI:2;
      unsigned RTREN1:1;
      unsigned TXREQ1:1;
      unsigned TXERR1:1;
      unsigned TXLARB1:1;
      unsigned TXABT1:1;
      unsigned TXEN1:1;
    };
    struct {
      unsigned TX0PRI0:1;
      unsigned TX0PRI1:1;
      unsigned :6;
      unsigned TX1PRI0:1;
      unsigned TX1PRI1:1;
    };
  };
} C1TR01CONBITS;
extern volatile C1TR01CONBITS C1TR01CONbits __attribute__((__sfr__));

extern volatile unsigned int C1RXM0EID __attribute__((__sfr__));
__extension__ typedef struct tagC1RXM0EIDBITS {
  union {
    struct {
      unsigned EID0:1;
      unsigned EID1:1;
      unsigned EID2:1;
      unsigned EID3:1;
      unsigned EID4:1;
      unsigned EID5:1;
      unsigned EID6:1;
      unsigned EID7:1;
      unsigned EID8:1;
      unsigned EID9:1;
      unsigned EID10:1;
      unsigned EID11:1;
      unsigned EID12:1;
      unsigned EID13:1;
      unsigned EID14:1;
      unsigned EID15:1;
    };
    struct {
      unsigned EID:16;
    };
  };
} C1RXM0EIDBITS;
extern volatile C1RXM0EIDBITS C1RXM0EIDbits __attribute__((__sfr__));

extern volatile unsigned int C1TR23CON __attribute__((__sfr__));
__extension__ typedef struct tagC1TR23CONBITS {
  union {
    struct {
      unsigned TX2PRI:2;
      unsigned RTREN2:1;
      unsigned TXREQ2:1;
      unsigned TXERR2:1;
      unsigned TXLARB2:1;
      unsigned TXABT2:1;
      unsigned TXEN2:1;
      unsigned TX3PRI:2;
      unsigned RTREN3:1;
      unsigned TXREQ3:1;
      unsigned TXERR3:1;
      unsigned TXLARB3:1;
      unsigned TXABT3:1;
      unsigned TXEN3:1;
    };
    struct {
      unsigned TX2PRI0:1;
      unsigned TX2PRI1:1;
      unsigned :6;
      unsigned TX3PRI0:1;
      unsigned TX3PRI1:1;
    };
  };
} C1TR23CONBITS;
extern volatile C1TR23CONBITS C1TR23CONbits __attribute__((__sfr__));

extern volatile unsigned int C1RXM1SID __attribute__((__sfr__));
__extension__ typedef struct tagC1RXM1SIDBITS {
  union {
    struct {
      unsigned EID16:1;
      unsigned EID17:1;
      unsigned :1;
      unsigned MIDE:1;
      unsigned :1;
      unsigned SID0:1;
      unsigned SID1:1;
      unsigned SID2:1;
      unsigned SID3:1;
      unsigned SID4:1;
      unsigned SID5:1;
      unsigned SID6:1;
      unsigned SID7:1;
      unsigned SID8:1;
      unsigned SID9:1;
      unsigned SID10:1;
    };
    struct {
      unsigned EID:2;
      unsigned :3;
      unsigned SID:11;
    };
  };
} C1RXM1SIDBITS;
extern volatile C1RXM1SIDBITS C1RXM1SIDbits __attribute__((__sfr__));

extern volatile unsigned int C1TR45CON __attribute__((__sfr__));
__extension__ typedef struct tagC1TR45CONBITS {
  union {
    struct {
      unsigned TX4PRI:2;
      unsigned RTREN4:1;
      unsigned TXREQ4:1;
      unsigned TXERR4:1;
      unsigned TXLARB4:1;
      unsigned TXABT4:1;
      unsigned TXEN4:1;
      unsigned TX5PRI:2;
      unsigned RTREN5:1;
      unsigned TXREQ5:1;
      unsigned TXERR5:1;
      unsigned TXLARB5:1;
      unsigned TXABT5:1;
      unsigned TXEN5:1;
    };
    struct {
      unsigned TX4PRI0:1;
      unsigned TX4PRI1:1;
      unsigned :6;
      unsigned TX5PRI0:1;
      unsigned TX5PRI1:1;
    };
  };
} C1TR45CONBITS;
extern volatile C1TR45CONBITS C1TR45CONbits __attribute__((__sfr__));

extern volatile unsigned int C1RXM1EID __attribute__((__sfr__));
__extension__ typedef struct tagC1RXM1EIDBITS {
  union {
    struct {
      unsigned EID0:1;
      unsigned EID1:1;
      unsigned EID2:1;
      unsigned EID3:1;
      unsigned EID4:1;
      unsigned EID5:1;
      unsigned EID6:1;
      unsigned EID7:1;
      unsigned EID8:1;
      unsigned EID9:1;
      unsigned EID10:1;
      unsigned EID11:1;
      unsigned EID12:1;
      unsigned EID13:1;
      unsigned EID14:1;
      unsigned EID15:1;
    };
    struct {
      unsigned EID:16;
    };
  };
} C1RXM1EIDBITS;
extern volatile C1RXM1EIDBITS C1RXM1EIDbits __attribute__((__sfr__));

extern volatile unsigned int C1TR67CON __attribute__((__sfr__));
__extension__ typedef struct tagC1TR67CONBITS {
  union {
    struct {
      unsigned TX6PRI:2;
      unsigned RTREN6:1;
      unsigned TXREQ6:1;
      unsigned TXERR6:1;
      unsigned TXLARB6:1;
      unsigned TXABT6:1;
      unsigned TXEN6:1;
      unsigned TX7PRI:2;
      unsigned RTREN7:1;
      unsigned TXREQ7:1;
      unsigned TXERR7:1;
      unsigned TXLARB7:1;
      unsigned TXABT7:1;
      unsigned TXEN7:1;
    };
    struct {
      unsigned TX6PRI0:1;
      unsigned TX6PRI1:1;
      unsigned :6;
      unsigned TX7PRI0:1;
      unsigned TX7PRI1:1;
    };
  };
} C1TR67CONBITS;
extern volatile C1TR67CONBITS C1TR67CONbits __attribute__((__sfr__));

extern volatile unsigned int C1RXM2SID __attribute__((__sfr__));
__extension__ typedef struct tagC1RXM2SIDBITS {
  union {
    struct {
      unsigned EID16:1;
      unsigned EID17:1;
      unsigned :1;
      unsigned MIDE:1;
      unsigned :1;
      unsigned SID0:1;
      unsigned SID1:1;
      unsigned SID2:1;
      unsigned SID3:1;
      unsigned SID4:1;
      unsigned SID5:1;
      unsigned SID6:1;
      unsigned SID7:1;
      unsigned SID8:1;
      unsigned SID9:1;
      unsigned SID10:1;
    };
    struct {
      unsigned EID:2;
      unsigned :3;
      unsigned SID:11;
    };
  };
} C1RXM2SIDBITS;
extern volatile C1RXM2SIDBITS C1RXM2SIDbits __attribute__((__sfr__));

extern volatile unsigned int C1RXM2EID __attribute__((__sfr__));
__extension__ typedef struct tagC1RXM2EIDBITS {
  union {
    struct {
      unsigned EID0:1;
      unsigned EID1:1;
      unsigned EID2:1;
      unsigned EID3:1;
      unsigned EID4:1;
      unsigned EID5:1;
      unsigned EID6:1;
      unsigned EID7:1;
      unsigned EID8:1;
      unsigned EID9:1;
      unsigned EID10:1;
      unsigned EID11:1;
      unsigned EID12:1;
      unsigned EID13:1;
      unsigned EID14:1;
      unsigned EID15:1;
    };
    struct {
      unsigned EID:16;
    };
  };
} C1RXM2EIDBITS;
extern volatile C1RXM2EIDBITS C1RXM2EIDbits __attribute__((__sfr__));

extern volatile unsigned int C1RXD __attribute__((__sfr__));
extern volatile unsigned int C1RXF0SID __attribute__((__sfr__));
__extension__ typedef struct tagC1RXF0SIDBITS {
  union {
    struct {
      unsigned EID16:1;
      unsigned EID17:1;
      unsigned :1;
      unsigned EXIDE:1;
      unsigned :1;
      unsigned SID0:1;
      unsigned SID1:1;
      unsigned SID2:1;
      unsigned SID3:1;
      unsigned SID4:1;
      unsigned SID5:1;
      unsigned SID6:1;
      unsigned SID7:1;
      unsigned SID8:1;
      unsigned SID9:1;
      unsigned SID10:1;
    };
    struct {
      unsigned EID:2;
      unsigned :3;
      unsigned SID:11;
    };
  };
} C1RXF0SIDBITS;
extern volatile C1RXF0SIDBITS C1RXF0SIDbits __attribute__((__sfr__));

extern volatile unsigned int C1RXF0EID __attribute__((__sfr__));
typedef struct tagC1RXF0EIDBITS {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} C1RXF0EIDBITS;
extern volatile C1RXF0EIDBITS C1RXF0EIDbits __attribute__((__sfr__));

extern volatile unsigned int C1TXD __attribute__((__sfr__));
extern volatile unsigned int C1RXF1SID __attribute__((__sfr__));
__extension__ typedef struct tagC1RXF1SIDBITS {
  union {
    struct {
      unsigned EID16:1;
      unsigned EID17:1;
      unsigned :1;
      unsigned EXIDE:1;
      unsigned :1;
      unsigned SID0:1;
      unsigned SID1:1;
      unsigned SID2:1;
      unsigned SID3:1;
      unsigned SID4:1;
      unsigned SID5:1;
      unsigned SID6:1;
      unsigned SID7:1;
      unsigned SID8:1;
      unsigned SID9:1;
      unsigned SID10:1;
    };
    struct {
      unsigned :5;
      unsigned SID:11;
    };
  };
} C1RXF1SIDBITS;
extern volatile C1RXF1SIDBITS C1RXF1SIDbits __attribute__((__sfr__));

extern volatile unsigned int C1RXF1EID __attribute__((__sfr__));
typedef struct tagC1RXF1EIDBITS {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} C1RXF1EIDBITS;
extern volatile C1RXF1EIDBITS C1RXF1EIDbits __attribute__((__sfr__));

extern volatile unsigned int C1RXF2SID __attribute__((__sfr__));
__extension__ typedef struct tagC1RXF2SIDBITS {
  union {
    struct {
      unsigned EID16:1;
      unsigned EID17:1;
      unsigned :1;
      unsigned EXIDE:1;
      unsigned :1;
      unsigned SID0:1;
      unsigned SID1:1;
      unsigned SID2:1;
      unsigned SID3:1;
      unsigned SID4:1;
      unsigned SID5:1;
      unsigned SID6:1;
      unsigned SID7:1;
      unsigned SID8:1;
      unsigned SID9:1;
      unsigned SID10:1;
    };
    struct {
      unsigned :5;
      unsigned SID:11;
    };
  };
} C1RXF2SIDBITS;
extern volatile C1RXF2SIDBITS C1RXF2SIDbits __attribute__((__sfr__));

extern volatile unsigned int C1RXF2EID __attribute__((__sfr__));
typedef struct tagC1RXF2EIDBITS {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} C1RXF2EIDBITS;
extern volatile C1RXF2EIDBITS C1RXF2EIDbits __attribute__((__sfr__));

extern volatile unsigned int C1RXF3SID __attribute__((__sfr__));
__extension__ typedef struct tagC1RXF3SIDBITS {
  union {
    struct {
      unsigned EID16:1;
      unsigned EID17:1;
      unsigned :1;
      unsigned EXIDE:1;
      unsigned :1;
      unsigned SID0:1;
      unsigned SID1:1;
      unsigned SID2:1;
      unsigned SID3:1;
      unsigned SID4:1;
      unsigned SID5:1;
      unsigned SID6:1;
      unsigned SID7:1;
      unsigned SID8:1;
      unsigned SID9:1;
      unsigned SID10:1;
    };
    struct {
      unsigned :5;
      unsigned SID:11;
    };
  };
} C1RXF3SIDBITS;
extern volatile C1RXF3SIDBITS C1RXF3SIDbits __attribute__((__sfr__));

extern volatile unsigned int C1RXF3EID __attribute__((__sfr__));
typedef struct tagC1RXF3EIDBITS {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} C1RXF3EIDBITS;
extern volatile C1RXF3EIDBITS C1RXF3EIDbits __attribute__((__sfr__));

extern volatile unsigned int C1RXF4SID __attribute__((__sfr__));
__extension__ typedef struct tagC1RXF4SIDBITS {
  union {
    struct {
      unsigned EID16:1;
      unsigned EID17:1;
      unsigned :1;
      unsigned EXIDE:1;
      unsigned :1;
      unsigned SID0:1;
      unsigned SID1:1;
      unsigned SID2:1;
      unsigned SID3:1;
      unsigned SID4:1;
      unsigned SID5:1;
      unsigned SID6:1;
      unsigned SID7:1;
      unsigned SID8:1;
      unsigned SID9:1;
      unsigned SID10:1;
    };
    struct {
      unsigned :5;
      unsigned SID:11;
    };
  };
} C1RXF4SIDBITS;
extern volatile C1RXF4SIDBITS C1RXF4SIDbits __attribute__((__sfr__));

extern volatile unsigned int C1RXF4EID __attribute__((__sfr__));
typedef struct tagC1RXF4EIDBITS {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} C1RXF4EIDBITS;
extern volatile C1RXF4EIDBITS C1RXF4EIDbits __attribute__((__sfr__));

extern volatile unsigned int C1RXF5SID __attribute__((__sfr__));
__extension__ typedef struct tagC1RXF5SIDBITS {
  union {
    struct {
      unsigned EID16:1;
      unsigned EID17:1;
      unsigned :1;
      unsigned EXIDE:1;
      unsigned :1;
      unsigned SID0:1;
      unsigned SID1:1;
      unsigned SID2:1;
      unsigned SID3:1;
      unsigned SID4:1;
      unsigned SID5:1;
      unsigned SID6:1;
      unsigned SID7:1;
      unsigned SID8:1;
      unsigned SID9:1;
      unsigned SID10:1;
    };
    struct {
      unsigned :5;
      unsigned SID:11;
    };
  };
} C1RXF5SIDBITS;
extern volatile C1RXF5SIDBITS C1RXF5SIDbits __attribute__((__sfr__));

extern volatile unsigned int C1RXF5EID __attribute__((__sfr__));
typedef struct tagC1RXF5EIDBITS {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} C1RXF5EIDBITS;
extern volatile C1RXF5EIDBITS C1RXF5EIDbits __attribute__((__sfr__));

extern volatile unsigned int C1RXF6SID __attribute__((__sfr__));
__extension__ typedef struct tagC1RXF6SIDBITS {
  union {
    struct {
      unsigned EID16:1;
      unsigned EID17:1;
      unsigned :1;
      unsigned EXIDE:1;
      unsigned :1;
      unsigned SID0:1;
      unsigned SID1:1;
      unsigned SID2:1;
      unsigned SID3:1;
      unsigned SID4:1;
      unsigned SID5:1;
      unsigned SID6:1;
      unsigned SID7:1;
      unsigned SID8:1;
      unsigned SID9:1;
      unsigned SID10:1;
    };
    struct {
      unsigned :5;
      unsigned SID:11;
    };
  };
} C1RXF6SIDBITS;
extern volatile C1RXF6SIDBITS C1RXF6SIDbits __attribute__((__sfr__));

extern volatile unsigned int C1RXF6EID __attribute__((__sfr__));
typedef struct tagC1RXF6EIDBITS {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} C1RXF6EIDBITS;
extern volatile C1RXF6EIDBITS C1RXF6EIDbits __attribute__((__sfr__));

extern volatile unsigned int C1RXF7SID __attribute__((__sfr__));
__extension__ typedef struct tagC1RXF7SIDBITS {
  union {
    struct {
      unsigned EID16:1;
      unsigned EID17:1;
      unsigned :1;
      unsigned EXIDE:1;
      unsigned :1;
      unsigned SID0:1;
      unsigned SID1:1;
      unsigned SID2:1;
      unsigned SID3:1;
      unsigned SID4:1;
      unsigned SID5:1;
      unsigned SID6:1;
      unsigned SID7:1;
      unsigned SID8:1;
      unsigned SID9:1;
      unsigned SID10:1;
    };
    struct {
      unsigned :5;
      unsigned SID:11;
    };
  };
} C1RXF7SIDBITS;
extern volatile C1RXF7SIDBITS C1RXF7SIDbits __attribute__((__sfr__));

extern volatile unsigned int C1RXF7EID __attribute__((__sfr__));
typedef struct tagC1RXF7EIDBITS {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} C1RXF7EIDBITS;
extern volatile C1RXF7EIDBITS C1RXF7EIDbits __attribute__((__sfr__));

extern volatile unsigned int C1RXF8SID __attribute__((__sfr__));
__extension__ typedef struct tagC1RXF8SIDBITS {
  union {
    struct {
      unsigned EID16:1;
      unsigned EID17:1;
      unsigned :1;
      unsigned EXIDE:1;
      unsigned :1;
      unsigned SID0:1;
      unsigned SID1:1;
      unsigned SID2:1;
      unsigned SID3:1;
      unsigned SID4:1;
      unsigned SID5:1;
      unsigned SID6:1;
      unsigned SID7:1;
      unsigned SID8:1;
      unsigned SID9:1;
      unsigned SID10:1;
    };
    struct {
      unsigned :5;
      unsigned SID:11;
    };
  };
} C1RXF8SIDBITS;
extern volatile C1RXF8SIDBITS C1RXF8SIDbits __attribute__((__sfr__));

extern volatile unsigned int C1RXF8EID __attribute__((__sfr__));
typedef struct tagC1RXF8EIDBITS {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} C1RXF8EIDBITS;
extern volatile C1RXF8EIDBITS C1RXF8EIDbits __attribute__((__sfr__));

extern volatile unsigned int C1RXF9SID __attribute__((__sfr__));
__extension__ typedef struct tagC1RXF9SIDBITS {
  union {
    struct {
      unsigned EID16:1;
      unsigned EID17:1;
      unsigned :1;
      unsigned EXIDE:1;
      unsigned :1;
      unsigned SID0:1;
      unsigned SID1:1;
      unsigned SID2:1;
      unsigned SID3:1;
      unsigned SID4:1;
      unsigned SID5:1;
      unsigned SID6:1;
      unsigned SID7:1;
      unsigned SID8:1;
      unsigned SID9:1;
      unsigned SID10:1;
    };
    struct {
      unsigned :5;
      unsigned SID:11;
    };
  };
} C1RXF9SIDBITS;
extern volatile C1RXF9SIDBITS C1RXF9SIDbits __attribute__((__sfr__));

extern volatile unsigned int C1RXF9EID __attribute__((__sfr__));
typedef struct tagC1RXF9EIDBITS {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} C1RXF9EIDBITS;
extern volatile C1RXF9EIDBITS C1RXF9EIDbits __attribute__((__sfr__));

extern volatile unsigned int C1RXF10SID __attribute__((__sfr__));
__extension__ typedef struct tagC1RXF10SIDBITS {
  union {
    struct {
      unsigned EID16:1;
      unsigned EID17:1;
      unsigned :1;
      unsigned EXIDE:1;
      unsigned :1;
      unsigned SID0:1;
      unsigned SID1:1;
      unsigned SID2:1;
      unsigned SID3:1;
      unsigned SID4:1;
      unsigned SID5:1;
      unsigned SID6:1;
      unsigned SID7:1;
      unsigned SID8:1;
      unsigned SID9:1;
      unsigned SID10:1;
    };
    struct {
      unsigned :5;
      unsigned SID:11;
    };
  };
} C1RXF10SIDBITS;
extern volatile C1RXF10SIDBITS C1RXF10SIDbits __attribute__((__sfr__));

extern volatile unsigned int C1RXF10EID __attribute__((__sfr__));
typedef struct tagC1RXF10EIDBITS {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} C1RXF10EIDBITS;
extern volatile C1RXF10EIDBITS C1RXF10EIDbits __attribute__((__sfr__));

extern volatile unsigned int C1RXF11SID __attribute__((__sfr__));
__extension__ typedef struct tagC1RXF11SIDBITS {
  union {
    struct {
      unsigned EID16:1;
      unsigned EID17:1;
      unsigned :1;
      unsigned EXIDE:1;
      unsigned :1;
      unsigned SID0:1;
      unsigned SID1:1;
      unsigned SID2:1;
      unsigned SID3:1;
      unsigned SID4:1;
      unsigned SID5:1;
      unsigned SID6:1;
      unsigned SID7:1;
      unsigned SID8:1;
      unsigned SID9:1;
      unsigned SID10:1;
    };
    struct {
      unsigned :5;
      unsigned SID:11;
    };
  };
} C1RXF11SIDBITS;
extern volatile C1RXF11SIDBITS C1RXF11SIDbits __attribute__((__sfr__));

extern volatile unsigned int C1RXF11EID __attribute__((__sfr__));
typedef struct tagC1RXF11EIDBITS {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} C1RXF11EIDBITS;
extern volatile C1RXF11EIDBITS C1RXF11EIDbits __attribute__((__sfr__));

extern volatile unsigned int C1RXF12SID __attribute__((__sfr__));
__extension__ typedef struct tagC1RXF12SIDBITS {
  union {
    struct {
      unsigned EID16:1;
      unsigned EID17:1;
      unsigned :1;
      unsigned EXIDE:1;
      unsigned :1;
      unsigned SID0:1;
      unsigned SID1:1;
      unsigned SID2:1;
      unsigned SID3:1;
      unsigned SID4:1;
      unsigned SID5:1;
      unsigned SID6:1;
      unsigned SID7:1;
      unsigned SID8:1;
      unsigned SID9:1;
      unsigned SID10:1;
    };
    struct {
      unsigned :5;
      unsigned SID:11;
    };
  };
} C1RXF12SIDBITS;
extern volatile C1RXF12SIDBITS C1RXF12SIDbits __attribute__((__sfr__));

extern volatile unsigned int C1RXF12EID __attribute__((__sfr__));
typedef struct tagC1RXF12EIDBITS {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} C1RXF12EIDBITS;
extern volatile C1RXF12EIDBITS C1RXF12EIDbits __attribute__((__sfr__));

extern volatile unsigned int C1RXF13SID __attribute__((__sfr__));
__extension__ typedef struct tagC1RXF13SIDBITS {
  union {
    struct {
      unsigned EID16:1;
      unsigned EID17:1;
      unsigned :1;
      unsigned EXIDE:1;
      unsigned :1;
      unsigned SID0:1;
      unsigned SID1:1;
      unsigned SID2:1;
      unsigned SID3:1;
      unsigned SID4:1;
      unsigned SID5:1;
      unsigned SID6:1;
      unsigned SID7:1;
      unsigned SID8:1;
      unsigned SID9:1;
      unsigned SID10:1;
    };
    struct {
      unsigned :5;
      unsigned SID:11;
    };
  };
} C1RXF13SIDBITS;
extern volatile C1RXF13SIDBITS C1RXF13SIDbits __attribute__((__sfr__));

extern volatile unsigned int C1RXF13EID __attribute__((__sfr__));
typedef struct tagC1RXF13EIDBITS {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} C1RXF13EIDBITS;
extern volatile C1RXF13EIDBITS C1RXF13EIDbits __attribute__((__sfr__));

extern volatile unsigned int C1RXF14SID __attribute__((__sfr__));
__extension__ typedef struct tagC1RXF14SIDBITS {
  union {
    struct {
      unsigned EID16:1;
      unsigned EID17:1;
      unsigned :1;
      unsigned EXIDE:1;
      unsigned :1;
      unsigned SID0:1;
      unsigned SID1:1;
      unsigned SID2:1;
      unsigned SID3:1;
      unsigned SID4:1;
      unsigned SID5:1;
      unsigned SID6:1;
      unsigned SID7:1;
      unsigned SID8:1;
      unsigned SID9:1;
      unsigned SID10:1;
    };
    struct {
      unsigned :5;
      unsigned SID:11;
    };
  };
} C1RXF14SIDBITS;
extern volatile C1RXF14SIDBITS C1RXF14SIDbits __attribute__((__sfr__));

extern volatile unsigned int C1RXF14EID __attribute__((__sfr__));
typedef struct tagC1RXF14EIDBITS {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} C1RXF14EIDBITS;
extern volatile C1RXF14EIDBITS C1RXF14EIDbits __attribute__((__sfr__));

extern volatile unsigned int C1RXF15SID __attribute__((__sfr__));
__extension__ typedef struct tagC1RXF15SIDBITS {
  union {
    struct {
      unsigned EID16:1;
      unsigned EID17:1;
      unsigned :1;
      unsigned EXIDE:1;
      unsigned :1;
      unsigned SID0:1;
      unsigned SID1:1;
      unsigned SID2:1;
      unsigned SID3:1;
      unsigned SID4:1;
      unsigned SID5:1;
      unsigned SID6:1;
      unsigned SID7:1;
      unsigned SID8:1;
      unsigned SID9:1;
      unsigned SID10:1;
    };
    struct {
      unsigned :5;
      unsigned SID:11;
    };
  };
} C1RXF15SIDBITS;
extern volatile C1RXF15SIDBITS C1RXF15SIDbits __attribute__((__sfr__));

extern volatile unsigned int C1RXF15EID __attribute__((__sfr__));
typedef struct tagC1RXF15EIDBITS {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} C1RXF15EIDBITS;
extern volatile C1RXF15EIDBITS C1RXF15EIDbits __attribute__((__sfr__));

extern volatile unsigned int C2CTRL1 __attribute__((__sfr__));
__extension__ typedef struct tagC2CTRL1BITS {
  union {
    struct {
      unsigned WIN:1;
      unsigned :2;
      unsigned CANCAP:1;
      unsigned :1;
      unsigned OPMODE:3;
      unsigned REQOP:3;
      unsigned CANCKS:1;
      unsigned ABAT:1;
      unsigned CANSIDL:1;
    };
    struct {
      unsigned :5;
      unsigned OPMODE0:1;
      unsigned OPMODE1:1;
      unsigned OPMODE2:1;
      unsigned REQOP0:1;
      unsigned REQOP1:1;
      unsigned REQOP2:1;
    };
  };
} C2CTRL1BITS;
extern volatile C2CTRL1BITS C2CTRL1bits __attribute__((__sfr__));

extern volatile unsigned int C2CTRL2 __attribute__((__sfr__));
__extension__ typedef struct tagC2CTRL2BITS {
  union {
    struct {
      unsigned DNCNT:5;
    };
    struct {
      unsigned DNCNT0:1;
      unsigned DNCNT1:1;
      unsigned DNCNT2:1;
      unsigned DNCNT3:1;
      unsigned DNCNT4:1;
    };
  };
} C2CTRL2BITS;
extern volatile C2CTRL2BITS C2CTRL2bits __attribute__((__sfr__));

extern volatile unsigned int C2VEC __attribute__((__sfr__));
__extension__ typedef struct tagC2VECBITS {
  union {
    struct {
      unsigned ICODE:7;
      unsigned :1;
      unsigned FILHIT:5;
    };
    struct {
      unsigned ICODE0:1;
      unsigned ICODE1:1;
      unsigned ICODE2:1;
      unsigned ICODE3:1;
      unsigned ICODE4:1;
      unsigned ICODE5:1;
      unsigned ICODE6:1;
      unsigned :1;
      unsigned FILHIT0:1;
      unsigned FILHIT1:1;
      unsigned FILHIT2:1;
      unsigned FILHIT3:1;
      unsigned FILHIT4:1;
    };
  };
} C2VECBITS;
extern volatile C2VECBITS C2VECbits __attribute__((__sfr__));

extern volatile unsigned int C2FCTRL __attribute__((__sfr__));
__extension__ typedef struct tagC2FCTRLBITS {
  union {
    struct {
      unsigned FSA:5;
      unsigned :8;
      unsigned DMABS:3;
    };
    struct {
      unsigned FSA0:1;
      unsigned FSA1:1;
      unsigned FSA2:1;
      unsigned FSA3:1;
      unsigned FSA4:1;
      unsigned :8;
      unsigned DMABS0:1;
      unsigned DMABS1:1;
      unsigned DMABS2:1;
    };
  };
} C2FCTRLBITS;
extern volatile C2FCTRLBITS C2FCTRLbits __attribute__((__sfr__));

extern volatile unsigned int C2FIFO __attribute__((__sfr__));
__extension__ typedef struct tagC2FIFOBITS {
  union {
    struct {
      unsigned FNRB:6;
      unsigned :2;
      unsigned FBP:6;
    };
    struct {
      unsigned FNRB0:1;
      unsigned FNRB1:1;
      unsigned FNRB2:1;
      unsigned FNRB3:1;
      unsigned FNRB4:1;
      unsigned FNRB5:1;
      unsigned :2;
      unsigned FBP0:1;
      unsigned FBP1:1;
      unsigned FBP2:1;
      unsigned FBP3:1;
      unsigned FBP4:1;
      unsigned FBP5:1;
    };
  };
} C2FIFOBITS;
extern volatile C2FIFOBITS C2FIFObits __attribute__((__sfr__));

extern volatile unsigned int C2INTF __attribute__((__sfr__));
typedef struct tagC2INTFBITS {
  unsigned TBIF:1;
  unsigned RBIF:1;
  unsigned RBOVIF:1;
  unsigned FIFOIF:1;
  unsigned :1;
  unsigned ERRIF:1;
  unsigned WAKIF:1;
  unsigned IVRIF:1;
  unsigned EWARN:1;
  unsigned RXWARN:1;
  unsigned TXWARN:1;
  unsigned RXBP:1;
  unsigned TXBP:1;
  unsigned TXBO:1;
} C2INTFBITS;
extern volatile C2INTFBITS C2INTFbits __attribute__((__sfr__));

extern volatile unsigned int C2INTE __attribute__((__sfr__));
typedef struct tagC2INTEBITS {
  unsigned TBIE:1;
  unsigned RBIE:1;
  unsigned RBOVIE:1;
  unsigned FIFOIE:1;
  unsigned :1;
  unsigned ERRIE:1;
  unsigned WAKIE:1;
  unsigned IVRIE:1;
} C2INTEBITS;
extern volatile C2INTEBITS C2INTEbits __attribute__((__sfr__));

extern volatile unsigned int C2EC __attribute__((__sfr__));
__extension__ typedef struct tagC2ECBITS {
  union {
    struct {
      unsigned RERRCNT:8;
      unsigned TERRCNT:8;
    };
    struct {
      unsigned RERRCNT0:1;
      unsigned RERRCNT1:1;
      unsigned RERRCNT2:1;
      unsigned RERRCNT3:1;
      unsigned RERRCNT4:1;
      unsigned RERRCNT5:1;
      unsigned RERRCNT6:1;
      unsigned RERRCNT7:1;
      unsigned TERRCNT0:1;
      unsigned TERRCNT1:1;
      unsigned TERRCNT2:1;
      unsigned TERRCNT3:1;
      unsigned TERRCNT4:1;
      unsigned TERRCNT5:1;
      unsigned TERRCNT6:1;
      unsigned TERRCNT7:1;
    };
  };
} C2ECBITS;
extern volatile C2ECBITS C2ECbits __attribute__((__sfr__));

extern volatile unsigned char C2RERRCNT __attribute__((__sfr__));
extern volatile unsigned char C2TERRCNT __attribute__((__sfr__));
extern volatile unsigned int C2CFG1 __attribute__((__sfr__));
__extension__ typedef struct tagC2CFG1BITS {
  union {
    struct {
      unsigned BRP:6;
      unsigned SJW:2;
    };
    struct {
      unsigned BRP0:1;
      unsigned BRP1:1;
      unsigned BRP2:1;
      unsigned BRP3:1;
      unsigned BRP4:1;
      unsigned BRP5:1;
      unsigned SJW0:1;
      unsigned SJW1:1;
    };
  };
} C2CFG1BITS;
extern volatile C2CFG1BITS C2CFG1bits __attribute__((__sfr__));

extern volatile unsigned int C2CFG2 __attribute__((__sfr__));
__extension__ typedef struct tagC2CFG2BITS {
  union {
    struct {
      unsigned PRSEG:3;
      unsigned SEG1PH:3;
      unsigned SAM:1;
      unsigned SEG2PHTS:1;
      unsigned SEG2PH:3;
      unsigned :3;
      unsigned WAKFIL:1;
    };
    struct {
      unsigned PRSEG0:1;
      unsigned PRSEG1:1;
      unsigned PRSEG2:1;
      unsigned SEG1PH0:1;
      unsigned SEG1PH1:1;
      unsigned SEG1PH2:1;
      unsigned :2;
      unsigned SEG2PH0:1;
      unsigned SEG2PH1:1;
      unsigned SEG2PH2:1;
    };
  };
} C2CFG2BITS;
extern volatile C2CFG2BITS C2CFG2bits __attribute__((__sfr__));

extern volatile unsigned int C2FEN1 __attribute__((__sfr__));
__extension__ typedef struct tagC2FEN1BITS {
  union {
    struct {
      unsigned FLTEN0:1;
      unsigned FLTEN1:1;
      unsigned FLTEN2:1;
      unsigned FLTEN3:1;
      unsigned FLTEN4:1;
      unsigned FLTEN5:1;
      unsigned FLTEN6:1;
      unsigned FLTEN7:1;
      unsigned FLTEN8:1;
      unsigned FLTEN9:1;
      unsigned FLTEN10:1;
      unsigned FLTEN11:1;
      unsigned FLTEN12:1;
      unsigned FLTEN13:1;
      unsigned FLTEN14:1;
      unsigned FLTEN15:1;
    };
    struct {
      unsigned FLTEN:16;
    };
  };
} C2FEN1BITS;
extern volatile C2FEN1BITS C2FEN1bits __attribute__((__sfr__));

extern volatile unsigned int C2FMSKSEL1 __attribute__((__sfr__));
__extension__ typedef struct tagC2FMSKSEL1BITS {
  union {
    struct {
      unsigned F0MSK:2;
      unsigned F1MSK:2;
      unsigned F2MSK:2;
      unsigned F3MSK:2;
      unsigned F4MSK:2;
      unsigned F5MSK:2;
      unsigned F6MSK:2;
      unsigned F7MSK:2;
    };
    struct {
      unsigned F0MSK0:1;
      unsigned F0MSK1:1;
      unsigned F1MSK0:1;
      unsigned F1MSK1:1;
      unsigned F2MSK0:1;
      unsigned F2MSK1:1;
      unsigned F3MSK0:1;
      unsigned F3MSK1:1;
      unsigned F4MSK0:1;
      unsigned F4MSK1:1;
      unsigned F5MSK0:1;
      unsigned F5MSK1:1;
      unsigned F6MSK0:1;
      unsigned F6MSK1:1;
      unsigned F7MSK0:1;
      unsigned F7MSK1:1;
    };
  };
} C2FMSKSEL1BITS;
extern volatile C2FMSKSEL1BITS C2FMSKSEL1bits __attribute__((__sfr__));

extern volatile unsigned int C2FMSKSEL2 __attribute__((__sfr__));
__extension__ typedef struct tagC2FMSKSEL2BITS {
  union {
    struct {
      unsigned F8MSK:2;
      unsigned F9MSK:2;
      unsigned F10MSK:2;
      unsigned F11MSK:2;
      unsigned F12MSK:2;
      unsigned F13MSK:2;
      unsigned F14MSK:2;
      unsigned F15MSK:2;
    };
    struct {
      unsigned F8MSK0:1;
      unsigned F8MSK1:1;
      unsigned F9MSK0:1;
      unsigned F9MSK1:1;
      unsigned F10MSK0:1;
      unsigned F10MSK1:1;
      unsigned F11MSK0:1;
      unsigned F11MSK1:1;
      unsigned F12MSK0:1;
      unsigned F12MSK1:1;
      unsigned F13MSK0:1;
      unsigned F13MSK1:1;
      unsigned F14MSK0:1;
      unsigned F14MSK1:1;
      unsigned F15MSK0:1;
      unsigned F15MSK1:1;
    };
  };
} C2FMSKSEL2BITS;
extern volatile C2FMSKSEL2BITS C2FMSKSEL2bits __attribute__((__sfr__));

extern volatile unsigned int C2BUFPNT1 __attribute__((__sfr__));
__extension__ typedef struct tagC2BUFPNT1BITS {
  union {
    struct {
      unsigned F0BP:4;
      unsigned F1BP:4;
      unsigned F2BP:4;
      unsigned F3BP:4;
    };
    struct {
      unsigned F0BP0:1;
      unsigned F0BP1:1;
      unsigned F0BP2:1;
      unsigned F0BP3:1;
      unsigned F1BP0:1;
      unsigned F1BP1:1;
      unsigned F1BP2:1;
      unsigned F1BP3:1;
      unsigned F2BP0:1;
      unsigned F2BP1:1;
      unsigned F2BP2:1;
      unsigned F2BP3:1;
      unsigned F3BP0:1;
      unsigned F3BP1:1;
      unsigned F3BP2:1;
      unsigned F3BP3:1;
    };
  };
} C2BUFPNT1BITS;
extern volatile C2BUFPNT1BITS C2BUFPNT1bits __attribute__((__sfr__));

extern volatile unsigned int C2RXFUL1 __attribute__((__sfr__));
typedef struct tagC2RXFUL1BITS {
  unsigned RXFUL0:1;
  unsigned RXFUL1:1;
  unsigned RXFUL2:1;
  unsigned RXFUL3:1;
  unsigned RXFUL4:1;
  unsigned RXFUL5:1;
  unsigned RXFUL6:1;
  unsigned RXFUL7:1;
  unsigned RXFUL8:1;
  unsigned RXFUL9:1;
  unsigned RXFUL10:1;
  unsigned RXFUL11:1;
  unsigned RXFUL12:1;
  unsigned RXFUL13:1;
  unsigned RXFUL14:1;
  unsigned RXFUL15:1;
} C2RXFUL1BITS;
extern volatile C2RXFUL1BITS C2RXFUL1bits __attribute__((__sfr__));

extern volatile unsigned int C2BUFPNT2 __attribute__((__sfr__));
__extension__ typedef struct tagC2BUFPNT2BITS {
  union {
    struct {
      unsigned F4BP:4;
      unsigned F5BP:4;
      unsigned F6BP:4;
      unsigned F7BP:4;
    };
    struct {
      unsigned F4BP0:1;
      unsigned F4BP1:1;
      unsigned F4BP2:1;
      unsigned F4BP3:1;
      unsigned F5BP0:1;
      unsigned F5BP1:1;
      unsigned F5BP2:1;
      unsigned F5BP3:1;
      unsigned F6BP0:1;
      unsigned F6BP1:1;
      unsigned F6BP2:1;
      unsigned F6BP3:1;
      unsigned F7BP0:1;
      unsigned F7BP1:1;
      unsigned F7BP2:1;
      unsigned F7BP3:1;
    };
  };
} C2BUFPNT2BITS;
extern volatile C2BUFPNT2BITS C2BUFPNT2bits __attribute__((__sfr__));

extern volatile unsigned int C2RXFUL2 __attribute__((__sfr__));
typedef struct tagC2RXFUL2BITS {
  unsigned RXFUL16:1;
  unsigned RXFUL17:1;
  unsigned RXFUL18:1;
  unsigned RXFUL19:1;
  unsigned RXFUL20:1;
  unsigned RXFUL21:1;
  unsigned RXFUL22:1;
  unsigned RXFUL23:1;
  unsigned RXFUL24:1;
  unsigned RXFUL25:1;
  unsigned RXFUL26:1;
  unsigned RXFUL27:1;
  unsigned RXFUL28:1;
  unsigned RXFUL29:1;
  unsigned RXFUL30:1;
  unsigned RXFUL31:1;
} C2RXFUL2BITS;
extern volatile C2RXFUL2BITS C2RXFUL2bits __attribute__((__sfr__));

extern volatile unsigned int C2BUFPNT3 __attribute__((__sfr__));
__extension__ typedef struct tagC2BUFPNT3BITS {
  union {
    struct {
      unsigned F8BP:4;
      unsigned F9BP:4;
      unsigned F10BP:4;
      unsigned F11BP:4;
    };
    struct {
      unsigned F8BP0:1;
      unsigned F8BP1:1;
      unsigned F8BP2:1;
      unsigned F8BP3:1;
      unsigned F9BP0:1;
      unsigned F9BP1:1;
      unsigned F9BP2:1;
      unsigned F9BP3:1;
      unsigned F10BP0:1;
      unsigned F10BP1:1;
      unsigned F10BP2:1;
      unsigned F10BP3:1;
      unsigned F11BP0:1;
      unsigned F11BP1:1;
      unsigned F11BP2:1;
      unsigned F11BP3:1;
    };
  };
} C2BUFPNT3BITS;
extern volatile C2BUFPNT3BITS C2BUFPNT3bits __attribute__((__sfr__));

extern volatile unsigned int C2BUFPNT4 __attribute__((__sfr__));
__extension__ typedef struct tagC2BUFPNT4BITS {
  union {
    struct {
      unsigned F12BP:4;
      unsigned F13BP:4;
      unsigned F14BP:4;
      unsigned F15BP:4;
    };
    struct {
      unsigned F12BP0:1;
      unsigned F12BP1:1;
      unsigned F12BP2:1;
      unsigned F12BP3:1;
      unsigned F13BP0:1;
      unsigned F13BP1:1;
      unsigned F13BP2:1;
      unsigned F13BP3:1;
      unsigned F14BP0:1;
      unsigned F14BP1:1;
      unsigned F14BP2:1;
      unsigned F14BP3:1;
      unsigned F15BP0:1;
      unsigned F15BP1:1;
      unsigned F15BP2:1;
      unsigned F15BP3:1;
    };
  };
} C2BUFPNT4BITS;
extern volatile C2BUFPNT4BITS C2BUFPNT4bits __attribute__((__sfr__));

extern volatile unsigned int C2RXOVF1 __attribute__((__sfr__));
typedef struct tagC2RXOVF1BITS {
  unsigned RXOVF0:1;
  unsigned RXOVF1:1;
  unsigned RXOVF2:1;
  unsigned RXOVF3:1;
  unsigned RXOVF4:1;
  unsigned RXOVF5:1;
  unsigned RXOVF6:1;
  unsigned RXOVF7:1;
  unsigned RXOVF8:1;
  unsigned RXOVF9:1;
  unsigned RXOVF10:1;
  unsigned RXOVF11:1;
  unsigned RXOVF12:1;
  unsigned RXOVF13:1;
  unsigned RXOVF14:1;
  unsigned RXOVF15:1;
} C2RXOVF1BITS;
extern volatile C2RXOVF1BITS C2RXOVF1bits __attribute__((__sfr__));

extern volatile unsigned int C2RXOVF2 __attribute__((__sfr__));
typedef struct tagC2RXOVF2BITS {
  unsigned RXOVF16:1;
  unsigned RXOVF17:1;
  unsigned RXOVF18:1;
  unsigned RXOVF19:1;
  unsigned RXOVF20:1;
  unsigned RXOVF21:1;
  unsigned RXOVF22:1;
  unsigned RXOVF23:1;
  unsigned RXOVF24:1;
  unsigned RXOVF25:1;
  unsigned RXOVF26:1;
  unsigned RXOVF27:1;
  unsigned RXOVF28:1;
  unsigned RXOVF29:1;
  unsigned RXOVF30:1;
  unsigned RXOVF31:1;
} C2RXOVF2BITS;
extern volatile C2RXOVF2BITS C2RXOVF2bits __attribute__((__sfr__));

extern volatile unsigned int C2RXM0SID __attribute__((__sfr__));
__extension__ typedef struct tagC2RXM0SIDBITS {
  union {
    struct {
      unsigned EID16:1;
      unsigned EID17:1;
      unsigned :1;
      unsigned MIDE:1;
      unsigned :1;
      unsigned SID0:1;
      unsigned SID1:1;
      unsigned SID2:1;
      unsigned SID3:1;
      unsigned SID4:1;
      unsigned SID5:1;
      unsigned SID6:1;
      unsigned SID7:1;
      unsigned SID8:1;
      unsigned SID9:1;
      unsigned SID10:1;
    };
    struct {
      unsigned EID:2;
      unsigned :3;
      unsigned SID:11;
    };
  };
} C2RXM0SIDBITS;
extern volatile C2RXM0SIDBITS C2RXM0SIDbits __attribute__((__sfr__));

extern volatile unsigned int C2TR01CON __attribute__((__sfr__));
__extension__ typedef struct tagC2TR01CONBITS {
  union {
    struct {
      unsigned TX0PRI:2;
      unsigned RTREN0:1;
      unsigned TXREQ0:1;
      unsigned TXERR0:1;
      unsigned TXLARB0:1;
      unsigned TXABT0:1;
      unsigned TXEN0:1;
      unsigned TX1PRI:2;
      unsigned RTREN1:1;
      unsigned TXREQ1:1;
      unsigned TXERR1:1;
      unsigned TXLARB1:1;
      unsigned TXABT1:1;
      unsigned TXEN1:1;
    };
    struct {
      unsigned TX0PRI0:1;
      unsigned TX0PRI1:1;
      unsigned :6;
      unsigned TX1PRI0:1;
      unsigned TX1PRI1:1;
    };
  };
} C2TR01CONBITS;
extern volatile C2TR01CONBITS C2TR01CONbits __attribute__((__sfr__));

extern volatile unsigned int C2RXM0EID __attribute__((__sfr__));
__extension__ typedef struct tagC2RXM0EIDBITS {
  union {
    struct {
      unsigned EID0:1;
      unsigned EID1:1;
      unsigned EID2:1;
      unsigned EID3:1;
      unsigned EID4:1;
      unsigned EID5:1;
      unsigned EID6:1;
      unsigned EID7:1;
      unsigned EID8:1;
      unsigned EID9:1;
      unsigned EID10:1;
      unsigned EID11:1;
      unsigned EID12:1;
      unsigned EID13:1;
      unsigned EID14:1;
      unsigned EID15:1;
    };
    struct {
      unsigned EID:16;
    };
  };
} C2RXM0EIDBITS;
extern volatile C2RXM0EIDBITS C2RXM0EIDbits __attribute__((__sfr__));

extern volatile unsigned int C2TR23CON __attribute__((__sfr__));
__extension__ typedef struct tagC2TR23CONBITS {
  union {
    struct {
      unsigned TX2PRI:2;
      unsigned RTREN2:1;
      unsigned TXREQ2:1;
      unsigned TXERR2:1;
      unsigned TXLARB2:1;
      unsigned TXABT2:1;
      unsigned TXEN2:1;
      unsigned TX3PRI:2;
      unsigned RTREN3:1;
      unsigned TXREQ3:1;
      unsigned TXERR3:1;
      unsigned TXLARB3:1;
      unsigned TXABT3:1;
      unsigned TXEN3:1;
    };
    struct {
      unsigned TX2PRI0:1;
      unsigned TX2PRI1:1;
      unsigned :6;
      unsigned TX3PRI0:1;
      unsigned TX3PRI1:1;
    };
  };
} C2TR23CONBITS;
extern volatile C2TR23CONBITS C2TR23CONbits __attribute__((__sfr__));

extern volatile unsigned int C2RXM1SID __attribute__((__sfr__));
__extension__ typedef struct tagC2RXM1SIDBITS {
  union {
    struct {
      unsigned EID16:1;
      unsigned EID17:1;
      unsigned :1;
      unsigned MIDE:1;
      unsigned :1;
      unsigned SID0:1;
      unsigned SID1:1;
      unsigned SID2:1;
      unsigned SID3:1;
      unsigned SID4:1;
      unsigned SID5:1;
      unsigned SID6:1;
      unsigned SID7:1;
      unsigned SID8:1;
      unsigned SID9:1;
      unsigned SID10:1;
    };
    struct {
      unsigned EID:2;
      unsigned :3;
      unsigned SID:11;
    };
  };
} C2RXM1SIDBITS;
extern volatile C2RXM1SIDBITS C2RXM1SIDbits __attribute__((__sfr__));

extern volatile unsigned int C2TR45CON __attribute__((__sfr__));
__extension__ typedef struct tagC2TR45CONBITS {
  union {
    struct {
      unsigned TX4PRI:2;
      unsigned RTREN4:1;
      unsigned TXREQ4:1;
      unsigned TXERR4:1;
      unsigned TXLARB4:1;
      unsigned TXABT4:1;
      unsigned TXEN4:1;
      unsigned TX5PRI:2;
      unsigned RTREN5:1;
      unsigned TXREQ5:1;
      unsigned TXERR5:1;
      unsigned TXLARB5:1;
      unsigned TXABT5:1;
      unsigned TXEN5:1;
    };
    struct {
      unsigned TX4PRI0:1;
      unsigned TX4PRI1:1;
      unsigned :6;
      unsigned TX5PRI0:1;
      unsigned TX5PRI1:1;
    };
  };
} C2TR45CONBITS;
extern volatile C2TR45CONBITS C2TR45CONbits __attribute__((__sfr__));

extern volatile unsigned int C2RXM1EID __attribute__((__sfr__));
__extension__ typedef struct tagC2RXM1EIDBITS {
  union {
    struct {
      unsigned EID0:1;
      unsigned EID1:1;
      unsigned EID2:1;
      unsigned EID3:1;
      unsigned EID4:1;
      unsigned EID5:1;
      unsigned EID6:1;
      unsigned EID7:1;
      unsigned EID8:1;
      unsigned EID9:1;
      unsigned EID10:1;
      unsigned EID11:1;
      unsigned EID12:1;
      unsigned EID13:1;
      unsigned EID14:1;
      unsigned EID15:1;
    };
    struct {
      unsigned EID:16;
    };
  };
} C2RXM1EIDBITS;
extern volatile C2RXM1EIDBITS C2RXM1EIDbits __attribute__((__sfr__));

extern volatile unsigned int C2TR67CON __attribute__((__sfr__));
__extension__ typedef struct tagC2TR67CONBITS {
  union {
    struct {
      unsigned TX6PRI:2;
      unsigned RTREN6:1;
      unsigned TXREQ6:1;
      unsigned TXERR6:1;
      unsigned TXLARB6:1;
      unsigned TXABT6:1;
      unsigned TXEN6:1;
      unsigned TX7PRI:2;
      unsigned RTREN7:1;
      unsigned TXREQ7:1;
      unsigned TXERR7:1;
      unsigned TXLARB7:1;
      unsigned TXABT7:1;
      unsigned TXEN7:1;
    };
    struct {
      unsigned TX6PRI0:1;
      unsigned TX6PRI1:1;
      unsigned :6;
      unsigned TX7PRI0:1;
      unsigned TX7PRI1:1;
    };
  };
} C2TR67CONBITS;
extern volatile C2TR67CONBITS C2TR67CONbits __attribute__((__sfr__));

extern volatile unsigned int C2RXM2SID __attribute__((__sfr__));
__extension__ typedef struct tagC2RXM2SIDBITS {
  union {
    struct {
      unsigned EID16:1;
      unsigned EID17:1;
      unsigned :1;
      unsigned MIDE:1;
      unsigned :1;
      unsigned SID0:1;
      unsigned SID1:1;
      unsigned SID2:1;
      unsigned SID3:1;
      unsigned SID4:1;
      unsigned SID5:1;
      unsigned SID6:1;
      unsigned SID7:1;
      unsigned SID8:1;
      unsigned SID9:1;
      unsigned SID10:1;
    };
    struct {
      unsigned EID:2;
      unsigned :3;
      unsigned SID:11;
    };
  };
} C2RXM2SIDBITS;
extern volatile C2RXM2SIDBITS C2RXM2SIDbits __attribute__((__sfr__));

extern volatile unsigned int C2RXM2EID __attribute__((__sfr__));
__extension__ typedef struct tagC2RXM2EIDBITS {
  union {
    struct {
      unsigned EID0:1;
      unsigned EID1:1;
      unsigned EID2:1;
      unsigned EID3:1;
      unsigned EID4:1;
      unsigned EID5:1;
      unsigned EID6:1;
      unsigned EID7:1;
      unsigned EID8:1;
      unsigned EID9:1;
      unsigned EID10:1;
      unsigned EID11:1;
      unsigned EID12:1;
      unsigned EID13:1;
      unsigned EID14:1;
      unsigned EID15:1;
    };
    struct {
      unsigned EID:16;
    };
  };
} C2RXM2EIDBITS;
extern volatile C2RXM2EIDBITS C2RXM2EIDbits __attribute__((__sfr__));

extern volatile unsigned int C2RXD __attribute__((__sfr__));
extern volatile unsigned int C2RXF0SID __attribute__((__sfr__));
__extension__ typedef struct tagC2RXF0SIDBITS {
  union {
    struct {
      unsigned EID16:1;
      unsigned EID17:1;
      unsigned :1;
      unsigned EXIDE:1;
      unsigned :1;
      unsigned SID0:1;
      unsigned SID1:1;
      unsigned SID2:1;
      unsigned SID3:1;
      unsigned SID4:1;
      unsigned SID5:1;
      unsigned SID6:1;
      unsigned SID7:1;
      unsigned SID8:1;
      unsigned SID9:1;
      unsigned SID10:1;
    };
    struct {
      unsigned :5;
      unsigned SID:11;
    };
  };
} C2RXF0SIDBITS;
extern volatile C2RXF0SIDBITS C2RXF0SIDbits __attribute__((__sfr__));

extern volatile unsigned int C2RXF0EID __attribute__((__sfr__));
typedef struct tagC2RXF0EIDBITS {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} C2RXF0EIDBITS;
extern volatile C2RXF0EIDBITS C2RXF0EIDbits __attribute__((__sfr__));

extern volatile unsigned int C2TXD __attribute__((__sfr__));
extern volatile unsigned int C2RXF1SID __attribute__((__sfr__));
__extension__ typedef struct tagC2RXF1SIDBITS {
  union {
    struct {
      unsigned EID16:1;
      unsigned EID17:1;
      unsigned :1;
      unsigned EXIDE:1;
      unsigned :1;
      unsigned SID0:1;
      unsigned SID1:1;
      unsigned SID2:1;
      unsigned SID3:1;
      unsigned SID4:1;
      unsigned SID5:1;
      unsigned SID6:1;
      unsigned SID7:1;
      unsigned SID8:1;
      unsigned SID9:1;
      unsigned SID10:1;
    };
    struct {
      unsigned :5;
      unsigned SID:11;
    };
  };
} C2RXF1SIDBITS;
extern volatile C2RXF1SIDBITS C2RXF1SIDbits __attribute__((__sfr__));

extern volatile unsigned int C2RXF1EID __attribute__((__sfr__));
typedef struct tagC2RXF1EIDBITS {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} C2RXF1EIDBITS;
extern volatile C2RXF1EIDBITS C2RXF1EIDbits __attribute__((__sfr__));

extern volatile unsigned int C2RXF2SID __attribute__((__sfr__));
__extension__ typedef struct tagC2RXF2SIDBITS {
  union {
    struct {
      unsigned EID16:1;
      unsigned EID17:1;
      unsigned :1;
      unsigned EXIDE:1;
      unsigned :1;
      unsigned SID0:1;
      unsigned SID1:1;
      unsigned SID2:1;
      unsigned SID3:1;
      unsigned SID4:1;
      unsigned SID5:1;
      unsigned SID6:1;
      unsigned SID7:1;
      unsigned SID8:1;
      unsigned SID9:1;
      unsigned SID10:1;
    };
    struct {
      unsigned :5;
      unsigned SID:11;
    };
  };
} C2RXF2SIDBITS;
extern volatile C2RXF2SIDBITS C2RXF2SIDbits __attribute__((__sfr__));

extern volatile unsigned int C2RXF2EID __attribute__((__sfr__));
typedef struct tagC2RXF2EIDBITS {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} C2RXF2EIDBITS;
extern volatile C2RXF2EIDBITS C2RXF2EIDbits __attribute__((__sfr__));

extern volatile unsigned int C2RXF3SID __attribute__((__sfr__));
__extension__ typedef struct tagC2RXF3SIDBITS {
  union {
    struct {
      unsigned EID16:1;
      unsigned EID17:1;
      unsigned :1;
      unsigned EXIDE:1;
      unsigned :1;
      unsigned SID0:1;
      unsigned SID1:1;
      unsigned SID2:1;
      unsigned SID3:1;
      unsigned SID4:1;
      unsigned SID5:1;
      unsigned SID6:1;
      unsigned SID7:1;
      unsigned SID8:1;
      unsigned SID9:1;
      unsigned SID10:1;
    };
    struct {
      unsigned :5;
      unsigned SID:11;
    };
  };
} C2RXF3SIDBITS;
extern volatile C2RXF3SIDBITS C2RXF3SIDbits __attribute__((__sfr__));

extern volatile unsigned int C2RXF3EID __attribute__((__sfr__));
typedef struct tagC2RXF3EIDBITS {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} C2RXF3EIDBITS;
extern volatile C2RXF3EIDBITS C2RXF3EIDbits __attribute__((__sfr__));

extern volatile unsigned int C2RXF4SID __attribute__((__sfr__));
__extension__ typedef struct tagC2RXF4SIDBITS {
  union {
    struct {
      unsigned EID16:1;
      unsigned EID17:1;
      unsigned :1;
      unsigned EXIDE:1;
      unsigned :1;
      unsigned SID0:1;
      unsigned SID1:1;
      unsigned SID2:1;
      unsigned SID3:1;
      unsigned SID4:1;
      unsigned SID5:1;
      unsigned SID6:1;
      unsigned SID7:1;
      unsigned SID8:1;
      unsigned SID9:1;
      unsigned SID10:1;
    };
    struct {
      unsigned :5;
      unsigned SID:11;
    };
  };
} C2RXF4SIDBITS;
extern volatile C2RXF4SIDBITS C2RXF4SIDbits __attribute__((__sfr__));

extern volatile unsigned int C2RXF4EID __attribute__((__sfr__));
typedef struct tagC2RXF4EIDBITS {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} C2RXF4EIDBITS;
extern volatile C2RXF4EIDBITS C2RXF4EIDbits __attribute__((__sfr__));

extern volatile unsigned int C2RXF5SID __attribute__((__sfr__));
__extension__ typedef struct tagC2RXF5SIDBITS {
  union {
    struct {
      unsigned EID16:1;
      unsigned EID17:1;
      unsigned :1;
      unsigned EXIDE:1;
      unsigned :1;
      unsigned SID0:1;
      unsigned SID1:1;
      unsigned SID2:1;
      unsigned SID3:1;
      unsigned SID4:1;
      unsigned SID5:1;
      unsigned SID6:1;
      unsigned SID7:1;
      unsigned SID8:1;
      unsigned SID9:1;
      unsigned SID10:1;
    };
    struct {
      unsigned :5;
      unsigned SID:11;
    };
  };
} C2RXF5SIDBITS;
extern volatile C2RXF5SIDBITS C2RXF5SIDbits __attribute__((__sfr__));

extern volatile unsigned int C2RXF5EID __attribute__((__sfr__));
typedef struct tagC2RXF5EIDBITS {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} C2RXF5EIDBITS;
extern volatile C2RXF5EIDBITS C2RXF5EIDbits __attribute__((__sfr__));

extern volatile unsigned int C2RXF6SID __attribute__((__sfr__));
__extension__ typedef struct tagC2RXF6SIDBITS {
  union {
    struct {
      unsigned EID16:1;
      unsigned EID17:1;
      unsigned :1;
      unsigned EXIDE:1;
      unsigned :1;
      unsigned SID0:1;
      unsigned SID1:1;
      unsigned SID2:1;
      unsigned SID3:1;
      unsigned SID4:1;
      unsigned SID5:1;
      unsigned SID6:1;
      unsigned SID7:1;
      unsigned SID8:1;
      unsigned SID9:1;
      unsigned SID10:1;
    };
    struct {
      unsigned :5;
      unsigned SID:11;
    };
  };
} C2RXF6SIDBITS;
extern volatile C2RXF6SIDBITS C2RXF6SIDbits __attribute__((__sfr__));

extern volatile unsigned int C2RXF6EID __attribute__((__sfr__));
typedef struct tagC2RXF6EIDBITS {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} C2RXF6EIDBITS;
extern volatile C2RXF6EIDBITS C2RXF6EIDbits __attribute__((__sfr__));

extern volatile unsigned int C2RXF7SID __attribute__((__sfr__));
__extension__ typedef struct tagC2RXF7SIDBITS {
  union {
    struct {
      unsigned EID16:1;
      unsigned EID17:1;
      unsigned :1;
      unsigned EXIDE:1;
      unsigned :1;
      unsigned SID0:1;
      unsigned SID1:1;
      unsigned SID2:1;
      unsigned SID3:1;
      unsigned SID4:1;
      unsigned SID5:1;
      unsigned SID6:1;
      unsigned SID7:1;
      unsigned SID8:1;
      unsigned SID9:1;
      unsigned SID10:1;
    };
    struct {
      unsigned :5;
      unsigned SID:11;
    };
  };
} C2RXF7SIDBITS;
extern volatile C2RXF7SIDBITS C2RXF7SIDbits __attribute__((__sfr__));

extern volatile unsigned int C2RXF7EID __attribute__((__sfr__));
typedef struct tagC2RXF7EIDBITS {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} C2RXF7EIDBITS;
extern volatile C2RXF7EIDBITS C2RXF7EIDbits __attribute__((__sfr__));

extern volatile unsigned int C2RXF8SID __attribute__((__sfr__));
__extension__ typedef struct tagC2RXF8SIDBITS {
  union {
    struct {
      unsigned EID16:1;
      unsigned EID17:1;
      unsigned :1;
      unsigned EXIDE:1;
      unsigned :1;
      unsigned SID0:1;
      unsigned SID1:1;
      unsigned SID2:1;
      unsigned SID3:1;
      unsigned SID4:1;
      unsigned SID5:1;
      unsigned SID6:1;
      unsigned SID7:1;
      unsigned SID8:1;
      unsigned SID9:1;
      unsigned SID10:1;
    };
    struct {
      unsigned :5;
      unsigned SID:11;
    };
  };
} C2RXF8SIDBITS;
extern volatile C2RXF8SIDBITS C2RXF8SIDbits __attribute__((__sfr__));

extern volatile unsigned int C2RXF8EID __attribute__((__sfr__));
typedef struct tagC2RXF8EIDBITS {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} C2RXF8EIDBITS;
extern volatile C2RXF8EIDBITS C2RXF8EIDbits __attribute__((__sfr__));

extern volatile unsigned int C2RXF9SID __attribute__((__sfr__));
__extension__ typedef struct tagC2RXF9SIDBITS {
  union {
    struct {
      unsigned EID16:1;
      unsigned EID17:1;
      unsigned :1;
      unsigned EXIDE:1;
      unsigned :1;
      unsigned SID0:1;
      unsigned SID1:1;
      unsigned SID2:1;
      unsigned SID3:1;
      unsigned SID4:1;
      unsigned SID5:1;
      unsigned SID6:1;
      unsigned SID7:1;
      unsigned SID8:1;
      unsigned SID9:1;
      unsigned SID10:1;
    };
    struct {
      unsigned :5;
      unsigned SID:11;
    };
  };
} C2RXF9SIDBITS;
extern volatile C2RXF9SIDBITS C2RXF9SIDbits __attribute__((__sfr__));

extern volatile unsigned int C2RXF9EID __attribute__((__sfr__));
typedef struct tagC2RXF9EIDBITS {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} C2RXF9EIDBITS;
extern volatile C2RXF9EIDBITS C2RXF9EIDbits __attribute__((__sfr__));

extern volatile unsigned int C2RXF10SID __attribute__((__sfr__));
__extension__ typedef struct tagC2RXF10SIDBITS {
  union {
    struct {
      unsigned EID16:1;
      unsigned EID17:1;
      unsigned :1;
      unsigned EXIDE:1;
      unsigned :1;
      unsigned SID0:1;
      unsigned SID1:1;
      unsigned SID2:1;
      unsigned SID3:1;
      unsigned SID4:1;
      unsigned SID5:1;
      unsigned SID6:1;
      unsigned SID7:1;
      unsigned SID8:1;
      unsigned SID9:1;
      unsigned SID10:1;
    };
    struct {
      unsigned :5;
      unsigned SID:11;
    };
  };
} C2RXF10SIDBITS;
extern volatile C2RXF10SIDBITS C2RXF10SIDbits __attribute__((__sfr__));

extern volatile unsigned int C2RXF10EID __attribute__((__sfr__));
typedef struct tagC2RXF10EIDBITS {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} C2RXF10EIDBITS;
extern volatile C2RXF10EIDBITS C2RXF10EIDbits __attribute__((__sfr__));

extern volatile unsigned int C2RXF11SID __attribute__((__sfr__));
__extension__ typedef struct tagC2RXF11SIDBITS {
  union {
    struct {
      unsigned EID16:1;
      unsigned EID17:1;
      unsigned :1;
      unsigned EXIDE:1;
      unsigned :1;
      unsigned SID0:1;
      unsigned SID1:1;
      unsigned SID2:1;
      unsigned SID3:1;
      unsigned SID4:1;
      unsigned SID5:1;
      unsigned SID6:1;
      unsigned SID7:1;
      unsigned SID8:1;
      unsigned SID9:1;
      unsigned SID10:1;
    };
    struct {
      unsigned :5;
      unsigned SID:11;
    };
  };
} C2RXF11SIDBITS;
extern volatile C2RXF11SIDBITS C2RXF11SIDbits __attribute__((__sfr__));

extern volatile unsigned int C2RXF11EID __attribute__((__sfr__));
typedef struct tagC2RXF11EIDBITS {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} C2RXF11EIDBITS;
extern volatile C2RXF11EIDBITS C2RXF11EIDbits __attribute__((__sfr__));

extern volatile unsigned int C2RXF12SID __attribute__((__sfr__));
__extension__ typedef struct tagC2RXF12SIDBITS {
  union {
    struct {
      unsigned EID16:1;
      unsigned EID17:1;
      unsigned :1;
      unsigned EXIDE:1;
      unsigned :1;
      unsigned SID0:1;
      unsigned SID1:1;
      unsigned SID2:1;
      unsigned SID3:1;
      unsigned SID4:1;
      unsigned SID5:1;
      unsigned SID6:1;
      unsigned SID7:1;
      unsigned SID8:1;
      unsigned SID9:1;
      unsigned SID10:1;
    };
    struct {
      unsigned :5;
      unsigned SID:11;
    };
  };
} C2RXF12SIDBITS;
extern volatile C2RXF12SIDBITS C2RXF12SIDbits __attribute__((__sfr__));

extern volatile unsigned int C2RXF12EID __attribute__((__sfr__));
typedef struct tagC2RXF12EIDBITS {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} C2RXF12EIDBITS;
extern volatile C2RXF12EIDBITS C2RXF12EIDbits __attribute__((__sfr__));

extern volatile unsigned int C2RXF13SID __attribute__((__sfr__));
__extension__ typedef struct tagC2RXF13SIDBITS {
  union {
    struct {
      unsigned EID16:1;
      unsigned EID17:1;
      unsigned :1;
      unsigned EXIDE:1;
      unsigned :1;
      unsigned SID0:1;
      unsigned SID1:1;
      unsigned SID2:1;
      unsigned SID3:1;
      unsigned SID4:1;
      unsigned SID5:1;
      unsigned SID6:1;
      unsigned SID7:1;
      unsigned SID8:1;
      unsigned SID9:1;
      unsigned SID10:1;
    };
    struct {
      unsigned :5;
      unsigned SID:11;
    };
  };
} C2RXF13SIDBITS;
extern volatile C2RXF13SIDBITS C2RXF13SIDbits __attribute__((__sfr__));

extern volatile unsigned int C2RXF13EID __attribute__((__sfr__));
typedef struct tagC2RXF13EIDBITS {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} C2RXF13EIDBITS;
extern volatile C2RXF13EIDBITS C2RXF13EIDbits __attribute__((__sfr__));

extern volatile unsigned int C2RXF14SID __attribute__((__sfr__));
__extension__ typedef struct tagC2RXF14SIDBITS {
  union {
    struct {
      unsigned EID16:1;
      unsigned EID17:1;
      unsigned :1;
      unsigned EXIDE:1;
      unsigned :1;
      unsigned SID0:1;
      unsigned SID1:1;
      unsigned SID2:1;
      unsigned SID3:1;
      unsigned SID4:1;
      unsigned SID5:1;
      unsigned SID6:1;
      unsigned SID7:1;
      unsigned SID8:1;
      unsigned SID9:1;
      unsigned SID10:1;
    };
    struct {
      unsigned :5;
      unsigned SID:11;
    };
  };
} C2RXF14SIDBITS;
extern volatile C2RXF14SIDBITS C2RXF14SIDbits __attribute__((__sfr__));

extern volatile unsigned int C2RXF14EID __attribute__((__sfr__));
typedef struct tagC2RXF14EIDBITS {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} C2RXF14EIDBITS;
extern volatile C2RXF14EIDBITS C2RXF14EIDbits __attribute__((__sfr__));

extern volatile unsigned int C2RXF15SID __attribute__((__sfr__));
__extension__ typedef struct tagC2RXF15SIDBITS {
  union {
    struct {
      unsigned EID16:1;
      unsigned EID17:1;
      unsigned :1;
      unsigned EXIDE:1;
      unsigned :1;
      unsigned SID0:1;
      unsigned SID1:1;
      unsigned SID2:1;
      unsigned SID3:1;
      unsigned SID4:1;
      unsigned SID5:1;
      unsigned SID6:1;
      unsigned SID7:1;
      unsigned SID8:1;
      unsigned SID9:1;
      unsigned SID10:1;
    };
    struct {
      unsigned :5;
      unsigned SID:11;
    };
  };
} C2RXF15SIDBITS;
extern volatile C2RXF15SIDBITS C2RXF15SIDbits __attribute__((__sfr__));

extern volatile unsigned int C2RXF15EID __attribute__((__sfr__));
typedef struct tagC2RXF15EIDBITS {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} C2RXF15EIDBITS;
extern volatile C2RXF15EIDBITS C2RXF15EIDbits __attribute__((__sfr__));

extern volatile unsigned int ODCA __attribute__((__sfr__));
typedef struct tagODCABITS {
  unsigned ODCA0:1;
  unsigned ODCA1:1;
  unsigned ODCA2:1;
  unsigned ODCA3:1;
  unsigned ODCA4:1;
  unsigned ODCA5:1;
  unsigned :6;
  unsigned ODCA12:1;
  unsigned ODCA13:1;
  unsigned ODCA14:1;
  unsigned ODCA15:1;
} ODCABITS;
extern volatile ODCABITS ODCAbits __attribute__((__sfr__));

extern volatile unsigned int ODCD __attribute__((__sfr__));
typedef struct tagODCDBITS {
  unsigned ODCD0:1;
  unsigned ODCD1:1;
  unsigned ODCD2:1;
  unsigned ODCD3:1;
  unsigned ODCD4:1;
  unsigned ODCD5:1;
  unsigned ODCD6:1;
  unsigned ODCD7:1;
  unsigned ODCD8:1;
  unsigned ODCD9:1;
  unsigned ODCD10:1;
  unsigned ODCD11:1;
  unsigned ODCD12:1;
  unsigned ODCD13:1;
  unsigned ODCD14:1;
  unsigned ODCD15:1;
} ODCDBITS;
extern volatile ODCDBITS ODCDbits __attribute__((__sfr__));

extern volatile unsigned int ODCF __attribute__((__sfr__));
typedef struct tagODCFBITS {
  unsigned ODCF0:1;
  unsigned ODCF1:1;
  unsigned ODCF2:1;
  unsigned ODCF3:1;
  unsigned ODCF4:1;
  unsigned ODCF5:1;
  unsigned ODCF6:1;
  unsigned ODCF7:1;
  unsigned ODCF8:1;
  unsigned :3;
  unsigned ODCF12:1;
  unsigned ODCF13:1;
} ODCFBITS;
extern volatile ODCFBITS ODCFbits __attribute__((__sfr__));

extern volatile unsigned int ODCG __attribute__((__sfr__));
typedef struct tagODCGBITS {
  unsigned ODCG0:1;
  unsigned ODCG1:1;
  unsigned ODCG2:1;
  unsigned ODCG3:1;
  unsigned :2;
  unsigned ODCG6:1;
  unsigned ODCG7:1;
  unsigned ODCG8:1;
  unsigned ODCG9:1;
  unsigned :2;
  unsigned ODCG12:1;
  unsigned ODCG13:1;
  unsigned ODCG14:1;
  unsigned ODCG15:1;
} ODCGBITS;
extern volatile ODCGBITS ODCGbits __attribute__((__sfr__));

extern volatile unsigned int RCON __attribute__((__sfr__));
typedef struct tagRCONBITS {
  unsigned POR:1;
  unsigned BOR:1;
  unsigned IDLE:1;
  unsigned SLEEP:1;
  unsigned WDTO:1;
  unsigned SWDTEN:1;
  unsigned SWR:1;
  unsigned EXTR:1;
  unsigned VREGS:1;
  unsigned :5;
  unsigned IOPUWR:1;
  unsigned TRAPR:1;
} RCONBITS;
extern volatile RCONBITS RCONbits __attribute__((__sfr__));

extern volatile unsigned int OSCCON __attribute__((__sfr__));
__extension__ typedef struct tagOSCCONBITS {
  union {
    struct {
      unsigned OSWEN:1;
      unsigned LPOSCEN:1;
      unsigned :1;
      unsigned CF:1;
      unsigned :1;
      unsigned LOCK:1;
      unsigned :1;
      unsigned CLKLOCK:1;
      unsigned NOSC:3;
      unsigned :1;
      unsigned COSC:3;
    };
    struct {
      unsigned :8;
      unsigned NOSC0:1;
      unsigned NOSC1:1;
      unsigned NOSC2:1;
      unsigned :1;
      unsigned COSC0:1;
      unsigned COSC1:1;
      unsigned COSC2:1;
    };
  };
} OSCCONBITS;
extern volatile OSCCONBITS OSCCONbits __attribute__((__sfr__));

extern volatile unsigned char OSCCONL __attribute__((__sfr__));
extern volatile unsigned char OSCCONH __attribute__((__sfr__));
extern volatile unsigned int CLKDIV __attribute__((__sfr__));
__extension__ typedef struct tagCLKDIVBITS {
  union {
    struct {
      unsigned PLLPRE:5;
      unsigned :1;
      unsigned PLLPOST:2;
      unsigned FRCDIV:3;
      unsigned DOZEN:1;
      unsigned DOZE:3;
      unsigned ROI:1;
    };
    struct {
      unsigned PLLPRE0:1;
      unsigned PLLPRE1:1;
      unsigned PLLPRE2:1;
      unsigned PLLPRE3:1;
      unsigned PLLPRE4:1;
      unsigned :1;
      unsigned PLLPOST0:1;
      unsigned PLLPOST1:1;
      unsigned FRCDIV0:1;
      unsigned FRCDIV1:1;
      unsigned FRCDIV2:1;
      unsigned :1;
      unsigned DOZE0:1;
      unsigned DOZE1:1;
      unsigned DOZE2:1;
    };
  };
} CLKDIVBITS;
extern volatile CLKDIVBITS CLKDIVbits __attribute__((__sfr__));

extern volatile unsigned int PLLFBD __attribute__((__sfr__));
__extension__ typedef struct tagPLLFBDBITS {
  union {
    struct {
      unsigned PLLDIV:9;
    };
    struct {
      unsigned PLLDIV0:1;
      unsigned PLLDIV1:1;
      unsigned PLLDIV2:1;
      unsigned PLLDIV3:1;
      unsigned PLLDIV4:1;
      unsigned PLLDIV5:1;
      unsigned PLLDIV6:1;
      unsigned PLLDIV7:1;
      unsigned PLLDIV8:1;
    };
  };
} PLLFBDBITS;
extern volatile PLLFBDBITS PLLFBDbits __attribute__((__sfr__));

extern volatile unsigned int OSCTUN __attribute__((__sfr__));
__extension__ typedef struct tagOSCTUNBITS {
  union {
    struct {
      unsigned TUN:6;
    };
    struct {
      unsigned TUN0:1;
      unsigned TUN1:1;
      unsigned TUN2:1;
      unsigned TUN3:1;
      unsigned TUN4:1;
      unsigned TUN5:1;
    };
  };
} OSCTUNBITS;
extern volatile OSCTUNBITS OSCTUNbits __attribute__((__sfr__));

extern volatile unsigned int BSRAM __attribute__((__sfr__));
typedef struct tagBSRAMBITS {
  unsigned RL_BSR:1;
  unsigned IR_BSR:1;
  unsigned IW_BSR:1;
} BSRAMBITS;
extern volatile BSRAMBITS BSRAMbits __attribute__((__sfr__));

extern volatile unsigned int SSRAM __attribute__((__sfr__));
typedef struct tagSSRAMBITS {
  unsigned RL_SSR:1;
  unsigned IR_SSR:1;
  unsigned IW_SSR:1;
} SSRAMBITS;
extern volatile SSRAMBITS SSRAMbits __attribute__((__sfr__));

extern volatile unsigned int NVMCON __attribute__((__sfr__));
__extension__ typedef struct tagNVMCONBITS {
  union {
    struct {
      unsigned NVMOP:4;
      unsigned :2;
      unsigned ERASE:1;
      unsigned :6;
      unsigned WRERR:1;
      unsigned WREN:1;
      unsigned WR:1;
    };
    struct {
      unsigned NVMOP0:1;
      unsigned NVMOP1:1;
      unsigned NVMOP2:1;
      unsigned NVMOP3:1;
    };
  };
} NVMCONBITS;
extern volatile NVMCONBITS NVMCONbits __attribute__((__sfr__));

extern volatile unsigned int NVMKEY __attribute__((__sfr__));
extern volatile unsigned int PMD1 __attribute__((__sfr__));
typedef struct tagPMD1BITS {
  unsigned AD1MD:1;
  unsigned C1MD:1;
  unsigned C2MD:1;
  unsigned SPI1MD:1;
  unsigned SPI2MD:1;
  unsigned U1MD:1;
  unsigned U2MD:1;
  unsigned I2C1MD:1;
  unsigned DCIMD:1;
  unsigned :2;
  unsigned T1MD:1;
  unsigned T2MD:1;
  unsigned T3MD:1;
  unsigned T4MD:1;
  unsigned T5MD:1;
} PMD1BITS;
extern volatile PMD1BITS PMD1bits __attribute__((__sfr__));

extern volatile unsigned int PMD2 __attribute__((__sfr__));
typedef struct tagPMD2BITS {
  unsigned OC1MD:1;
  unsigned OC2MD:1;
  unsigned OC3MD:1;
  unsigned OC4MD:1;
  unsigned OC5MD:1;
  unsigned OC6MD:1;
  unsigned OC7MD:1;
  unsigned OC8MD:1;
  unsigned IC1MD:1;
  unsigned IC2MD:1;
  unsigned IC3MD:1;
  unsigned IC4MD:1;
  unsigned IC5MD:1;
  unsigned IC6MD:1;
  unsigned IC7MD:1;
  unsigned IC8MD:1;
} PMD2BITS;
extern volatile PMD2BITS PMD2bits __attribute__((__sfr__));

extern volatile unsigned int PMD3 __attribute__((__sfr__));
typedef struct tagPMD3BITS {
  unsigned AD2MD:1;
  unsigned I2C2MD:1;
  unsigned :10;
  unsigned T6MD:1;
  unsigned T7MD:1;
  unsigned T8MD:1;
  unsigned T9MD:1;
} PMD3BITS;
extern volatile PMD3BITS PMD3bits __attribute__((__sfr__));
# 11651 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/dsPIC33F/h/p33FJ256GP710A.h" 3 4
extern void *_DMA_BASE __attribute__((far));
# 11660 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/dsPIC33F/h/p33FJ256GP710A.h" 3 4
extern __attribute__((space(prog))) int _FBS;
# 11733 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/dsPIC33F/h/p33FJ256GP710A.h" 3 4
extern __attribute__((space(prog))) int _FSS;
# 11806 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/dsPIC33F/h/p33FJ256GP710A.h" 3 4
extern __attribute__((space(prog))) int _FGS;
# 11842 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/dsPIC33F/h/p33FJ256GP710A.h" 3 4
extern __attribute__((space(prog))) int _FOSCSEL;
# 11884 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/dsPIC33F/h/p33FJ256GP710A.h" 3 4
extern __attribute__((space(prog))) int _FOSC;
# 11927 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/dsPIC33F/h/p33FJ256GP710A.h" 3 4
extern __attribute__((space(prog))) int _FWDT;
# 12006 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/dsPIC33F/h/p33FJ256GP710A.h" 3 4
extern __attribute__((space(prog))) int _FPOR;
# 12041 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/dsPIC33F/h/p33FJ256GP710A.h" 3 4
extern __attribute__((space(prog))) int _FICD;
# 12073 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/dsPIC33F/h/p33FJ256GP710A.h" 3 4
extern __attribute__((space(prog))) int _FUID0;
# 12092 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/dsPIC33F/h/p33FJ256GP710A.h" 3 4
extern __attribute__((space(prog))) int _FUID1;
# 12111 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/dsPIC33F/h/p33FJ256GP710A.h" 3 4
extern __attribute__((space(prog))) int _FUID2;
# 12130 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/dsPIC33F/h/p33FJ256GP710A.h" 3 4
extern __attribute__((space(prog))) int _FUID3;
# 68 "../../libDCM/../libUDB/libUDB_defines.h" 2
# 1 "../../libDCM/../libUDB/ConfigUDB4.h" 1
# 69 "../../libDCM/../libUDB/libUDB_defines.h" 2
# 100 "../../libDCM/../libUDB/libUDB_defines.h"
# 1 "../../libDCM/../libUDB/boardRotation_defines.h" 1
# 101 "../../libDCM/../libUDB/libUDB_defines.h" 2
# 156 "../../libDCM/../libUDB/libUDB_defines.h"
typedef char boolean;



struct ADchannel {
 int input;
 int value;
 int offset;
 long sum ;
};


struct udb_flag_bits {
   unsigned int unused : 6 ;
   unsigned int a2d_read : 1 ;
   unsigned int radio_on : 1 ;
   } ;
# 226 "../../libDCM/../libUDB/libUDB_defines.h"
extern int magMessage ;
extern int vref_adj ;
# 29 "../../libDCM/../libUDB/libUDB.h" 2
# 1 "../../libDCM/../libUDB/magnetometerOptions.h" 1
# 30 "../../libDCM/../libUDB/libUDB.h" 2
# 1 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/generic/h/dsp.h" 1 3 4
# 40 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/generic/h/dsp.h" 3 4
# 1 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../include/stdlib.h" 1 3 4






# 1 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../include/stddef.h" 1 3 4



typedef int ptrdiff_t;
typedef unsigned int size_t;
typedef short unsigned int wchar_t;






extern int errno;
# 8 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../include/stdlib.h" 2 3 4
# 16 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../include/stdlib.h" 3 4
typedef struct {
 int quot;
 int rem;
} div_t;
typedef struct {
 unsigned quot;
 unsigned rem;
} udiv_t;
typedef struct {
 long quot;
 long rem;
} ldiv_t;
typedef struct {
 unsigned long quot;
 unsigned long rem;
} uldiv_t;
# 47 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../include/stdlib.h" 3 4
extern double atof(const char *);
extern double strtod(const char *, const char **);


extern int atoi(const char *);
extern unsigned xtoi(const char *);
extern long atol(const char *);
extern long strtol(const char *, char **, int);
extern unsigned long strtoul(const char *, char **, int);

extern long long strtoll(const char *, char **, int);
extern unsigned long long strtoull(const char *, char **, int);

extern int rand(void);
extern void srand(unsigned int);
extern void * calloc(size_t, size_t);
extern div_t div(int numer, int denom);
extern udiv_t udiv(unsigned numer, unsigned denom);
extern ldiv_t ldiv(long numer, long denom);
extern uldiv_t uldiv(unsigned long numer,unsigned long denom);




extern void * malloc(size_t);
extern void free(void *);
extern void * realloc(void *, size_t);

extern void abort(void);
extern void exit(int);
extern int atexit(void (*)(void));
extern char * getenv(const char *);
extern char ** environ;
extern int system(char *);
extern void qsort(void *, size_t, size_t, int (*)(const void *, const void *));
extern void * bsearch(const void *, void *, size_t, size_t, int(*)(const void *, const void *));
extern int abs(int);
extern long labs(long);

extern char * itoa(char * buf, int val, int base);
extern char * utoa(char * buf, unsigned val, int base);
extern char * ltoa(char * buf, long val, int base);


extern char * ultoa(char * buf, unsigned long val, int base);
# 41 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/generic/h/dsp.h" 2 3 4
# 1 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../include/math.h" 1 3 4
# 38 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../include/math.h" 3 4
float acosf(float);
float asinf(float);
float atanf(float);
float atan2f(float, float);
float cosf(float);
float sinf(float);
float tanf(float);
float coshf(float);
float sinhf(float);
float tanhf(float);
float expf(float);
float frexpf(float, int *);
float ldexpf(float, int);
float logf(float);
float log10f(float);





float modff(float, void *);



float powf(float, float);
float sqrtf(float);
float ceilf(float);
float fabsf(float);
float floorf(float);
float fmodf(float, float);






long double acosl(long double);
long double asinl(long double);
long double atanl(long double);
long double atan2l(long double, long double);
long double cosl(long double);
long double sinl(long double);
long double tanl(long double);
long double coshl(long double);
long double sinhl(long double);
long double tanhl(long double);
long double expl(long double);
long double frexpl(long double, int *);
long double ldexpl(long double, int);
long double logl(long double);
long double log10l(long double);
long double modfl(long double, long double *);
long double powl(long double, long double);
long double sqrtl(long double);
long double ceill(long double);
long double fabsl(long double);
long double floorl(long double);
long double fmodl(long double, long double);
# 138 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../include/math.h" 3 4
unsigned long __udiv3216(unsigned long, unsigned int);
         long __div3216(long, int);
# 42 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/generic/h/dsp.h" 2 3 4
# 92 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/generic/h/dsp.h" 3 4
typedef int fractional;



typedef struct {
  fractional real;
  fractional imag;
} fractcomplex;
# 130 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/generic/h/dsp.h" 3 4
extern fractional Float2Fract (
   float aVal
);
extern float Fract2Float (
   fractional aVal
);
# 219 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/generic/h/dsp.h" 3 4
extern fractional VectorMax (



   int numElems,
   fractional* srcV,
   int* maxIndex


);



extern fractional VectorMin (



   int numElems,
   fractional* srcV,
   int* minIndex


);



extern fractional* VectorCopy (





   int numElems,
   fractional* dstV,
   fractional* srcV


);



extern fractional* VectorZeroPad (





   int numElems,
   int numZeros,
   fractional* dstV,
   fractional* srcV


);



extern fractional* VectorNegate (


   int numElems,
   fractional* dstV,
   fractional* srcV


);



extern fractional* VectorScale (


   int numElems,
   fractional* dstV,
   fractional* srcV,
   fractional sclVal


);



extern fractional* VectorAdd (




   int numElems,
   fractional* dstV,
   fractional* srcV1,
   fractional* srcV2


);



extern fractional* VectorSubtract (




   int numElems,
   fractional* dstV,
   fractional* srcV1,
   fractional* srcV2


);



extern fractional* VectorMultiply (




   int numElems,
   fractional* dstV,
   fractional* srcV1,
   fractional* srcV2


);



extern fractional VectorDotProduct (



   int numElems,
   fractional* srcV1,
   fractional* srcV2


);



extern fractional VectorPower (


   int numElems,
   fractional* srcV


);



extern fractional* VectorConvolve (

   int numElems1,
   int numElems2,

   fractional* dstV,

   fractional* srcV1,
   fractional* srcV2


);



extern fractional* VectorCorrelate (

   int numElems1,
   int numElems2,

   fractional* dstV,

   fractional* srcV1,
   fractional* srcV2


);
# 438 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/generic/h/dsp.h" 3 4
extern fractional* BartlettInit (


   int numElems,
   fractional* window


);



extern fractional* BlackmanInit (


   int numElems,
   fractional* window


);



extern fractional* HammingInit (


   int numElems,
   fractional* window


);



extern fractional* HanningInit (


   int numElems,
   fractional* window


);



extern fractional* KaiserInit (


   int numElems,
   fractional* window,
   float betaVal


);



extern fractional* VectorWindow (



   int numElems,
   fractional* dstV,
   fractional* srcV,
   fractional* window


);
# 600 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/generic/h/dsp.h" 3 4
extern fractional* MatrixScale (


   int numRows,
   int numCols,
   fractional* dstM,
   fractional* srcM,
   fractional sclVal


);



extern fractional* MatrixTranspose (


   int numRows,
   int numCols,
   fractional* dstM,
   fractional* srcM


);



float* MatrixInvert (


   int numRowsCols,

   float* dstM,
   float* srcM,
   float* pivotFlag,
   int* swappedRows,
   int* swappedCols






);



extern fractional* MatrixAdd (




   int numRows,
   int numCols,
   fractional* dstM,
   fractional* srcM1,
   fractional* srcM2


);



extern fractional* MatrixSubtract (




   int numRows,
   int numCols,
   fractional* dstM,
   fractional* srcM1,
   fractional* srcM2


);



extern fractional* MatrixMultiply (







   int numRows1,
   int numCols1Rows2,

   int numCols2,
   fractional* dstM,
   fractional* srcM1,
   fractional* srcM2


);
# 753 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/generic/h/dsp.h" 3 4
typedef struct {
   int numCoeffs;



   fractional* coeffsBase;
# 770 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/generic/h/dsp.h" 3 4
   fractional* coeffsEnd;






   int coeffsPage;




   fractional* delayBase;






   fractional* delayEnd;

   fractional* delay;
} FIRStruct;


extern void FIRStructInit (
   FIRStruct* FIRFilter,
   int numCoeffs,



   fractional* coeffsBase,
# 813 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/generic/h/dsp.h" 3 4
   int coeffsPage,




   fractional* delayBase
# 827 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/generic/h/dsp.h" 3 4
);


extern void FIRDelayInit (
   FIRStruct* filter


);


extern void FIRInterpDelayInit (

   FIRStruct* filter,
   int rate
);


extern fractional* FIR (
   int numSamps,
   fractional* dstSamps,

   fractional* srcSamps,

   FIRStruct* filter
# 859 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/generic/h/dsp.h" 3 4
);


extern fractional* FIRDecimate (
   int numSamps,

   fractional* dstSamps,

   fractional* srcSamps,

   FIRStruct* filter,





   int rate


);


extern fractional* FIRInterpolate (
   int numSamps,

   fractional* dstSamps,

   fractional* srcSamps,

   FIRStruct* filter,




   int rate


);


extern fractional* FIRLattice (
   int numSamps,
   fractional* dstSamps,

   fractional* srcSamps,

   FIRStruct* filter
# 914 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/generic/h/dsp.h" 3 4
);

extern fractional* FIRLMS (
# 926 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/generic/h/dsp.h" 3 4
   int numSamps,
   fractional* dstSamps,

   fractional* srcSamps,

   FIRStruct* filter,






   fractional* refSamps,

   fractional muVal


);

extern fractional* FIRLMSNorm (
# 962 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/generic/h/dsp.h" 3 4
   int numSamps,
   fractional* dstSamps,

   fractional* srcSamps,

   FIRStruct* filter,






   fractional* refSamps,

   fractional muVal,
   fractional* energyEstimate






);
# 1047 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/generic/h/dsp.h" 3 4
typedef struct {
  int numSectionsLess1;

  fractional* coeffsBase;




  int coeffsPage;


  fractional* delayBase;


  fractional initialGain;
  int finalShift;






} IIRCanonicStruct;

extern fractional* IIRCanonic (

   int numSamps,
   fractional* dstSamps,

   fractional* srcSamps,

   IIRCanonicStruct* filter


);

extern void IIRCanonicInit (
   IIRCanonicStruct* filter
);



typedef struct {
  int numSectionsLess1;

  fractional* coeffsBase;




  int coeffsPage;


  fractional* delayBase1;


  fractional* delayBase2;


  int finalShift;





} IIRTransposedStruct;

extern fractional* IIRTransposed (

   int numSamps,
   fractional* dstSamps,

   fractional* srcSamps,

   IIRTransposedStruct* filter


);

extern void IIRTransposedInit (
   IIRTransposedStruct* filter
);



typedef struct {
   int order;

   fractional* kappaVals;


   fractional* gammaVals;



   int coeffsPage;


   fractional* delay;


} IIRLatticeStruct;

extern fractional* IIRLattice (
   int numSamps,
   fractional* dstSamps,

   fractional* srcSamps,

   IIRLatticeStruct* filter


);

extern void IIRLatticeInit (
   IIRLatticeStruct* filter
);
# 1218 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/generic/h/dsp.h" 3 4
extern fractcomplex* TwidFactorInit (



   int log2N,



   fractcomplex* twidFactors,
   int conjFlag
# 1236 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/generic/h/dsp.h" 3 4
);



extern fractcomplex* BitReverseComplex (

   int log2N,
   fractcomplex* srcCV



);



extern fractcomplex* FFTComplex (

   int log2N,
   fractcomplex* dstCV,



   fractcomplex* srcCV,


   fractcomplex* twidFactors,







   int factPage
# 1279 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/generic/h/dsp.h" 3 4
);



extern fractcomplex* FFTComplexIP (

   int log2N,
   fractcomplex* srcCV,


   fractcomplex* twidFactors,







   int factPage
# 1307 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/generic/h/dsp.h" 3 4
);



extern fractcomplex* IFFTComplex (

   int log2N,
   fractcomplex* dstCV,
   fractcomplex* srcCV,



   fractcomplex* twidFactors,







   int factPage
# 1336 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/generic/h/dsp.h" 3 4
);



extern fractcomplex* IFFTComplexIP (

   int log2N,
   fractcomplex* srcCV,



   fractcomplex* twidFactors,







   int factPage
# 1364 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/generic/h/dsp.h" 3 4
);



extern long toneGen32b(
  int samp,
  int tone,
  int phase,
  long *tfBuff
);



extern void BitReverseReal32bIP(
  int N,
  long *ipBuff
);



extern long* FFTReal32bIP(
   int log2N,
   int N,
   long *ipBuff,
   long *tfBuf,
   int factPage
);



extern long* FFTReal32b(
   int log2N,
   int N,
   long *opBuff,
   long *ipBuff,
   long *tfBuf,
   int factPage
);



extern void FFTComplex32bIP(
  int log2N,
  long *ipBuff,
  long *tfBuff,
  int factPage
);



extern void realFft32bIP(
  int log2N,
  long *ipBuff,
  long *tfBuff,
  int factPage
);



extern long* IFFTReal32bIP(
   int log2N,
   int N,
   long *ipBuff,
   long *tfBuf,
   int factPage
);



extern long* IFFTReal32b(
   int log2N,
   int N,
   long *opBuff,
   long *ipBuff,
   long *tfBuf,
   int factPage
);



extern void IFFTComplex32bIP(
  int log2N,
  long *ipBuff,
  long *tfBuff,
  int factPage
);



extern void realiFft32bIP(
  int log2N,
  long *ipBuff,
  long *tfBuff,
  int factPage
);



extern void MagnitudeCplx32bIP(
  int N,
  long *ipBuff
);



extern fractcomplex* CosFactorInit (



   int log2N,



   fractcomplex* cosFactors




);



extern fractional* DCT (

   int log2N,
   fractional* dstV,


   fractional* srcV,

   fractcomplex* cosFactors,
# 1505 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/generic/h/dsp.h" 3 4
   fractcomplex* twidFactors,
# 1517 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/generic/h/dsp.h" 3 4
   int factPage
# 1531 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/generic/h/dsp.h" 3 4
);



extern fractional* DCTIP (

   int log2N,
   fractional* srcV,


   fractcomplex* cosFactors,
# 1552 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/generic/h/dsp.h" 3 4
   fractcomplex* twidFactors,
# 1564 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/generic/h/dsp.h" 3 4
   int factPage
# 1578 "/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/generic/h/dsp.h" 3 4
);



extern fractional* SquareMagnitudeCplx (
   int numelems,
   fractcomplex* srcV,
   fractional* dstV







);



typedef struct {
        fractional* abcCoefficients;


        fractional* controlHistory;

        fractional controlOutput;
        fractional measuredOutput;
        fractional controlReference;
} tPID;



extern void PIDCoeffCalc(
        fractional* kCoeffs,
        tPID* controller
);



extern void PIDInit (
        tPID* controller
);




extern fractional* PID (
        tPID* controller
);
# 31 "../../libDCM/../libUDB/libUDB.h" 2
# 57 "../../libDCM/../libUDB/libUDB.h"
void udb_init(void);






void udb_run(void);







void udb_background_callback_periodic(void);



void udb_background_trigger(void);



void udb_background_callback_triggered(void);



unsigned char udb_cpu_load(void);
# 93 "../../libDCM/../libUDB/libUDB.h"
extern unsigned int udb_heartbeat_counter ;
# 102 "../../libDCM/../libUDB/libUDB.h"
extern int udb_pwIn[];






extern int udb_pwTrim[];





extern int udb_pwOut[];



extern union udb_fbts_byte { struct udb_flag_bits _ ; char B ; } udb_flags ;



int udb_servo_pulsesat(long pw);



void udb_servo_record_trims(void);



void udb_servo_callback_prepare_outputs(void);


void udb_callback_radio_did_turn_off( void );




void udb_set_action_state(boolean newValue);



extern struct ADchannel udb_xaccel, udb_yaccel, udb_zaccel;
extern struct ADchannel udb_xrate, udb_yrate, udb_zrate;
extern struct ADchannel udb_vref;
extern struct ADchannel udb_analogInputs[];
# 165 "../../libDCM/../libUDB/libUDB.h"
void udb_a2d_record_offsets(void);
void udb_callback_read_sensors(void);







extern fractional udb_magFieldBody[3];
extern fractional udb_magOffset[3];



void udb_magnetometer_callback_data_available(void);
# 193 "../../libDCM/../libUDB/libUDB.h"
void udb_gps_set_rate(long rate);
boolean udb_gps_check_rate(long rate);


void udb_gps_start_sending_data(void);



int udb_gps_callback_get_byte_to_send(void);


void udb_gps_callback_received_byte(char rxchar);







void udb_serial_set_rate(long rate);
boolean udb_serial_check_rate(long rate);


void udb_serial_start_sending_data(void);



int udb_serial_callback_get_byte_to_send(void);


void udb_serial_callback_received_byte(char rxchar);





void osd_spi_write(char address, char byte) ;
void osd_spi_write_byte(char byte) ;
void osd_spi_write_location(int loc) ;
void osd_spi_write_string(const unsigned char *str) ;
void osd_spi_write_vertical_string_at_location(int loc, const unsigned char *str) ;
void osd_spi_erase_chars(unsigned char n) ;






void osd_spi_write_number(long val, char num_digits, char num_flags, char header, char footer) ;
# 251 "../../libDCM/../libUDB/libUDB.h"
unsigned char eeprom_ByteWrite(unsigned int address, unsigned char data);
unsigned char eeprom_ByteRead(unsigned int address, unsigned char *data);





unsigned char eeprom_PageWrite(unsigned int address, unsigned char *data, unsigned char numbytes);
unsigned char eeprom_SequentialRead(unsigned int address, unsigned char *data, unsigned int numbytes);
# 27 "../../libDCM/libDCM.h" 2
# 1 "../../libDCM/libDCM_defines.h" 1
# 27 "../../libDCM/libDCM_defines.h"
struct relative2D { int x ; int y ; } ;
struct absolute2D { long x ; long y ; } ;
struct relative3D { int x ; int y ; int z ; } ;
struct absolute3D { long x ; long y ; long z ; } ;
struct waypoint3D { long x ; long y ; int z ; } ;
struct fixedOrigin3D {long x; long y; float z;} ;

struct dcm_flag_bits {
   unsigned int unused : 4 ;
   unsigned int rollpitch_req : 1 ;
   unsigned int gps_history_valid : 1 ;
   unsigned int dead_reckon_enable : 1 ;
   unsigned int reckon_req : 1 ;
   unsigned int first_mag_reading : 1 ;
   unsigned int mag_drift_req : 1 ;
   unsigned int yaw_req : 1 ;
   unsigned int skip_yaw_drift : 1 ;
   unsigned int nav_capable : 1 ;
   unsigned int nmea_passthrough : 1 ;
   unsigned int init_finished : 1 ;
   unsigned int calib_finished : 1 ;
   } ;
# 28 "../../libDCM/libDCM.h" 2
# 41 "../../libDCM/libDCM.h"
void dcm_init( void ) ;
void dcm_calibrate(void) ;
void dcm_set_origin_location(long o_long, long o_lat, long o_alt) ;



void dcm_callback_gps_location_updated(void) ;



void dcm_enable_yaw_drift_correction(boolean enabled) ;



void dcm_servo_callback_prepare_outputs(void) ;


struct relative3D dcm_absolute_to_relative(struct waypoint3D absolute) ;



void gps_startup_sequence(unsigned int gpscount ) ;


boolean gps_nav_valid(void) ;



int cosine ( signed char angle ) ;
int sine ( signed char angle ) ;
signed char arcsine ( int y ) ;
signed char rect_to_polar ( struct relative2D *xy ) ;
int rect_to_polar16 ( struct relative2D *xy ) ;
void rotate( struct relative2D *xy , signed char angle ) ;


unsigned int sqrt_int ( unsigned int ) ;
unsigned int sqrt_long ( unsigned long int ) ;


unsigned int vector2_mag( int , int ) ;
unsigned int vector3_mag( int , int , int ) ;


unsigned int vector2_normalize( int result[] , int input[] ) ;
unsigned int vector3_normalize( int result[] , int input[] ) ;




extern union dcm_fbts_word { struct dcm_flag_bits _ ; int W ; } dcm_flags ;


extern fractional rmat[] ;
extern fractional omegaAccum[] ;
extern fractional omegagyro[] ;
extern fractional accelEarth[] ;

extern struct relative3D GPSlocation ;
extern struct relative3D GPSvelocity ;
extern struct relative2D velocity_thru_air ;
extern int estimatedWind[3] ;

extern unsigned int air_speed_3DIMU ;
extern int total_energy ;

extern union longww IMUlocationx , IMUlocationy , IMUlocationz ;
extern union longww IMUvelocityx , IMUvelocityy , IMUvelocityz ;


extern signed char calculated_heading ;
extern int gps_data_age ;

extern unsigned int ground_velocity_magnitudeXY ;
extern unsigned int air_speed_magnitudeXY;

extern union longbbbb lat_gps , long_gps , alt_sl_gps ;
extern union longbbbb lat_origin , long_origin , alt_origin ;
# 23 "../main.c" 2

boolean didCalibrate = 0;

void send_fast_telemetry(void);
void send_telemetry(void);
void motorCntrl(void);
void setup_origin(void);

extern unsigned int pid_gains[];
extern unsigned long uptime;

const int max_tilt = (int) (45 * .7111);
int commanded_tilt_gain;


int flight_mode = 0;

extern int pitch_step;


boolean udb_throttle_enable = 0;

unsigned int tailFlash = 0;

extern union longww primary_voltage;
extern unsigned int lowVoltageWarning;

int main(void) {

    udb_init();
    dcm_init();






    eeprom_SequentialRead((0X400), (unsigned char *) pid_gains, 6);
# 73 "../main.c"
    udb_serial_set_rate(222222);

    LATEbits.LATE2 = 1;
    LATDbits.LATD4 = 1;

    commanded_tilt_gain = 2 * sine(max_tilt) / 1000;


    udb_run();

    return 0;
}

void check_failsafe() {

    if (udb_pwIn[5] < 3000)
        udb_throttle_enable = 1;
    else
        udb_throttle_enable = 0;
}

void check_flight_mode(void) {

    if (udb_pwIn[6] < 2500)
        flight_mode = 2;
    else if (udb_pwIn[6] < 3500)
        flight_mode = 1;
    else
        flight_mode = 0;
}

void storeGain(int index) {
    if (index >= 0 && index < 4) {

        unsigned int address = (0X400) + (2 * index);
        eeprom_ByteWrite(address++, (unsigned char) pid_gains[index]);
        eeprom_ByteWrite(address, (unsigned char) (pid_gains[index] >> 8));
    }
}

void storeGains(void) {
    eeprom_PageWrite((0X400), (unsigned char*) pid_gains, 8);
}

void adjust_gain(int index, int delta) {
    if (delta > 0) {
        tailFlash = 1;
        delta = 0b0100000000000000 * 0.05;;
    } else {
        tailFlash = 2;
        delta = -0b0100000000000000 * 0.05;;
    }
    pid_gains[index] += delta;
    pid_gains[index] = (((0) > (pid_gains[index])) ? (0) : (pid_gains[index]));
}

extern boolean sendGains;

int gainAdjIndex[] = {0, 1, 2};

void check_gain_adjust(void) {
    static int gainState = -1;
    static int gainIndex = 0;
    static int lastGainChVal = 0;




    switch (gainState) {
        case -1:
            if (dcm_flags._.calib_finished) {

                lastGainChVal = udb_pwIn[8];
                gainState = 0;
            }
            break;
        case 0:

            if (udb_throttle_enable) {
                gainState = 1;

                lastGainChVal = udb_pwIn[8];
            }
            break;
        case 1:

            gainIndex = gainAdjIndex[flight_mode];
            if (udb_throttle_enable) {


                int gain_delta = udb_pwIn[8] - lastGainChVal;
                if (abs(gain_delta) > 20) {
                    lastGainChVal = udb_pwIn[8];
                    adjust_gain(gainIndex, gain_delta);
                }
            } else {

                gainState = 0;
                storeGain(gainIndex);
                sendGains = 1;
            }
            break;
    }
}

void update_pid_gains(void) {


    if (udb_flags._.radio_on) {

        check_failsafe();

        check_flight_mode();
        check_gain_adjust();
    }
}

void run_background_task() {


    static int lastUptime = 0;
    if ((uptime - lastUptime) >= 400 / 20) {
        lastUptime = uptime;
        if (1 != 0)

            update_pid_gains();
        else
            udb_throttle_enable = 1;
    }



    {__asm__ volatile ("pwrsav #1");};

    return;
}




void udb_background_callback_periodic(void) {
    if (!didCalibrate) {

        ((LATEbits.LATE1) = !(LATEbits.LATE1));

        if (udb_flags._.radio_on && dcm_flags._.calib_finished) {


            lowVoltageWarning = 0;
            tailFlash = 10;
            if ((primary_voltage._.W1 > 9750) && (primary_voltage._.W1 < 16900)) {
                if (primary_voltage._.W1 > 12700) {
                    lowVoltageWarning = 12900;
                    tailFlash = 4;
                } else {
                    lowVoltageWarning = 9750;
                    tailFlash = 3;
                }
            }



            udb_servo_record_trims();

            dcm_calibrate();
            didCalibrate = 1;
        }
    } else {




    }

    return;
}




void dcm_callback_gps_location_updated(void) {
    return;
}




void dcm_servo_callback_prepare_outputs(void) {
    static int pidCounter = 0;
    static int telCounter = 0;


    if (udb_flags._.radio_on) {
        LATEbits.LATE2 = 0;
    } else {
        LATEbits.LATE2 = 1;
    }


    if (++pidCounter >= 400 / 400) {
        pidCounter = 0;
        motorCntrl();
    }

    if (0 || didCalibrate) {



        if (0 || udb_throttle_enable || sendGains) {
            switch (2) {
                case 0:
                case 1:
                case 2:

                    if (++telCounter >= 400 / 40) {
                        telCounter = 0;
                        send_telemetry();
                    }
                    break;
                case 10:

                    if (++telCounter >= 400 / 100) {
                        telCounter = 0;
                        send_fast_telemetry();
                    }
                    break;
            }
        }
    }
    return;
}

void udb_callback_radio_did_turn_off(void) {
}
