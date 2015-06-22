#define COMMON_BASE 0x0000
#define CH_BASE     0x0000

/*
**  Global registers
*/
enum {
	REG_MR       = COMMON_BASE + 0x0000, //Mode Register
	REG_GAR      = COMMON_BASE + 0x0001, //Gateway IP Register
	REG_SUBR     = COMMON_BASE + 0x0005, //Subnet mask Register
	REG_SHAR     = COMMON_BASE + 0x0009, //Source MAC Register
	REG_SIPR     = COMMON_BASE + 0x000F, //Source IP Register
	REG_ILLT     = COMMON_BASE + 0x0013, //Interrupt Low-Level Timer
	REG_IR       = COMMON_BASE + 0x0015, //Interrupt Register
	REG_IMR      = COMMON_BASE + 0x0016, //Interrupt Mask Register
	REG_SIR      = COMMON_BASE + 0x0017, //Socket Interrupt Register
	REG_SIMR     = COMMON_BASE + 0x0018, //Socket Interrupt Mask Register
	REG_RTR      = COMMON_BASE + 0x0019, //Retry time
	REG_RCR      = COMMON_BASE + 0x001B, //Retry Count
	REG_PTIMER   = COMMON_BASE + 0x001C, //PPP LCP Request Timer
	REG_PMAGIC   = COMMON_BASE + 0x001D, //PPP LCP Magic Number
	REG_PHAR     = COMMON_BASE + 0x001E, //PPP Destination MAC Address
	REG_PSID     = COMMON_BASE + 0x0024, //PPP Session Identification
	REG_PMRU     = COMMON_BASE + 0x0026, //PPP Maximum Segment Size
	REG_UIPR     = COMMON_BASE + 0x0028, //Unreachable IP address in UDP mode
	REG_UPORT    = COMMON_BASE + 0x002C, //Unreachable Port in UDP mode
	REG_PHYCFGR  = COMMON_BASE + 0x002E, //PHY configuration register
	REG_VERSIONR = COMMON_BASE + 0x0039  //Chip version
};

// Mode register flags
enum {
	MR_RESET      = 0x80,	//If this bit is '1' internal registers will be initialized, it will then be cleared
	MR_WOL        = 0x20,   //Wake-on-line. If this bit is '1' then the magic packet will cause a WOL interrupt
	MR_PING_BLOCK = 0x10,   //If this bit is '1' there is no response to the ping request (ping is blocked)
	MR_PPPOE      = 0x08,   //This is set to connect to ADSL through PPPoE
	MR_FARP       = 0x02,   //This bit enables force ARP mode
};

// Interrupt register flags
enum {  
	IR_CONFLICT        = 0x80,     //an ARP request had the same IP address as the source IP
	IR_UDP_UNREACHABLE = 0x40,     //A UDP Packet was sent to an unreachable address
	IR_PPPOE_CLOSED    = 0x20,     //The PPPoE connection was closed
	IR_WAKE_ON_LAN     = 0x10      //Wake-on-LAN magic packet received
};
#define IR_Sn_INTERRUPT(N) (0x01 << N) //There was an interrupt on socket N

/*
**  Socket-specific registers for the four possible sockets
*/
#define REG_Sn_MR         (CH_BASE + 0x0000)  //Socket N Mode Register
#define REG_Sn_CR         (CH_BASE + 0x0001)  //Socket N Command Register
#define REG_Sn_IR         (CH_BASE + 0x0002)  //Socket N Interrupt Register
#define REG_Sn_SR         (CH_BASE + 0x0003)  //Socket N Status Register
#define REG_Sn_PORT       (CH_BASE + 0x0004)  //Socket N Source Port Register
#define REG_Sn_DHAR       (CH_BASE + 0x0006)  //Socket N Destination Hardware Address Register
#define REG_Sn_DIPR       (CH_BASE + 0x000C)  //Socket N Destination IP Address Register
#define REG_Sn_DPORT      (CH_BASE + 0x0010)  //Socket N Destination Port Register
#define REG_Sn_MSSR       (CH_BASE + 0x0012)  //Socket N Maximum Segment Size Register
#define REG_Sn_TOS        (CH_BASE + 0x0015)  //Socket N IP Type-Of-Service Register
#define REG_Sn_TTL        (CH_BASE + 0x0016)  //Socket N Time-To-Live Register
#define REG_Sn_RXBUF_SIZE (CH_BASE + 0x001E)  //Socket N Receive Buffer Size
#define REG_Sn_TXBUF_SIZE (CH_BASE + 0x001F)  //Socket N Transmit Buffer Size
#define REG_Sn_TX_FSR     (CH_BASE + 0x0020)  //Socket N TX Buffer Free Size Register
#define REG_Sn_TX_RD      (CH_BASE + 0x0022)  //Socket N TX Buffer Read Pointer Register
#define REG_Sn_TX_WR      (CH_BASE + 0x0024)  //Socket N TX Buffer Write Pointer Register
#define REG_Sn_RX_RSR     (CH_BASE + 0x0026)  //Socket N RX Buffer Received Size Register
#define REG_Sn_RX_RD      (CH_BASE + 0x0028)  //Socket N RX Buffer Read Pointer Register
#define REG_Sn_RX_WR      (CH_BASE + 0x002A)  //Socket N RX Buffer Write Pointer Register
#define REG_Sn_IMR        (CH_BASE + 0x002C)  //Socket N Interrupt Mask Register
#define REG_Sn_FRAG       (CH_BASE + 0x002D)  //Socket N Fragment offset
#define REG_Sn_KPALVTR    (CH_BASE + 0x002F)  //Socket N Keep alive timer

