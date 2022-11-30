#include "stdio.h"
#include "main.h"

// ***************************************************************************
// Signel
// ***************************************************************************

#define EEPROM1_CS_ON		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
#define EEPROM1_CS_OFF		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);

// ***************************************************************************
// M95M02-DR 		(ST serial eeprom device )
// ***************************************************************************
#define WREN					0x06    // SPI Flash opcode: Write Enable
#define WRDI						0x04    // SPI Flash opcode: Write Disable / End AAI mode
#define RDSR						0x05    // SPI Flash opcode: Read Status Register
#define WRSR					0x01    // SPI Flash opcode: Write Status Register
#define READ						0x03    // SPI Flash opcode: Read up 
#define WRITE					0x02    // SPI Flash opcode: Write one byte
#define RDID						0x83    // SPI Flash opcode: Read ID Page
#define WRID						0x82    // SPI Flash opcode: Write ID Page
#define RDLS						0x83
#define LID						0x82
#define SPI_READ					0xFF 

// ***************************************************************************
// Macro
// ***************************************************************************

void WriteEnable(void);
void WriteDisable(void);
BYTE Read_Status_Reg(void);
void Write_Status_Reg(BYTE StatusReg);
void	ONE_Byte_Write(WORD page, BYTE start, BYTE Val);
BYTE ONE_Byte_Read(WORD page, BYTE start);
void	Write_Memory(WORD page, BYTE start, BYTE len, SBYTE *data);
void Read_Memory(WORD page, BYTE start, BYTE len, SBYTE *Array);

