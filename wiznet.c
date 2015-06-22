#include <stdint.h>
#include "util.h"
#include "io_assignment.h"
#include "wiznet_arch.h"
#include "wiznet.h"
#include "wiznet_io.h"
#include "wiznet_regs.h"

#define NULL ((void*)0)

/* Globals for keeping track of the wiznet transmit and receive buffers
*/
uint16_t wiznetTXMemSize[WIZNET_MAX_SOCKETS];
uint16_t wiznetTXMemBase[WIZNET_MAX_SOCKETS];
uint16_t wiznetRXMemSize[WIZNET_MAX_SOCKETS];
uint16_t wiznetRXMemBase[WIZNET_MAX_SOCKETS];

/* Globals for the buffer streaming protocol
*/
uint16_t wiznetBufferWriteCur ,wiznetBufferReadCur;
int wiznetBufferWriteSocket, wiznetBufferReadSocket;



/* This function resets the wiznet chip and waits until it is stable
*/
void wiznetReset(void) {
	wiznetSetMode(MR_RESET);
	while(wiznetGetMode()&MR_RESET);
}

/* This function initializes the wiznet chip (Mode, Memory and Interrupts)
**
** bufSize - an array of sizes, in kilobytes, for each socket's in/output
**           buffers.
*/
void wiznetInit(uint8_t bufSize[]) {
	//Set-up the buffer and interrupt time
	wiznetInitBufferSizes(bufSize, bufSize);
	wiznetSetInterruptAssertWaitTime(4);

	// Default values for the retry counts and times
	wiznetSetRetryTime(0x3E8);
	wiznetSetRetryCount(5);
#ifdef WIZNET_INTERRUPTS_ENABLED
	wiznetSetInterruptMask(IR_CONFLICT);
#endif 
}

/* This function sets the allocation of buffer memory in the wiznet.
**
** rx - array of sizes, in kilobytes, for the read buffers
** tx - array of sizes, in kilobytes, for the writes buffers
** TODO Proper checking that memory request is rational
*/
void wiznetInitBufferSizes(uint8_t* rx, uint8_t* tx) {
	int i;

	// Initialize pointers to the current positions
	// of the buffers.
	// Only one read and one write buffer can be active at
	// a time.
	wiznetBufferWriteCur = 0;
	wiznetBufferWriteSocket = -1;
	wiznetBufferReadCur = 0;
	wiznetBufferReadSocket = -1;

	// Send the set-up commands for each of the buffer
	// sizes.
	for (i = 0 ; i < WIZNET_MAX_SOCKETS; i++) {
		wiznetSetSocketRXBufferSize(i, rx[i]);
		wiznetSetSocketTXBufferSize(i, tx[i]);
	}
}

/* This function enables interrupts on a given socket
**
** socket - The socket number on which to enable
**          interrupts.
*/
void wiznetSocketEnableInterrupts(uint8_t socket) {
	wiznetSetInterruptsOnSocketsMask(wiznetGetInterruptsOnSocketsMask() | (1<<socket));
}

/* This function disables interrupts on a given socket
**
** socket - The socket number on which to enable
**          interrupts.
*/
void wiznetSocketDisableInterrupts(uint8_t socket) {
	wiznetSetInterruptsOnSocketsMask(wiznetGetInterruptsOnSocketsMask() & ~(1<<socket));
}

/* This function checks which sockets are displaying that they
** have an interrupt waiting to be serviced and returns
** the appropriate bitmask to the calling function
**
** returns - 8-bit bitmask showing 1 bit per socket
*/
uint8_t wiznetGetSocketInts(void) {
	return wiznetGetInterruptsOnSockets();
}

/* This function clears the data-received interrupt on
** the socket. 
**
** socket - the socket to clear the interrupt on
*/
void wiznetClearSocketRecvInt(uint8_t socket) {
	wiznetSetSocketInterrupt(socket, Sn_IR_RECEIVE);
}

/* This function gets the interrupt register for
** a specific socket.
**
** socket - socket number
**
** returns - 8 bit interrupt mask
*/
uint8_t wiznetGetSocketInt(uint8_t socket) {
	return wiznetGetSocketInterrupt(socket);
}

