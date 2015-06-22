#include <stdint.h>
#include "wiznet_regs_defs.h"

/*
** Global register access functions
**
************************************/


/* This function writes the mode register (MR)
*/
static inline void wiznetSetMode(uint8_t val) {
	wiznetRegWriteByte(-1, REG_MR, val);
}

/* This function returns the mode register (MR)
*/
static inline uint8_t wiznetGetMode(void) {
	return wiznetRegReadByte(-1, REG_MR);
}

/* This function writes the gateway IP
*/
static inline void wiznetSetGatewayIP(uint8_t* gwIP) {
	wiznetRegWriteIP(-1, REG_GAR, gwIP);
}

/* This function reads the gateway IP
*/
static inline void wiznetGetGatewayIP(uint8_t* gwIP) {
	wiznetRegReadIP(-1, REG_GAR, gwIP);
}

/* This function writes the subnet mask register
*/
static inline void wiznetSetSubnetMask(uint8_t* snIP) {
	wiznetRegWriteIP(-1, REG_SUBR, snIP);
}

/* This function reads the subnet mask register
*/
static inline void wiznetGetSubnetMask(uint8_t* snIP) {
	wiznetRegReadIP(-1, REG_SUBR, snIP);
}

/* This function sets up Source ethernet MAC address.
** This function should usually be called with the globally set MAC
*/
static inline void wiznetSetSourceMAC(uint8_t* mac) {
	wiznetRegWriteMAC(-1, REG_SHAR, mac);
}

/* This function reads the Source ethernet MAC address into a buffer.
** This function should usually be called with the globally set MAC
*/
static inline void wiznetGetSourceMAC(uint8_t* mac) {
	wiznetRegReadMAC(-1, REG_SHAR, mac);
}

/* This function writes the source IP register
*/
static inline void wiznetSetSourceIP(uint8_t* sIP) {
	wiznetRegWriteIP(-1, REG_SIPR, sIP);
}

/* This function reads the source IP register
*/
static inline void wiznetGetSourceIP(uint8_t* sIP) {
	wiznetRegReadIP(-1, REG_SIPR, sIP);
}

/* This functions writes the Interrupt Low Level Timer, AKA Interrupt asser wait time
*/
static inline void wiznetSetInterruptAssertWaitTime(uint16_t ittl) {
	wiznetRegWriteWord(-1, REG_ILLT, ittl);
}

/* This function reads the Interrupt Low Level Timer, AKA Interrupt asser wait time
*/
static inline uint16_t wiznetGetInterruptAssertWaitTime(void) {
	return wiznetRegReadWord(-1, REG_ILLT);
}

/* This function writes to the interrupt register
*/
static inline void wiznetSetInterrupts(uint8_t isr) {
	wiznetRegWriteByte(-1, REG_IR, isr);
}

/* This function reads from the interrupt register
*/
static inline uint8_t wiznetGetInterrupts(void) {
	return wiznetRegReadByte(-1, REG_IR);
}

/* This function sets the interrupt mask register to enable/disable the interrupts
** 1 - interrupt enabled
*/
static inline void wiznetSetInterruptMask(uint8_t mask) {
	wiznetRegWriteByte(-1, REG_IMR,mask);
}

/* This function reads from the interrupt mask register
*/
static inline uint8_t wiznetGetInteruptMask(void) {
	return wiznetRegReadByte(-1, REG_IMR);
}

/* This function writes to the socket interrupt register
*/
static inline void wiznetSetInterruptsOnSockets(uint8_t isr) {
	wiznetRegWriteByte(-1, REG_SIR, isr);
}

/* This function reads from the socket interrupt register
*/
static inline uint8_t wiznetGetInterruptsOnSockets(void) {
	return wiznetRegReadByte(-1, REG_SIR);
}

