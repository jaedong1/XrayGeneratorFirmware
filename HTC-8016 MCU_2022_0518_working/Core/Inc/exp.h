
#include "stdio.h"
#include "main.h"
#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdlib.h>

#define	NO_ERR						0	//	에러발생하지 않음 

#define W_kV_PER_V					10      // 10 is Ref 1 V = Output 10 kV ,           25 is Ref 1 V = Output 25 kV
#define W_OPAMP_kV_SCALE			3       // 4 is OPAMP input 1V = OPAMP output = 4V , 8 is OPAMP input 1V = OPAMP output = 8V 

#define W_mA_PER_V					4   	// 10 is Ref 1 V = Output 10 mA (Relay All OFF 기준),   25 is Ref 1 V = Output 25mA (Relay All OFF 기준)
#define W_OPAMP_mA_SCALE			4		// 4 is OPAMP input 1V = OPAMP output = 4V ,    8 is OPAMP input 1V = OPAMP output = 8V 

#define W_A_TO_mA					1000    // A를 mA 단위로 변경 
#define W_RESOLUTION				4095    // 2^10 - 1 = 4095
#define W_MCU_REF_VCC				3.3     // MCU AREF Pin 입력 전압 (V)

// 가중치 계산 
//----------------------------------------------------------------------------------------------
// Conv1kVToData        = 1kV 당 DAC 데이터 값
// Conv1mAToData        = 1mA 당 DAC 데이터 값
// W_RESOLUTION         = MCU DAC 해상도 
// W_MCU_REF_VCC        = MCU analog 기준 전압 레벨 
// W_kV_PER_V           = 1 V 당 출력 kV 값
// W_OPAMP_kV_SCALE     = MCU 출력에서 TL494 입력까지 OP-AMP 증폭율
// W_mA_PER_V           = 1 V 당 출력 mA 값
// W_OPAMP_mA_SCALE     = MCU 출력에서 TL494 입력까지 OP-AMP 증폭율
#if 0

#define CONV_1kV_To_DATA	(W_RESOLUTION * W_OPAMP_kV_SCALE / W_kV_PER_V / W_MCU_REF_VCC)
#define CONV_0_01mA_To_DATA	(W_RESOLUTION * W_OPAMP_mA_SCALE / W_mA_PER_V / W_MCU_REF_VCC)

#else

#define CONV_1kV_To_DATA	(W_RESOLUTION / (W_MCU_REF_VCC * W_kV_PER_V * W_OPAMP_kV_SCALE))    
#define CONV_0_01mA_To_DATA	(W_RESOLUTION / (W_MCU_REF_VCC * W_mA_PER_V * W_OPAMP_mA_SCALE)) 

#endif

#define CONV_30kV_To_DATA		30 * CONV_1kV_To_DATA			// 30 kV ADC Data 
#define CONV_0_6mA_To_DATA		0.6 * CONV_0_01mA_To_DATA			// 0.8 mA ADC Data 

#define CONV_1V_To_DATA	135         

#define STB_KV_HIGH_CMP		10 * CONV_1kV_To_DATA			// 10 kV ADC Data 
#define STB_MA_HIGH_CMP		3 * CONV_0_01mA_To_DATA			//	3mA ADC Data
#define STB_FIL_HIGH_CMP	150	

#define RDY_FIL_LOW_CMP	100

#define EXP_MA_LOW_CMP	(g_xray.mA * CONV_0_01mA_To_DATA) * 0.8		// mA 조사 기준 20% Low
#define EXP_MA_HIGH_CMP	(g_xray.mA * CONV_0_01mA_To_DATA) * 1.2		// mA 조사 기준 20% High
#define PPS_MA_HIGH_CMP	(MAX_mA_SETTING * 1.25) * CONV_0_01mA_To_DATA
#define EXP_KV_LOW_CMP	(g_xray.kV * CONV_1kV_To_DATA) * 0.8		// kV 조사 기준 20% Low
#define EXP_KV_HIGH_CMP	(g_xray.kV * CONV_1kV_To_DATA) * 1.2		// kV 조사 기준 20% High

#define DC_MON_LOW_CMP	CONV_1V_To_DATA * 22    //DC 22V

#define MAX_kV_DAC_VALUE (MAX_kV_SETTING * 1.3) * CONV_1kV_To_DATA
#define MAX_mA_DAC_VALUE (MAX_mA_SETTING * 1.3) * CONV_0_01mA_To_DATA
#define MAX_Pre_DAC_VALUE 1200
#define MAX_Pon_DAC_VALUE MAX_mA_DAC_VALUE
#define MAX_Poff_DAC_VALUE 700

#define MIN_kV_DAC_VALUE (MIN_kV_SETTING * 0.7) * CONV_1kV_To_DATA
#define MIN_mA_DAC_VALUE (MIN_mA_SETTING * 0.7) * CONV_0_01mA_To_DATA
#define MIN_Pre_DAC_VALUE 1
#define MIN_Pon_DAC_VALUE 1
#define MIN_Poff_DAC_VALUE 1

extern BYTE sw_RDY, sw_EXP;
extern BYTE Hand_Switch_Status;
extern DAC_HandleTypeDef hdac;
extern BYTE T_100ms_Err_chk;
extern WORD T_1ms_STB_Err_chk;
extern BYTE T_1ms_RDY_Err_chk;
extern uint16_t T_1ms_Exp_Err_chk;
extern uint16_t T_1ms_Exp_Pulse_Err_chk;
extern char conv_TX_DATA[20];

extern void XRAY_RESET(void);

extern void EXP_Rating_Set(void);

extern void EXP_PPS_Handler(void);
extern void EXP_NOR_Handler(void);
extern void Set_kV_CalData(BYTE kV, BYTE data);
extern void Set_mA_CalData(BYTE kV, BYTE mA, BYTE data);
extern void Set_Preh_CalData(BYTE kV, BYTE mA, BYTE data);
extern void Set_Pron_CalData(BYTE kV, BYTE mA, BYTE data);
extern void Set_Prof_CalData(BYTE kV, BYTE mA, BYTE data);

extern void Error_Send(void);
