/*******************************************************************************
 *
 * FILE NAME          : MxL608_OEM_Drv.h
 * 
 * AUTHOR             : Dong Liu 
 *
 * DATE CREATED       : 11/23/2011
 *
 * DESCRIPTION        : Header file for MxL601_OEM_Drv.c
 *
 *******************************************************************************
 *                Copyright (c) 2010, MaxLinear, Inc.
 ******************************************************************************/

#ifndef __MxL608_OEM_DRV_H__
#define __MxL608_OEM_DRV_H__

/******************************************************************************
    Include Header Files
    (No absolute paths - paths handled by make file)
******************************************************************************/

#include "MxL608_MaxLinearDataTypes.h"
#include "MxL608_Debug.h"

/******************************************************************************
    Macros
******************************************************************************/

/******************************************************************************
    User-Defined Types (Typedefs)
******************************************************************************/

/******************************************************************************
    Global Variable Declarations
******************************************************************************/
extern void * MxL608_OEM_DataPtr[];
/*****************************************************************************
    Prototypes
******************************************************************************/

MXL608_STATUS MxLWare608_OEM_SetActiveI2C(void *pi2c);
MXL608_STATUS MxLWare608_OEM_WriteRegister(UINT8_608 devId, UINT8_608 regAddr, UINT8_608 regData);
MXL608_STATUS MxLWare608_OEM_ReadRegister(UINT8_608 devId, UINT8_608 regAddr, UINT8_608 *regDataPtr);
void MxLWare608_OEM_Sleep(UINT16_608 delayTimeInMs);  

#endif /* __MxL601_OEM_DRV_H__*/