/* This function retrieves the global interrupt register
** from the wiznet.
**
** returns - 8-bit bitmask showing 1 bit per interrupt
*/
uint8_t wiznetGetDeviceInts(void){
	return wiznetGetInterrupts();
}


void wiznetClearDeviceInts(void){
	wiznetSetInterrupts(wiznetGetInterrupts());
}

/* This function sets up the IP communication layer for the wiznet device
**
** gip - Gateway IP address
** snm - Subnet mask IP address
** sip - Source IP address
*/
void wiznetConfigureIPLayer(uint8_t* gip, uint8_t* snm, uint8_t* sip) {
	wiznetSetGatewayIP(gip);
	wiznetSetSubnetMask(snm);
	wiznetSetSourceIP(sip);
}

/* This function retrieves the currently set MAC address
** from the device
**
** MAC - pointer to destination where MAC will be stored
*/
void wiznetGetDeviceMAC(uint8_t* MAC) {
	wiznetGetSourceMAC(MAC);
}

/* This function sets the current MAC address on the wiznet
** chip.
**
** MAC - pointer to source address where MAC is to be retrieved
**       from.
*/
void wiznetSetDeviceMAC(uint8_t* MAC) {
	wiznetSetSourceMAC(MAC);
}


/* This function initialize the channel in a particular mode, sets the port and opens the socket.
**
** socket   - the socket number (0-7)
** protocol - the socket protocol
** port     - the source port for the socket
** flag     - the option for the socket
** 
** returns 1 for sucess else 0.
*/
int wiznetOpenSocket(uint8_t socket, uint8_t protocol, uint16_t port, uint8_t flags) {
	uint8_t status;

	// Convert the provided socket protocol to the required flag on the wiznet
	switch (protocol) {
	case SOCK_UDP:
		protocol = Sn_MR_UDP;
		break;
	case SOCK_TCP:
		protocol = Sn_MR_TCP;
		break;
	default:
		break;
	}

	if ((protocol == Sn_MR_TCP) || (protocol == Sn_MR_UDP) || (protocol == Sn_MR_MACRAW)) {
		// Close socket if it's open
		if (wiznetGetSocketStatus(socket)!=Sn_SR_CLOSED)
			wiznetCloseSocket(socket);

		// Set socket mode
		wiznetSetSocketMode(socket, protocol | flags);

		// Set port number
		wiznetSetSocketSourcePort(socket, port);

		// Set-up interrupts on the socket to be opened
		// Interrupt on: Receive new data, receive disconnection signal, TCP retransmission
		//               timeout.
		wiznetSetSocketInterruptMask(socket, Sn_IR_RECEIVE | Sn_IR_DISCONNECT | Sn_IR_TIMEOUT);
		
		//process socket initialization
		wiznetSocketCommand(socket, Sn_CR_OPEN);
		while(wiznetGetSocketStatus(socket)==Sn_SR_CLOSED)
			if (wiznetGetSocketInterrupt(socket) & Sn_IR_TIMEOUT) {
				// If a socket takes too long to open, then a timeout error occurs
				wiznetCloseSocket(socket);
				return WIZNET_ERROR_SOCKET_TIMEOUT;
			}

		// Wait for the SOCK_SYNSENT to clear 
		while (wiznetGetSocketStatus(socket) == Sn_SR_SYN_SENT);

		// Check that the socket was succesfully set-up and is showing the correct
		// status code, as sent over.
		status = wiznetGetSocketStatus(socket);
		switch(protocol) {
		case Sn_MR_UDP:
			if (status != Sn_SR_UDP)
				return WIZNET_ERROR_SOCKET_OPEN;
			break;
		case Sn_MR_TCP:
			if (status != Sn_SR_INIT)
				return WIZNET_ERROR_SOCKET_OPEN;
			break;
		case Sn_MR_MACRAW:
			if (status != Sn_SR_MACRAW)
				return WIZNET_ERROR_SOCKET_OPEN;
		}
#ifdef WIZNET_INTERRUPTS_ENABLED
		wiznetSocketEnableInterrupts(socket);
#endif
		return WIZNET_SUCCESS;
	} else 
		return WIZNET_ERROR_UNKNOWN_PROTOCOL;
}

