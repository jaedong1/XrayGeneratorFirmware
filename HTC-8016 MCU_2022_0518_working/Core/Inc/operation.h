#ifndef __OPERATION_H
#define __OPERATION_H


#include "stdio.h"
#include "main.h"
#include "M95M02.h"
#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdlib.h>

#define sw_STB				        0
#define sw_RDY_start				1
#define sw_RDY_ing				2
#define sw_EXP_start				3
#define sw_EXP_on				4
#define sw_EXP_ing				5
#define sw_EXP_Limit				6

#define MEM_P0				0
#define MEM_P1				1
#define MEM_P2				2
#define MEM_P3				3
#define MEM_P4				4
#define MEM_P5				5
#define MEM_P6				6
#define MEM_P7				7
#define MEM_P8				8
#define MEM_P9				9

#define MEM_INIT_ADD			50

extern STLedStatus		CommLed;
extern UTDipSwitchControl	DipSwitch;
extern Option_Global		g_opt;
extern Xray_Global		g_xray;
extern Standard_Global		g_std;
extern Standard_Global		g_std_sum;
extern CAL_Global		g_cal;
extern STCalData                Init_CalData;
extern STCalData                Load_CalData;

extern BYTE	T_100ms_Sys_Led;
extern BYTE	T_100ms_Rdy_HandSwitch;
extern uint16_t	T_1ms_Exp_HandSwitch;
extern WORD	T_100ms_EXP_Switch_Waiting;
extern uint32_t	T_1ms_EXP_Switch_On;
extern uint16_t T_1ms_EXP_Time_Over_Error;
extern BYTE	T_1ms_RDY_HandSwitch;
extern BYTE	T_1ms_Limit_HandSwitch;
extern BYTE	PpsXrayStatus;

extern WORD	ERR_NO;
extern uint16_t	T_1ms_Exp_Err_chk;
extern WORD	T_1ms_STB_Err_chk;

extern uint16_t XrayOffErrorTimer;
extern uint8_t XrayOffErrorFlag;

extern uint8_t BuzzerState;

extern uint16_t Exp_Start_Timer;

extern void Init_WDT(void);
extern void Dipswitch_Handler(void);
extern void Watchdog_Handler(void);
extern void LED_Handler(void);
extern void Handswitch_Handler(void);
extern void UART_TxRxComLedRun(void);
extern void EEPROM_Initialize(void);
extern void Reset_Factory_Data(void);
extern void Load_Factory_Data(void);
extern void EEPROM_Initialize(void);
extern void Rating_Initialize(void);

#endif