/* This function sets the socket interrupt mask register to enable/disable the interrupts
** 1 - interrupt enabled
*/
static inline void wiznetSetInterruptsOnSocketsMask(uint8_t mask) {
	wiznetRegWriteByte(-1, REG_SIMR,mask);
}

/* This function reads from the socket interrupt mask register
*/
static inline uint8_t wiznetGetInterruptsOnSocketsMask(void) {
	return wiznetRegReadByte(-1, REG_SIMR);
}

/* This function writes to the retry time register
*/
static inline void wiznetSetRetryTime(uint16_t time) {
	wiznetRegWriteWord(-1, REG_RTR, time);
}

/* This function reads from the retry time register
*/
static inline uint16_t wiznetGetRetryTime(void) {
	return wiznetRegReadWord(-1, REG_RTR);
}

/* This function writes to the retry count register
*/
static inline void wiznetSetRetryCount(uint8_t count) {
	wiznetRegWriteByte(-1, REG_RCR, count);
}

/* This function reads from the retry count register
*/
static inline uint8_t wiznetGetRetryCount(void) {
	return wiznetRegReadByte(-1, REG_RCR);
}

//TODO: PPPoE stuff should go here but we'll certainly never use it...

/* This function reads from the UDP address-was-unreachable IP address
** There is no writing to this register
*/
static inline void wiznetGetUDPUnreachableIP(uint8_t* uIP) {
	wiznetRegReadIP(-1, REG_UIPR, uIP);
}

/* This function reads from the UDP address-was-unreachable port
** There is no writing to this register
*/
static inline uint16_t wiznetGetUDPUnreachablePort(void) {
	return wiznetRegReadWord(-1, REG_UPORT);
}

/* This function writes to the PHY configuration register
*/
static inline void wiznetSetPHYConfig(uint8_t config) {
	wiznetRegWriteByte(-1, REG_PHYCFGR, config);
}

/* This function reads from the PHY configuration register
*/
static inline uint8_t wiznetGetPHYConfig(void) {
	return wiznetRegReadByte(-1, REG_PHYCFGR);
}

/* This function reads the chip version
*/
static inline uint8_t wiznetGetChipVersion(void) {
	return wiznetRegReadByte(-1, REG_VERSIONR);
}


/*
** Socket-specific register access functions
**
*******************************************/

/* This function writes to the socket-specific mode register
*/
static inline void wiznetSetSocketMode(uint8_t socket, uint8_t mode) {
	wiznetRegWriteByte(socket, REG_Sn_MR, mode);
}

/* This function reads from the socket-specific mode register
*/
static inline uint8_t wiznetGetSocketMode(uint8_t socket) {
	return wiznetRegReadByte(socket, REG_Sn_MR);
}

/* This function write to the socket-specific command register
*/
static inline void wiznetSetSocketCommand(uint8_t socket, uint8_t command) {
	wiznetRegWriteByte(socket, REG_Sn_CR, command);
}

/* This function reads from the socket-specific command register
** The command register is cleared to 0 when a command has finished
*/
static inline uint8_t wiznetGetSocketCommand(uint8_t socket) {
	return wiznetRegReadByte(socket, REG_Sn_CR);
}

/* This function writea to the socket-specific interrupt register
*/
static inline void wiznetSetSocketInterrupt(uint8_t socket, uint8_t interrupt) {
	wiznetRegWriteByte(socket, REG_Sn_IR, interrupt);
}

/* This function reads from the socket-specific interrupt register
*/
static inline uint8_t wiznetGetSocketInterrupt(uint8_t socket) {
	return wiznetRegReadByte(socket, REG_Sn_IR);
}

/* This function reads from the socket-specific status register
** This function has no complementary write method
*/
static inline uint8_t wiznetGetSocketStatus(uint8_t socket) {
	return wiznetRegReadByte(socket, REG_Sn_SR);
}

/* This function writes the socket-specific source port
*/
static inline void wiznetSetSocketSourcePort(uint8_t socket, uint16_t port) {
	wiznetRegWriteWord(socket, REG_Sn_PORT, port);
}