/* This function connects a TCP socket to the desired DIPR and DPORT
**
** socket - the socket number (0-7)
** destIP - the destination IP to connect to
** destPort - the destination port to connect to
**
** returns - WIZNET_SUCCESS if succesful
**           WIZNET_ERROR_SOCKET_TIMEOUT if there is a timeout on connection attempt
**           WIZNET_ERROR_SOCKET_OPEN if the socket failed for some other reason
*/
int wiznetConnectSocket(uint8_t socket, uint8_t* destIP, uint16_t destPort) {
	//Check that the socket is opened with TCP mode
	if (!(wiznetGetSocketStatus(socket) & Sn_SR_INIT))
		return WIZNET_ERROR_SOCKET_NOT_READY;

#ifdef WIZNET_INTERRUPTS_ENABLED
	wiznetSocketDisableInterrupts(socket);
#endif
	// Set the destination end-point address and issue the connect command
	// to initialize connection to the target.
	wiznetSetSocketDestIP(socket, destIP);
	wiznetSetSocketDestPort(socket, destPort);
	wiznetSocketCommand(socket, Sn_CR_CONNECT);

	// Wait for connection to succeed. If the destination is unreachable
	// then a timeout error is returned
	while (wiznetGetSocketStatus(socket)==Sn_SR_INIT)
		if (wiznetGetSocketInterrupt(socket) & Sn_IR_TIMEOUT) {
			wiznetCloseSocket(socket);
			return WIZNET_ERROR_SOCKET_TIMEOUT;
		}

	// Wait for the SOCK_SYNSENT to clear
	while (wiznetGetSocketStatus(socket) == Sn_SR_SYN_SENT);

	// Final check that the socket connected succesfully and a stream link
	// is now fully established.
	if (wiznetGetSocketStatus(socket)!=Sn_SR_ESTABLISHED) {
			wiznetCloseSocket(socket);
			return WIZNET_ERROR_SOCKET_OPEN;
		}
	
#ifdef WIZNET_INTERRUPTS_ENABLED
	wiznetSocketEnableInterrupts(socket);
#endif
	return WIZNET_SUCCESS;
}

/* This function sets a TCP socket to listen mode.
**
** socket - the socket number (0-7)
*/

int wiznetListenOnSocket(uint8_t socket) {

	// Make sure that the socket has been opened and
	// is waiting for further action.
	if (wiznetGetSocketStatus(socket) != Sn_SR_INIT)
		return WIZNET_ERROR_SOCKET_NOT_READY;

#ifdef WIZNET_INTERRUPTS_ENABLED
	wiznetSocketDisableInterrupts(socket);
#endif

	// Set the socket to listen mode
	wiznetSocketCommand(socket, Sn_CR_LISTEN);

	// Wait for socket to start listening
	while (wiznetGetSocketStatus(socket) == Sn_SR_INIT);

	// Final check that the socket started listening succesfully.
	if (wiznetGetSocketStatus(socket) != Sn_SR_LISTEN) {
		wiznetCloseSocket(socket);
		return WIZNET_ERROR_SOCKET_OPEN;
	}

#ifdef WIZNET_INTERRUPTS_ENABLED
	wiznetSocketEnableInterrupts(socket);
#endif
	return WIZNET_SUCCESS;
}

/* This function closes a socket. It does not check if that socket is actually open.
**
** socket - The socket number to close
*/
void wiznetCloseSocket(uint8_t socket) {
	wiznetSocketCommand(socket, Sn_CR_CLOSE);

	// Interrupts on the closed socket will no longer be needed so shut them
	// off.
	wiznetSetSocketInterruptMask(socket, 0);
	wiznetSetSocketInterrupt(socket, 0x00);
#ifdef WIZNET_INTERRUPTS_ENABLED
	wiznetSocketDisableInterrupts(socket);
#endif

}

/* This function blocks until data is present on one of the interfaces
** This shouldn't really be used, because the wiznet is designed to function
** on an interrupt-basis. 
*/
void wiznetWaitForData(void) {
	int i;
	while (1)
		for (i=0;i<WIZNET_MAX_SOCKETS; i++)
			if (wiznetRecvPeek(i) > 0)
				return;
}

