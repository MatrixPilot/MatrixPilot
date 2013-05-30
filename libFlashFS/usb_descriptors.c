/********************************************************************
-usb_descriptors.c-
-------------------------------------------------------------------
Filling in the descriptor values in the usb_descriptors.c file:
-------------------------------------------------------------------

[Device Descriptors]
The device descriptor is defined as a USB_DEVICE_DESCRIPTOR type.  
This type is defined in usb_ch9.h  Each entry into this structure
needs to be the correct length for the data type of the entry.

[Configuration Descriptors]
The configuration descriptor was changed in v2.x from a structure
to a BYTE array.  Given that the configuration is now a byte array
each byte of multi-byte fields must be listed individually.  This
means that for fields like the total size of the configuration where
the field is a 16-bit value "64,0," is the correct entry for a
configuration that is only 64 bytes long and not "64," which is one
too few bytes.

The configuration attribute must always have the _DEFAULT
definition at the minimum. Additional options can be ORed
to the _DEFAULT attribute. Available options are _SELF and _RWU.
These definitions are defined in the usb_device.h file. The
_SELF tells the USB host that this device is self-powered. The
_RWU tells the USB host that this device supports Remote Wakeup.

[Endpoint Descriptors]
Like the configuration descriptor, the endpoint descriptors were 
changed in v2.x of the stack from a structure to a BYTE array.  As
endpoint descriptors also has a field that are multi-byte entities,
please be sure to specify both bytes of the field.  For example, for
the endpoint size an endpoint that is 64 bytes needs to have the size
defined as "64,0," instead of "64,"

Take the following example:
    // Endpoint Descriptor //
    0x07,                       //the size of this descriptor //
    USB_DESCRIPTOR_ENDPOINT,    //Endpoint Descriptor
    _EP02_IN,                   //EndpointAddress
    _INT,                       //Attributes
    0x08,0x00,                  //size (note: 2 bytes)
    0x02,                       //Interval

The first two parameters are self-explanatory. They specify the
length of this endpoint descriptor (7) and the descriptor type.
The next parameter identifies the endpoint, the definitions are
defined in usb_device.h and has the following naming
convention:
_EP<##>_<dir>
where ## is the endpoint number and dir is the direction of
transfer. The dir has the value of either 'OUT' or 'IN'.
The next parameter identifies the type of the endpoint. Available
options are _BULK, _INT, _ISO, and _CTRL. The _CTRL is not
typically used because the default control transfer endpoint is
not defined in the USB descriptors. When _ISO option is used,
addition options can be ORed to _ISO. Example:
_ISO|_AD|_FE
This describes the endpoint as an isochronous pipe with adaptive
and feedback attributes. See usb_device.h and the USB
specification for details. The next parameter defines the size of
the endpoint. The last parameter in the polling interval.

-------------------------------------------------------------------
Adding a USB String
-------------------------------------------------------------------
A string descriptor array should have the following format:

rom struct{byte bLength;byte bDscType;word string[size];}sdxxx={
sizeof(sdxxx),DSC_STR,<text>};

The above structure provides a means for the C compiler to
calculate the length of string descriptor sdxxx, where xxx is the
index number. The first two bytes of the descriptor are descriptor
length and type. The rest <text> are string texts which must be
in the unicode format. The unicode format is achieved by declaring
each character as a word type. The whole text string is declared
as a word array with the number of characters equals to <size>.
<size> has to be manually counted and entered into the array
declaration. Let's study this through an example:
if the string is "USB" , then the string descriptor should be:
(Using index 02)
rom struct{byte bLength;byte bDscType;word string[3];}sd002={
sizeof(sd002),DSC_STR,'U','S','B'};

A USB project may have multiple strings and the firmware supports
the management of multiple strings through a look-up table.
The look-up table is defined as:
rom const unsigned char *rom USB_SD_Ptr[]={&sd000,&sd001,&sd002};

The above declaration has 3 strings, sd000, sd001, and sd002.
Strings can be removed or added. sd000 is a specialized string
descriptor. It defines the language code, usually this is
US English (0x0409). The index of the string must match the index
position of the USB_SD_Ptr array, &sd000 must be in position
USB_SD_Ptr[0], &sd001 must be in position USB_SD_Ptr[1] and so on.
The look-up table USB_SD_Ptr is used by the get string handler
function.

-------------------------------------------------------------------

The look-up table scheme also applies to the configuration
descriptor. A USB device may have multiple configuration
descriptors, i.e. CFG01, CFG02, etc. To add a configuration
descriptor, user must implement a structure similar to CFG01.
The next step is to add the configuration descriptor name, i.e.
cfg01, cfg02,.., to the look-up table USB_CD_Ptr. USB_CD_Ptr[0]
is a dummy place holder since configuration 0 is the un-configured
state according to the definition in the USB specification.

********************************************************************/

/*********************************************************************
 * Descriptor specific type definitions are defined in:
 * usb_device.h
 *
 * Configuration options are defined in:
 * usb_config.h
 ********************************************************************/

