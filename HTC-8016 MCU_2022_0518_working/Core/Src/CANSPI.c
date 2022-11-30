#include "CANSPI.h"
#include "MCP2515.h"
#include <stdio.h>
#include "history.h"
#include "operation.h"
#include "command.h"
#include "main.h"
#include "exp.h"
#include "adc.h"

/** Local Function Prototypes */  
static uint32_t convertReg2ExtendedCANid(uint8_t tempRXBn_EIDH, uint8_t tempRXBn_EIDL, uint8_t tempRXBn_SIDH, uint8_t tempRXBn_SIDL);
static uint32_t convertReg2StandardCANid(uint8_t tempRXBn_SIDH, uint8_t tempRXBn_SIDL) ;
static void convertCANid2Reg(uint32_t tempPassedInID, uint8_t canIdType, id_reg_t *passedIdReg);

/** Local Variables */ 
ctrl_status_t ctrlStatus;
ctrl_error_status_t errorStatus;
id_reg_t idReg;

CAN_Q CAN_RX_PACKET;
CAN_Q CAN_TX_PACKET;

uCAN_MSG txMessage;
uCAN_MSG rxMessage;

/** CAN SPI APIs */ 

/* Sleep 모드 진입 */
void CANSPI_Sleep(void)
{
  /* Clear CAN bus wakeup interrupt */
  MCP2515_BitModify(MCP2515_CANINTF, 0x40, 0x00);        
  
  /* Enable CAN bus activity wakeup */
  MCP2515_BitModify(MCP2515_CANINTE, 0x40, 0x40);        
  
  MCP2515_SetSleepMode();
}

