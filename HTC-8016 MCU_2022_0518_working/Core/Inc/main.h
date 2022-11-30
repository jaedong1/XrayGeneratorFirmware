/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
// data type define
typedef unsigned char UBYTE;		//	1BYTE
typedef unsigned short int USINT;		//	2byte
typedef unsigned long int ULINT;		//	4byte


#define	BYTE		unsigned char
#define	WORD		unsigned int
#define	DWORD		unsigned long
#define	SBYTE		signed char
#define	SWORD		signed int
#define	SDWORD		signed long

typedef enum 
{
	false,true	
} bool;

#define _ON     1
#define _OFF    0

#define DIPSW_ON 0
#define DIPSW_OFF 1

#define CONT 0
#define PPS 1

#define TANK_TEST_MAX_kV_SETTING	88
#define TANK_TEST_MAX_mA_SETTING	16
#define TANK_TEST_MIN_mA_SETTING	1
#define MAX_kV_SETTING	80		// data 1 = 1kV
#define MIN_kV_SETTING	40
#define MAX_mA_SETTING	20		// data 1 = 0.01mA
#define MIN_mA_SETTING	4

#define kV_RANGE				7
#define mA_RANGE				20

#define SYSLED_ON HAL_GPIO_WritePin(SYS_LED_GPIO_Port, SYS_LED_Pin, GPIO_PIN_RESET)
#define SYSLED_OFF HAL_GPIO_WritePin(SYS_LED_GPIO_Port, SYS_LED_Pin, GPIO_PIN_SET) 
#define SYSLED_TOGGLE HAL_GPIO_TogglePin(SYS_LED_GPIO_Port, SYS_LED_Pin) 

#define BUZZER_ON HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET)
#define BUZZER_OFF HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET)
#define BUZZER_TOGGLE HAL_GPIO_TogglePin(BUZZER_GPIO_Port, BUZZER_Pin)

#define WDT_EN HAL_GPIO_WritePin(WDT_EN_GPIO_Port, WDT_EN_Pin, GPIO_PIN_RESET)
#define WDT_DIS HAL_GPIO_WritePin(WDT_EN_GPIO_Port, WDT_EN_Pin, GPIO_PIN_SET)

#define WDT_SIG HAL_GPIO_TogglePin(EXT_WDT_GPIO_Port, EXT_WDT_Pin)

#define EXP_SYNC_ON HAL_GPIO_WritePin(Exp_Sync_GPIO_Port, Exp_Sync_Pin, GPIO_PIN_RESET)
#define EXP_SYNC_OFF HAL_GPIO_WritePin(Exp_Sync_GPIO_Port, Exp_Sync_Pin, GPIO_PIN_SET)

#define PPS_MODE_ON HAL_GPIO_WritePin(PPS_MODE_GPIO_Port, PPS_MODE_Pin, GPIO_PIN_SET)
#define PPS_MODE_OFF	HAL_GPIO_WritePin(PPS_MODE_GPIO_Port, PPS_MODE_Pin, GPIO_PIN_RESET)

#define FIL_ON HAL_GPIO_WritePin(FIL_GPIO_Port, FIL_Pin, GPIO_PIN_SET)
#define FIL_OFF HAL_GPIO_WritePin(FIL_GPIO_Port, FIL_Pin, GPIO_PIN_RESET)

#define READY_DTC_ON	HAL_GPIO_WritePin(READY_GPIO_Port, READY_Pin, GPIO_PIN_SET)
#define READY_DTC_OFF	HAL_GPIO_WritePin(READY_GPIO_Port, READY_Pin, GPIO_PIN_RESET)

#define EXP_DTC_ON HAL_GPIO_WritePin(EXP_GPIO_Port, EXP_Pin, GPIO_PIN_SET)
#define EXP_DTC_OFF HAL_GPIO_WritePin(EXP_GPIO_Port, EXP_Pin, GPIO_PIN_RESET)

#define COMM_LED_ON HAL_GPIO_WritePin(Comm_LED_GPIO_Port, Comm_LED_Pin, GPIO_PIN_RESET)
#define COMM_LED_OFF HAL_GPIO_WritePin(Comm_LED_GPIO_Port, Comm_LED_Pin, GPIO_PIN_SET)

#define EXPOSURE_LED_ON HAL_GPIO_WritePin(Exposure_LED_GPIO_Port, Exposure_LED_Pin, GPIO_PIN_RESET)
#define EXPOSURE_LED_OFF HAL_GPIO_WritePin(Exposure_LED_GPIO_Port, Exposure_LED_Pin, GPIO_PIN_SET)

