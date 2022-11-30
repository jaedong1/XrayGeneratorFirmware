#include "main.h"
#include "uart.h"
#include "exp.h"
#include "command.h"
/************************************************************************************************************/
uint32_t RX_Packet[6]={0, };    // 1 : STX, 2 : CMD, 3 : DATA, 4 : Debug, 5 : CS, 6 : ETX
uint8_t TX_Packet[DATA_MAX]={0, };

uint8_t Tank_Test_Mode = 0;

extern UART_HandleTypeDef huart3;
extern BYTE T_Comm_Tx_Timer;

extern uint32_t Exp_Limit_Err_tm;

unsigned char Uart_Data[DATA_MAX]={0, };

UART_Q  U_PACKET, P_PACKET;

sUART_Q UART_RX_PACKET;
sUART_TX_Q UART_TX_PACKET;

uUART_MSG UART_RX_MSG;
sUART_TX_MSG UART_TX_MSG;
/************************************************************************************************************/
void increase_point_value(uint32_t* pData)      //패킷 헤더나 테일 값올리는 함수
{
      (*pData)++;
      
      if( BUFFER_SIZE==(*pData))
      {
        (*pData)=0;
      }
}
/************************************************************************************************************/
void Uart_EnQueue(uint16_t data)
{
      void *ptr;      

      if(UART_RX_PACKET.wr_point >= BUFFER_SIZE) UART_RX_PACKET.wr_point = 0;
    
      ptr = &UART_RX_PACKET.wr_point;
  
      UART_RX_PACKET.buf[UART_RX_PACKET.wr_point] = (uint8_t)data;       //버퍼에 usart로 받은 값 저장
      increase_point_value(ptr);      //헤드값올림
}
/************************************************************************************************************/
uint8_t Uart_DeQueue()
{
      void *ptr;

      if(UART_RX_PACKET.rd_point >= BUFFER_SIZE) UART_RX_PACKET.rd_point = 0;
      
      ptr = &UART_RX_PACKET.rd_point;
  
      uint8_t retVal=UART_RX_PACKET.buf[UART_RX_PACKET.rd_point];      //버퍼에 들어간 값을 다시 반환
      increase_point_value(ptr);      //테일값올림
      
      return retVal;
}
/************************************************************************************************************/
unsigned char FuncCalcCheckSum(unsigned char *ptr)  //ptr 부터 len만큼 bcc 계산
{
      register int i=0;
      unsigned short CheckSum=0;
      
      for(i=0;i<7;i++)
      {
        CheckSum+=ptr[i];
      }
      
      CheckSum&=0x00ff;
      return (unsigned char)CheckSum;
}
/************************************************************************************************************/
void MsgDataConvert(uUART_MSG *msg, uint8_t *buff)
{
        msg->cmd = (buff[1] << 8) | (buff[2]);
        msg->data = (buff[3] << 24) | (buff[4] << 16) | (buff[5] << 8) | (buff[6]);
        msg->debug = (buff[7] << 8) | (buff[8]);
}
/************************************************************************************************************/
void UART_CMD_Run(uUART_MSG *msg)
{
        uint16_t sendThermaldata = 0;
        uint32_t calResault = 0;

	switch(msg->cmd)
	{
		case CAN_COMM_kV_SETTING:	
		{
			if(Tank_Test_Mode == _ON) UART_TX_Queue_Add(CAN_COMM_ACK,Comm_Tank_Test_kV_setting(msg->data),kV_SET_ACK);
			else UART_TX_Queue_Add(CAN_COMM_ACK,Comm_kV_setting(msg->data),kV_SET_ACK);
			break;
		}
		
		case CAN_COMM_mA_SETTING:
		{	
			if(Tank_Test_Mode == _ON) UART_TX_Queue_Add(CAN_COMM_ACK,Comm_Tank_Test_mA_setting(msg->data),mA_SET_ACK);	
			else UART_TX_Queue_Add(CAN_COMM_ACK,Comm_mA_setting(msg->data),mA_SET_ACK);
			break;
		}

		case CAN_COMM_kV_FB_READ:
		{
			UART_TX_Queue_Add(msg->cmd,Comm_kV_FB_read(),msg->debug);	
			break;
		}
		
		case CAN_COMM_mA_FB_READ:
		{
			UART_TX_Queue_Add(msg->cmd,Comm_mA_FB_read(),msg->debug);
			break;
		}
		
		case CAN_COMM_XRAY_ON:
		{
                     if((Hand_Switch_Status >= sw_RDY_start) && (Hand_Switch_Status <= sw_EXP_ing)) UART_TX_Queue_Add(CAN_COMM_ACK,0x00000000,XRAY_ON_ACK);
                     else UART_TX_Queue_Add(CAN_ERR_EXP_SIGNAL_FAULT,0x00000000,0x0000);
			break;
		}

		case CAN_COMM_XRAY_OFF:
		{
			UART_TX_Queue_Add(CAN_COMM_ACK,0x00000000,XRAY_OFF_ACK);	
                     XrayOffErrorFlag = OFF;
			break;
		}

		case CAN_COMM_HEATSINK_TEMP_READ:
		{			         
		        sendThermaldata = Thermal_CAN_ConV(g_std.heatsink_temp);		               		        
			UART_TX_Queue_Add(msg->cmd,sendThermaldata,0x0000);
			
			break;
		}		
		
		case CAN_COMM_TANK_TEMP_READ:
		{
                        sendThermaldata = Thermal_CAN_ConV(g_std.tank_temp);		
			UART_TX_Queue_Add(msg->cmd,sendThermaldata,0x0000);
			
			break;
		}
		
		case CAN_COMM_XRAY_ONLY_ON:
		{
			UART_TX_Queue_Add(CAN_COMM_ACK,0x00000000,XRAY_ONLY_ON_ACK);
			
			break;
		}
		
		case CAN_COMM_XRAY_ONLY_OFF:
		{
			UART_TX_Queue_Add(CAN_COMM_ACK,0x00000000,XRAY_ONLY_OFF_ACK);
			
			break;
		}
		
		case CAN_COMM_GEN_READY_OK:	
		{	
			UART_TX_Queue_Add(CAN_COMM_ACK,msg->data,0x000A);
			break;
		}

		case CAN_COMM_VER_CHECK:
		{
			UART_TX_Queue_Add(CAN_COMM_VER_CHECK, HTC8016_VERSION, 0x0000);
			break;
		}
		
		case CAN_COMM_PREHEAT_ON:
		{
			UART_TX_Queue_Add(CAN_COMM_ACK, 0x00000000, PREHEAT_ON_ACK);
			
			break;
		}

		case CAN_COMM_NORMAL_MODE_ON:
		{	
			UART_TX_Queue_Add(CAN_COMM_ACK,0x00000000,NORMAL_MODE_ON_ACK);
			g_opt.exp_opt = CONT;
			break;
		}

		case CAN_COMM_PPS_MODE_ON:
		{	
			UART_TX_Queue_Add(CAN_COMM_ACK,0x00000000,PULSE_MODE_ON_ACK);
			g_opt.exp_opt = PPS;
			break;
		}

		case CAN_COMM_XRAY_REMOTE_MODE_ON:
		{
			UART_TX_Queue_Add(CAN_COMM_ACK,0x00000000,REMOTE_MODE_ON_ACK);
			break;
		}
		
		case CAN_COMM_XRAY_REMOTE_MODE_OFF:
		{
			UART_TX_Queue_Add(CAN_COMM_ACK,0x00000000,REMOTE_MODE_OFF_ACK);
			break;
		}
		
		case CAN_COMM_ERROR_INIT:
		{
			UART_TX_Queue_Add(CAN_COMM_ACK,0x00000000,ERROR_RESET_ACK);
			Comm_Error_Init();
			break;
		}

		case CAN_COMM_CAL_DATA_READ:
		{
		        calResault = Comm_CAN_Cal_Data_Read(msg->data);
			UART_TX_Queue_Add(msg->cmd,calResault,0);
			break;
		}

		case CAN_COMM_CAL_DATA_WRITE:
		{
		        calResault = Comm_CAN_Cal_Data_Write(msg->data);
			UART_TX_Queue_Add(msg->cmd,calResault,0);
			break;
		}	
		
		case COMM_TANK_TEST:	
		{
			Tank_Test_Mode = _ON;	
			Exp_Limit_Err_tm = 600000;
			break;
		}
		
		default:
		{
			//printf("CAN_COMM Error \n");	
		}
	}
}
/************************************************************************************************************/
void RXDataComm(unsigned char data)
{
      static unsigned int index=0;
      static unsigned char buff[BUFFER_SIZE]={0, };
      static unsigned int Uart_Start=0;
      
      if(data == STX && Uart_Start == OFF)     //패킷의 맨 처음부분이 들어왔을때 초기화시키고 플래그를  줌
      {
              index = 0;
              buff[index++] = data;
              Uart_Start = ON;
              
              return;
       }     
      
      else if(Uart_Start == ON)     //첫번째값이 지나고 다음값부터 들어옴
      {
              buff[index++] = data;
              
              if(index == PACKET_LENGTH_INDEX)
              {                      
                      Uart_Start = OFF;	// 통신이 일단 종료되었으므로 다음 STX를 잡기 위해 리셋 
                      memset(&UART_RX_MSG, 0x00, sizeof(UART_RX_MSG));	// 내부 Packet 초기화	
                      memcpy(&UART_RX_MSG, &buff, sizeof(UART_RX_MSG));	// 내부 Packet에 복사                      
                      MsgDataConvert(&UART_RX_MSG, buff);

                      if(data != ETX)
                      {	

                      }

                      else if(buff[CS_buff] != FuncCalcCheckSum(&buff[COMMAND]))
                      {   

                      }                       
                                                                 
                      else UART_CMD_Run(&UART_RX_MSG);

                      //memset(&UART_RX_MSG, 0x00, sizeof(UART_RX_MSG));	// 내부 Packet 초기화
                      memset(buff,0x00,sizeof(buff));
               }             
        }
 }
