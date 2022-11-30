#include <stdio.h>
#include "history.h"

void Printf_Initialize(void)
{
        setvbuf(stdout, NULL, _IONBF, 0);
}

void ShowVersion(void)
{
	printf("\r\n-----------------------------------------------------------\r\n");	
	printf(" Project Name  : " PROJECT_NAME "\r\n");	
	printf(" Board   Name  : " BOARD_NAME "\r\n");
	printf(" App     Ver.  : %d.%d.%d\r\n", MAJOR_NUM, MINOR_NUM, PATCH_NUM);
	printf(" Date  (%s)   Time  (%s)   \r\n",__DATE__, __TIME__);
	printf("---------------------------------------------------------------\r\n");
}