#include "main.h"
#include "Adc.h"
#include "operation.h"
#include "exp.h"
#include "CANSPI.h"
#include "uart.h"

ADC_STURCT ADC_DATA;
ADC_STURCT ADC_DATA_RAW;
ADC_STURCT ADC_DATA_SUM;

Error_STURCT ErrorTimer;
Error_STURCT ErrorFlag;

uint8_t TempErrorStartFlag = 0;

extern BYTE	Hand_Switch_Status;
extern WORD	ERR_NO;
extern BYTE T_100ms_Err_chk;
extern BYTE T_100ms_thermal_update;
extern WORD	T_1ms_STB_Err_chk;
extern BYTE 	T_1ms_RDY_Err_chk;
extern uint16_t 	T_1ms_Exp_Err_chk;
extern uint16_t 	T_1ms_Exp_Pulse_Err_chk;
extern uint16_t	T_1ms_EXP_Time_Over_Error;
extern uint32_t Exp_Limit_Err_tm;

extern uint16_t Exp_Limit_tm;
extern uint32_t T_1ms_EXP_Switch_On;
extern BYTE T_1ms_Feedback_Zero;

//WORD Test_kV[2], Test_mA[2];
//WORD Test_RAW[30];

/************************************************************************************************************************************/

void ADC_GetData(void)
{
	//if(T_25us_ADC_Flag==0)	return;

	//T_25us_ADC_Flag=0;

	static BYTE adc_index=0;

	if(adc_index < ADCAverageNum) 
	{
		//Test_RAW[adc_index] = ADC_DATA_RAW.kV_MON;
		ADC_DATA_SUM.kV_MON   += ADC_DATA_RAW.kV_MON;
		ADC_DATA_SUM.mA_MON   += ADC_DATA_RAW.mA_MON;
		ADC_DATA_SUM.FIL_MON  += ADC_DATA_RAW.FIL_MON;
		ADC_DATA_SUM.Tank_THER_MON += ADC_DATA_RAW.Tank_THER_MON;
		ADC_DATA_SUM.Heatsink_THER_MON += ADC_DATA_RAW.Heatsink_THER_MON;
		ADC_DATA_SUM.DC_MON  += ADC_DATA_RAW.DC_MON;

		adc_index++;
	}
	else
	{
		ADC_DATA.kV_MON   = ADC_DATA_SUM.kV_MON   / ADCAverageNum;
		ADC_DATA.mA_MON   = ADC_DATA_SUM.mA_MON   / ADCAverageNum;
		ADC_DATA.FIL_MON  = ADC_DATA_SUM.FIL_MON  / ADCAverageNum;
		ADC_DATA.Tank_THER_MON = ADC_DATA_SUM.Tank_THER_MON / ADCAverageNum;
		ADC_DATA.Heatsink_THER_MON = ADC_DATA_SUM.Heatsink_THER_MON / ADCAverageNum;              
		ADC_DATA.DC_MON = ADC_DATA_SUM.DC_MON / ADCAverageNum;

		ADC_DATA_SUM.kV_MON   = 0;
		ADC_DATA_SUM.mA_MON   = 0;
		ADC_DATA_SUM.FIL_MON  = 0;
		ADC_DATA_SUM.Tank_THER_MON = 0;
		ADC_DATA_SUM.Heatsink_THER_MON = 0;
		ADC_DATA_SUM.DC_MON = 0;

		//if(Test_kV[0] > ADC_DATA.kV_MON)	Test_kV[0] = ADC_DATA.kV_MON;
		//if(Test_kV[1] < ADC_DATA.kV_MON)	Test_kV[1] = ADC_DATA.kV_MON;
		//if(Test_mA[0] > ADC_DATA.mA_MON)	Test_mA[0] = ADC_DATA.mA_MON;
		//if(Test_mA[1] < ADC_DATA.mA_MON)	Test_mA[1] = ADC_DATA.mA_MON;
		adc_index=0;
	}
}

/************************************************************************************************************************************/

void ADC_Handler(void)
{
        // ADC data Conversion
        Thermal_ConV();
        Error_Code_check();	
	 //Thermal_Error_Code_check();         
		
	 if(ERR_NO == ERR_SYSTEM_INTERLOCK)
	 {
		  if(LOCK_CHK == _OFF) Comm_Error_Init();							
	 }
}