/* This function is used to transfer some data into the current buffer
**
** buf    - byte array with the data to be sent
** length - number of bytes to send
**
** TODO: Check that a buffer is actually set-up.
** TODO: Add in a boundary check for the back of the circular buffer
*/
void wiznetSendData(const uint8_t* buf, uint16_t length) {
	wiznetIOBegin(wiznetBufferWriteSocket, wiznetBufferWriteCur, 'w', 't');
	while (length--) {
		wiznetIOTransceive(*buf++);
		wiznetBufferWriteCur++;
	}
	wiznetIOFinish();
}

/* This function is used to transfer some data into the current buffer
** it will be SLIP encoded before transmission.
**
** buf    - byte array with the data to be sent
** length - number of bytes to send
**
** TODO: Check that a buffer is actually set-up.
** TODO: Add in a boundary check for the back of the circular buffer
*/
void wiznetSendSLIPData(const uint8_t* buf, uint16_t length) {
	uint8_t tmp;
	wiznetIOBegin(wiznetBufferWriteSocket, wiznetBufferWriteCur, 'w', 't');
	while (length--) {
		tmp = *buf++;
		if (tmp == 0xC0) {
			wiznetIOTransceive(0xDB);
			wiznetBufferWriteCur++;
			wiznetIOTransceive(0xDC);
		} else if (tmp == 0xDB) {
			wiznetIOTransceive(0xDB);
			wiznetBufferWriteCur++;
			wiznetIOTransceive(0xDD);
		} else 
			wiznetIOTransceive(tmp);
		wiznetBufferWriteCur++;
	}
	wiznetIOFinish();
}

/* This function forms the first half of a transactional send, on a UDP socket
** whereby the the send is started, data is written to the buffer and then the
** send is commited once all data has been transfered to the buffer with
** wiznetSendToCommit
**
** socket   - the socket number on which to send
** destIP   - the IP address to send to
** destPort - the destination port to send to
*/

int wiznetSendToBegin(uint8_t socket, uint8_t* destIP, uint16_t destPort) {
	//Buffer is already in use, finish the other read/write first!
	if (wiznetBufferWriteSocket > -1)
		return WIZNET_ERROR_SEND_COLLISION;
	wiznetSetSocketDestIP(socket, destIP);
	wiznetSetSocketDestPort(socket, destPort);

	wiznetBufferWriteCur = wiznetGetSocketTXWritePointer(socket);
	wiznetBufferWriteSocket = socket;
	return WIZNET_SUCCESS;
}

/* This function forms the first half of a transactional send, on a TCP socket
** whereby the the send is started, data is written to the buffer and then the
** send is commited once all data has been transfered to the buffer with
** wiznetSendToCommit.
**
** socket   - the socket number on which to send
** destIP   - the IP address to send to
** destPort - the destination port to send to
** 
** returns - WIZNET_SUCCESS if succesful
**         - WIZNET_ERROR_SEND_COLLISION if another socket is undergoing a write
*/
int wiznetSendBegin(uint8_t socket) {
	//Buffer is already in use, finish the other read/write first!
	if (wiznetBufferWriteSocket > -1)
		return WIZNET_ERROR_SEND_COLLISION;

	wiznetBufferWriteCur = wiznetGetSocketTXWritePointer(socket);
	wiznetBufferWriteSocket = socket;
	return WIZNET_SUCCESS;
}

/* This function forms the first half of a transactional send, in SLIP mode.
** otherwise, it is identical to wiznetSendBegin. Note that no detection is
** done on whether a socket is in SLIP mode or not and proper operation
** relies on the programmer not being a dumbfuck.
**
** 
** socket   - the socket number on which to send
** destIP   - the IP address to send to
** destPort - the destination port to send to
*/
int wiznetSendBeginSLIP(uint8_t socket) {
	int ret;
	uint8_t tmp = 0xC0;
	if ((ret = wiznetSendBegin(socket)) != WIZNET_SUCCESS)
		return ret;
	wiznetSendData(&tmp, sizeof(tmp));
	return WIZNET_SUCCESS;
}	

