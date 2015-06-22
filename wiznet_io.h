#ifndef wiznetSPITransceiveByte
	#error "wiznetSPITransceiveByte was not defined"
#endif
#ifndef wiznetSPIChipEnable
	#error "wiznetSPIChipEnable was not defined"
#endif
#ifndef wiznetSPIChipDisable
	#error "wiznetSPIChipDisable was not defined"
#endif

void wiznetRegWriteByte(int socket, uint16_t addr, uint8_t byte);
uint8_t wiznetRegReadByte(int socket, uint16_t addr);
void wiznetRegWriteWord(int socket, uint16_t addr, uint16_t word);
uint16_t wiznetRegReadWord(int socket, uint16_t addr);
void wiznetRegWriteIP(int socket, uint16_t addr, uint8_t* ip);
void wiznetRegReadIP(int socket, uint16_t addr, uint8_t* ip);
void wiznetRegWriteMAC(int socket, uint16_t addr, uint8_t* mac);
void wiznetRegReadMAC(int socket, uint16_t addr, uint8_t* mac);
void wiznetSocketCommand(int socket, uint8_t command);

int wiznetIOBegin(int socket, uint16_t address, char readWrite, char type);

inline uint8_t wiznetIOTransceive(uint8_t send) {
	return wiznetSPITransceiveByte(send);
}

inline void wiznetIOFinish(void) {
	wiznetSPIChipDisable();
}