#define FILAMENT_LED_ON HAL_GPIO_WritePin(Filament_LED_GPIO_Port, Filament_LED_Pin, GPIO_PIN_RESET)
#define FILAMENT_LED_OFF HAL_GPIO_WritePin(Filament_LED_GPIO_Port, Filament_LED_Pin, GPIO_PIN_SET)

#define LOCK_LED_ON HAL_GPIO_WritePin(Lock_LED_GPIO_Port, Lock_LED_Pin, GPIO_PIN_RESET)
#define LOCK_LED_OFF HAL_GPIO_WritePin(Lock_LED_GPIO_Port, Lock_LED_Pin, GPIO_PIN_SET)

#define OVER_VOLTAGE_LED_ON HAL_GPIO_WritePin(Over_Voltage_LED_GPIO_Port, Over_Voltage_LED_Pin, GPIO_PIN_RESET)
#define OVER_VOLTAGE_LED_OFF HAL_GPIO_WritePin(Over_Voltage_LED_GPIO_Port, Over_Voltage_LED_Pin, GPIO_PIN_SET)

#define OVER_CURRENT_LED_ON HAL_GPIO_WritePin(Over_Current_LED_GPIO_Port, Over_Current_LED_Pin, GPIO_PIN_RESET)
#define OVER_CURRENT_LED_OFF HAL_GPIO_WritePin(Over_Current_LED_GPIO_Port, Over_Current_LED_Pin, GPIO_PIN_SET)

#define INTERLOCK_LED_ON HAL_GPIO_WritePin(Inter_lock_LED_GPIO_Port, Inter_lock_LED_Pin, GPIO_PIN_RESET)
#define INTERLOCK_LED_OFF HAL_GPIO_WritePin(Inter_lock_LED_GPIO_Port, Inter_lock_LED_Pin, GPIO_PIN_SET)

#define OVER_TEMP_LED_ON HAL_GPIO_WritePin(Over_Temp_LED_GPIO_Port, Over_Temp_LED_Pin, GPIO_PIN_RESET)
#define OVER_TEMP_LED_OFF HAL_GPIO_WritePin(Over_Temp_LED_GPIO_Port, Over_Temp_LED_Pin, GPIO_PIN_SET)

typedef struct
{
	uint8_t	rx_count;
	uint8_t	tx_count;
}STLedStatus;

typedef union 
{
	uint8_t	data;
	struct {
			uint8_t sw1_debugmode   : 1;
			uint8_t sw2_IAPmode     : 1;
			uint8_t sw3_buzzeroff   : 1;
			uint8_t sw4_noerror     : 1;
			uint8_t spare           : 4;
		} bit;
}UTDipSwitchControl;

typedef struct
{
	uint8_t exp_opt;
	uint8_t tank_err_tmp;
	uint8_t heatsink_err_tmp;
}Option_Global;

typedef struct
{
	float kV;
	uint16_t kv_dac;
	float mA;
	uint16_t ma_dac;
        uint16_t pre_dac;
        uint16_t pron_dac;
        uint16_t prof_dac;
	uint8_t stat;
	uint8_t focus;
}Xray_Global;

typedef struct
{
	float 	tank_temp;
	float 	heatsink_temp;
	uint32_t exp_cnt;
	uint8_t thermal_chk;
}Standard_Global;

typedef struct
{
	uint8_t kv;     //kV Calibration Data
	uint8_t mA;     //mA Calibration Data
	uint8_t prh;    //Preheat Calibration Data
	uint8_t pon;    //PPS On time mA Ref Calibration Data
	uint8_t pof;    //PPS Off time mA Ref Calibration Data
}
CAL_Global;