/* This function is used to end the writing to a transmission buffer and send the data out
** it forms the second half of a SendToBegin/SendToCommit transaction pair
** This function re-enables interrupts
**
** returns - WIZNET_SUCCESS if succesful, error code otherwise
*/
int wiznetSendToCommit(void) {
	//No transaction in progress: Failure!
	if (wiznetBufferWriteSocket == -1)
		return WIZNET_ERROR_NOT_SENDING;

	wiznetSetSocketTXWritePointer(wiznetBufferWriteSocket, wiznetBufferWriteCur);
	wiznetSocketCommand(wiznetBufferWriteSocket, Sn_CR_SEND);

	while (!(wiznetGetSocketInterrupt(wiznetBufferWriteSocket) & Sn_IR_SEND_OK)) {
		if (wiznetGetSocketInterrupt(wiznetBufferWriteSocket) & Sn_IR_TIMEOUT) {
			wiznetSetSocketInterrupt(wiznetBufferWriteSocket, (Sn_IR_SEND_OK | Sn_IR_TIMEOUT));
			goto fail;
		}
	}
	wiznetSetSocketInterrupt(wiznetBufferWriteSocket, Sn_IR_SEND_OK);
	wiznetBufferWriteSocket = -1;
	return WIZNET_SUCCESS;
fail:
	wiznetBufferWriteSocket = -1;
	return WIZNET_ERROR_SEND_DATA;
	
}

/* This function is used to end the writing to a transmission buffer and send the data out
** it forms the second half of a SendBegin/SendCommit transaction pair
** This function re-enables interrupts
** This function is identical to wiznetSendToCommit and is included as a convienience.
**
** returns - WIZNET_SUCCESS if succesful, error code otherwise
*/
int wiznetSendCommit(void) {
	return wiznetSendToCommit();
}

/* This function is used to end the writing to a transmission buffer and send the data out
** it forms the second half of a SendBegin/SendCommit transaction pair
** This function re-enables interrupts
** This function is identical to wiznetSendToCommit and is included as a convienience.
**
** returns - WIZNET_SUCCESS if succesful, error code otherwise
*/
int wiznetSendCommitSLIP(void) {
	uint8_t tmp = 0xC0;
	wiznetSendData(&tmp, sizeof(tmp));
	return wiznetSendToCommit();
}

/* This function is used to abandon the writing to a transmission buffer
** without sending the data out
** This function re-enables interrupts
*/
int wiznetSendToAbandon(void) {
	wiznetBufferWriteSocket = -1;
	return WIZNET_SUCCESS;
}

/* This function is used to abandon the writing to a transmission buffer
** without sending the data out
** This function re-enables interrupts
*/
int wiznetSendAbandon(void) {
	return wiznetSendToAbandon();
}

/* This function gets the current position of the buffer so that it can be restored
** at a later time.
** This function takes no arguments
*/
uint16_t wiznetGetBufferWritePosition(void) {
	return wiznetBufferWriteCur;
}

/* This function sets the current position of the buffer as has been previously
** read
**
** pos - position from wiznetGetBufferPosition
*/
void wiznetSetBufferWritePosition(uint16_t pos) {
	wiznetBufferWriteCur = pos;
}

/* This function alters the current position of the buffer by a relative move
** amount (equivalent to fseek(SEEK_CUR))
**
** pos - amount by which to change the buffer position
*/
void wiznetSetRelBufferWritePosition(int16_t change) {
	wiznetBufferWriteCur += change;
}

/* This function is used to initialize reading from the reception buffer
** This function will disable interrupts until the reading is finished
**
** socket - socket whose reception buffer should be read from
**
** returns - WIZNET_SUCCESS if succesful
**         - WIZNET_ERROR_RECV_COLLISION if a recv was already in progress
*/
int wiznetRecvBegin(uint8_t socket) {
	//Buffer is already in use, finish the other read/write first!
	if (wiznetBufferReadSocket > -1)
		return WIZNET_ERROR_RECV_COLLISION;
	wiznetBufferReadCur = wiznetGetSocketRXReadPointer(socket);
	wiznetBufferReadSocket = socket;
//	wiznetDisableInterrupts();
	return WIZNET_SUCCESS;
}