/************************************************************************************************************************************/
void Error_Code_check(void)
{
        if((ERR_NO != NO_ERR) || (DipSwitch.bit.sw4_noerror == DIPSW_ON) ) 				{ return; }              

	// Stand by status
	
	if (Hand_Switch_Status == sw_STB )
	{
		if(T_1ms_STB_Err_chk < 300) 	{ return; }
#if 0
        if(ADC_DATA.DC_MON < DC_MON_LOW_CMP)
        {
			/* ERR_NO = */ 
			XRAY_RESET();           
        }	

		if (LOCK_CHK == _ON)	/* Interlock Error */
		{
			ERR_NO = ERR_SYSTEM_INTERLOCK;
			XRAY_RESET();
			LOCK_LED_ON;		
		}         

		if (TANK_INTERLOCK_CHK == _ON)	/* Interlock Error */
		{
			ERR_NO = CAN_ERR_TANK_INTERLOCK; 
			XRAY_RESET();
			INTERLOCK_LED_ON;
		}		
#endif		
		if(T_1ms_STB_Err_chk < 1000) 	{ return; }
#if 0
		if (ADC_DATA.kV_MON > STB_KV_HIGH_CMP)	/* tand by 상태에서 10kV 이상 F/B이 감지 되었을 때 Error */
		{
			ERR_NO = STB_kV_FB_ERR;
			XRAY_RESET();
                        return;
		}

		if (ADC_DATA.mA_MON > STB_MA_HIGH_CMP)	/* Stand by 상태에서 3mA 이상 F/B이 감지 되었을 때 Error */
		{
			ERR_NO = STB_mA_FB_ERR;
			XRAY_RESET();
                        return;
		}

		if (ADC_DATA.FIL_MON > STB_FIL_HIGH_CMP)	/* Stand by 상태에서 Filament F/B이 감지 되었을 때 Error */
		{
			ERR_NO = STB_Fil_FB_ERR;
			XRAY_RESET();
                        return;
		}	
#endif		
	}

	// Ready status
	if ( (Hand_Switch_Status == sw_RDY_ing ) || (Hand_Switch_Status == sw_EXP_start ) )
	{
		if(T_1ms_RDY_Err_chk <= 100) 	{ return; }

		if(ADC_DATA.DC_MON < DC_MON_LOW_CMP)
        {
			/* ERR_NO = */ 
			XRAY_RESET();           
        }	

		if (LOCK_CHK == _ON)	/* Interlock Error */
		{
			ERR_NO = ERR_SYSTEM_INTERLOCK;
			XRAY_RESET();
			LOCK_LED_ON;		
		}         

		if (TANK_INTERLOCK_CHK == _ON)	/* Interlock Error */
		{
			ERR_NO = CAN_ERR_TANK_INTERLOCK; 
			XRAY_RESET();
			INTERLOCK_LED_ON;
		}
		
		if (ADC_DATA.FIL_MON < RDY_FIL_LOW_CMP)	/* Ready 시 Filament F/B이 감지 되지 않을 때 Error */
		{
			ERR_NO = CAN_ERR_FILAMENT_FAULT;
			XRAY_RESET();
			return;
		}		
	}

	// exposure status
	if (Hand_Switch_Status == sw_EXP_ing)
	{
#if 1	
		if(MA_OVER_CHK == MA_OVER)
		{
			ERR_NO = CAN_ERR_mA_OVER;
			XRAY_RESET();
			OVER_CURRENT_LED_ON;
			return;
		}
		if(KV_OVER_CHK == KV_OVER)
		{
			ERR_NO = CAN_ERR_kV_OVER;
			XRAY_RESET();	
			OVER_VOLTAGE_LED_ON;
			return;
		}		
#endif		
		if(g_opt.exp_opt == CONT)
		{
			if(T_1ms_Exp_Err_chk <= 50) 	{ return; }
                        
			if (ADC_DATA.kV_MON > EXP_KV_HIGH_CMP)	/* X-ray 조사 시 설정 전압보다 10% 이상일 시 Error */
			{
		        	if(ErrorFlag.kV_Over == OFF)
		        	{
                                	ErrorFlag.kV_Over = ON;
                                	ErrorTimer.kV_Over = 0;
		        	}

		        	else if((ErrorFlag.kV_Over == ON) && (ErrorTimer.kV_Over >= ERROR_TIME_KV_OVER))
		        	{
        				ERR_NO = CAN_ERR_kV_OVER;
        				XRAY_RESET();	
        				OVER_VOLTAGE_LED_ON;
        				return;
		        	}
			}	

			else
			{
                        	ErrorFlag.kV_Over = OFF;
                        	ErrorTimer.kV_Over = OFF;
			}

		
			if (ADC_DATA.mA_MON > EXP_MA_HIGH_CMP)	/* X-ray 조사 시 설정 전류보다 10% 이상일 시 Error */
			{	
		        	if(ErrorFlag.mA_Over == OFF)
		        	{
                                	ErrorFlag.mA_Over = ON;
                                	ErrorTimer.mA_Over = 0;
		        	}

		        	else if((ErrorFlag.mA_Over == ON) && (ErrorTimer.mA_Over >= ERROR_TIME_MA_OVER))
		        	{
        				ERR_NO = CAN_ERR_mA_OVER;
        				XRAY_RESET();
        				OVER_CURRENT_LED_ON;
                                	return;
		        	}	
			}

			else
			{
                                ErrorFlag.mA_Over = OFF;
                                ErrorTimer.mA_Over = OFF;
			}	
		}
		
		else if(g_opt.exp_opt == PPS)
		{
			if(T_1ms_Exp_Pulse_Err_chk <= 3) 	{ return; }
			if(T_1ms_Exp_Err_chk <= 100) 	{ return; }
			
			if (ADC_DATA.kV_MON > EXP_KV_HIGH_CMP)	/* X-ray 조사 시 설정 전압보다 10% 이상일 시 Error */
			{
		        	if(ErrorFlag.kV_Over == OFF)
		        	{
                                	ErrorFlag.kV_Over = ON;
                                	ErrorTimer.kV_Over = 0;
		        	}

		        	else if((ErrorFlag.kV_Over == ON) && (ErrorTimer.kV_Over >= ERROR_TIME_KV_OVER))
		        	{
        				ERR_NO = CAN_ERR_kV_OVER;
        				XRAY_RESET();	
        				OVER_VOLTAGE_LED_ON;
        				return;
		        	}
			}	

			else
			{
                        	ErrorFlag.kV_Over = OFF;
                        	ErrorTimer.kV_Over = OFF;
			}

		
			if (ADC_DATA.mA_MON > PPS_MA_HIGH_CMP)	/* PPS조사 시 5V이상일 시 Error */
			{	
		        	if(ErrorFlag.mA_Over == OFF)
		        	{
                                	ErrorFlag.mA_Over = ON;
                                	ErrorTimer.mA_Over = 0;
		        	}

		        	else if((ErrorFlag.mA_Over == ON) && (ErrorTimer.mA_Over >= ERROR_TIME_MA_OVER))
		        	{
        				ERR_NO = CAN_ERR_mA_OVER;
        				XRAY_RESET();
        				OVER_CURRENT_LED_ON;
                                	return;
		        	}	
			}

			else
			{
                                ErrorFlag.mA_Over = OFF;
                                ErrorTimer.mA_Over = OFF;
			}			
		}	
		
		if( T_1ms_Exp_Err_chk <= 100 ) 							{ return; }
		if (g_opt.exp_opt == PPS) 	{ return; }
		
		// kV
		if (ADC_DATA.kV_MON < EXP_KV_LOW_CMP)	/* X-ray 조사 시 설정 전압보다 10% 이하일 시 Error */
		{
		        if(ErrorFlag.kV_Under == OFF)
		        {
                                ErrorFlag.kV_Under = ON;
                                ErrorTimer.kV_Under = 0;
		        }

		        else if((ErrorFlag.kV_Under == ON) && (ErrorTimer.kV_Under >= ERROR_TIME_KV_UNDER))
		        {
        			ERR_NO = CAN_ERR_kV_UNDER;
        			XRAY_RESET();
                                return;
		        }		
		}

		else
		{
                        ErrorFlag.kV_Under = OFF;
                        ErrorTimer.kV_Under = OFF;
		}
		
		// mA
		if (ADC_DATA.mA_MON < EXP_MA_LOW_CMP)	/* X-ray 조사 시 설정 전류보다 10% 이하일 시 Error */
		{
		        if(ErrorFlag.mA_Under == OFF)
		        {
                                ErrorFlag.mA_Under = ON;
                                ErrorTimer.mA_Under = 0;
		        }

		        else if((ErrorFlag.mA_Under == ON) && (ErrorTimer.mA_Under >= ERROR_TIME_MA_UNDER))
		        {
        			ERR_NO = CAN_ERR_mA_UNDER;
        			XRAY_RESET();
                                return;
		        }		
		}

		else
		{
                        ErrorFlag.mA_Under = OFF;
                        ErrorTimer.mA_Under = OFF;
		}		
	}	
}