/* This function reads the socket-specific source port
*/
static inline uint16_t wiznetGetSocketSourcePort(uint8_t socket) {
	return wiznetRegReadWord(socket, REG_Sn_PORT);
}

/* This function writes the destination MAC address
*/
static inline void wiznetSetSocketDestMAC(uint8_t socket, uint8_t* mac) {
	wiznetRegWriteMAC(socket, REG_Sn_DHAR, mac);
}

/* This function reads the socket-specific destination MAC address
*/
static inline void wiznetGetSocketDestMAC(uint8_t socket, uint8_t* mac) {
	wiznetRegReadMAC(socket, REG_Sn_DHAR, mac);
}

/* This function writes the socket-specific destination IP Address
*/
static inline void wiznetSetSocketDestIP(uint8_t socket, uint8_t* dIP) {
	wiznetRegWriteIP(socket, REG_Sn_DIPR, dIP);
}

/* This function reads from the socket-specific destination IP Address
*/
static inline void wiznetGetSocketDestIP(uint8_t socket, uint8_t* dIP) {
	wiznetRegReadIP(socket, REG_Sn_DIPR, dIP);
}

/* This function writes to the socket-specific destination port register
*/
static inline void wiznetSetSocketDestPort(uint8_t socket, uint16_t port) {
	wiznetRegWriteWord(socket, REG_Sn_DPORT, port);
}

/* This function reads from the socket-specific destination port register
*/
static inline uint16_t wiznetGetSocketDestPort(uint8_t socket) {
	return wiznetRegReadWord(socket, REG_Sn_DPORT);
}

/* This function writes to the socket-specific maximum segment size register
*/
static inline void wiznetSetSocketMaximumSegmentSize(uint8_t socket, uint8_t mssr) {
	wiznetRegWriteByte(socket, REG_Sn_MSSR, mssr);
}

/* This function reads from the socket-specific maximum segment size register
*/
static inline uint8_t wiznetGetSocketMaximumSegmentSize(uint8_t socket) {
	return wiznetRegReadByte(socket, REG_Sn_MSSR);
}

/* This function writes to the socket-specific Type-of-Service register
*/
static inline void wiznetSetSocketTypeOfService(uint8_t socket, uint8_t tos) {
	wiznetRegWriteByte(socket, REG_Sn_TOS, tos);
}

/* This function reads from the socket-specific Type-of-Service register
*/
static inline uint8_t wiznetGetSocketTypeOfService(uint8_t socket) {
	return wiznetRegReadByte(socket, REG_Sn_TOS);
}

/* This function writes to the socket-specific Time-to-Live register
*/
static inline void wiznetSetSocketTimeToLive(uint8_t socket, uint8_t ttl) {
	wiznetRegWriteByte(socket, REG_Sn_TTL, ttl);
}

/* This function reads from the socket-specific Time-to-Live register
*/
static inline uint8_t wiznetGetSocketTimeToLive(uint8_t socket) {
	return wiznetRegReadByte(socket, REG_Sn_TTL);
}

/* This function writes to the socket-specific reception buffer size register
*/
static inline void wiznetSetSocketRXBufferSize(uint8_t socket, uint8_t size) {
	wiznetRegWriteByte(socket, REG_Sn_RXBUF_SIZE, size);
}

/* This function reads from the socket-specific reception buffer size register
*/
static inline uint8_t wiznetGetSocketRXBufferSize(uint8_t socket) {
	return wiznetRegReadByte(socket, REG_Sn_RXBUF_SIZE);
}

/* This function writes to the socket-specific transmission buffer size register
*/
static inline void wiznetSetSocketTXBufferSize(uint8_t socket, uint8_t size) {
	wiznetRegWriteByte(socket, REG_Sn_TXBUF_SIZE, size);
}

