

#define NETWORK_INTERFACE_NONE                  0
#define NETWORK_INTERFACE_WIFI_MRF24WG          1
#define NETWORK_INTERFACE_ETHERNET_ENC624J600   2
#define NETWORK_INTERFACE_ETHERNET_ENC28J60     3

////////////////////////////////////////////////////////////////////////////////
// TCP/UDP/IP protocols with Network interface
// Enable a network interface over SPI for internet access.
// WiFi is for short range use. For testing use the home WiFi and then a cell phone hotspot on-board.
// For Ethernet a wired router with a high-gain WiFi antenna can work quite far with a directional basestation antenna
// For additional IP tweaks see TCPIPConfig.h, HardwareProfile.h, MyIpOptions.h and edit MyTelemetry[]
// Select a network interface by defining NETWORK_INTERFACE to one of these options:
// NETWORK_INTERFACE_NONE
// NETWORK_INTERFACE_WIFI_MRF24WG           // 802.11g 54 MBit
// NETWORK_INTERFACE_ETHERNET_ENC624J600    // 10/100 MBit
// NETWORK_INTERFACE_ETHERNET_ENC28J60      // 10 MBit

//#define NETWORK_INTERFACE               (NETWORK_INTERFACE_NONE)
//#define NETWORK_INTERFACE               (NETWORK_INTERFACE_WIFI_MRF24WG)
#define NETWORK_INTERFACE               (NETWORK_INTERFACE_ETHERNET_ENC28J60)
//#define NETWORK_INTERFACE               (NETWORK_INTERFACE_ETHERNET_ENC624J600)