/* This function is used to initialize reading from the reception buffer
** This function will disable interrupts until the reading is finished
**
** socket - socket whose reception buffer should be read from
**
** returns - WIZNET_SUCCESS if succesful
**         - WIZNET_ERROR_RECV_COLLISION if a recv was already in progress
**         - WIZNET_ERROR_NO_SLIP_HEADER if datagram was not start with C0
*/
int wiznetRecvBeginSLIP(uint8_t socket) {
	uint8_t tmp;
	//Buffer is already in use, finish the other read/write first!
	if (wiznetBufferReadSocket > -1)
		return WIZNET_ERROR_RECV_COLLISION;
	wiznetBufferReadCur = wiznetGetSocketRXReadPointer(socket);
	wiznetBufferReadSocket = socket;
	
	wiznetRecvData(&tmp, sizeof(tmp));
	if (tmp != 0xC0) {
		wiznetRecvAbandon();
		return WIZNET_ERROR_NO_SLIP_HEADER;
	}
//	wiznetDisableInterrupts();
	return WIZNET_SUCCESS;
}

/* This function is used to transfer some data from the current buffer
**
** buf    - byte array to which the data should be copied
**        - if buffer is NULL, data is skipped instead
** length - number of bytes to receive
**
** TODO: Check that a buffer is actually set-up.
** TODO: Add in a boundary check for the back of the circular buffer
** TODO: What if the user wants to read from the transmit buffer?
*/
void wiznetRecvData(uint8_t* buf, uint16_t length) {
	wiznetIOBegin(wiznetBufferReadSocket, wiznetBufferReadCur, 'r', 'r');
	if (buf == NULL)
		while (length--) {
			wiznetIOTransceive(0xFF);
			wiznetBufferReadCur++;
		}
	else
		while (length--) {
			*buf++ = wiznetIOTransceive(0xFF);
			wiznetBufferReadCur++;
		}
	wiznetIOFinish();
}

/* This function is used to transfer some data from the current wiznet
** buffer under the assumption that it has been SLIP encoded.
**
** buf    - byte array to which the data should be copied
**        - if buffer is NULL, data is skipped instead
** length - number of bytes to receive
**
** returns - 0 for succesful read, -1 for truncated read (end character
**           encountered.
** TODO: Check that a buffer is actually set-up.
** TODO: Add in a boundary check for the back of the circular buffer
** TODO: What if the user wants to read from the transmit buffer?
** TODO: Error detection if escape character is followed by unexpected character
*/
int wiznetRecvSLIPData(uint8_t* buf, uint16_t length) {
	uint8_t tmp;
	wiznetIOBegin(wiznetBufferReadSocket, wiznetBufferReadCur, 'r', 'r');
	while (length--) {
		tmp = wiznetIOTransceive(0xFF);
		if (tmp == 0xC0) {
			wiznetIOFinish();
			return WIZNET_ERROR_PREMATURE_SLIP_END;
		} else if (tmp == 0xDB) {
			wiznetBufferReadCur++;
			tmp = wiznetIOTransceive(0xFF); 
			if (tmp == 0xDC)
				tmp = 0xC0;
			else if (tmp == 0xDD)
				tmp = 0xDB;
		}
		if (buf != NULL)
			*buf++ = tmp;
		wiznetBufferReadCur++;
	}
	wiznetIOFinish();
	return WIZNET_SUCCESS;
}

/* This function reads a UDP header from the currently active
** read buffer, as set-up with wiznetRecvBegin
**
** sIP - Pointer to the destination for the IP address
** sPort - Pointer to the destination for the IP port number
**
** returns the length of the datagram
*/
uint16_t wiznetRecvHeaderUDP(uint8_t* sIP, uint16_t* sPort) {
	uint8_t nul[4];
	uint16_t len;

	// If a NULL is provided for the IP or Port then 
	// do not store that the IP or port respectively
	if (sIP == NULL)
		sIP = nul;
	if (sPort == NULL)
		sPort = (uint16_t*)nul;

	// Grab the sIP and port
	wiznetRecvData(sIP, 4);
	wiznetRecvData(nul, 2);
	*sPort = (((uint16_t)nul[0])<<8) + nul[1];

	// Grab the length of the data in the UDP packet
	wiznetRecvData(nul, 2);
	len = (((uint16_t)nul[0])<<8) + nul[1];
	return len;
}