/* This function reads from the socket-specific transmission buffer size register
*/
static inline uint8_t wiznetGetSocketTXBufferSize(uint8_t socket) {
	return wiznetRegReadByte(socket, REG_Sn_TXBUF_SIZE);
}

/* This function reads from the socket-specific Transmission buffer free size register
** There is no write function for this register
*/
static inline uint16_t wiznetGetSocketTXFreeSize(uint8_t socket) {
	return wiznetRegReadWord(socket, REG_Sn_TX_FSR);
}

/* This function reads from the socket-specific Transmission buffer read-pointer register
** There is no write function for this register
*/
static inline uint16_t wiznetGetSocketTXReadPointer(uint8_t socket) {
	return wiznetRegReadWord(socket, REG_Sn_TX_RD);
}

/* This function writes to the socket-specific Transmission write pointer register
*/
static inline void wiznetSetSocketTXWritePointer(uint8_t socket, uint16_t addr) {
	wiznetRegWriteWord(socket, REG_Sn_TX_WR, addr);
}

/* This function reads from the socket-specific Transmission write pointer register
*/
static inline uint16_t wiznetGetSocketTXWritePointer(uint8_t socket) {
	return wiznetRegReadWord(socket, REG_Sn_TX_WR);
}

/* This function reads from the socket-specific Reception buffer received size register
** There is no write function for this register
*/
static inline uint16_t wiznetGetSocketRXReceivedSize(uint8_t socket) {
	return wiznetRegReadWord(socket, REG_Sn_RX_RSR);
}

/* This function reads from the socket-specific reception buffer read-pointer register
*/
static inline uint16_t wiznetGetSocketRXReadPointer(uint8_t socket) {
	return wiznetRegReadWord(socket, REG_Sn_RX_RD);
}
/* This function writes to the socket-specific reception buffer read-pointer register
*/
static inline void wiznetSetSocketRXReadPointer(uint8_t socket, uint16_t addr) {
	wiznetRegWriteWord(socket, REG_Sn_RX_RD, addr);
}

/* This function writes to the socket-specific Reception write pointer register
*/
static inline void wiznetSetSocketRXWritePointer(uint8_t socket, uint16_t addr) {
	wiznetRegWriteWord(socket, REG_Sn_RX_WR, addr);
}

/* This function reads from the socket-specific Reception write pointer register
*/
static inline uint16_t wiznetGetSocketRXWritePointer(uint8_t socket) {
	return wiznetRegReadWord(socket, REG_Sn_RX_WR);
}

/* This function writea to the socket-specific interrupt mask register
*/
static inline void wiznetSetSocketInterruptMask(uint8_t socket, uint8_t mask) {
	wiznetRegWriteByte(socket, REG_Sn_IMR, mask);
}

/* This function reads from the socket-specific interrupt mask register
*/
static inline uint8_t wiznetGetSocketInterruptMask(uint8_t socket) {
	return wiznetRegReadByte(socket, REG_Sn_IMR);
}

/* This function writea to the socket-specific fragment offset register
*/
static inline void wiznetSetFragmentOffset(uint8_t socket, uint16_t frag) {
	wiznetRegWriteWord(socket, REG_Sn_FRAG, frag);
}

/* This function reads from the socket-specific fragment offset register
*/
static inline uint16_t wiznetGetFragmentOffset(uint8_t socket) {
	return wiznetRegReadWord(socket, REG_Sn_FRAG);
}

/* This function writea to the socket-specific keep-alive time register 
*/
static inline void wiznetSetSocketKeepAliveTime(uint8_t socket, uint8_t time) {
	wiznetRegWriteByte(socket, REG_Sn_KPALVTR, time);
}

/* This function reads from the socket-specific keep-alive time register
*/
static inline uint8_t wiznetGetSocketKeepAliveTime(uint8_t socket) {
	return wiznetRegReadByte(socket, REG_Sn_KPALVTR);
}