/************************************************************************************************************/
void UART_TX_Queue_Add(uint16_t cmd, uint32_t data, uint16_t debug)
{
#if 0 
        UART_TX_PACKET.buf[UART_TX_PACKET.wr_point][0] = STX;
        UART_TX_PACKET.buf[UART_TX_PACKET.wr_point][1] = (uint8_t)(cmd >> 8);
        UART_TX_PACKET.buf[UART_TX_PACKET.wr_point][2] = (uint8_t)(cmd);
        UART_TX_PACKET.buf[UART_TX_PACKET.wr_point][3] = (uint8_t)(data >> 24);
        UART_TX_PACKET.buf[UART_TX_PACKET.wr_point][4] = (uint8_t)(data >> 16);
        UART_TX_PACKET.buf[UART_TX_PACKET.wr_point][5] = (uint8_t)(data >> 8);
        UART_TX_PACKET.buf[UART_TX_PACKET.wr_point][6] = (uint8_t)(data);
        UART_TX_PACKET.buf[UART_TX_PACKET.wr_point][7] = (uint8_t)(debug >> 8);
        UART_TX_PACKET.buf[UART_TX_PACKET.wr_point][8] = (uint8_t)(debug);
        UART_TX_PACKET.buf[UART_TX_PACKET.wr_point][9] = FuncCalcCheckSum(&UART_TX_PACKET.buf[UART_TX_PACKET.wr_point][COMMAND]);
        UART_TX_PACKET.buf[UART_TX_PACKET.wr_point][10] = ETX;
        
        UART_TX_PACKET.wr_point++;

        if (UART_TX_PACKET.wr_point >= UART_QSIZE)
        {
            UART_TX_PACKET.wr_point = 0;
        }
 #else
        UART_TX_PACKET.buf[0][0] = STX;
        UART_TX_PACKET.buf[0][1] = (uint8_t)(cmd >> 8);
        UART_TX_PACKET.buf[0][2] = (uint8_t)(cmd);
        UART_TX_PACKET.buf[0][3] = (uint8_t)(data >> 24);
        UART_TX_PACKET.buf[0][4] = (uint8_t)(data >> 16);
        UART_TX_PACKET.buf[0][5] = (uint8_t)(data >> 8);
        UART_TX_PACKET.buf[0][6] = (uint8_t)(data);
        UART_TX_PACKET.buf[0][7] = (uint8_t)(debug >> 8);
        UART_TX_PACKET.buf[0][8] = (uint8_t)(debug);
        UART_TX_PACKET.buf[0][9] = FuncCalcCheckSum(&UART_TX_PACKET.buf[0][COMMAND]);
        UART_TX_PACKET.buf[0][10] = ETX;     
        
        void *ptr;
  
	UART_TX_MSG.stx = UART_TX_PACKET.buf[0][0];
	UART_TX_MSG.cmd1 = UART_TX_PACKET.buf[0][1];
	UART_TX_MSG.cmd2 = UART_TX_PACKET.buf[0][2];
	UART_TX_MSG.data1 = UART_TX_PACKET.buf[0][3];
	UART_TX_MSG.data2 = UART_TX_PACKET.buf[0][4];
	UART_TX_MSG.data3 = UART_TX_PACKET.buf[0][5];
	UART_TX_MSG.data4 = UART_TX_PACKET.buf[0][6];
	UART_TX_MSG.debug1 = UART_TX_PACKET.buf[0][7];
	UART_TX_MSG.debug2 = UART_TX_PACKET.buf[0][8];
	UART_TX_MSG.cs = UART_TX_PACKET.buf[0][9];
	UART_TX_MSG.etx = UART_TX_PACKET.buf[0][10];
        
        ptr = &UART_TX_MSG;

        HAL_Delay(1);

	while(HAL_UART_Transmit_IT(&huart1, ptr, PACKET_LENGTH_INDEX)==HAL_OK);
#endif       
}
/************************************************************************************************************/
void UART_TX_Data()
{       
        void *ptr;

        CommLed.tx_count = 8;
        COMM_LED_ON;        
  
	UART_TX_MSG.stx = UART_TX_PACKET.buf[UART_TX_PACKET.rd_point][0];
	UART_TX_MSG.cmd1 = UART_TX_PACKET.buf[UART_TX_PACKET.rd_point][1];
	UART_TX_MSG.cmd2 = UART_TX_PACKET.buf[UART_TX_PACKET.rd_point][2];
	UART_TX_MSG.data1 = UART_TX_PACKET.buf[UART_TX_PACKET.rd_point][3];
	UART_TX_MSG.data2 = UART_TX_PACKET.buf[UART_TX_PACKET.rd_point][4];
	UART_TX_MSG.data3 = UART_TX_PACKET.buf[UART_TX_PACKET.rd_point][5];
	UART_TX_MSG.data4 = UART_TX_PACKET.buf[UART_TX_PACKET.rd_point][6];
	UART_TX_MSG.debug1 = UART_TX_PACKET.buf[UART_TX_PACKET.rd_point][7];
	UART_TX_MSG.debug2 = UART_TX_PACKET.buf[UART_TX_PACKET.rd_point][8];
	UART_TX_MSG.cs = UART_TX_PACKET.buf[UART_TX_PACKET.rd_point][9];
	UART_TX_MSG.etx = UART_TX_PACKET.buf[UART_TX_PACKET.rd_point][10];
        
        ptr = &UART_TX_MSG;
        
        HAL_Delay(1);

	while(HAL_UART_Transmit_IT(&huart1, ptr, PACKET_LENGTH_INDEX)==HAL_OK);
}
/************************************************************************************************************/
uint8_t UART_TX_Queue_Process()
{
	if(UART_TX_PACKET.wr_point != UART_TX_PACKET.rd_point)
        {		        
                UART_TX_Data();
                
		for(int i=0; i<UART_MAX_DATA; i++)
		{
                        UART_TX_PACKET.buf[UART_TX_PACKET.rd_point][i] = NULL;
                }

		UART_TX_PACKET.rd_point++;

		if (UART_TX_PACKET.rd_point >= UART_QSIZE)     UART_TX_PACKET.rd_point = 0;

		return (1);
	}
	else{ return(0); }
}
/************************************************************************************************************/
void UART_TX_Handler(void)
{
        if(T_Comm_Tx_Timer > 9)
        {
                UART_TX_Queue_Process();
                T_Comm_Tx_Timer = 0;
        }
}

