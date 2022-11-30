#include "main.h"
#include "operation.h"
#include "exp.h"
#include "CANSPI.h"
#include "uart.h"

STLedStatus             CommLed;
UTDipSwitchControl      DipSwitch;
Option_Global           g_opt;
Xray_Global             g_xray;
CAL_Global              g_cal;
Standard_Global         g_std;
Standard_Global         g_std_sum;

STCalData Init_CalData = 
{
/////////////////////////////////////////   Init data  /////////////////////////////////////////////////

        //int8_t Init_kV_data[kV_RANGE] =
        {
        //         60        70          80          90         100        110        120
                2,     0,      4,      4,      5,      0,      0
        },


        //int8_t Init_mA_data[kV_RANGE][mA_RANGE] = 
        {
        //60kV   5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {0,     0,      2,      0,      1,      0,      1,      0,      2,      0,      3,      0,      3,      0,      0,      0},
        //70kV   5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {1,     0,      1,      0,      1,      0,      2,      0,      2,      0,      3,      0,      3,      0,      0,      0},
        //80kV   5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {1,     0,      1,      0,      2,      0,      2,      0,      2,      0,      2,      0,      4,      0,      0,      0},
        //90kV   5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {2,     0,      1,      0,      2,      0,      2,      0,      2,      0,      4,      0,      4,      0,      0,      0},
        //100kV  5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {1,     0,      2,      0,      2,      0,      2,      0,      3,      0,      3,      0,      3,      0,      0,      0},
        //110kV  5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0},
        //120kV  5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0},     
        },

        //int8_t Init_PreHeat_data[kV_RANGE][mA_RANGE] = 
        {
        //60kV   5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {-15,     0,      -7,      0,      -8,      0,      -11,      0,      -9,      0,      -6,      0,      -6,      0,      0,      0},
        //70kV   5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {-5,     0,      -4,      0,      -4,      0,      -6,      0,      -5,      0,      -5,      0,      -1,      0,      0,      0},
        //80kV   5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {-8,     0,      -7,      0,      -4,      0,      -1,      0,      -4,      0,      -2,      0,      2,      0,      0,      0},
        //90kV   5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {10,     0,      -7,      0,      -4,      0,      -3,      0,      -5,      0,      -3,      0,      -3,      0,      0,      0},
        //100kV  5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {55,     0,      -4,      0,      -4,      0,      -2,      0,      -1,      0,      -5,      0,      -6,      0,      0,      0},
        //110kV  5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0},
        //120kV  5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0},     
        },

        //int8_t Init_PPS_ON_data[kV_RANGE][mA_RANGE] = 
        {
        //60kV   5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {4,     0,      -1,      0,      -1,      0,      5,      0,      1,      0,      -1,      0,      -3,      0,      0,      0},
        //70kV   5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {2,     0,      3,      0,      2,      0,      4,      0,      4,      0,      3,      0,      1,      0,      0,      0},
        //80kV   5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {3,     0,      7,      0,      4,      0,      4,      0,      6,      0,      5,      0,      3,      0,      0,      0},
        //90kV   5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {2,     0,      3,      0,      3,      0,      6,      0,      6,      0,      4,      0,      5,      0,      0,      0},
        //100kV  5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {4,     0,      5,      0,      5,      0,      4,      0,      5,      0,      4,      0,      4,      0,      0,      0},
        //110kV  5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0},
        //120kV  5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0},     
        },

        //int8_t Init_PPS_OFF_data[kV_RANGE][mA_RANGE] = 
        {
        //60kV   5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {10,     0,      5,      0,      7,      0,      11,      0,      12,      0,      8,      0,      5,      0,      0,      0},
        //70kV   5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {6,     0,      8,      0,      8,      0,      5,      0,      15,      0,      15,      0,      6,      0,      0,      0},
        //80kV   5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {4,     0,      7,      0,      6,      0,      6,      0,      6,      0,      9,      0,      13,      0,      0,      0},
        //90kV   5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {5,     0,      6,      0,      7,      0,      8,      0,      8,      0,      8,      0,      11,      0,      0,      0},
        //100kV  5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {7,     0,      6,      0,      7,      0,      6,      0,      8,      0,      8,      0,      9,      0,      0,      0},
        //110kV  5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0},
        //120kV  5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0},     
        }       
};