/************************************************************************************************************************************/

void Thermal_ConV(void)
{ 
        if(g_std.thermal_chk == _ON)
        {
                if(T_100ms_thermal_update <= 10)
                {
                	g_std_sum.tank_temp += (float)(ADC_DATA.Tank_THER_MON / CONV_1DEG_To_DATA);
                	g_std_sum.heatsink_temp += (float)(ADC_DATA.Heatsink_THER_MON / CONV_1DEG_To_DATA);
                }

                else
                {
                        g_std.tank_temp = g_std_sum.tank_temp / 10;
                        g_std.heatsink_temp = g_std_sum.heatsink_temp / 10;
                
                        g_std_sum.tank_temp = 0;
                        g_std_sum.heatsink_temp = 0;     

                        T_100ms_thermal_update = 0;
                        
                        TempErrorStartFlag = ON;
                } 

                g_std.thermal_chk = _OFF;                
        }        
}

/************************************************************************************************************************************/

void Thermal_Error_Code_check(void)
{      
       uint16_t sendThermaldata = 0;
  
       if(TempErrorStartFlag == OFF) return;
       if(Hand_Switch_Status != sw_STB) return;                //대기 상태에서만 온도 Error check
              
	// Tank Temperature Check (온도 Error는항상 걸리게 함)
	if((g_std.tank_temp < TEMP_LOWEST_DEGREE) || (g_std.tank_temp > g_opt.tank_err_tmp))	/* Tank 온도가 Error 설정 온도를 넘어갔을 때 Error */
	{
		if(ErrorFlag.Tank_Temp_Over == OFF)
		{
                	ErrorFlag.Tank_Temp_Over = ON;
                	ErrorTimer.Tank_Temp_Over = 0;
		}
		
		else if((ErrorFlag.Tank_Temp_Over == ON) && (ErrorTimer.Tank_Temp_Over >= ERROR_TEMP_OVER))
		{
			ERR_NO = ERR_TANK_TEMP_OVER;
			XRAY_RESET();
			OVER_TEMP_LED_ON;

			UART_TX_Queue_Add(ERR_NO, 0x00, 0x00);
		}
	}
	
	else
	{
       	ErrorFlag.Tank_Temp_Over = OFF;
              ErrorTimer.Tank_Temp_Over = OFF;
	}
	
	if((g_std.heatsink_temp < TEMP_LOWEST_DEGREE) || (g_std.heatsink_temp > g_opt.heatsink_err_tmp))	/* Tank 온도가 Error 설정 온도를 넘어갔을 때 Error */
	{
		if(ErrorFlag.Heatsink_Temp_Over == OFF)
		{
                	ErrorFlag.Heatsink_Temp_Over = ON;
                	ErrorTimer.Heatsink_Temp_Over = 0;
		}
		
		else if((ErrorFlag.Heatsink_Temp_Over == ON) && (ErrorTimer.Heatsink_Temp_Over >= ERROR_TEMP_OVER))
		{
			ERR_NO = ERR_HEATSINK_TEMP_OVER;
			XRAY_RESET();
			OVER_TEMP_LED_ON;

			UART_TX_Queue_Add(ERR_NO, 0x00, 0x00);
		}
	}
	
	else
	{
       	ErrorFlag.Heatsink_Temp_Over = OFF;
              ErrorTimer.Heatsink_Temp_Over = OFF;
	}     
        
        if(ERR_NO == ERR_TANK_TEMP_OVER)
        {
              if((g_std.tank_temp > TEMP_LOWEST_DEGREE) && (g_std.tank_temp <= g_opt.tank_err_tmp - TEMP_OK_DEGREE))
              {
                        Comm_Error_Init();
                        
                        sendThermaldata = Thermal_CAN_ConV(g_std.tank_temp);		
                        UART_TX_Queue_Add(CAN_COMM_TANK_TEMP_OK,sendThermaldata,0x00);
              }
        }
        
        if(ERR_NO == ERR_HEATSINK_TEMP_OVER)
        {
              if((g_std.heatsink_temp > TEMP_LOWEST_DEGREE) && (g_std.heatsink_temp <= g_opt.heatsink_err_tmp - TEMP_OK_DEGREE))
              {
                        Comm_Error_Init();
                        
                        sendThermaldata = Thermal_CAN_ConV(g_std.heatsink_temp);		
                        UART_TX_Queue_Add(CAN_COMM_TANK_TEMP_OK,sendThermaldata,0x00);
              }
        }         
}

/************************************************************************************************************************************/

uint32_t Thermal_CAN_ConV(float tempLawData)
{
        uint16_t temperatureData = 0;
        temperatureData = (uint16_t)(tempLawData * 10);

        return (temperatureData);
}

void Thermal_Initialize(void)
{
        g_std.thermal_chk = _ON;
        g_opt.tank_err_tmp = 60;
        g_opt.heatsink_err_tmp = 70;
        T_100ms_thermal_update = 0; 
}
