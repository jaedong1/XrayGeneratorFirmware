#ifndef __ADC_H
#define __ADC_H

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;


// LM35TD	IC 		사용	
// V_PER_1DEGREE        = 1도 당 전압 값
// W_OPAMP_THER_SCALE   = 온도 OPAMP 증폭 배율 

#define V_PER_1DEGREE		100
#define W_OPAMP_THER_SCALE	2
 
#define CONV_1DEG_To_DATA	(W_RESOLUTION * W_OPAMP_THER_SCALE / V_PER_1DEGREE / W_MCU_REF_VCC)

#define TEMP_LOWEST_DEGREE 10

#define TEMP_OK_DEGREE 2
 
#define LOCK_CHK		HAL_GPIO_ReadPin(Lock_GPIO_Port, Lock_Pin)
#define TANK_INTERLOCK_CHK	HAL_GPIO_ReadPin(Tank_Inter_Lock_GPIO_Port, Tank_Inter_Lock_Pin)

#define KV_OVER_CHK		HAL_GPIO_ReadPin(EP_OVER_GPIO_Port, EP_OVER_Pin)
#define MA_OVER_CHK		HAL_GPIO_ReadPin(IP_OVER_GPIO_Port, IP_OVER_Pin)

#define KV_OVER			0
#define MA_OVER			0

#define ERROR_TIME_KV_OVER 10
#define ERROR_TIME_KV_UNDER 10
#define ERROR_TIME_MA_OVER 10
#define ERROR_TIME_MA_UNDER 10

#define ERROR_TEMP_OVER 300

extern DAC_HandleTypeDef hdac;

typedef struct
{
	WORD	kV_MON;
	WORD	mA_MON ;       
	WORD	FIL_MON;
	WORD	Tank_THER_MON;
	WORD	Heatsink_THER_MON;
	WORD	DC_MON;
	
	float	kV_Value;
	float	mA_Value;
	float	Tank_THER_Value;
	float	Heatsink_THER_Value; 
}ADC_STURCT;

typedef struct
{
	uint8_t kV_Over;
	uint8_t kV_Under;
	uint8_t mA_Over;
	uint8_t mA_Under;
	uint16_t Tank_Temp_Over;
	uint16_t Heatsink_Temp_Over;
}Error_STURCT;

extern ADC_STURCT ADC_DATA_RAW;
extern ADC_STURCT ADC_DATA;

extern Error_STURCT ErrorTimer;
extern Error_STURCT ErrorFlag;

void ADC_Handler(void);
void Thermal_ConV(void);
void Thermal_Error_Code_check(void);
uint32_t Thermal_CAN_ConV(float tempLawData);
void ADC_GetData(void);
void Error_Code_check(void);
void Thermal_Initialize(void);

#define ADCAverageNum 8

extern BYTE T_25us_ADC_Flag;
extern uint8_t TempErrorStartFlag;

#endif

