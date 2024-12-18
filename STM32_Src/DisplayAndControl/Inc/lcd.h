/*
 * lcd.h
 *
 *  Created on: Dec 2, 2024
 *      Author: Mr.hDung
 */

#ifndef LCD_H_
#define LCD_H_

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "ucg.h"
#include "Ucglib.h"
#include "usart.h"

/******************************************************************************/
/*                                 DEFINEs                                    */
/******************************************************************************/
#define COLUMN_1			1
#define COLUMN_2			18
#define COLUMN_3			87

#define ROW_0				32
#define ROW_1				51
#define ROW_2				73
#define ROW_3				95
#define ROW_4				117

/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/
// Enum quản lý các màn hình
typedef enum
{
    SCREEN_HOME = 0,
    SCREEN_MENU_CONFIG,
    SCREEN_VALUE_DISTANCE,
	SCREEN_VALUE_SENSITIVITY,
	SCREEN_VALUE_LATENCY,
	SCREEN_VALUE_LIGHT
} screenType_e;

typedef enum
{
	MENU_DISTANCE = 1,
	MENU_SENSITIVITY,
	MENU_LATENCY,
	MENU_LIGHT
} menuItems_e;

typedef enum
{
	VALUE_1 = 1,
	VALUE_2,
	VALUE_3,
	VALUE_4
} value_e;

/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/


/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/
void LCD_Init (void);
void LCD_LoadOnStartup (void);
void LCD_ClearScreen (void);
void LCD_DisplayTempValue (uint8_t temperature);
void LCD_DisplayHumiValue (uint8_t humidity);
void LCD_DisplayLightValue (uint16_t light);
void LCD_DisplayDetectionState (uint8_t state);
void LCD_DisplayLedState (uint8_t state);
void LCD_ScreenHome (void);
void LCD_ScreenMenuConfig (void);
void LCD_ScreenDistanceValue (void);
void LCD_ScreenSensitivityValue (void);
void LCD_ScreenLatencyValue (void);
void LCD_ScreenLightValue (void);
void LCD_ScreenMoveUp (void);
void LCD_ScreenMoveDown (void);
void LCD_ScreenNextAndSet (void);
void LCD_ScreenBack (void);
void LCD_ScreenSelectPosition (value_e value);
void LCD_ScreenClearSelectPosition (value_e value);
void LCD_ScreenSetValue (value_e value);
void LCD_ScreenClearSetItem (value_e value);
void LCD_ScreenSetDistanceValue (value_e value);
void LCD_ScreenSetSensitivityValue (value_e value);
void LCD_ScreenSetLatencyValue (value_e value);
void LCD_ScreenSetLightValue (value_e value);
void LCD_ScreenValueSetMarker (value_e value);
screenType_e LCD_GetCurrentScreen (void);
menuItems_e LCD_GetCurrentSelected (void);
value_e LCD_GetDistanceValueSelected (void);
value_e LCD_GetSensitivityValueSelected (void);
value_e LCD_GetLatencyValueSelected (void);
value_e LCD_GetLightValueSelected (void);
uint8_t LCD_ResetItemSelect (void);

/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/


#endif /* LCD_H_ */

/* END FILE */