STCalData Load_CalData = 
{
/////////////////////////////////////////   Load data  /////////////////////////////////////////////////

        //int8_t Load_kV_data[kV_RANGE] =
        {
        //         60        70          80          90         100        110        120
                0,     0,      0,      0,      0,      0,      0
        },


        //int8_t Load_mA_data[kV_RANGE][mA_RANGE] = 
        {
        //60kV   5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0},
        //70kV   5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0},
        //80kV   5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0},
        //90kV   5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0},
        //100kV  5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0},
        //110kV  5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0},
        //120kV  5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0},     
        },

        //int8_t Load_PreHeat_data[kV_RANGE][mA_RANGE] = 
        {
        //60kV   5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0},
        //70kV   5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0},
        //80kV   5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0},
        //90kV   5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0},
        //100kV  5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0},
        //110kV  5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0},
        //120kV  5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0},     
        },

        //int8_t Load_PPS_ON_data[kV_RANGE][mA_RANGE] = 
        {
        //60kV   5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0},
        //70kV   5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0},
        //80kV   5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0},
        //90kV   5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0},
        //100kV  5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0},
        //110kV  5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0},
        //120kV  5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0},     
        },

        //int8_t Load_PPS_OFF_data[kV_RANGE][mA_RANGE] = 
        {
        //60kV   5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0},
        //70kV   5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0},
        //80kV   5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0},
        //90kV   5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0},
        //100kV  5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0},
        //110kV  5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0},
        //120kV  5           6           7            8           9           10          11         12          13          14          15          16         17          18          19          20
                {0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0},     
        }        
};

BYTE	Hand_Switch_Status = sw_STB;
BYTE	Hand_Switch_bak_Status = 0xff;
BYTE 	sw_RDY = 0, sw_EXP = 0;
BYTE    PpsXrayStatus = 0;

uint16_t Exp_Start_Timer = 0;

uint32_t Exp_Limit_tm = 0;
uint32_t Exp_Limit_Err_tm = 300000;
uint8_t BuzzerState = 0;

/***********************************************************************************************************************************
  DIP SW ON 시 동작 모드 
  DIP SW 1 : Debug Mode
  DIP SW 2 : IAP Mode
  DIP SW 3 : BUZZER OFF 
  DIP SW 4 : Error No Check  
***********************************************************************************************************************************/

void Dipswitch_Handler(void)
{
        DipSwitch.data = (uint8_t)((GPIOD -> IDR & 0x0f00) >> 8);
}

/***********************************************************************************************************************************/

void Init_WDT(void)
{
	if(DipSwitch.bit.sw1_debugmode == DIPSW_OFF)	
	{
		WDT_EN;
		printf("WDT_EN \r\n");		
	}
	else	
	{
		WDT_DIS;
		printf("WDT_DIS \r\n");			
	}
}

/***********************************************************************************************************************************/

void Watchdog_Handler(void)
{
	if(DipSwitch.bit.sw1_debugmode == DIPSW_OFF)
	{
		WDT_SIG;
	}
}

/***********************************************************************************************************************************/

void LED_Handler(void)
{
	if (T_100ms_Sys_Led > 5)
	{
		SYSLED_TOGGLE;
		T_100ms_Sys_Led = 0;
	}
}

/***********************************************************************************************************************************/

