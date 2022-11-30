#ifndef __COMMAND_H
#define	__COMMAND_H

#include "main.h"

extern uint32_t Comm_Tank_Test_kV_setting(uint32_t kV_data);
extern uint32_t Comm_kV_setting(uint32_t kV_data);
extern uint32_t Comm_mA_setting(uint32_t mA_data);
extern uint32_t Comm_Tank_Test_mA_setting(uint32_t mA_data);
extern uint32_t Comm_kV_FB_read(void);
extern uint32_t Comm_mA_FB_read(void);
extern void Comm_XRAY_ON(void);
extern void Comm_XRAY_OFF(void);
extern uint32_t Comm_Tank_Temp_read(void);
extern void Comm_XRAY_Only_ON(void);
extern void Comm_XRAY_Only_OFF(void);
extern void Comm_Generator_ready_OK(void);
extern void Comm_firmware_version_check(void);
extern void Comm_Preheart_ON(void);
extern void Comm_Normal_mode_ON(void);
extern void Comm_PPS_mode_ON(void);
extern void Comm_XRAY_remote_mode_ON(void);
extern void Comm_XRAY_remote_mode_OFF(void);
extern void Comm_Error_Init(void);
extern uint32_t Comm_CAN_Cal_Data_Read(uint32_t CAN_Data);
extern uint32_t Comm_CAN_Cal_Data_Write(uint32_t CAN_Data);

#define kV_CAL 1
#define mA_CAL 2
#define PRE_CAL 3
#define PPS_ON_CAL 4
#define PPS_OFF_CAL 5

#endif	/* __COMMAND_H */

