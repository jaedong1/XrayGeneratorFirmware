/*
*	FileName	: history.h
*	Project		: HTC-8016 MCU board
*	Description	:
*				- ����Ʈ���� ���� �� ������ �����丮 ����.
*	History		:
*/
#ifndef __HISTORY_H
#define __HISTORY_H

/**************************************************************************************************/
/* Version */
#define MAJOR_NUM			1u
#define MINOR_NUM			0u
#define PATCH_NUM			0u


/**************************************************************************************************/
#define MAJOR_NUM_SHIFT		16u
#define MINOR_NUM_SHIFT		8u
#define PATCH_NUM_SHIFT		0u

#define VERSION_MAJOR		(MAJOR_NUM << MAJOR_NUM_SHIFT)
#define VERSION_MINOR		(MINOR_NUM << MINOR_NUM_SHIFT)
#define VERSION_PATCH		(PATCH_NUM << PATCH_NUM_SHIFT)

#define SOFTWARE_VERSION	(VERSION_MAJOR | VERSION_MINOR | VERSION_PATCH)
#define HTC8016_VERSION (MAJOR_NUM * 100) + (MINOR_NUM * 10) + (PATCH_NUM)

#define PROJECT_NAME		"HTC-8016"
#define BOARD_NAME		"MCU"

extern void Printf_Initialize(void);
extern void ShowVersion(void);
/**************************************************************************************************/
/*	version info : major.minor.patch
	major : ���� ������ ������ ���� �ϵ���� �������� ����Ʈ��� ����� ��� ����.
	minor : ����� �߰��ǰų�, ���� �䱸�� ���� ����� ����� ��� ����.
	patch : ���װ� ������ ��� ����.
*/
/**************************************************************************************************/

/*

*/


/**************************************************************************************************/
#endif
