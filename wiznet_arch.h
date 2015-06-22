#ifdef ARCH_XMEGA
#include <avr/io.h>
#ifdef WIZNET_INTERRUPTS_ENABLED
#include <avr/interrupt.h> 
extern void wiznetEnableInterrupts(void);
extern void wiznetDisableInterrupts(void);

#else
static inline void wiznetEnableInterrupts(void){}
static inline void wiznetDisableInterrupts(void){}
#endif

inline uint8_t SPITransceiveByte(uint8_t data) {
	GLOBAL_WIZNET_SPI_CONTROLLER.DATA = data;      // initiate write
	while((GLOBAL_WIZNET_SPI_CONTROLLER.STATUS & SPI_IF_bm) == 0);
	return GLOBAL_WIZNET_SPI_CONTROLLER.DATA;
}
#define wiznetSPITransceiveByte SPITransceiveByte
#define wiznetSPIChipEnable() GLOBAL_TMC26X_SPI_CONTROLLER.CTRL = SPI_ENABLE_bm | SPI_MASTER_bm | SPI_MODE_0_gc | SPI_PRESCALER_DIV16_gc; GLOBAL_WIZNET_SPI_SELECT_PORT.OUTCLR = GLOBAL_WIZNET_SPI_SELECT_PIN
#define wiznetSPIChipDisable() GLOBAL_WIZNET_SPI_SELECT_PORT.OUTSET = GLOBAL_WIZNET_SPI_SELECT_PIN

#endif