/* CAN 통신 초기화 */
bool CANSPI_Initialize(void)
{
  RXF0 RXF0reg;
  RXF1 RXF1reg;
  RXF2 RXF2reg;
  RXF3 RXF3reg;
  RXF4 RXF4reg;
  RXF5 RXF5reg;
  RXM0 RXM0reg;
  RXM1 RXM1reg;

/*
	[  Mask & Filter truth table ]
	 Mask │Filter│  ID  │Return
	  Bit │  Bit │  Bit │
	───┼───┼───┼───
	  0   │  x   │  x   │Accept
	  1   │  0   │  0   │Accept
	  1   │  0   │  1   │Reject
	  1   │  1   │  0   │Reject
	  1   │  1   │  1   │Accept

	==== Vatech PCT-2400 ID setting ====
	TX ID : 0x12FF3401
	RX ID : 0x12FF3402
	     => 0b1 0010 1111 1111 0011 0100 0000 0010

	-------------------------------- Mask setting -------------------------------
	Reg H = [	SID10 	SID9 	SID8 	SID7 	SID6 	SID5 	SID4 	SID3	]
	Reg L = [	SID2 	SID1 	SID0 	  -		  -	 	  -		EID17 	EID16	]
	Reg 8 = [	EID15 	EID14	EID13	EID12	EID11	EID10	EID9 	EID8	]
	Reg 0 = [	EID7  	EID6 	EID5 	EID4 	EID3 	EID2 	EID1 	EID0	]

	- = 0
	-----------------------------------------------------------------------------

	Reg H = [	1		1		1		1		1		1		1		1		]
	Reg L = [	1		1		1		0		0		0		1		1		]
	Reg 8 = [	1		1		1		1		1		1		1		1		]
	Reg 0 = [	1		1		1		1		1		1		1		1		]

	RX Mask reg setting
	Reg H : 0b 1111 1111
	Reg L : 0b 1110 0011
	Reg 0 : 0b 1111 1111
	Reg 8 : 0b 1111 1111


	------------------------------- Filter setting ------------------------------
	Reg H = [	SID10 	SID9 	SID8 	SID7 	SID6 	SID5 	SID4 	SID3	]
	Reg L = [	SID2 	SID1 	SID0 	  -		EXIDE 	  -		EID17 	EID16	]
	Reg 8 = [	EID15 	EID14	EID13	EID12	EID11	EID10	EID9 	EID8	]
	Reg 0 = [	EID7  	EID6 	EID5 	EID4 	EID3 	EID2 	EID1 	EID0	]

	EXIDE = 1(Extended ID Enable)
	- = 0
	-----------------------------------------------------------------------------

	Reg H = [	1 		0 		0 		1 		0 		1 		1 		1		]
	Reg L = [	1		1		1		0		1		0		1		1		]
	Reg 8 = [	0		0		1		1		0		1		0		0		]
	Reg 0 = [	0		0		0		0		0		0		1		0		]

	RX Filter reg setting
	Reg H : 0b 1001 0111 
	Reg L : 0b 1110 1011
	Reg 8 : 0b 0011 0100
	Reg 0 : 0b 0000 0010
*/

  /* Rx Mask values 초기화 */
  RXM0reg.RXM0SIDH = 0xFF;
  RXM0reg.RXM0SIDL = 0xE3;
  RXM0reg.RXM0EID8 = 0xFF;
  RXM0reg.RXM0EID0 = 0xFF;
  
  RXM1reg.RXM1SIDH = 0xFF;
  RXM1reg.RXM1SIDL = 0xE3;
  RXM1reg.RXM1EID8 = 0xFF;
  RXM1reg.RXM1EID0 = 0xFF;
  
  /* Rx Filter values 초기화 _active Filter0 (RXB0/1CTRL Reg setting) */
  RXF0reg.RXF0SIDH = 0x97;      
  RXF0reg.RXF0SIDL = 0xEB;
  RXF0reg.RXF0EID8 = 0x34;
  RXF0reg.RXF0EID0 = 0x02;
  
  RXF1reg.RXF1SIDH = 0x00;
  RXF1reg.RXF1SIDL = 0x00;
  RXF1reg.RXF1EID8 = 0x00;
  RXF1reg.RXF1EID0 = 0x00;
  
  RXF2reg.RXF2SIDH = 0x00;
  RXF2reg.RXF2SIDL = 0x00;
  RXF2reg.RXF2EID8 = 0x00;
  RXF2reg.RXF2EID0 = 0x00;
  
  RXF3reg.RXF3SIDH = 0x00;
  RXF3reg.RXF3SIDL = 0x00;
  RXF3reg.RXF3EID8 = 0x00;
  RXF3reg.RXF3EID0 = 0x00;
  
  RXF4reg.RXF4SIDH = 0x00;
  RXF4reg.RXF4SIDL = 0x00;
  RXF4reg.RXF4EID8 = 0x00;
  RXF4reg.RXF4EID0 = 0x00;
  
  RXF5reg.RXF5SIDH = 0x00;
  RXF5reg.RXF5SIDL = 0x18;
  RXF5reg.RXF5EID8 = 0x00;
  RXF5reg.RXF5EID0 = 0x00;

  /* MCP2515 초기화 , SPI 통신 상태 확인 */
  if(!MCP2515_Initialize())
    return false;
    
  /* Configuration 모드로 설정 */
  if(!MCP2515_SetConfigMode())
    return false;
  
  /* Filter & Mask 값 설정 */
  MCP2515_WriteByteSequence(MCP2515_RXM0SIDH, MCP2515_RXM0EID0, &(RXM0reg.RXM0SIDH));
  MCP2515_WriteByteSequence(MCP2515_RXM1SIDH, MCP2515_RXM1EID0, &(RXM1reg.RXM1SIDH));
  MCP2515_WriteByteSequence(MCP2515_RXF0SIDH, MCP2515_RXF0EID0, &(RXF0reg.RXF0SIDH));
  MCP2515_WriteByteSequence(MCP2515_RXF1SIDH, MCP2515_RXF1EID0, &(RXF1reg.RXF1SIDH));
  MCP2515_WriteByteSequence(MCP2515_RXF2SIDH, MCP2515_RXF2EID0, &(RXF2reg.RXF2SIDH));
  MCP2515_WriteByteSequence(MCP2515_RXF3SIDH, MCP2515_RXF3EID0, &(RXF3reg.RXF3SIDH));
  MCP2515_WriteByteSequence(MCP2515_RXF4SIDH, MCP2515_RXF4EID0, &(RXF4reg.RXF4SIDH));
  MCP2515_WriteByteSequence(MCP2515_RXF5SIDH, MCP2515_RXF5EID0, &(RXF5reg.RXF5SIDH));
  
  /* Accept All (Standard + Extended) */
  MCP2515_WriteByte(MCP2515_RXB0CTRL, 0x04);    //Enable BUKT, Accept Filter 0
  MCP2515_WriteByte(MCP2515_RXB1CTRL, 0x00);    //Accept Filter 0

  /* 
  * Tq     = 2 * (brp(0) + 1) / 8000000 (8Mhz OSC) = 0.250us
  * Tbit   = (SYNC_SEG(1 fixed) + PROP_SEG + PS1 + PS2)
  *        = 1tq + 3tq + 1tq + 3tq = 8tq
  * CAN Fs = 0.25us * 16tq = 2us = 500kbps
  *
  * 8MHz | 500 kbps setting CNF1 : 0x00, CNF2 : 0x09, CNF3 : 0x02 
  */
  MCP2515_WriteByte(MCP2515_CNF1, 0x00);
  MCP2515_WriteByte(MCP2515_CNF2, 0x90);
  MCP2515_WriteByte(MCP2515_CNF3, 0x02);

  MCP2515_WriteByte(MCP2515_CANINTE, 0x03);     //rx interrupt enable        
  
  /* Normal 모드로 설정 */
  if(!MCP2515_SetNormalMode())
    return false;
  
  return true;
}