/* This function is used to end the reading from a reception buffer
** and move to the next packet in the reception stream
** This function re-enables interrupts
**
** returns - WIZNET_SUCCESS if succesful
**         - WIZNET_ERROR_NOT_RECVING if no receive was in progress
*/
int wiznetRecvCommit(uint16_t len) {
	uint16_t tmp;

	// Check to see whether a read socket is actually in
	// the process of receiving data.
	if (wiznetBufferReadSocket == -1)
		return WIZNET_ERROR_NOT_RECVING;

	// If len is specified as zero, then it is assumed we are reading
	// from a stream-based socket, so set the wiznet buffer to the
	// current position, otherwise we are using a datagram so we
	// know the length and can skip to the end.
	if (len == 0)
		tmp = wiznetBufferReadCur;
	else
		tmp = wiznetGetSocketRXReadPointer(wiznetBufferReadSocket)+len+8;

	// Set the wiznet registers to advance the read buffer and
	// issue the receive commit command.
	wiznetSetSocketRXReadPointer(wiznetBufferReadSocket, tmp);
//	wiznetEnableInterrupts();
	wiznetSocketCommand(wiznetBufferReadSocket, Sn_CR_RECEIVE);
	wiznetBufferReadSocket = -1;
	return WIZNET_SUCCESS;
}

/* This function is used to end the reading from a reception buffer
** and move to the next packet in the reception stream
** This function re-enables interrupts
**
** returns - WIZNET_SUCCESS if succesful
**         - WIZNET_ERROR_NOT_RECVING if no receive was in progress
*/
int wiznetRecvCommitSLIP(void) {
	uint8_t tmpByte = 0x00;

	// Check to see whether a read socket is actually in
	// the process of receiving data.
	if (wiznetBufferReadSocket == -1)
		return WIZNET_ERROR_NOT_RECVING;
	
	// Advancing to the end of a SLIP datagram is slightly more complex
	// and involves scanning over the read buffer until the end character
	// is encountered.
	wiznetIOBegin(wiznetBufferReadSocket, wiznetBufferReadCur, 'r', 'r');
	while (tmpByte != 0xC0) {
		tmpByte = wiznetIOTransceive(0xFF);
		wiznetBufferReadCur++;
	}
	wiznetIOFinish();

	// Set the wiznet registers to advance the read buffer and
	// issue the receive commit command.
	wiznetSetSocketRXReadPointer(wiznetBufferReadSocket, wiznetBufferReadCur);
//	wiznetEnableInterrupts();
	wiznetSocketCommand(wiznetBufferReadSocket, Sn_CR_RECEIVE);
	wiznetBufferReadSocket = -1;
	return WIZNET_SUCCESS;
}


/* This function is used to abandon the reading of a reception buffer
** without moving to the next packet
** This function re-enables interrupts
*/
int wiznetRecvAbandon(void) {
	wiznetBufferReadSocket = -1;
	return WIZNET_SUCCESS;
//	wiznetEnableInterrupts();
}

/* This function checks how much data is resident in the specified socket RX buffer
**
** socket - the socket number in which to peek
*/
int wiznetRecvPeek(uint8_t socket) {
	return wiznetGetSocketRXReceivedSize(socket);
}


/* This function gets the current position of the buffer so that it can be restored
** at a later time.
** This function takes no arguments
*/
uint16_t wiznetGetBufferReadPosition(void) {
	return wiznetBufferReadCur;
}

/* This function sets the current position of the buffer as has been previously
** read
**
** pos - position from wiznetGetBufferPosition
*/
void wiznetSetBufferReadPosition(uint16_t pos) {
	wiznetBufferReadCur = pos;
}

/* This function alters the current position of the buffer by a relative move
** amount (equivalent to fseek(SEEK_CUR))
**
** pos - amount by which to change the buffer position
*/
void wiznetSetRelBufferReadPosition(int16_t change) {
	wiznetBufferReadCur += change;
}

