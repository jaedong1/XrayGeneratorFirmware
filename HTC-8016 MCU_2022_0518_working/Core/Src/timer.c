#include "main.h"
#include "timer.h"
#include "operation.h"
#include "adc.h"

BYTE	T_100ms_Sys_Led = 0;
BYTE	T_100ms_Rdy_HandSwitch = 0;
WORD	T_100ms_EXP_Switch_Waiting = 0;
uint32_t	T_1ms_EXP_Switch_On = 0;
BYTE	T_100ms_Err_chk = 0;
BYTE	T_100ms_thermal_update = 0;
uint16_t	T_1ms_EXP_Time_Over_Error = 0;

BYTE 	T_1ms_RDY_Err_chk = 0;
uint16_t 	T_1ms_Exp_Err_chk = 0;
uint16_t 	T_1ms_Exp_Pulse_Err_chk = 0;
uint16_t	T_1ms_Exp_HandSwitch = 0;
BYTE	T_1ms_RDY_HandSwitch = 0;
BYTE	T_1ms_Limit_HandSwitch = 0;
BYTE	T_25us_ADC_Flag = 0;
BYTE	T_1ms_Feedback_Zero = 0;
BYTE	T_Comm_Tx_Timer = 0; 
WORD 	T_1ms_STB_Err_chk = 0;

uint16_t XrayOffErrorTimer = 0;
uint8_t XrayOffErrorFlag = 0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)	
{
  	if(htim->Instance == TIM1)
	{
		if(BuzzerState == _ON) BUZZER_TOGGLE;
		else BUZZER_OFF;
	}  	
  
	if(htim->Instance == TIM6)	// 100ms timer
	{
		T_100ms_thermal_update++;
		T_100ms_Sys_Led++;
		T_100ms_Rdy_HandSwitch++;
		T_100ms_EXP_Switch_Waiting++;	
		T_100ms_Err_chk++;

		g_std.thermal_chk = _ON;
	}

	if(htim-> Instance == TIM7)	//	1ms timer
	{
		T_1ms_Exp_HandSwitch++;
		T_1ms_RDY_HandSwitch++;
		T_1ms_Limit_HandSwitch++;
		T_1ms_STB_Err_chk++;
		T_1ms_RDY_Err_chk++;
		T_1ms_Exp_Err_chk++;		
		T_1ms_Exp_Pulse_Err_chk++;
		T_1ms_Feedback_Zero++;
        T_1ms_EXP_Switch_On++;
		T_1ms_EXP_Time_Over_Error++;
		T_Comm_Tx_Timer++;

		ErrorTimer.kV_Over++;
		ErrorTimer.kV_Under++;
		ErrorTimer.mA_Over++;
		ErrorTimer.mA_Under++;
		
		ErrorTimer.Tank_Temp_Over++;
		ErrorTimer.Heatsink_Temp_Over++;

		XrayOffErrorTimer++;
		Exp_Start_Timer++;
	}   
}