// Socket Mode register flags and values
enum {
	//flags
	Sn_MR_MULTICASTING   = 0x80,  //Enable multicasting for UDP
	Sn_MR_BCAST_BLOCKING = 0x40,  //Enable to block reception of UDP broadcast packets
	Sn_MR_NO_ACK_DELAY   = 0x20,  //Enabled undelayed ACK transmission for TCP
	Sn_MR_UCAST_BLOCKING = 0x10,  //Enable to block unicast reception in UDP multicast mode
	//values
	Sn_MR_CLOSE          = 0x00,  //Unused (closed) socket
	Sn_MR_TCP            = 0x01,  //TCP Mode
	Sn_MR_UDP            = 0x02,  //UDP Mode
//	Sn_MR_IPRAW          = 0x03,  //IP-RAW Mode (e.g. for ICMP or IGMP)
	Sn_MR_MACRAW         = 0x04,  //MAC-RAW
//	Sn_MR_PPPOE          = 0x05   //PPPoE Mode
};

// Socket Command register values
enum {
	Sn_CR_OPEN           = 0x01,  //Initialise or open socket
	Sn_CR_LISTEN         = 0x02,  //Wait for a connection request in TCP mode
	Sn_CR_CONNECT        = 0x04,  //Send connection request in TCP mode
	Sn_CR_DISCONNECT     = 0x08,  //Send closing request in TCP mode
	Sn_CR_CLOSE          = 0x10,  //Close socket
	Sn_CR_SEND           = 0x20,  //Send data between Sn_TX_RD and Sn_TX_WR
	Sn_CR_SEND_MAC       = 0x21,  //Send to destination hardware address Sn_DHAR in UDP Mode
	Sn_CR_SEND_KEEPALIVE = 0x22,  //Send a keepalive signal in TCP mode
	//TODO: The official drivers has a bunch of PPPoE commands here (not for w5100)
	Sn_CR_RECEIVE        = 0x40   //Receive data into the RX memory buffer
};

// Socket Interrupt register flags
enum {
	//TODO: The official drivers has a bunch of PPPoE flags here (not for w5100)
	Sn_IR_SEND_OK    = 0x10,  //Send operation is completed
	Sn_IR_TIMEOUT    = 0x08,  //Timeout occured during connection, termination or data transmission
	Sn_IR_RECEIVE    = 0x04,  //Data was received or data still remains after Sn_CR_RECEIVE
	Sn_IR_DISCONNECT = 0x02,  //Connection termination requested or finished
	Sn_IR_CONNECT    = 0x01   //Connection was established
};

//Socket Status register values
enum {
	Sn_SR_CLOSED       = 0x00,  //Sn_CR_CLOSE command was issued and timeout occured or connection terminated
	Sn_SR_INIT         = 0x13,  //Sn_MR is set as TCP and Sn_CR_OPEN was sent. To be followed by LISTEN or CONNECT
	Sn_SR_LISTEN       = 0x14,  //Sn_CR_LISTEN was sent and the socket is open in server mode
	Sn_SR_SYN_SENT     = 0x15,  //Sn_CR_CONNECT was issued
	Sn_SR_SYN_RECEIVED = 0x16,  //Connectin request received from remote peer
	Sn_SR_ESTABLISHED  = 0x17,  //Follows SYN_SENT or SYN_RECEIVED when the TCP connection is established
	Sn_SR_FIN_WAIT     = 0x18,  //Connection termination is begun
	Sn_SR_CLOSING      = 0x1A,  //Connection termination is underway
	Sn_SR_TIME_WAIT    = 0x1B,  //Waiting on timeout during termination
	Sn_SR_CLOSE_WAIT   = 0x1C,  //Connection termination request received from remote server. waiting for closure.
	Sn_SR_LAST_ACK     = 0x1D,  //I don't fucking know, I don't fucking care.
	Sn_SR_UDP          = 0x22,  //Sn_CR_OPEN was issued and Sn_MR was set to UDP
	Sn_SR_IPRAW        = 0x32,  //Sn_CR_OPEN was issued and Sn_MR was set to IPRAW
	Sn_SR_MACRAW       = 0x42,  //Sn_CR_OPEN was issued and Sn_MR was set to MACRAW
	Sn_SR_PPPOE        = 0x5F   //S0_CR_OPEN was issued and S0_MR was set to PPPOE
};
//Socket Protocol Register values
//From http://www.IANA.org/assignments/protocol-numbers
enum {
	Sn_PROTO_IP        = 0,   //Dummy for IP
	Sn_PROTO_ICMP      = 1,   //Control message protocol
	Sn_PROTO_IGMP      = 2,   //Internet group management protocol
	Sn_PROTO_TCP       = 6,   //TCP
	Sn_PROTO_UDP       = 17,  //UDP
	Sn_PROTO_RAW       = 255, //Raw IP packet
};

