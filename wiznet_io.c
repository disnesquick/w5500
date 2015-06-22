#include <stdint.h>
#include "io_assignment.h"
#include "util.h"
#include "wiznet_arch.h"
#include "wiznet.h"
#include "wiznet_io.h"
#include "wiznet_regs.h"

#define NULL ((void*)0)
int wiznetIOBegin(int socket, uint16_t address, char readWrite, char type) {
	uint8_t bm = (readWrite == 'w' ? 1 : 0) << 2;	   //WARNING: readWrite is not checked to be 'r' or 'w' only
	if (socket != -1) {
		bm |= (socket << 5);
		switch (type) {
		case 'x':
			bm |= 1 << 3;
			break;
		case 't':
			bm |= 2 << 3;
			break;	
		case 'r':
			bm |= 3 << 3;
			break;
		default:
			return -1;
		}
	}
	wiznetSPIChipEnable();
	wiznetSPITransceiveByte(BYTE1(address));   // address phase H
	wiznetSPITransceiveByte(BYTE0(address));   // address phase L
	wiznetSPITransceiveByte(bm);               // control phase
	return 0;
}

/* This function writes a single byte (8-bit) to the wiznet.
*/
void wiznetRegWriteByte(int socket, uint16_t addr, uint8_t byte) {
	wiznetIOBegin(socket, addr, 'w', 'x');
	wiznetSPITransceiveByte(byte);
	wiznetIOFinish();
}

uint8_t wiznetRegReadByte(int socket, uint16_t addr) {
	uint8_t ret;
	wiznetIOBegin(socket, addr, 'r', 'x');
	ret = wiznetSPITransceiveByte(0xBF);
	wiznetIOFinish();
	return ret;
}

/* This function writes a single word (16-bit) to the wiznet.
** The wiznet uses big-endian format for words.
*/
void wiznetRegWriteWord(int socket, uint16_t addr, uint16_t word) {
	wiznetIOBegin(socket, addr, 'w', 'x');
	wiznetSPITransceiveByte(BYTE1(word));
	wiznetSPITransceiveByte(BYTE0(word));
	wiznetIOFinish();
}

/* This function reads a single word (16-bit) from the wiznet.
** The wiznet uses big-endian format for words.
*/
uint16_t wiznetRegReadWord(int socket, uint16_t addr) {
	uint16_t ret;
	wiznetIOBegin(socket, addr, 'r', 'x');
	ret = (uint16_t)wiznetSPITransceiveByte(0xBE);
	ret = (ret << 8) + (uint16_t)wiznetSPITransceiveByte(0xEF);
	wiznetIOFinish();
	return ret;
}

/* This function is used for all IP-related writes
** reg  - wiznet register to write to
** addr - buffer the read the IP address from
*/
void wiznetRegWriteIP(int socket, uint16_t addr, uint8_t* ip) {
	wiznetIOBegin(socket, addr, 'w', 'x');
	wiznetSPITransceiveByte(ip[0]);
	wiznetSPITransceiveByte(ip[1]);
	wiznetSPITransceiveByte(ip[2]);
	wiznetSPITransceiveByte(ip[3]);
	wiznetIOFinish();
}

/* This function is used for all IP-related reads
** reg  - wiznet register address to read from
** addr - buffer to store the IP address
*/
void wiznetRegReadIP(int socket, uint16_t addr, uint8_t* ip) {
	wiznetIOBegin(socket, addr, 'r', 'x');
	ip[0] = wiznetSPITransceiveByte(0xDE);
	ip[1] = wiznetSPITransceiveByte(0xAD);
	ip[2] = wiznetSPITransceiveByte(0xBE);
	ip[3] = wiznetSPITransceiveByte(0xEF);
	wiznetIOFinish();
}

/* This function writes an ethernet MAC address.
*/
void wiznetRegWriteMAC(int socket, uint16_t addr, uint8_t* mac) {
	wiznetIOBegin(socket, addr, 'w', 'x');
	wiznetSPITransceiveByte(mac[0]);
	wiznetSPITransceiveByte(mac[1]);
	wiznetSPITransceiveByte(mac[2]);
	wiznetSPITransceiveByte(mac[3]);
	wiznetSPITransceiveByte(mac[4]);
	wiznetSPITransceiveByte(mac[5]);
	wiznetIOFinish();
}

/* This function reads an ethernet MAC address into a buffer.
*/
void wiznetRegReadMAC(int socket, uint16_t addr, uint8_t* mac) {
	wiznetIOBegin(socket, addr, 'r', 'x');
	mac[0] = wiznetSPITransceiveByte(0xC0);
	mac[1] = wiznetSPITransceiveByte(0xFF);
	mac[2] = wiznetSPITransceiveByte(0xEE);
	mac[3] = wiznetSPITransceiveByte(0xBA);
	mac[4] = wiznetSPITransceiveByte(0xBE);
	mac[5] = wiznetSPITransceiveByte(0x69);
	wiznetIOFinish();
}

/* Run a socket command on a particular socket and waits for completion.
** 
** socket - the socket number (0-3)
** val    - the command
*/
void wiznetSocketCommand(int socket, uint8_t command) {
	wiznetSetSocketCommand(socket, command);
	while(wiznetGetSocketCommand(socket));
}