/************************************************************************************************************/
void UART_RX_Handler(void)
{  
      unsigned char recieve_data=0;
      
      if(Packet_Empty()==NO)    //데이터값이 비어있지 않으면 데이터를 디큐로 받아오고 함수로 넘겨줌
      {
            CommLed.rx_count = 8;
            COMM_LED_ON;
      
            recieve_data=Uart_DeQueue();
            RXDataComm(recieve_data);
      }
}
/************************************************************************************************************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)
	{
		while(HAL_UART_Receive_IT(&huart1, (uint8_t*)U_PACKET.rx_byte, 1)==HAL_OK)
		Uart_EnQueue(U_PACKET.rx_byte[0]);
	}

	if(huart->Instance == USART3)
	{
		while(HAL_UART_Receive_IT(&huart3, (uint8_t*)P_PACKET.rx_byte, 1)==HAL_OK)		
			
		P_PACKET.rx_buf[P_PACKET.wr_point][P_PACKET.buf_cnt] = P_PACKET.rx_byte[0];
		P_PACKET.buf_cnt++;

		if(P_PACKET.rx_buf[P_PACKET.wr_point][0] != PRINT_STX) 
		{
			P_PACKET.buf_cnt = 0;
		}

		if(P_PACKET.rx_buf[P_PACKET.wr_point][P_PACKET.buf_cnt-1] == PRINT_ETX)
		{
			P_PACKET.wr_point++;
			P_PACKET.buf_cnt = 0;
		}
        
		if (P_PACKET.wr_point >= UART_QSIZE)
		{
			P_PACKET.wr_point = 0;
			P_PACKET.buf_cnt = 0;
		}
	}

}