void Handswitch_Handler(void)
{
	sw_RDY = HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_1) ? 0:1; 
	sw_EXP = HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_2) ? 0:1; 	

	if (Hand_Switch_bak_Status != Hand_Switch_Status) { Hand_Switch_bak_Status = Hand_Switch_Status; }

	switch (Hand_Switch_Status)
	{
		case sw_STB:
		{
			Hand_Switch_Status = sw_RDY_start;
			T_1ms_RDY_HandSwitch = 0;			
		}
		break;


		case sw_RDY_start:
		{
			if (T_1ms_RDY_HandSwitch > 40)		// 40ms
			{			
				Hand_Switch_Status = sw_RDY_ing;
				T_100ms_Rdy_HandSwitch = 0;
			}
		}
		break;


		case sw_RDY_ing :
		{
			if (sw_EXP == _ON)
			{
				Exp_Start_Timer = 0;
				
				Hand_Switch_Status = sw_EXP_start;
			}
		}
		break;

		case sw_EXP_start :
		{
			if (sw_EXP == _ON)
			{
				if(Exp_Start_Timer >= 500)
				{
					Hand_Switch_Status = sw_EXP_on;				

					UART_TX_Queue_Add(CAN_COMM_READYUP, 0, 0);				
				}				
			}	

			else
			{
				Hand_Switch_Status = sw_STB;
			}
		}
		break;
		
		case sw_EXP_on :
		{
			if (sw_EXP == _ON)
			{
				Hand_Switch_Status = sw_EXP_ing;
				UART_TX_Queue_Add(CAN_COMM_XRAY_START, 0, 0);				
			}

			else
			{
				Hand_Switch_Status = sw_STB;
			}
		}
		break;
		
		case sw_EXP_ing :
		{
			// Continuous mode
			if(g_opt.exp_opt == CONT)
			{
				if (sw_EXP == _ON)
				{
					if (T_1ms_EXP_Switch_On >= Exp_Limit_Err_tm)	// 5min
					{						
						Hand_Switch_Status = sw_EXP_Limit;
						XRAY_RESET();		
						T_1ms_Limit_HandSwitch = 0;	
                                                
                        UART_TX_Queue_Add(CAN_ERR_EXP_COMM_FAULT, 0x00, 0x00);                                                
					}
				}
				else
				{
                    Hand_Switch_Status = sw_EXP_Limit;
                    
                    XrayOffErrorFlag = ON;
                    XrayOffErrorTimer = 0;
				}
            }

			// PPS Mode
			if(g_opt.exp_opt == PPS)
			{
				if (sw_EXP == _ON)
				{
					if (T_1ms_EXP_Switch_On >= Exp_Limit_Err_tm)	// 5min
					{						
						Hand_Switch_Status = sw_EXP_Limit;	
						XRAY_RESET();
						T_1ms_Limit_HandSwitch = 0;

						UART_TX_Queue_Add(CAN_ERR_EXP_COMM_FAULT, 0x00, 0x00);
					}
				}
				else
				{
                    Hand_Switch_Status = sw_EXP_Limit;
                    
                    XrayOffErrorFlag = ON;
                    XrayOffErrorTimer = 0;
				}				
			}
		}
		break;

		case sw_EXP_Limit :
		{
			if (sw_EXP == _OFF)
			{
				if (T_1ms_Limit_HandSwitch > 20)		// 20ms
				{
					Hand_Switch_Status = sw_STB;
				}
			}
		}
		break;

		
	}
        
        if(XrayOffErrorFlag == ON)
        {
                if(XrayOffErrorTimer >= 500)
                {
                        UART_TX_Queue_Add(CAN_ERR_EXP_COMM_FAULT, 0x00, 0x00);
                        XrayOffErrorFlag = OFF;
                }
        }
}

/***********************************************************************************************************************************/

void UART_TxRxComLedRun(void)
{
	if(CommLed.rx_count > 0) { if(--CommLed.rx_count == 0) COMM_LED_OFF; }
	if(CommLed.tx_count > 0) { if(--CommLed.tx_count == 0) COMM_LED_OFF; }
}

/***********************************************************************************************************************************/

void Reset_Factory_Data(void)
{
  	int8_t MEM_INIT_DATA = 0x55;
  
  	Write_Memory(MEM_P0, MEM_INIT_ADD, 1, &MEM_INIT_DATA);
	Write_Memory(MEM_P0, 0, kV_RANGE, Init_CalData.kV_data);

        for(int i = 0; i < kV_RANGE; i++)
        {
                Write_Memory(MEM_P1, (i * mA_RANGE), mA_RANGE, &Init_CalData.mA_data[i][0]);
                Write_Memory(MEM_P2, (i * mA_RANGE), mA_RANGE, &Init_CalData.PreHeat_data[i][0]);
                Write_Memory(MEM_P3, (i * mA_RANGE), mA_RANGE, &Init_CalData.PPS_ON_data[i][0]);
                Write_Memory(MEM_P4, (i * mA_RANGE), mA_RANGE, &Init_CalData.PPS_OFF_data[i][0]);
        }	     
}

/***********************************************************************************************************************************/

void Load_Factory_Data(void)
{
	Read_Memory(MEM_P0, 0, kV_RANGE, Load_CalData.kV_data);

        for(int i = 0; i < kV_RANGE; i++)
        {
                Read_Memory(MEM_P1, (i * mA_RANGE), mA_RANGE, &Load_CalData.mA_data[i][0]);
                Read_Memory(MEM_P2, (i * mA_RANGE), mA_RANGE, &Load_CalData.PreHeat_data[i][0]);
                Read_Memory(MEM_P3, (i * mA_RANGE), mA_RANGE, &Load_CalData.PPS_ON_data[i][0]);
                Read_Memory(MEM_P4, (i * mA_RANGE), mA_RANGE, &Load_CalData.PPS_OFF_data[i][0]);
        }	
}

/***********************************************************************************************************************************/

void EEPROM_Initialize(void)
{
  	int8_t flashReadData = 0;
	int8_t MEM_INIT_DATA = 0x55;
  
    Write_Status_Reg(0x00);
	
	Read_Memory(MEM_P0, MEM_INIT_ADD, 1, &flashReadData);
	
	if(flashReadData != MEM_INIT_DATA) Reset_Factory_Data();
    Load_Factory_Data();
}

void Rating_Initialize(void)
{
	g_xray.kV = MIN_kV_SETTING;
	g_xray.mA = MIN_mA_SETTING;

	EXP_Rating_Set();
}
