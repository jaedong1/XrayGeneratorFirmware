
#include "stdio.h"
#include "main.h"
#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdlib.h>

#define	NO_ERR						0	//	�����߻����� ���� 

#define W_kV_PER_V					10      // 10 is Ref 1 V = Output 10 kV ,           25 is Ref 1 V = Output 25 kV
#define W_OPAMP_kV_SCALE			3       // 4 is OPAMP input 1V = OPAMP output = 4V , 8 is OPAMP input 1V = OPAMP output = 8V 

#define W_mA_PER_V					4   	// 10 is Ref 1 V = Output 10 mA (Relay All OFF ����),   25 is Ref 1 V = Output 25mA (Relay All OFF ����)
#define W_OPAMP_mA_SCALE			4		// 4 is OPAMP input 1V = OPAMP output = 4V ,    8 is OPAMP input 1V = OPAMP output = 8V 

#define W_A_TO_mA					1000    // A�� mA ������ ���� 
#define W_RESOLUTION				4095    // 2^10 - 1 = 4095
#define W_MCU_REF_VCC				3.3     // MCU AREF Pin �Է� ���� (V)

// ����ġ ��� 
//----------------------------------------------------------------------------------------------
// Conv1kVToData        = 1kV �� DAC ������ ��
// Conv1mAToData        = 1mA �� DAC ������ ��
// W_RESOLUTION         = MCU DAC �ػ� 
// W_MCU_REF_VCC        = MCU analog ���� ���� ���� 
// W_kV_PER_V           = 1 V �� ��� kV ��
// W_OPAMP_kV_SCALE     = MCU ��¿��� TL494 �Է±��� OP-AMP ������
// W_mA_PER_V           = 1 V �� ��� mA ��
// W_OPAMP_mA_SCALE     = MCU ��¿��� TL494 �Է±��� OP-AMP ������
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

#define EXP_MA_LOW_CMP	(g_xray.mA * CONV_0_01mA_To_DATA) * 0.8		// mA ���� ���� 20% Low
#define EXP_MA_HIGH_CMP	(g_xray.mA * CONV_0_01mA_To_DATA) * 1.2		// mA ���� ���� 20% High
#define PPS_MA_HIGH_CMP	(MAX_mA_SETTING * 1.25) * CONV_0_01mA_To_DATA
#define EXP_KV_LOW_CMP	(g_xray.kV * CONV_1kV_To_DATA) * 0.8		// kV ���� ���� 20% Low
#define EXP_KV_HIGH_CMP	(g_xray.kV * CONV_1kV_To_DATA) * 1.2		// kV ���� ���� 20% High

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