/* CAN 메시지 전송 */
uint8_t CANSPI_Transmit(uCAN_MSG *tempCanMsg) 
{
  uint8_t returnValue = 0;

  CommLed.tx_count = 8;
  COMM_LED_ON;
  
  idReg.tempSIDH = 0;
  idReg.tempSIDL = 0;
  idReg.tempEID8 = 0;
  idReg.tempEID0 = 0;
  
  ctrlStatus.ctrl_status = MCP2515_ReadStatus();
  
  /* 현재 Transmission 이 Pending 되지 않은 버퍼를 찾아서 전송한다. */
  if (ctrlStatus.TXB0REQ != 1)
  {
	/* ID Type 에 맞게 변환 */
	convertCANid2Reg(tempCanMsg->frame.id, tempCanMsg->frame.idType, &idReg);
    
    /* Tx Buffer에 전송할 데이터 Loading */
    MCP2515_LoadTxSequence(MCP2515_LOAD_TXB0SIDH, &(idReg.tempSIDH), tempCanMsg->frame.dlc, &(tempCanMsg->frame.data0));
    
    /* Tx Buffer의 데이터 전송 요청 */
    MCP2515_RequestToSend(MCP2515_RTS_TX0);
    
    returnValue = 1;
  }
  else if (ctrlStatus.TXB1REQ != 1)
  {
    convertCANid2Reg(tempCanMsg->frame.id, tempCanMsg->frame.idType, &idReg);
    
    MCP2515_LoadTxSequence(MCP2515_LOAD_TXB1SIDH, &(idReg.tempSIDH), tempCanMsg->frame.dlc, &(tempCanMsg->frame.data0));
    MCP2515_RequestToSend(MCP2515_RTS_TX1);
    
    returnValue = 1;
  }
  else if (ctrlStatus.TXB2REQ != 1)
  {
    convertCANid2Reg(tempCanMsg->frame.id, tempCanMsg->frame.idType, &idReg);
    
    MCP2515_LoadTxSequence(MCP2515_LOAD_TXB2SIDH, &(idReg.tempSIDH), tempCanMsg->frame.dlc, &(tempCanMsg->frame.data0));
    MCP2515_RequestToSend(MCP2515_RTS_TX2);
    
    returnValue = 1;
  }
  
  return (returnValue);
}

