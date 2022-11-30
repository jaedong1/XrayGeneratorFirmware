#include "main.h"
#include "stm32f4xx_hal.h"
#include "CANSPI.h"
#include "uart_debug.h"
#include <string.h>

#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)

PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit(&huart3, (uint8_t*)&ch, 1, 0xFFFF);
    return ch;
}

uint8_t Read_Print_Data(uint8_t *arr)
{
	unsigned char i;

	if (P_PACKET.wr_point != P_PACKET.rd_point)
	{
		for(i=0; i<UART_MAX_DATA; i++)
		{
			arr[i] = P_PACKET.rx_buf[P_PACKET.rd_point][i];
			P_PACKET.rx_buf[P_PACKET.rd_point][i] = NULL;
		}
		P_PACKET.rd_point++;

		if (P_PACKET.rd_point >= UART_QSIZE)     P_PACKET.rd_point = 0;

		return (1);
	}
	else
	{ 
		return(0); 
	}
}


// Syntex : 74 01 02 03 ... 73
void Debug_Handler(void)
{
	unsigned char		g_ucUartData[UART_MAX_DATA] = {0};
        	                   	
	if( !(Read_Print_Data(g_ucUartData) )  )
	{
		return;
	}

	switch (g_ucUartData[1])
	{
		case 0:
		{
			printf("==========================================\n");
			printf("   1 : Flash Memory \n");
			printf("   2 : mcp2515_init \n");			
			printf("   3 : CAN DATA Send \n");
			printf("   4 : CAN DATA RCV \n");
			printf("==========================================\n");
		}
		break;

		case 1:		//	Flash Test
		{
			if (g_ucUartData[2] == 0)		// Write
			{
				//ONE_Byte_Write(g_ucUartData[3], g_ucUartData[4], g_ucUartData[5]);
			}
			else if (g_ucUartData[2] == 1)	// Read
			{
				//printf (" 0x%x  \n", ONE_Byte_Read(g_ucUartData[3], g_ucUartData[4] ));
			}
			
			else if (g_ucUartData[2] == 2)	// write memory
			{
				//uint8_t t_array[10] = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
				//Write_Memory(g_ucUartData[3], g_ucUartData[4], g_ucUartData[5], (uint8_t *)t_array);
			}
			else if (g_ucUartData[2] == 3)	// read memory
			{
				uint8_t t_array[255] = {0,};
				uint8_t i=0;
			
				//Read_Memory(g_ucUartData[3], g_ucUartData[4], g_ucUartData[5], t_array);

				for (i=0 ; i< g_ucUartData[5] ; i++)
				{
					printf(" %d - 0x%x \n", i, t_array[i]);
				}
			}
			else if (g_ucUartData[2] == 4)
			{
				//uint8_t t_array[10] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
				//Write_Memory(0, 0, 0x0a, (uint8_t *)t_array);
			}

		}
		break;


		case 0x11:	
		{
			if ( CANSPI_Initialize() == true)
			{
				printf("CANSPI_Initialize \n");
			}
			else
			{
				printf("NG ! CANSPI_Initialize \n");
			}

		}
		break;

		case 0x12:	
		{
			CAN_TX_TEST();
			printf("CAN_TX_TEST \n");
		}
		break;

		case 0x13:	
		{
			MCP2515_Reset();
			printf("MCP2515_Reset \n");
		}
		break;
		
		case 0x14:	
		{
			uint8_t data = 0xff;
			data = MCP2515_ReadByte(g_ucUartData[2]);
			printf(" [0x%x] - 0x%x \n" , g_ucUartData[2], data);
		}
		break;

		case 0x15:	
		{
			MCP2515_WriteByte(g_ucUartData[2], g_ucUartData[3]);
		}
		break;
    }
}
/************************************************************************************************************/
