#ifndef __CAN_SPI_H
#define	__CAN_SPI_H

#include "stm32f4xx_hal.h"
#include "main.h"

#define CAN_MAX_DATA					8
#define CAN_QSIZE						10

#define CAN_PROTOCOL					3

#define CAN_COMM						0
#define CAN_DATA						1
#define CAN_DEBUG						2



#define	CAN_COMM_kV_SETTING				0x2131
#define	CAN_COMM_mA_SETTING			0x2132
#define	CAN_COMM_kV_FB_READ				0x2135
#define	CAN_COMM_mA_FB_READ			0x2136
#define	CAN_COMM_XRAY_ON				0x2137
#define	CAN_COMM_XRAY_OFF				0x2139
#define	CAN_COMM_HEATSINK_TEMP_READ		0x2140
#define	CAN_COMM_TANK_TEMP_READ			0x2141
#define	CAN_COMM_XRAY_ONLY_ON			0x2142
#define	CAN_COMM_XRAY_ONLY_OFF			0x2143
#define	CAN_COMM_GEN_READY_OK			0x2157
#define	CAN_COMM_VER_CHECK				0x7159
#define	CAN_COMM_PREHEAT_ON			0x2160
#define	CAN_COMM_NORMAL_MODE_ON			0x2179
#define	CAN_COMM_PPS_MODE_ON			0x217A
#define	CAN_COMM_XRAY_REMOTE_MODE_ON	        0x2090
#define	CAN_COMM_XRAY_REMOTE_MODE_OFF	        0x2091
#define	CAN_COMM_ERROR_INIT				0x217E


#define	CAN_COMM_TANK_TEMP_OK			0x7156
#define CAN_COMM_ACK				0x7158
#define	CAN_COMM_READYUP				0x7160
#define	CAN_COMM_XRAY_START			0x7170

#define CAN_COMM_CAL_DATA_READ			0x0321
#define CAN_COMM_CAL_DATA_WRITE			0x0322

#define COMM_TANK_TEST				0x5252

#define ERROR_RESET_ACK				0x0000
#define kV_SET_ACK					0x0001
#define mA_SET_ACK					0x0002
#define XRAY_ON_ACK					0x0003
#define XRAY_OFF_ACK					0x0004
#define XRAY_ONLY_ON_ACK				0x0005
#define XRAY_ONLY_OFF_ACK				0x0006
#define PREHEAT_ON_ACK				0x0009
#define GENERATOR_READY				0x000A
#define NORMAL_MODE_ON_ACK				0x000B
#define PULSE_MODE_ON_ACK				0x000C
#define REMOTE_MODE_ON_ACK				0x0010
#define REMOTE_MODE_OFF_ACK				0x0011

#define CAN_ERR_TANK_INTERLOCK			0x7146
#define CAN_ERR_TANK_TEMP_OVER			0x7152
#define CAN_ERR_EXP_SIGNAL_FAULT		        0x7154
#define CAN_ERR_EXP_COMM_FAULT			0x7155
#define CAN_ERR_kV_UNDER				0x7161
#define CAN_ERR_kV_OVER				0x7162
#define CAN_ERR_mA_UNDER				0x7163
#define CAN_ERR_mA_OVER				0x7164
#define CAN_ERR_PROTOCOL_FAULT			0x7165
#define CAN_ERR_INPUT_SIGNAL_FAULT		        0x7166
#define CAN_ERR_FILAMENT_FAULT			0x7168
#define CAN_ERR_EXPTIME_OVER			        0x7169

#define ERR_TANK_TEMP_OVER			        0x8100
#define ERR_HEATSINK_TEMP_OVER		        0x8101

#define ERR_SYSTEM_INTERLOCK		        0x8102

#define CAN_INT_CHK		HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4)

typedef struct
{
	uint8_t	buf[CAN_QSIZE][CAN_MAX_DATA];
	uint8_t	wr_point;
	uint8_t	rd_point;   
}CAN_Q;

typedef union {
  struct {
    uint8_t idType;
    uint32_t id;
    uint8_t dlc;
    uint8_t data0;
    uint8_t data1;
    uint8_t data2;
    uint8_t data3;
    uint8_t data4;
    uint8_t data5;
    uint8_t data6;
    uint8_t data7;
  } frame;
  uint8_t array[14];
}uCAN_MSG;

#define dSTANDARD_CAN_MSG_ID_2_0B 	1
#define dEXTENDED_CAN_MSG_ID_2_0B 	2

extern bool CANSPI_Initialize(void);
extern void CANSPI_Sleep(void);
extern uint8_t CANSPI_Transmit(uCAN_MSG *tempCanMsg);
extern uint8_t CANSPI_Receive(uCAN_MSG *tempCanMsg);
extern uint8_t CANSPI_messagesInBuffer(void);
extern uint8_t CANSPI_isBussOff(void);
extern uint8_t CANSPI_isRxErrorPassive(void);
extern uint8_t CANSPI_isTxErrorPassive(void);

extern void CAN_TX_TEST(void);
extern void CAN_Response(uint32_t Comm, uint32_t data, uint32_t debug);
void CAN_TX_Data(void);
extern void CAN_RX_Handler(void);
extern void CAN_TX_Handler(void);
extern void CAN_RX_Queue_Add(void);
extern void CAN_TX_Queue_Add(uint16_t Comm, uint32_t data, uint16_t debug);
extern uint8_t CAN_RX_Queue_Process(uint32_t *arr);
extern uint8_t CAN_TX_Queue_Process(void);
extern void CAN_CMD_Run(uint32_t *CAN);


extern uint32_t Comm_Tank_Test_kV_setting(uint32_t kV_data);
extern uint32_t Comm_kV_setting(uint32_t kV_data);
extern uint32_t Comm_mA_setting(uint32_t mA_data);
extern uint32_t Comm_kV_FB_read(void);
extern uint32_t Comm_mA_FB_read(void);
extern void Comm_XRAY_ON(void);
extern void Comm_XRAY_OFF(void);
extern uint32_t Comm_Tank_Temp_read(void);
extern void Comm_XRAY_Only_ON(void);
extern void Comm_XRAY_Only_OFF(void);
extern void Comm_Generator_ready_OK(void);
extern void Comm_firmware_version_check(void);
extern void Comm_Preheart_ON(void);
extern void Comm_Normal_mode_ON(void);
extern void Comm_PPS_mode_ON(void);
extern void Comm_XRAY_remote_mode_ON(void);
extern void Comm_XRAY_remote_mode_OFF(void);
extern void Comm_Error_Init(void);

#endif	/* __CAN_SPI_H */