/* CAN 메시지 수신 */
uint8_t CANSPI_Receive(uCAN_MSG *tempCanMsg) 
{
  uint8_t returnValue = 0;
  rx_reg_t rxReg;
  ctrl_rx_status_t rxStatus;
  
  rxStatus.ctrl_rx_status = MCP2515_GetRxStatus();
  
  /* 버퍼에 수신된 메시지가 있는지 확인  */
  if (rxStatus.rxBuffer != 0)
  {
    /* 어떤 버퍼에 메시지가 있는지 확인 후 처리 */
    if ((rxStatus.rxBuffer == MSG_IN_RXB0)|(rxStatus.rxBuffer == MSG_IN_BOTH_BUFFERS))
    {
      MCP2515_ReadRxSequence(MCP2515_READ_RXB0SIDH, rxReg.rx_reg_array, sizeof(rxReg.rx_reg_array));
    }
    else if (rxStatus.rxBuffer == MSG_IN_RXB1)
    {
      MCP2515_ReadRxSequence(MCP2515_READ_RXB1SIDH, rxReg.rx_reg_array, sizeof(rxReg.rx_reg_array));
    }
    
    /* Extended 타입 */
    if (rxStatus.msgType == dEXTENDED_CAN_MSG_ID_2_0B)
    {
      tempCanMsg->frame.idType = (uint8_t) dEXTENDED_CAN_MSG_ID_2_0B;
      tempCanMsg->frame.id = convertReg2ExtendedCANid(rxReg.RXBnEID8, rxReg.RXBnEID0, rxReg.RXBnSIDH, rxReg.RXBnSIDL);
    } 
    else 
    {
      /* Standard 타입 */
      tempCanMsg->frame.idType = (uint8_t) dSTANDARD_CAN_MSG_ID_2_0B;
      tempCanMsg->frame.id = convertReg2StandardCANid(rxReg.RXBnSIDH, rxReg.RXBnSIDL);
    }
    
    tempCanMsg->frame.dlc   = rxReg.RXBnDLC;
    tempCanMsg->frame.data0 = rxReg.RXBnD0;
    tempCanMsg->frame.data1 = rxReg.RXBnD1;
    tempCanMsg->frame.data2 = rxReg.RXBnD2;
    tempCanMsg->frame.data3 = rxReg.RXBnD3;
    tempCanMsg->frame.data4 = rxReg.RXBnD4;
    tempCanMsg->frame.data5 = rxReg.RXBnD5;
    tempCanMsg->frame.data6 = rxReg.RXBnD6;
    tempCanMsg->frame.data7 = rxReg.RXBnD7;
    
    returnValue = 1;
  }
  
  return (returnValue);
}

/* 수신 버퍼에 메시지가 있는지 체크 */
uint8_t CANSPI_messagesInBuffer(void)
{
  uint8_t messageCount = 0;
  
  ctrlStatus.ctrl_status = MCP2515_ReadStatus();
  
  if(ctrlStatus.RX0IF != 0)
  {
    messageCount++;
  }
  
  if(ctrlStatus.RX1IF != 0)
  {
    messageCount++;
  }
  
  return (messageCount);
}

/* CAN BUS 가 Offline 인지 체크 */
uint8_t CANSPI_isBussOff(void)
{
  uint8_t returnValue = 0;
  
  errorStatus.error_flag_reg = MCP2515_ReadByte(MCP2515_EFLG);
  
  if(errorStatus.TXBO == 1)
  {
    returnValue = 1;
  }
  
  return (returnValue);
}

/* Rx Passive Error 상태인지 체크 */
uint8_t CANSPI_isRxErrorPassive(void)
{
  uint8_t returnValue = 0;
  
  errorStatus.error_flag_reg = MCP2515_ReadByte(MCP2515_EFLG);
  
  if(errorStatus.RXEP == 1)
  {
    returnValue = 1;
  }
  
  return (returnValue);
}

/* Tx Passive Error 상태인지 체크 */
uint8_t CANSPI_isTxErrorPassive(void)
{
  uint8_t returnValue = 0;
  
  errorStatus.error_flag_reg = MCP2515_ReadByte(MCP2515_EFLG);
  
  if(errorStatus.TXEP == 1)
  {
    returnValue = 1;
  }
  
  return (returnValue);
}

/* Register 저장값을 Extended ID 타입으로 변환하기 위한 함수 */
static uint32_t convertReg2ExtendedCANid(uint8_t tempRXBn_EIDH, uint8_t tempRXBn_EIDL, uint8_t tempRXBn_SIDH, uint8_t tempRXBn_SIDL) 
{
  uint32_t returnValue = 0;
  uint32_t ConvertedID = 0;
  uint8_t CAN_standardLo_ID_lo2bits;
  uint8_t CAN_standardLo_ID_hi3bits;
  
  CAN_standardLo_ID_lo2bits = (tempRXBn_SIDL & 0x03);
  CAN_standardLo_ID_hi3bits = (tempRXBn_SIDL >> 5);
  ConvertedID = (tempRXBn_SIDH << 3);
  ConvertedID = ConvertedID + CAN_standardLo_ID_hi3bits;
  ConvertedID = (ConvertedID << 2);
  ConvertedID = ConvertedID + CAN_standardLo_ID_lo2bits;
  ConvertedID = (ConvertedID << 8);
  ConvertedID = ConvertedID + tempRXBn_EIDH;
  ConvertedID = (ConvertedID << 8);
  ConvertedID = ConvertedID + tempRXBn_EIDL;
  returnValue = ConvertedID;    
  return (returnValue);
}

