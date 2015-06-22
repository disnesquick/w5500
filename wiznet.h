#include <stdint.h>
#define WIZNET_MAX_SOCKETS 8
#define WIZNET_MAX_BUFFER_SIZE 0x4000

enum {
	WIZNET_SUCCESS = 0,
	WIZNET_ERROR_SOCKET_OPEN = -1,
	WIZNET_ERROR_UNKNOWN_PROTOCOL = -2,
	WIZNET_ERROR_SOCKET_NOT_READY = -3,
	WIZNET_ERROR_SOCKET_TIMEOUT = -4,
	WIZNET_ERROR_NOT_SENDING = -5,
	WIZNET_ERROR_SEND_DATA = -6,
	WIZNET_ERROR_SEND_COLLISION = -7,
	WIZNET_ERROR_NOT_RECVING = -8,
	WIZNET_ERROR_RECV_DATA = -9,
	WIZNET_ERROR_RECV_COLLISION = -10,
	WIZNET_ERROR_NO_SLIP_HEADER = -11,
	WIZNET_ERROR_PREMATURE_SLIP_END = -12
};

enum {
	SOCK_UDP = 0,
	SOCK_TCP = 1
};

void wiznetReset(void);
void wiznetInit(uint8_t bufSize[]);
void wiznetInitBufferSizes(uint8_t* rx, uint8_t* tx);

int wiznetOpenSocket(uint8_t socket, uint8_t protocol, uint16_t port, uint8_t flags);
void wiznetCloseSocket(uint8_t socket);
int wiznetConnectSocket(uint8_t socket, uint8_t* destIP, uint16_t destPort);
int wiznetListenOnSocket(uint8_t socket);

void wiznetWaitForData(void);
uint16_t wiznetRecvHeaderUDP(uint8_t* sIP, uint16_t* sPort);

uint8_t wiznetGetSocketInts(void);
uint8_t wiznetGetDeviceInts(void);
void wiznetClearDeviceInts(void);
void wiznetClearSocketRecvInt(uint8_t socket);
void wiznetGetDeviceMAC(uint8_t* MAC);
void wiznetSetDeviceMAC(uint8_t* MAC);
void wiznetConfigureIPLayer(uint8_t* gip, uint8_t* snm, uint8_t* sip);

int wiznetRecvBegin(uint8_t socket);
int wiznetRecvBeginSLIP(uint8_t socket);
int wiznetRecvCommit(uint16_t len);
int wiznetRecvCommitSLIP(void);
int wiznetRecvAbandon(void);
int wiznetRecvPeek(uint8_t socket);
void wiznetRecvData(uint8_t* buf, uint16_t length);
int wiznetRecvSLIPData(uint8_t* buf, uint16_t length);
uint16_t wiznetGetBufferReadPosition(void);
void wiznetSetBufferReadPosition(uint16_t pos);
void wiznetSetRelBufferReadPosition(int16_t change);

int wiznetSendToBegin(uint8_t socket, uint8_t* destIP, uint16_t destPort);
int wiznetSendBegin(uint8_t socket);
int wiznetSendBeginSLIP(uint8_t socket);
int wiznetSendToCommit(void);
int wiznetSendCommit(void);
int wiznetSendCommitSLIP(void);
int wiznetSendToAbandon(void);
int wiznetSendAbandon(void);
void wiznetSendData(const uint8_t* buf, uint16_t length);
void wiznetSendSLIPData(const uint8_t* buf, uint16_t length);
uint16_t wiznetGetBufferWritePosition(void);
void wiznetSetBufferWritePosition(uint16_t pos);