#ifndef __USB_DESCRIPTORS_C
#define __USB_DESCRIPTORS_C

/** INCLUDES *******************************************************/
#include "./USB/usb.h"
#include "./USB/usb_function_msd.h"
#include "./USB/usb_function_cdc.h"

/** CONSTANTS ******************************************************/
#if defined(__18CXX)
#pragma romdata
#endif

/* Device Descriptor */
ROM USB_DEVICE_DESCRIPTOR device_dsc=
{
    0x12,    // Size of this descriptor in bytes
    USB_DESCRIPTOR_DEVICE,                // DEVICE descriptor type
    0x0200,                 // USB Spec Release Number in BCD format
    0xEF,                   // Class Code "MISC_DEVICE" (ex: uses IAD descriptor)  
    0x02,                   // Subclass code
    0x01,                   // Protocol code
    USB_EP0_BUFF_SIZE,      // Max packet size for EP0, see usb_config.h
    0x04D8,                 // Vendor ID
    0x0057,                 // Product ID
    0x0001,                 // Device release number in BCD format
    0x01,                   // Manufacturer string index
    0x02,                   // Product string index
    0x03,                   // Device serial number string index
    0x01                    // Number of possible configurations
};

/* Configuration 1 Descriptor */
ROM BYTE configDescriptor1[]={
    /* Configuration Descriptor */
    9,    // Size of this descriptor in bytes
    USB_DESCRIPTOR_CONFIGURATION,                // CONFIGURATION descriptor type
    98, 0,                  // Total length of data for this cfg
    3,                      // Number of interfaces in this cfg
    1,                      // Index value of this configuration
    2,                      // Configuration string index
    _DEFAULT | _SELF,       // Attributes, see usb_device.h
    50,                     // Max power consumption (2X mA)

//---------------MSD Function 1 Descriptors------------------------
    /* Interface Descriptor */
    9,   // Size of this descriptor in bytes
    USB_DESCRIPTOR_INTERFACE,               // INTERFACE descriptor type
    MSD_INTF_ID,            // Interface Number
    0,                      // Alternate Setting Number
    2,                      // Number of endpoints in this intf
    MSD_INTF,               // Class code
    MSD_INTF_SUBCLASS,      // Subclass code
    MSD_PROTOCOL, 		    // Protocol code
    0,                      // Interface string index

    /* Endpoint Descriptor */
    7,
    USB_DESCRIPTOR_ENDPOINT,
    _EP01_IN,_BULK,
    MSD_IN_EP_SIZE,0x00,
    0x01,

    /* Endpoint Descriptor */
    7,
    USB_DESCRIPTOR_ENDPOINT,
    _EP01_OUT,
    _BULK,
    MSD_OUT_EP_SIZE,0x00,
    0x01,

//---------------IAD Descriptor------------------------------------
    /* Interface Association Descriptor: CDC Function 1*/ 
	0x08,             //sizeof(USB_IAD_DSC), // Size of this descriptor in bytes 
	0x0B,             // Interface assocication descriptor type 
	CDC_COMM_INTF_ID, // The first associated interface 
	2,                // Number of contiguous associated interface 
	COMM_INTF,        // bInterfaceClass of the first interface 
	ABSTRACT_CONTROL_MODEL, // bInterfaceSubclass of the first interface 
	V25TER,           // bInterfaceProtocol of the first interface 
	0,                // Interface string index 						

//---------------CDC Function 1 Descriptors------------------------

    /* Interface Descriptor: CDC Function 1, Status (communication) Interface */
    0x09,   //sizeof(USB_INTF_DSC),   // Size of this descriptor in bytes
    USB_DESCRIPTOR_INTERFACE,               // INTERFACE descriptor type
    CDC_COMM_INTF_ID,       // Interface Number
    0,                      // Alternate Setting Number
    1,                      // Number of endpoints in this intf
    COMM_INTF,              // Class code
    ABSTRACT_CONTROL_MODEL, // Subclass code
    V25TER,                 // Protocol code
    0,                      // Interface string index

    /* CDC Class-Specific Descriptors */
    //5 bytes: Header Functional Descriptor
    sizeof(USB_CDC_HEADER_FN_DSC), //Size of this descriptor in bytes (5)
    CS_INTERFACE,               //bDescriptorType (class specific)
    DSC_FN_HEADER,              //bDescriptorSubtype (header functional descriptor)
    0x20, 0x01,                 //bcdCDC (CDC spec version this fw complies with: v1.20 [stored in little endian])

    //4 bytes: Abstract Control Management Functional Descriptor
    sizeof(USB_CDC_ACM_FN_DSC), //Size of this descriptor in bytes (4)
    CS_INTERFACE,               //bDescriptorType (class specific)
    DSC_FN_ACM,                 //bDescriptorSubtype (abstract control management)
    USB_CDC_ACM_FN_DSC_VAL,     //bmCapabilities: (see PSTN120.pdf Table 4)

    //5 bytes: Union Functional Descriptor
    sizeof(USB_CDC_UNION_FN_DSC), //Size of this descriptor in bytes (5)
    CS_INTERFACE,                 //bDescriptorType (class specific)
    DSC_FN_UNION,                 //bDescriptorSubtype (union functional)
    CDC_COMM_INTF_ID,             //bControlInterface: Interface number of the communication class interface (1)
    CDC_DATA_INTF_ID,             //bSubordinateInterface0: Data class interface #2 is subordinate to this interface

    //5 bytes: Call Management Functional Descriptor
    sizeof(USB_CDC_CALL_MGT_FN_DSC), //Size of this descriptor in bytes (5)
    CS_INTERFACE,                    //bDescriptorType (class specific)
    DSC_FN_CALL_MGT,                 //bDescriptorSubtype (call management functional)
    0x00,                            //bmCapabilities: device doesn't handle call management
    CDC_DATA_INTF_ID,                //bDataInterface: Data class interface ID used for the optional call management

    /* Endpoint Descriptor */
    0x07,/*sizeof(USB_EP_DSC)*/
    USB_DESCRIPTOR_ENDPOINT,    //Endpoint Descriptor
    _EP02_IN,                   //EndpointAddress
    _INTERRUPT,                 //Attributes
    CDC_COMM_IN_EP_SIZE,0x00,   //size
    0x02,                       //Interval

    /* Interface Descriptor: CDC Function 1, Data Interface*/
    0x09,//sizeof(USB_INTF_DSC),   // Size of this descriptor in bytes
    USB_DESCRIPTOR_INTERFACE,      // INTERFACE descriptor type
    CDC_DATA_INTF_ID,       // Interface Number
    0,                      // Alternate Setting Number
    2,                      // Number of endpoints in this intf
    DATA_INTF,              // Class code
    0,                      // Subclass code
    NO_PROTOCOL,            // Protocol code
    0,                      // Interface string index
    
    /* Endpoint Descriptor */
    //sizeof(USB_EP_DSC),DSC_EP,_EP03_OUT,_BULK,CDC_BULK_OUT_EP_SIZE,0x00,
    0x07,/*sizeof(USB_EP_DSC)*/
    USB_DESCRIPTOR_ENDPOINT,    //Endpoint Descriptor
    _EP03_OUT,            //EndpointAddress
    _BULK,                       //Attributes
    CDC_DATA_OUT_EP_SIZE,0x00,                  //size
    0x00,                       //Interval

    /* Endpoint Descriptor */
    //sizeof(USB_EP_DSC),DSC_EP,_EP03_IN,_BULK,CDC_BULK_IN_EP_SIZE,0x00
    0x07,/*sizeof(USB_EP_DSC)*/
    USB_DESCRIPTOR_ENDPOINT,    //Endpoint Descriptor
    _EP03_IN,            //EndpointAddress
    _BULK,                       //Attributes
    CDC_DATA_IN_EP_SIZE,0x00,                  //size
    0x00                       //Interval
};

