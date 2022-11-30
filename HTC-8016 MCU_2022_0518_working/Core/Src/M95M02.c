#include "M95M02.h"

extern SPI_HandleTypeDef hspi2;

BYTE SpiTxBuf[6] = {0,};

void WriteEnable(void)
{
	SpiTxBuf[0] = WREN;

	EEPROM1_CS_ON;
	HAL_SPI_Transmit(&hspi2, SpiTxBuf,1,100);
	EEPROM1_CS_OFF;
}

void WriteDisable(void)
{
	SpiTxBuf[0] = WRDI;

	EEPROM1_CS_ON;
	HAL_SPI_Transmit(&hspi2, SpiTxBuf,1,100);
	EEPROM1_CS_OFF;
}

BYTE Read_Status_Reg(void)
{
	BYTE SpiRxBuf[1] = {0};

	SpiTxBuf[0] = RDSR;
	EEPROM1_CS_ON;
	HAL_SPI_Transmit(&hspi2, SpiTxBuf,1,100);
	HAL_SPI_Receive(&hspi2, SpiRxBuf,1,100);
	EEPROM1_CS_OFF;

	return SpiRxBuf[0];
}

void Write_Status_Reg(BYTE StatusReg)
{
	SpiTxBuf[0] = WRSR;
	SpiTxBuf[1] = StatusReg;

	WriteEnable();

	EEPROM1_CS_ON;
	HAL_SPI_Transmit(&hspi2, SpiTxBuf,2,100);
	EEPROM1_CS_OFF;

	HAL_Delay(15);
}


void	ONE_Byte_Write(WORD page, BYTE start, BYTE Val)
{
	DWORD	start_address;
	BYTE	add1=0, add2=0, add3=0;
      
	WriteEnable();

	start_address = page * 256;
	add1 = (start_address & 0xFF0000) >> 16;
	add2 = (start_address & 0x00FF00) >> 8;
	add3 = (start_address & 0x0000FF) >> 0;

	SpiTxBuf[0] = WRITE;
	SpiTxBuf[1] = add1;
	SpiTxBuf[2] = add2;
	SpiTxBuf[3] = add3 + start;
	SpiTxBuf[4] = Val;

	EEPROM1_CS_ON;
	HAL_SPI_Transmit(&hspi2, SpiTxBuf,5,100);	
	EEPROM1_CS_OFF;

	HAL_Delay(15);
}


BYTE ONE_Byte_Read(WORD page, BYTE start)
{
	BYTE SpiRxBuf[6] = {0};

	DWORD	start_address;
	BYTE	add1=0, add2=0, add3=0;
	
	start_address = page * 256;
	add1 = (start_address & 0xFF0000) >> 16;
	add2 = (start_address & 0x00FF00) >> 8;
	add3 = (start_address & 0x0000FF) >> 0;

	SpiTxBuf[0] = READ;
	SpiTxBuf[1] = add1;
	SpiTxBuf[2] = add2;
	SpiTxBuf[3] = add3+start;

	EEPROM1_CS_ON;

	HAL_SPI_Transmit(&hspi2, SpiTxBuf,4,100);
	HAL_SPI_Receive(&hspi2, SpiRxBuf,1,100);

	EEPROM1_CS_OFF;
	return SpiRxBuf[0];
}

void	Write_Memory(WORD page, BYTE start, BYTE len, SBYTE *data)
{
	DWORD	start_address;
	BYTE	add1=0, add2=0, add3=0;
      
	WriteEnable();

	start_address = page * 256;
	add1 = (start_address & 0xFF0000) >> 16;
	add2 = (start_address & 0x00FF00) >> 8;
	add3 = (start_address & 0x0000FF) >> 0;

	SpiTxBuf[0] = WRITE;
	SpiTxBuf[1] = add1;
	SpiTxBuf[2] = add2;
	SpiTxBuf[3] = add3 + start;

	EEPROM1_CS_ON;
	HAL_SPI_Transmit(&hspi2, SpiTxBuf,4,100);	
	HAL_SPI_Transmit(&hspi2, (uint8_t *)data, len, 100);	
	EEPROM1_CS_OFF;

	HAL_Delay(15);
}

void Read_Memory(WORD page, BYTE start, BYTE len, SBYTE *Array)
{
	DWORD	start_address;
	BYTE	add1=0, add2=0, add3=0;

	start_address = page * 256;

	add1 = (start_address & 0xFF0000) >> 16;
	add2 = (start_address & 0x00FF00) >> 8;
	add3 = (start_address & 0x0000FF) >> 0;
	
	SpiTxBuf[0] = READ;
	SpiTxBuf[1] = add1;
	SpiTxBuf[2] = add2;
	SpiTxBuf[3] = add3 + start;
	
	EEPROM1_CS_ON;
	HAL_SPI_Transmit(&hspi2, SpiTxBuf,4,100);
	HAL_SPI_Receive(&hspi2, (uint8_t *)Array, len,100);
	EEPROM1_CS_OFF;
}
