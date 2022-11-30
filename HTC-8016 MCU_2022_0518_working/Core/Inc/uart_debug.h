#ifndef _UART_DEBUG_H_
#define _UART_DEBUG_H_
/************************************************************************************************************/

#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "MCP2515.h"

/************************************************************************************************************/

#define PRINT_STX	0x74
#define PRINT_ETX	0x73

#define UART_MAX_DATA	11
#define UART_QSIZE			50

typedef struct
{
	uint8_t	rx_byte[1];
	uint8_t	rx_buf[UART_QSIZE][UART_MAX_DATA];
	uint8_t	wr_point;
	uint8_t	rd_point;   
	uint8_t	buf_cnt;

}UART_Q;

extern uint8_t Read_Print_Data(uint8_t *arr);
extern void Debug_Handler(void);
/************************************************************************************************************/
extern UART_HandleTypeDef huart3;
extern UART_Q  U_PACKET, P_PACKET;
/************************************************************************************************************/
#endif
