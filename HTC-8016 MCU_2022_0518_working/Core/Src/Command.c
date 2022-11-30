#include "Command.h"
#include "operation.h"
#include "exp.h"
#include "adc.h"

uint32_t Comm_Tank_Test_kV_setting(uint32_t kV_data)
{
	if(kV_data <= TANK_TEST_MAX_kV_SETTING && kV_data >= MIN_kV_SETTING)
	{
		g_xray.kV = kV_data;
	}

	EXP_Rating_Set();
	
	return (uint32_t)g_xray.kV;
}

uint32_t Comm_kV_setting(uint32_t kV_data)
{
	if(kV_data <= MAX_kV_SETTING && kV_data >= MIN_kV_SETTING)
	{
		g_xray.kV = kV_data;
	}

	EXP_Rating_Set();
	
	return (uint32_t)g_xray.kV;
}

uint32_t Comm_Tank_Test_mA_setting(uint32_t mA_data)
{
	if(mA_data <= TANK_TEST_MAX_mA_SETTING && mA_data >= TANK_TEST_MIN_mA_SETTING)
	{
		g_xray.mA = mA_data;		
	}

	EXP_Rating_Set();
	
	return (uint32_t)g_xray.mA;
}

uint32_t Comm_mA_setting(uint32_t mA_data)
{
	if(mA_data <= MAX_mA_SETTING && mA_data >= MIN_mA_SETTING)
	{
		g_xray.mA = mA_data;
	}

	EXP_Rating_Set();
	
	return (uint32_t)g_xray.mA;
}

uint32_t Comm_kV_FB_read(void)
{
	return (uint32_t)(ADC_DATA.kV_MON / CONV_1kV_To_DATA);
}

uint32_t Comm_mA_FB_read(void)	
{
	return (uint32_t)(ADC_DATA.mA_MON / CONV_0_01mA_To_DATA);
}

void Comm_XRAY_ON(void)	
{

}

void Comm_XRAY_OFF(void)	
{

}

uint32_t Comm_Tank_Temp_read(void)	
{
	return 0x259;
}

void Comm_XRAY_Only_ON(void)	
{

}

void Comm_XRAY_Only_OFF(void)	
{

}

void Comm_Generator_ready_OK(void)	
{

}

void Comm_firmware_version_check(void)	
{

}

void Comm_Preheart_ON(void)	
{

}

void Comm_Normal_mode_ON(void)	
{

}

void Comm_PPS_mode_ON(void)	
{

}

void Comm_XRAY_remote_mode_ON(void)	
{

}

void Comm_XRAY_remote_mode_OFF(void)	
{

}

void Comm_Error_Init(void)	
{
        XRAY_RESET();
	TempErrorStartFlag = _OFF;

        LOCK_LED_OFF;        
        OVER_VOLTAGE_LED_OFF;
        OVER_CURRENT_LED_OFF;
        INTERLOCK_LED_OFF;
        OVER_TEMP_LED_OFF;

        ERR_NO = NO_ERR;
}

/***********************************************************************************************************************************/

uint32_t Comm_CAN_Cal_Data_Read(uint32_t CAN_Data)
{
        int8_t ReadCalData = 0;
        uint8_t Cal_Type, Cal_kV, Cal_mA = 0;
        uint32_t retVal = 0;

        Cal_Type = (uint8_t)((CAN_Data & 0xff000000) >> 24);
        Cal_kV = (uint8_t)((CAN_Data & 0x00ff0000) >> 16);
        Cal_mA = (uint8_t)((CAN_Data & 0x0000ff00) >> 8);   

        switch(Cal_Type)
        {
                case kV_CAL : 
                {
                        ReadCalData = Load_CalData.kV_data[Cal_kV];
                        break;
                }         

                case mA_CAL : 
                {
                        ReadCalData = Load_CalData.mA_data[Cal_kV][Cal_mA];
                        break;
                }    

                case PRE_CAL : 
                {
                        ReadCalData = Load_CalData.PreHeat_data[Cal_kV][Cal_mA];
                        break;
                }    

                case PPS_ON_CAL : 
                {
                        ReadCalData = Load_CalData.PPS_ON_data[Cal_kV][Cal_mA];
                        break;
                }    

                case PPS_OFF_CAL : 
                {
                        ReadCalData= Load_CalData.PPS_OFF_data[Cal_kV][Cal_mA];
                        break;
                }                    
        }

        retVal = (uint32_t)((uint8_t)ReadCalData | CAN_Data);
        return (retVal);
}

/***********************************************************************************************************************************/

uint32_t Comm_CAN_Cal_Data_Write(uint32_t CAN_Data)
{
        int8_t Cal_Data, ReadCalData = 0;
        uint8_t Cal_Type, Cal_kV, Cal_mA, Cal_Memory_Address = 0;                
        uint32_t retVal = 0;

        Cal_Type = (uint8_t)((CAN_Data & 0xff000000) >> 24);
        Cal_kV = (uint8_t)((CAN_Data & 0x00ff0000) >> 16);
        Cal_mA = (uint8_t)((CAN_Data & 0x0000ff00) >> 8); 
        Cal_Data = (int8_t)(CAN_Data);

        Cal_Memory_Address = (Cal_kV * mA_RANGE) + Cal_mA;

        switch(Cal_Type)
        {
                case kV_CAL : 
                {                        
                        ONE_Byte_Write(MEM_P0, Cal_kV, Cal_Data);
                        Load_CalData.kV_data[Cal_kV] = ONE_Byte_Read(MEM_P0, Cal_kV);
                        ReadCalData = Load_CalData.kV_data[Cal_kV];
                        break;
                }         

                case mA_CAL : 
                {
                        ONE_Byte_Write(MEM_P1, Cal_Memory_Address, Cal_Data);
                        Load_CalData.mA_data[Cal_kV][Cal_mA] = ONE_Byte_Read(MEM_P1, Cal_Memory_Address);
                        ReadCalData = Load_CalData.mA_data[Cal_kV][Cal_mA];
                        break;
                }    

                case PRE_CAL : 
                {
                        ONE_Byte_Write(MEM_P2, Cal_Memory_Address, Cal_Data);
                        Load_CalData.PreHeat_data[Cal_kV][Cal_mA] = ONE_Byte_Read(MEM_P2, Cal_Memory_Address);
                        ReadCalData = Load_CalData.PreHeat_data[Cal_kV][Cal_mA];
                        break;
                }    

                case PPS_ON_CAL : 
                {
                        ONE_Byte_Write(MEM_P3, Cal_Memory_Address, Cal_Data);
                        Load_CalData.PPS_ON_data[Cal_kV][Cal_mA] = ONE_Byte_Read(MEM_P3, Cal_Memory_Address);
                        ReadCalData = Load_CalData.PPS_ON_data[Cal_kV][Cal_mA];
                        break;
                }    

                case PPS_OFF_CAL : 
                {
                        ONE_Byte_Write(MEM_P4, Cal_Memory_Address, Cal_Data);
                        Load_CalData.PPS_OFF_data[Cal_kV][Cal_mA] = ONE_Byte_Read(MEM_P4, Cal_Memory_Address);
                        ReadCalData = Load_CalData.PPS_OFF_data[Cal_kV][Cal_mA];
                        break;
                }                    
        }

        retVal = (uint32_t)((uint8_t)ReadCalData | CAN_Data);
        return (retVal);
}