//Language code string descriptor
ROM struct{BYTE bLength;BYTE bDscType;WORD string[1];}sd000={
    sizeof(sd000),
    USB_DESCRIPTOR_STRING,
    {0x0409
    }
};

//Manufacturer string descriptor
ROM struct{BYTE bLength;BYTE bDscType;WORD string[25];}sd001={
sizeof(sd001),USB_DESCRIPTOR_STRING,
{'M','i','c','r','o','c','h','i','p',' ',
'T','e','c','h','n','o','l','o','g','y',' ','I','n','c','.'
}};

//Product string descriptor
ROM struct{BYTE bLength;BYTE bDscType;WORD string[28];}sd002={
sizeof(sd002),USB_DESCRIPTOR_STRING,
{'M','i','c','r','o','c','h','i','p',' ','C','o','m','p','o','s','i','t','e',' ','D','e','v','i','c','e'
}};

//Serial number string descriptor.  Note: This should be unique for each unit 
//built on the assembly line.  Plugging in two units simultaneously with the 
//same serial number into a single machine can cause problems.  Additionally, not 
//all hosts support all character values in the serial number string.  The MSD 
//Bulk Only Transport (BOT) specs v1.0 restrict the serial number to consist only
//of ASCII characters "0" through "9" and capital letters "A" through "F".
ROM struct{BYTE bLength;BYTE bDscType;WORD string[12];}sd003={
sizeof(sd003),USB_DESCRIPTOR_STRING,
{'1','2','3','4','5','6','7','8','9','9','9','9'}};

//Array of configuration descriptors
ROM BYTE *ROM USB_CD_Ptr[]=
{
    (ROM BYTE *ROM)&configDescriptor1
};

//Array of string descriptors
ROM BYTE *ROM USB_SD_Ptr[]=
{
    (ROM BYTE *ROM)&sd000,
    (ROM BYTE *ROM)&sd001,
    (ROM BYTE *ROM)&sd002,
    (ROM BYTE *ROM)&sd003
};


#endif
/** EOF usb_descriptors.c ***************************************************/