/* Register 저장값을 Standard ID 타입으로 변환하기 위한 함수 */
static uint32_t convertReg2StandardCANid(uint8_t tempRXBn_SIDH, uint8_t tempRXBn_SIDL) 
{
  uint32_t returnValue = 0;
  uint32_t ConvertedID;
  
  ConvertedID = (tempRXBn_SIDH << 3);
  ConvertedID = ConvertedID + (tempRXBn_SIDL >> 5);
  returnValue = ConvertedID;
  
  return (returnValue);
}

/* CAN ID를 Register에 저장하기 위한 변환 함수 */
static void convertCANid2Reg(uint32_t tempPassedInID, uint8_t canIdType, id_reg_t *passedIdReg) 
{
  uint8_t wipSIDL = 0;
  
  if (canIdType == dEXTENDED_CAN_MSG_ID_2_0B) 
  {
    //EID0
    passedIdReg->tempEID0 = 0xFF & tempPassedInID;
    tempPassedInID = tempPassedInID >> 8;
    
    //EID8
    passedIdReg->tempEID8 = 0xFF & tempPassedInID;
    tempPassedInID = tempPassedInID >> 8;
    
    //SIDL
    wipSIDL = 0x03 & tempPassedInID;
    tempPassedInID = tempPassedInID << 3;
    wipSIDL = (0xE0 & tempPassedInID) + wipSIDL;
    wipSIDL = wipSIDL + 0x08;
    passedIdReg->tempSIDL = 0xEB & wipSIDL;
    
    //SIDH
    tempPassedInID = tempPassedInID >> 8;
    passedIdReg->tempSIDH = 0xFF & tempPassedInID;
  } 
  else
  {
    passedIdReg->tempEID8 = 0;
    passedIdReg->tempEID0 = 0;
    tempPassedInID = tempPassedInID << 5;
    passedIdReg->tempSIDL = 0xFF & tempPassedInID;
    tempPassedInID = tempPassedInID >> 8;
    passedIdReg->tempSIDH = 0xFF & tempPassedInID;
  }
  
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////


void CAN_TX_TEST(void)
{
	txMessage.frame.idType = dEXTENDED_CAN_MSG_ID_2_0B;
	txMessage.frame.id = 0x12FF3401;
	txMessage.frame.dlc = 8;
	txMessage.frame.data0 = 0xff;
	txMessage.frame.data1 = 0x00;
	txMessage.frame.data2 = 0xff;
	txMessage.frame.data3 = 0x00;
	txMessage.frame.data4 = 0xff;
	txMessage.frame.data5 = 0x00;
	txMessage.frame.data6 = 0xff;
	txMessage.frame.data7 = 0x00;

	while(!CANSPI_Transmit(&txMessage))
	{
	        
	}  
}

uint32_t TestCommand[10];
uint32_t TestData;
uint32_t TestDebug;

void CAN_Response(uint32_t Comm, uint32_t data, uint32_t debug)
{    
	txMessage.frame.idType = dEXTENDED_CAN_MSG_ID_2_0B;
	txMessage.frame.id = 0x12FF3401;
	txMessage.frame.dlc = 8;
	txMessage.frame.data0 = (uint8_t)(Comm>>8);
	txMessage.frame.data1 = (uint8_t)(Comm);
	txMessage.frame.data2 = (uint8_t)(data>>24);
	txMessage.frame.data3 = (uint8_t)(data>>16);
	txMessage.frame.data4 = (uint8_t)(data>>8);
	txMessage.frame.data5 = (uint8_t)(data);
	txMessage.frame.data6 = (uint8_t)(debug>>8);
	txMessage.frame.data7 = (uint8_t)(debug);	

	while(!CANSPI_Transmit(&txMessage))
	{
	        
	}        
	//printf("%x\r\n",Comm);
	//printf("%x\r\n",debug);
}

void CAN_TX_Data(void)
{
	txMessage.frame.idType = dEXTENDED_CAN_MSG_ID_2_0B;
	txMessage.frame.id = 0x12FF3401;
	txMessage.frame.dlc = 8;
	txMessage.frame.data0 = CAN_TX_PACKET.buf[CAN_TX_PACKET.rd_point][0];
	txMessage.frame.data1 = CAN_TX_PACKET.buf[CAN_TX_PACKET.rd_point][1];
	txMessage.frame.data2 = CAN_TX_PACKET.buf[CAN_TX_PACKET.rd_point][2];
	txMessage.frame.data3 = CAN_TX_PACKET.buf[CAN_TX_PACKET.rd_point][3];
	txMessage.frame.data4 = CAN_TX_PACKET.buf[CAN_TX_PACKET.rd_point][4];
	txMessage.frame.data5 = CAN_TX_PACKET.buf[CAN_TX_PACKET.rd_point][5];
	txMessage.frame.data6 = CAN_TX_PACKET.buf[CAN_TX_PACKET.rd_point][6];
	txMessage.frame.data7 = CAN_TX_PACKET.buf[CAN_TX_PACKET.rd_point][7];
#if 1
	while(!CANSPI_Transmit(&txMessage))
	{
	        
	}	
#else
        CANSPI_Transmit(&txMessage);
#endif
	//printf("H S : %x", CAN_TX_PACKET.buf[CAN_TX_PACKET.rd_point][7]);
}

/**
  * @brief  CAN RX Handler
  *
  * @note   전송 받은 데이터를 Queue에 쌓고, 유효성 확인 후 명령어 실행 
  *
  * @param  NONE
  *
  * @retval None
  */
void CAN_RX_Handler(void)
{
 	uint32_t RX_CAN[CAN_PROTOCOL] = {0, 0, 0};
	
	if(CAN_INT_CHK == _OFF) CAN_RX_Queue_Add();
	if(!(CAN_RX_Queue_Process(RX_CAN))) {return;}
    
	CAN_CMD_Run(RX_CAN);
}

void CAN_TX_Handler(void)
{
	CAN_TX_Queue_Process();
}

/**
  * @brief  CAN RX Handler
  *
  * @note   전송 받은 데이터를 Queue에 쌓는 함수
  *
  * @param  NONE
  *
  * @retval None
  */
void CAN_RX_Queue_Add(void)
{
	/*
		CAN -> SPI 로 들어온 데이터를 CAN_Queue에 쌓는다.
	*/
	//while(CANSPI_Receive(&rxMessage))
        if(CANSPI_Receive(&rxMessage))
	{	
		CAN_RX_PACKET.buf[CAN_RX_PACKET.wr_point][0]	= rxMessage.frame.data0;
		CAN_RX_PACKET.buf[CAN_RX_PACKET.wr_point][1]	= rxMessage.frame.data1;
		CAN_RX_PACKET.buf[CAN_RX_PACKET.wr_point][2]	= rxMessage.frame.data2;
		CAN_RX_PACKET.buf[CAN_RX_PACKET.wr_point][3]	= rxMessage.frame.data3;
		CAN_RX_PACKET.buf[CAN_RX_PACKET.wr_point][4]	= rxMessage.frame.data4;
		CAN_RX_PACKET.buf[CAN_RX_PACKET.wr_point][5]	= rxMessage.frame.data5;
		CAN_RX_PACKET.buf[CAN_RX_PACKET.wr_point][6]	= rxMessage.frame.data6;
		CAN_RX_PACKET.buf[CAN_RX_PACKET.wr_point][7]	= rxMessage.frame.data7;

		CAN_RX_PACKET.wr_point++;

	        if (CAN_RX_PACKET.wr_point >= CAN_QSIZE)
	        {
	            CAN_RX_PACKET.wr_point = 0;
	        }
	}	

	//printf("H R : %x", CAN_RX_PACKET.buf[CAN_RX_PACKET.rd_point][7]);
}

void CAN_TX_Queue_Add(uint16_t cmd, uint32_t data, uint16_t debug)
{
        CAN_TX_PACKET.buf[CAN_TX_PACKET.wr_point][0] = (uint8_t)(cmd >> 8);
        CAN_TX_PACKET.buf[CAN_TX_PACKET.wr_point][1] = (uint8_t)(cmd);
        CAN_TX_PACKET.buf[CAN_TX_PACKET.wr_point][2] = (uint8_t)(data >> 24);
        CAN_TX_PACKET.buf[CAN_TX_PACKET.wr_point][3] = (uint8_t)(data >> 16);
        CAN_TX_PACKET.buf[CAN_TX_PACKET.wr_point][4] = (uint8_t)(data >> 8);
        CAN_TX_PACKET.buf[CAN_TX_PACKET.wr_point][5] = (uint8_t)(data);
        CAN_TX_PACKET.buf[CAN_TX_PACKET.wr_point][6] = (uint8_t)(debug >> 8);
        CAN_TX_PACKET.buf[CAN_TX_PACKET.wr_point][7] = (uint8_t)(debug);
        
        CAN_TX_PACKET.wr_point++;

        if (CAN_TX_PACKET.wr_point >= CAN_QSIZE)
        {
            CAN_TX_PACKET.wr_point = 0;
        }
}
/**
  * @brief  CAN RX Handler
  *
  * @note   Queue의 데이터를 처리 
  *
  * @param  NONE
  *
  * @retval None
  */
uint8_t CAN_RX_Queue_Process(uint32_t *arr)
{
	uint8_t i;

	if(CAN_RX_PACKET.wr_point != CAN_RX_PACKET.rd_point)
        {		
                CommLed.rx_count = 8;
                COMM_LED_ON;
                
            	arr[CAN_COMM] 	=  (CAN_RX_PACKET.buf[CAN_RX_PACKET.rd_point][0]<<8)  | (CAN_RX_PACKET.buf[CAN_RX_PACKET.rd_point][1]);
        	arr[CAN_DATA] 	=  (CAN_RX_PACKET.buf[CAN_RX_PACKET.rd_point][2]<<24) | (CAN_RX_PACKET.buf[CAN_RX_PACKET.rd_point][3]<<16) | (CAN_RX_PACKET.buf[CAN_RX_PACKET.rd_point][4]<<8) | (CAN_RX_PACKET.buf[CAN_RX_PACKET.rd_point][5]);
        	arr[CAN_DEBUG]	=  (CAN_RX_PACKET.buf[CAN_RX_PACKET.rd_point][6]<<8)  | (CAN_RX_PACKET.buf[CAN_RX_PACKET.rd_point][7]);
		
		for(i=0; i<CAN_MAX_DATA; i++)
		{
                        CAN_RX_PACKET.buf[CAN_RX_PACKET.rd_point][i] = NULL;
                }
		CAN_RX_PACKET.rd_point++;

		if (CAN_RX_PACKET.rd_point >= CAN_QSIZE)     CAN_RX_PACKET.rd_point = 0;

		return (1);
	}
	else{ return(0); }
}

uint8_t CAN_TX_Queue_Process()
{
	if(CAN_TX_PACKET.wr_point != CAN_TX_PACKET.rd_point)
        {		        
                CAN_TX_Data();
                
		for(int i=0; i<CAN_MAX_DATA; i++)
		{
                        CAN_TX_PACKET.buf[CAN_TX_PACKET.rd_point][i] = NULL;
                }

		CAN_TX_PACKET.rd_point++;

		if (CAN_TX_PACKET.rd_point >= CAN_QSIZE)     CAN_TX_PACKET.rd_point = 0;

		return (1);
	}
	else{ return(0); }
}

/**
  * @brief  CAN RX Handler
  *
  * @note   Comm에 맞는 함수 실행 
  *
  * @param  NONE
  *
  * @retval None
  */
  
void CAN_CMD_Run(uint32_t *CAN)
{
        uint16_t sendThermaldata = 0;
        uint32_t calResault = 0;

	switch(CAN[CAN_COMM])
	{
		case CAN_COMM_kV_SETTING:	
		{
			CAN_TX_Queue_Add(CAN_COMM_ACK,Comm_kV_setting(CAN[CAN_DATA]),kV_SET_ACK);
			break;
		}
		
		case CAN_COMM_mA_SETTING:
		{	
			CAN_TX_Queue_Add(CAN_COMM_ACK,Comm_mA_setting(CAN[CAN_DATA]),mA_SET_ACK);	
			break;
		}

		case CAN_COMM_kV_FB_READ:
		{
			CAN_TX_Queue_Add(CAN[CAN_COMM],Comm_kV_FB_read(),CAN[CAN_DEBUG]);	
			break;
		}
		
		case CAN_COMM_mA_FB_READ:
		{
			CAN_TX_Queue_Add(CAN[CAN_COMM],Comm_mA_FB_read(),CAN[CAN_DEBUG]);
			break;
		}
		
		case CAN_COMM_XRAY_ON:
		{
			CAN_TX_Queue_Add(CAN_COMM_ACK,0x00000000,XRAY_ON_ACK);	
			
			break;
		}

		case CAN_COMM_XRAY_OFF:
		{
			CAN_TX_Queue_Add(CAN_COMM_ACK,0x00000000,XRAY_OFF_ACK);	
			
			break;
		}

		case CAN_COMM_HEATSINK_TEMP_READ:
		{			         
		        sendThermaldata = Thermal_CAN_ConV(g_std.heatsink_temp);		               		        
			CAN_TX_Queue_Add(CAN[CAN_COMM],sendThermaldata,0x0000);
			
			break;
		}		
		
		case CAN_COMM_TANK_TEMP_READ:
		{
                        sendThermaldata = Thermal_CAN_ConV(g_std.tank_temp);		
			CAN_TX_Queue_Add(CAN[CAN_COMM],sendThermaldata,0x0000);
			
			break;
		}
		
		case CAN_COMM_XRAY_ONLY_ON:
		{
			CAN_TX_Queue_Add(CAN_COMM_ACK,0x00000000,XRAY_ONLY_ON_ACK);
			
			break;
		}
		
		case CAN_COMM_XRAY_ONLY_OFF:
		{
			CAN_TX_Queue_Add(CAN_COMM_ACK,0x00000000,XRAY_ONLY_OFF_ACK);
			
			break;
		}
		
		case CAN_COMM_GEN_READY_OK:	
		{	
			CAN_TX_Queue_Add(CAN_COMM_ACK,CAN[CAN_DATA],0x000A);			
			break;
		}

		case CAN_COMM_VER_CHECK:
		{
			CAN_TX_Queue_Add(CAN_COMM_VER_CHECK, HTC8016_VERSION, 0x0000);
			break;
		}
		
		case CAN_COMM_PREHEAT_ON:
		{
			CAN_TX_Queue_Add(CAN_COMM_ACK, 0x00000000, PREHEAT_ON_ACK);
			
			break;
		}

		case CAN_COMM_NORMAL_MODE_ON:
		{	
			CAN_TX_Queue_Add(CAN_COMM_ACK,0x00000000,NORMAL_MODE_ON_ACK);
			g_opt.exp_opt = CONT;
			break;
		}

		case CAN_COMM_PPS_MODE_ON:
		{	
			CAN_TX_Queue_Add(CAN_COMM_ACK,0x00000000,PULSE_MODE_ON_ACK);
			g_opt.exp_opt = PPS;
			break;
		}

		case CAN_COMM_XRAY_REMOTE_MODE_ON:
		{
			CAN_TX_Queue_Add(CAN_COMM_ACK,0x00000000,REMOTE_MODE_ON_ACK);
			break;
		}
		
		case CAN_COMM_XRAY_REMOTE_MODE_OFF:
		{
			CAN_TX_Queue_Add(CAN_COMM_ACK,0x00000000,REMOTE_MODE_OFF_ACK);
			break;
		}
		
		case CAN_COMM_ERROR_INIT:
		{
			CAN_TX_Queue_Add(CAN_COMM_ACK,0x00000000,ERROR_RESET_ACK);
			Comm_Error_Init();
			break;
		}

		case CAN_COMM_CAL_DATA_READ:
		{
		        calResault = Comm_CAN_Cal_Data_Read(CAN[CAN_DATA]);
			CAN_TX_Queue_Add(CAN[CAN_COMM],calResault,0);
			break;
		}

		case CAN_COMM_CAL_DATA_WRITE:
		{
		        calResault = Comm_CAN_Cal_Data_Write(CAN[CAN_DATA]);
			CAN_TX_Queue_Add(CAN[CAN_COMM],calResault,0);
			break;
		}		
		
		default:
		{
			//printf("CAN_COMM Error \n");	
		}
	}
}


