#ifndef _UART_H_
#define _UART_H_
/************************************************************************************************************/

#include "CANSPI.h"
#include "command.h"
#include "uart_debug.h"
#include "operation.h"
#include "history.h"
#include "adc.h"

#define OFF 0
#define ON 1
#define NO 0

#define STX 0xF0
#define ETX 0xF4
#define ACK 0xF2
#define NAK 0xF3
#define BUFFER_SIZE 1024
#define PACKET_LENGTH_INDEX 11
#define CS_buff 9

#define HEADER 0
#define COMMAND 1
#define LENGTH 2
#define DATA 3
#define CS length+3
#define TAIL CS+1

#define HEADERK50       0
#define LENGTHK50 1
#define DATAK50         2
#define CHECKSUMK50 length+2
#define TAILK50         length+3
#define TX_START_K50    length+4

#define KV_DATA 3
#define MAS_DATA 4
#define mA_DATA 4
#define KV_CAL_DATA 5
#define MODE_DATA 5
#define MA_CAL_DATA 6
#define PRE_CAL_DATA 7
#define PRE_CAL_2_DATA 8
#define ETX_index buff[LENGTH]+4
#define PACKET_LENGTH length+5
#define BACKUP_PACKET_LENGTH TX_Packet[LENGTH]+5
#define DATA_MAX 11

#define ON_10KV_TERM_SHIFT 7
#define OFF_10KV_TERM_SHIFT 8
#define PREHEAT_10KV_TERM_SHIFT 9

#define ERROR_CODE 0x30

#define LENGTH_DATA i
#define WIDTH_DATA i+4
#define COMM_ERR 22

#define FUNC_LAMP_ON() HAL_GPIO_WritePin(GPIOE,GPIO_PIN_11,GPIO_PIN_RESET)
#define FUNC_LAMP_OFF() HAL_GPIO_WritePin(GPIOE,GPIO_PIN_11,GPIO_PIN_SET)

#define SEND_NAK() Uart_Data[0]=RX_Packet[COMMAND]; FuncSendPacket(NAK,1,&Uart_Data[0])
#define Comm_Version_Chk() FuncSendPacket(COMM_CHK,1,&Uart_Data[0])

#define Packet_Empty() (UART_RX_PACKET.wr_point == UART_RX_PACKET.rd_point)

#define NEW_TX_START length+4

/************************************************************************************************************/
typedef struct
{
        uint8_t	buf[BUFFER_SIZE];                
        uint16_t	wr_point;
        uint16_t	rd_point;       
}sUART_Q;
/************************************************************************************************************/
typedef struct
{
        uint8_t	buf[UART_QSIZE][UART_MAX_DATA];                
        uint8_t	wr_point;
        uint8_t	rd_point;       
}sUART_TX_Q;
/************************************************************************************************************/
typedef struct {
    uint8_t stx;
    uint16_t cmd;
    uint32_t data;
    uint16_t debug;
    uint8_t cs;
    uint8_t etx;    
} __attribute__((packed)) uUART_MSG;
/************************************************************************************************************/
typedef struct
{
    uint8_t stx;
    uint8_t cmd1;
    uint8_t cmd2;
    uint8_t data1;
    uint8_t data2;
    uint8_t data3;
    uint8_t data4;
    uint8_t debug1;
    uint8_t debug2;
    uint8_t cs;
    uint8_t etx;
}sUART_TX_MSG;
/************************************************************************************************************/

extern UART_Q U_PACKET, P_PACKET;

void increase_point_value(uint32_t* pData);
void Uart_EnQueue(uint16_t data);
uint8_t Uart_DeQueue();
unsigned char FuncCalcCheckSum(unsigned char *ptr);
void UART_CMD_Run(uUART_MSG *msg);
void RXDataComm(unsigned char data);
void UART_TX_Queue_Add(uint16_t cmd, uint32_t data, uint16_t debug);
void UART_TX_Data();
uint8_t UART_TX_Queue_Process();
void UART_TX_Handler(void);
void UART_RX_Handler(void);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

/************************************************************************************************************/
extern UART_HandleTypeDef huart1;
extern unsigned char Uart_Data[DATA_MAX];
extern uint32_t RX_Packet[6];
/************************************************************************************************************/
#endif