typedef struct
{
        int8_t kV_data[kV_RANGE];
        int8_t mA_data[kV_RANGE][mA_RANGE];
        int8_t PreHeat_data[kV_RANGE][mA_RANGE];
        int8_t PPS_ON_data[kV_RANGE][mA_RANGE];
        int8_t PPS_OFF_data[kV_RANGE][mA_RANGE];       
}STCalData;
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define CAN_RESET_Pin GPIO_PIN_3
#define CAN_RESET_GPIO_Port GPIOE
#define SPI4_CS_ON_1_Pin GPIO_PIN_13
#define SPI4_CS_ON_1_GPIO_Port GPIOC
#define EXT_WDT_Pin GPIO_PIN_0
#define EXT_WDT_GPIO_Port GPIOC
#define WDT_EN_Pin GPIO_PIN_1
#define WDT_EN_GPIO_Port GPIOC
#define kV_MON_Pin GPIO_PIN_0
#define kV_MON_GPIO_Port GPIOA
#define mA_MON_Pin GPIO_PIN_1
#define mA_MON_GPIO_Port GPIOA
#define FIL_MON_Pin GPIO_PIN_2
#define FIL_MON_GPIO_Port GPIOA
#define Tank_Thermal_ADC_Pin GPIO_PIN_3
#define Tank_Thermal_ADC_GPIO_Port GPIOA
#define mA_DAC_Pin GPIO_PIN_4
#define mA_DAC_GPIO_Port GPIOA
#define kV_DAC_Pin GPIO_PIN_5
#define kV_DAC_GPIO_Port GPIOA
#define Heatsink_Thermal_ADC_Pin GPIO_PIN_6
#define Heatsink_Thermal_ADC_GPIO_Port GPIOA
#define DC_MON_Pin GPIO_PIN_7
#define DC_MON_GPIO_Port GPIOA
#define Lock_LED_Pin GPIO_PIN_8
#define Lock_LED_GPIO_Port GPIOE
#define Exposure_LED_Pin GPIO_PIN_9
#define Exposure_LED_GPIO_Port GPIOE
#define Filament_LED_Pin GPIO_PIN_10
#define Filament_LED_GPIO_Port GPIOE
#define Comm_LED_Pin GPIO_PIN_11
#define Comm_LED_GPIO_Port GPIOE
#define Over_Voltage_LED_Pin GPIO_PIN_12
#define Over_Voltage_LED_GPIO_Port GPIOE
#define Over_Current_LED_Pin GPIO_PIN_13
#define Over_Current_LED_GPIO_Port GPIOE
#define Inter_lock_LED_Pin GPIO_PIN_14
#define Inter_lock_LED_GPIO_Port GPIOE
#define Over_Temp_LED_Pin GPIO_PIN_15
#define Over_Temp_LED_GPIO_Port GPIOE
#define SPI2_CS_ON_1_Pin GPIO_PIN_12
#define SPI2_CS_ON_1_GPIO_Port GPIOB
#define SYS_LED_Pin GPIO_PIN_13
#define SYS_LED_GPIO_Port GPIOB
#define EP_OVER_Pin GPIO_PIN_14
#define EP_OVER_GPIO_Port GPIOB
#define IP_OVER_Pin GPIO_PIN_15
#define IP_OVER_GPIO_Port GPIOB
#define DIP_SW1_Pin GPIO_PIN_8
#define DIP_SW1_GPIO_Port GPIOD
#define DIP_SW2_Pin GPIO_PIN_9
#define DIP_SW2_GPIO_Port GPIOD
#define DIP_SW3_Pin GPIO_PIN_10
#define DIP_SW3_GPIO_Port GPIOD
#define DIP_SW4_Pin GPIO_PIN_11
#define DIP_SW4_GPIO_Port GPIOD
#define Lock_Pin GPIO_PIN_0
#define Lock_GPIO_Port GPIOD
#define Ready_On_Pin GPIO_PIN_1
#define Ready_On_GPIO_Port GPIOD
#define EXP_On_Pin GPIO_PIN_2
#define EXP_On_GPIO_Port GPIOD
#define Exp_Sync_Pin GPIO_PIN_4
#define Exp_Sync_GPIO_Port GPIOD
#define Tank_Inter_Lock_Pin GPIO_PIN_5
#define Tank_Inter_Lock_GPIO_Port GPIOD
#define BUZZER_Pin GPIO_PIN_3
#define BUZZER_GPIO_Port GPIOB
#define FAN_Pin GPIO_PIN_4
#define FAN_GPIO_Port GPIOB
#define READY_Pin GPIO_PIN_5
#define READY_GPIO_Port GPIOB
#define EXP_Pin GPIO_PIN_6
#define EXP_GPIO_Port GPIOB
#define FIL_Pin GPIO_PIN_7
#define FIL_GPIO_Port GPIOB
#define PPS_MODE_Pin GPIO_PIN_8
#define PPS_MODE_GPIO_Port GPIOB
#define OV_RST_O_Pin GPIO_PIN_9
#define OV_RST_O_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
