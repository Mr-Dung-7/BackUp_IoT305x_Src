/*
 * lcd.c
 *
 *  Created on: Dec 2, 2024
 *      Author: Mr.hDung
 */

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include "lcd.h"

/******************************************************************************/
/*                                 DEFINEs                                    */
/******************************************************************************/


/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/


/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/
ucg_t g_ucg;

char g_strTemp[30] = "";
char g_strHumi[30] = "";
char g_strLight[30] = "";
char g_symbolStr[] = ">";
char g_clearSymbolStr[] = "   ";
char g_setStr[] = "--- Ok";
char g_clearSetStr[] = "          ";

screenType_e 	g_currentScreen = SCREEN_HOME;
uint8_t 		g_currentSelect = VALUE_1;

value_e			g_menuItemSelected = VALUE_1;
value_e 		g_distanceValueSelected = VALUE_1;
value_e 		g_sensitivityValueSelected = VALUE_1;
value_e 		g_latencyValueSelected = VALUE_1;
value_e 		g_lightValueSelected = VALUE_1;
value_e			g_saveDistanceValueSelected = VALUE_1;

/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/


/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/
/*
 * @func:		LCD_Init
 *
 * @brief:
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_Init (void)
{
	Ucglib4WireSWSPI_begin(&g_ucg, UCG_FONT_MODE_SOLID);
	ucg_ClearScreen(&g_ucg);
	ucg_SetFont(&g_ucg, ucg_font_ncenR10_hf);
	ucg_SetColor(&g_ucg, 0, 255, 255, 255);
	ucg_SetColor(&g_ucg, 1, 0, 0, 0);
	ucg_SetRotate180(&g_ucg);
}

/*
 * @func:		LCD_LoadOnStartup
 *
 * @brief:
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_LoadOnStartup (void)
{
	ucg_DrawString(&g_ucg, 33, 40, 0, "IOT305x");
	ucg_DrawString(&g_ucg, 55, 65, 0, "by");
	ucg_DrawString(&g_ucg, 22, 90, 0, "Hoang Dung");
}

/*
 * @func:		LCD_ClearScreen
 *
 * @brief:
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ClearScreen (void)
{
	ucg_ClearScreen(&g_ucg);
	ucg_SetFont(&g_ucg, ucg_font_ncenR10_hf);
}

/*
 * @func:		LCD_DisplayTempValue
 *
 * @brief:
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_DisplayTempValue (uint8_t temperature)
{
	// Store temperature values------------------------------------------------------------
	memset(g_strTemp, 0, sizeof(g_strTemp));

	sprintf(g_strTemp, "Temp: %d oC     ", temperature);

	// Display on LCD----------------------------------------------------------------------
	ucg_DrawString(&g_ucg, 2, 40, 0, g_strTemp);
}

/*
 * @func:		LCD_DisplayHumiValue
 *
 * @brief:
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_DisplayHumiValue (uint8_t humidity)
{
	// Store humidity values---------------------------------------------------------------
	memset(g_strHumi, 0, sizeof(g_strHumi));

	sprintf(g_strHumi, "Humi: %d %%     ", humidity);

	// Display on LCD----------------------------------------------------------------------
	ucg_DrawString(&g_ucg, 2, 60, 0, g_strHumi);
}

/*
 * @func:		LCD_DisplayLightValue
 *
 * @brief:
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_DisplayLightValue (uint16_t light)
{
	// Store light intensity values--------------------------------------------------------
	memset(g_strLight, 0, sizeof(g_strLight));

	sprintf(g_strLight, "Light: %d Lux     ", light);

	// Display on LCD----------------------------------------------------------------------
	ucg_DrawString(&g_ucg, 2, 80, 0, g_strLight);
}

/*
 * @func:		LCD_DisplayDetectionState
 *
 * @brief:
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_DisplayDetectionState (uint8_t state)
{
	if (state)
	{
		ucg_DrawString(&g_ucg, 2, 100, 0, "Motion: Yes   ");
	}
	else
	{
		ucg_DrawString(&g_ucg, 2, 100, 0, "Motion: No   ");
	}

}

/*
 * @func:		LCD_DisplayLedState
 *
 * @brief:
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_DisplayLedState (uint8_t state)
{
	if (state)
	{
		ucg_DrawString(&g_ucg, 2, 109, 0, "Led: On   ");
	}
	else
	{
		ucg_DrawString(&g_ucg, 2, 120, 0, "Led: Off   ");
	}
}

/*
 * @func:		LCD_DisplayNetworkState
 *
 * @brief:
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
//void LCD_DisplayNetworkState (uint8_t state)
//{
//	if (state)
//	{
//		ucg_DrawString(&g_ucg, 2, , 0, "Nwk: Open   ");
//	}
//	else
//	{
//		ucg_DrawString(&g_ucg, 2, , 0, "Nwk: Close   ");
//	}
//}



/*
 * @func:		LCD_ScreenHome
 *
 * @brief:
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenHome (void)
{
	g_currentScreen = SCREEN_HOME;
	uint16_t *frame = USART_GetFrame();

	LCD_ClearScreen();
	ucg_DrawString(&g_ucg, 42, 15, 0, "HOME");
	LCD_DisplayTempValue(CMD_DATA_TEMP);
	LCD_DisplayHumiValue(CMD_DATA_HUMI);
	LCD_DisplayLightValue(CMD_DATA_LIGHT);
	LCD_DisplayDetectionState(CMD_DATA_DETEC);
	LCD_DisplayLedState(CMD_DATA_LED);
}


/*
 * @func:		LCD_ScreenMenuConfig
 *
 * @brief:
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenMenuConfig (void)
{
	g_currentScreen = SCREEN_MENU_CONFIG;
	g_currentSelect = g_menuItemSelected;

	LCD_ClearScreen();
	ucg_DrawString(&g_ucg, 21, 15, 0, "Menu Config");
	ucg_DrawString(&g_ucg, COLUMN_1, ROW_0, 0, "-------------------------");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_1, 0, "1. Distance");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_2, 0, "2. Sensitivity");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_3, 0, "3. Latency");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_4, 0, "4. Light");

	LCD_ScreenSelectPosition(g_menuItemSelected);
}

/*
 * @func:		LCD_ScreenDistanceValue
 *
 * @brief:
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenDistanceValue (void)
{
	g_currentScreen = SCREEN_VALUE_DISTANCE;
	g_currentSelect = g_distanceValueSelected;

	LCD_ClearScreen();
	ucg_DrawString(&g_ucg, 14, 15, 0, "Distance Value");
	ucg_DrawString(&g_ucg, COLUMN_1, ROW_0, 0, "-------------------------");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_1, 0, "1m");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_2, 0, "2m");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_3, 0, "3m");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_4, 0, "4m");

	LCD_ScreenValueSetMarker(g_distanceValueSelected);
}

/*
 * @func:		LCD_ScreenSensitivityValue
 *
 * @brief:
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenSensitivityValue (void)
{
	g_currentScreen = SCREEN_VALUE_SENSITIVITY;
	g_currentSelect = g_sensitivityValueSelected;

	LCD_ClearScreen();
	ucg_DrawString(&g_ucg, 9, 15, 0, "Sensitivity Value");
	ucg_DrawString(&g_ucg, COLUMN_1, ROW_0, 0, "-------------------------");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_1, 0, "20");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_2, 0, "40");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_3, 0, "60");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_4, 0, "80");

	LCD_ScreenValueSetMarker(g_sensitivityValueSelected);
}

/*
 * @func:		LCD_ScreenLatencyValue
 *
 * @brief:
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenLatencyValue (void)
{
	g_currentScreen = SCREEN_VALUE_LATENCY;
	g_currentSelect = g_latencyValueSelected;

	LCD_ClearScreen();
	ucg_DrawString(&g_ucg, 16, 15, 0, "Latency Value");
	ucg_DrawString(&g_ucg, COLUMN_1, ROW_0, 0, "-------------------------");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_1, 0, "2s");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_2, 0, "4s");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_3, 0, "6s");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_4, 0, "8s");

	LCD_ScreenValueSetMarker(g_latencyValueSelected);
}

/*
 * @func:		LCD_ScreenLightValue
 *
 * @brief:
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenLightValue (void)
{
	g_currentScreen = SCREEN_VALUE_LIGHT;
	g_currentSelect = g_lightValueSelected;

	LCD_ClearScreen();
	ucg_DrawString(&g_ucg, 25, 15, 0, "Light Value");
	ucg_DrawString(&g_ucg, COLUMN_1, ROW_0, 0, "-------------------------");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_1, 0, "10 Lux");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_2, 0, "200 Lux");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_3, 0, "600 Lux");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_4, 0, "1000 Lux");

	LCD_ScreenValueSetMarker(g_lightValueSelected);
}

/*
 * @func:		LCD_ScreenMoveUp
 *
 * @brief:
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenMoveUp (void)
{
	if (g_currentScreen == SCREEN_HOME)
	{
		return;
	}

    // Xóa lựa chọn ở vị trí cũ
	LCD_ScreenClearSelectPosition(g_currentSelect);

    // Nếu đang ở giá trị đầu tiên và nhấn lên, chuyển sang giá trị cuối cùng
    if (g_currentSelect == VALUE_1)
    {
    	g_currentSelect = VALUE_4;
    }
    else
    {
    	g_currentSelect--;
    }

    // Đặt dấu lựa chọn tại vị trí mới
    LCD_ScreenSelectPosition(g_currentSelect);
}

/*
 * @func:		LCD_ScreenMoveDown
 *
 * @brief:		Ham di chuyen xuong de lua chon muc can su dung
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenMoveDown (void)
{
	if (g_currentScreen == SCREEN_HOME)
	{
		return;
	}

    // Xóa lựa chọn ở vị trí cũ
	LCD_ScreenClearSelectPosition(g_currentSelect);

    // Nếu đang ở giá trị cuối cùng và nhấn xuống, chuyển về giá trị đầu tiên
    if (g_currentSelect == VALUE_4)
    {
    	g_currentSelect = VALUE_1;
    }
    else
    {
    	g_currentSelect++;
    }

    // Đặt dấu lựa chọn tại vị trí mới
    LCD_ScreenSelectPosition(g_currentSelect);
}

/*
 * @func:		LCD_ScreenNext
 *
 * @brief:
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenNextAndSet (void)
{
	switch (g_currentScreen)
	{
		case SCREEN_HOME:
			break;

		case SCREEN_MENU_CONFIG:
		{
			switch (g_currentSelect)
			{
				case MENU_DISTANCE:
				{
					LCD_ScreenDistanceValue();
					break;
				}

				case MENU_SENSITIVITY:
				{
					LCD_ScreenSensitivityValue();
					break;
				}

				case MENU_LATENCY:
				{
					LCD_ScreenLatencyValue();
					break;
				}

				case MENU_LIGHT:
				{
					LCD_ScreenLightValue();
					break;
				}

				default:
					break;
			}

			break;
		}

		case SCREEN_VALUE_DISTANCE:
		{
			LCD_ScreenSetDistanceValue(g_distanceValueSelected);	// Distance Value dang la gia tri cu
			USART_SendDistanceValue(g_distanceValueSelected);
			break;
		}

		case SCREEN_VALUE_SENSITIVITY:
		{
			LCD_ScreenSetSensitivityValue(g_sensitivityValueSelected);
			USART_SendDistanceValue(g_sensitivityValueSelected);
			break;
		}

		case SCREEN_VALUE_LATENCY:
		{
			LCD_ScreenSetLatencyValue(g_latencyValueSelected);
			USART_SendDistanceValue(g_latencyValueSelected);
			break;
		}

		case SCREEN_VALUE_LIGHT:
		{
			LCD_ScreenSetLightValue(g_lightValueSelected);
			USART_SendDistanceValue(g_lightValueSelected);
			break;
		}

		default:
			break;
	}
}

/*
 * @func:		LCD_ScreenBack
 *
 * @brief:
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenBack (void)
{
	switch (g_currentScreen)
	{
		case SCREEN_HOME:
			break;

		case SCREEN_MENU_CONFIG:
			break;

		case SCREEN_VALUE_DISTANCE:
		{
			g_menuItemSelected = VALUE_1;
			LCD_ScreenMenuConfig();
			break;
		}

		case SCREEN_VALUE_SENSITIVITY:
		{
			g_menuItemSelected = VALUE_2;
			LCD_ScreenMenuConfig();
			break;
		}

		case SCREEN_VALUE_LATENCY:
		{
			g_menuItemSelected = VALUE_3;
			LCD_ScreenMenuConfig();
			break;
		}

		case SCREEN_VALUE_LIGHT:
		{
			g_menuItemSelected = VALUE_4;
			LCD_ScreenMenuConfig();
			break;
		}

		default:
			break;
	}
}

/*
 * @func:		LCD_ScreenSelectPosition
 *
 * @brief:		Hàm vẽ biểu tượng thể hiện mục được lựa chọn
 *
 * @param:		value
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenSelectPosition (value_e value)
{
	switch (value)
	{
		case VALUE_1:
			ucg_DrawString(&g_ucg, COLUMN_1, ROW_1, 0, g_symbolStr);
			break;

		case VALUE_2:
			ucg_DrawString(&g_ucg, COLUMN_1, ROW_2, 0, g_symbolStr);
			break;

		case VALUE_3:
			ucg_DrawString(&g_ucg, COLUMN_1, ROW_3, 0, g_symbolStr);
			break;

		case VALUE_4:
			ucg_DrawString(&g_ucg, COLUMN_1, ROW_4, 0, g_symbolStr);
			break;

		default:
			break;
	}
}

/*
 * @func:		LCD_ScreenClearSelectPosition
 *
 * @brief:
 *
 * @param:		value
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenClearSelectPosition (value_e value)
{
	switch (value)
	{
		case VALUE_1:
			ucg_DrawString(&g_ucg, COLUMN_1, ROW_1, 0, g_clearSymbolStr);
			break;

		case VALUE_2:
			ucg_DrawString(&g_ucg, COLUMN_1, ROW_2, 0, g_clearSymbolStr);
			break;

		case VALUE_3:
			ucg_DrawString(&g_ucg, COLUMN_1, ROW_3, 0, g_clearSymbolStr);
			break;

		case VALUE_4:
			ucg_DrawString(&g_ucg, COLUMN_1, ROW_4, 0, g_clearSymbolStr);
			break;

		default:
			break;
	}
}

/*
 * @func:		LCD_ScreenSetValue
 *
 * @brief:		// Hiện chữ OK khi SET gia tri
 *
 * @param:		value
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenSetValue (value_e value)
{
	switch (value)
	{
		case VALUE_1:
			ucg_DrawString(&g_ucg, COLUMN_3, ROW_1, 0, g_setStr);
			break;

		case VALUE_2:
			ucg_DrawString(&g_ucg, COLUMN_3, ROW_2, 0, g_setStr);
			break;

		case VALUE_3:
			ucg_DrawString(&g_ucg, COLUMN_3, ROW_3, 0, g_setStr);
			break;

		case VALUE_4:
			ucg_DrawString(&g_ucg, COLUMN_3, ROW_4, 0, g_setStr);
			break;

		default:
			break;
	}
}

/*
 * @func:		LCD_ScreenClearSetItem
 *
 * @brief:		// Xóa chữ Oke
 *
 * @param:		value
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenClearSetItem (value_e value)
{
	switch (value)
	{
		case VALUE_1:
			ucg_DrawString(&g_ucg, COLUMN_3, ROW_1, 0, g_clearSetStr);
			break;

		case VALUE_2:
			ucg_DrawString(&g_ucg, COLUMN_3, ROW_2, 0, g_clearSetStr);
			break;

		case VALUE_3:
			ucg_DrawString(&g_ucg, COLUMN_3, ROW_3, 0, g_clearSetStr);
			break;

		case VALUE_4:
			ucg_DrawString(&g_ucg, COLUMN_3, ROW_4, 0, g_clearSetStr);
			break;

		default:
			break;
	}
}

/*
 * @func:		LCD_SetDistanceValue
 *
 * @brief:		Ham SET gia tri khoang cach phat hien (khi nhan B4)
 *
 * @param:		value
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenSetDistanceValue (value_e value)
{
	// Xoa OK o vi tri cu
	LCD_ScreenClearSetItem(value);

	// Luu gia tri khoang cach phat hien da duoc SET (gia tri moi)
	g_distanceValueSelected = g_currentSelect;

	// In Ok o vi tri moi
	LCD_ScreenSetValue(g_currentSelect);
}

/*
 * @func:		LCD_SetSensitivityValue
 *
 * @brief:		Ham SET gia tri do nhay
 *
 * @param:		value
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenSetSensitivityValue (value_e value)
{
	// Xoa OK o vi tri cu
	LCD_ScreenClearSetItem(value);

	// Luu gia tri do nhay da duoc SET
	g_sensitivityValueSelected = g_currentSelect;

	// In Ok o vi tri moi
	LCD_ScreenSetValue(g_currentSelect);
}

/*
 * @func:		LCD_SetLatencyValue
 *
 * @brief:		Ham SET gia tri do tre
 *
 * @param:		value
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenSetLatencyValue (value_e value)
{
	// Xoa OK o vi tri cu
	LCD_ScreenClearSetItem(value);

	// Luu gia tri do tre da duoc SET
	g_latencyValueSelected = g_currentSelect;

	// In Ok o vi tri moi
	LCD_ScreenSetValue(g_currentSelect);
}

/*
 * @func:		LCD_SetLightValue
 *
 * @brief:		Ham SET gia tri anh sang
 *
 * @param:		value
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenSetLightValue (value_e value)
{
	// Xoa OK o vi tri cu
	LCD_ScreenClearSetItem(value);

	// Luu gia tri anh sang da duoc SET
	g_lightValueSelected = g_currentSelect;

	// In Ok o vi tri moi
	LCD_ScreenSetValue(g_currentSelect);
}

/*
 * @func:		LCD_ScreenValueSetMarker
 *
 * @brief:		Ham hien thi gia tri da duoc SET truoc do khi vao lai man hinh cai dat gia tri tuong ung
 *
 * @param:		value
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenValueSetMarker (value_e value)
{
	switch (value)
	{
		case VALUE_1:
		{
	        LCD_ScreenSelectPosition(VALUE_1);
	        LCD_ScreenSetValue(VALUE_1);
	        break;
		}

		case VALUE_2:
		{
	        LCD_ScreenSelectPosition(VALUE_2);
	        LCD_ScreenSetValue(VALUE_2);
	        break;
		}

		case VALUE_3:
		{
	        LCD_ScreenSelectPosition(VALUE_3);
	        LCD_ScreenSetValue(VALUE_3);
	        break;
		}

		case VALUE_4:
		{
	        LCD_ScreenSelectPosition(VALUE_4);
	        LCD_ScreenSetValue(VALUE_4);
	        break;
		}

		default:
			break;
	}
}

/*
 * @func:		LCD_GetCurrentScreen
 *
 * @brief:		Lấy vi tri của màn hình hiện tại
 *
 * @param:		None
 *
 * @retval:		g_currentScreen
 *
 * @note:		None
 */
screenType_e LCD_GetCurrentScreen (void)
{
	return g_currentScreen;
}

/*
 * @func:		LCD_GetCurrentSelected
 *
 * @brief:		Lấy giá trị mục được lựa chọn hiện tại
 *
 * @param:		None
 *
 * @retval:		g_currentSelect
 *
 * @note:		None
 */
menuItems_e LCD_GetCurrentSelected (void)
{
	return g_currentSelect;
}

/*
 * @func:		LCD_GetDistanceValueSelected
 *
 * @brief:		Lấy giá trị khoảng cách được lựa chọn
 *
 * @param:		None
 *
 * @retval:		g_distanceValueSelected
 *
 * @note:		None
 */
value_e LCD_GetDistanceValueSelected (void)
{
	return g_distanceValueSelected;
}

/*
 * @func:		LCD_GetSensitivityValueSelected
 *
 * @brief:		Lấy giá trị độ nhạy được lựa chọn
 *
 * @param:		None
 *
 * @retval:		g_sensitivityValueSelected
 *
 * @note:		None
 */
value_e LCD_GetSensitivityValueSelected (void)
{
	return g_sensitivityValueSelected;
}

/*
 * @func:		LCD_GetLatencyValueSelected
 *
 * @brief:		Lấy giá trị độ trễ được lựa chọn
 *
 * @param:		None
 *
 * @retval:		g_latencyValueSelected
 *
 * @note:		None
 */
value_e LCD_GetLatencyValueSelected (void)
{
	return g_latencyValueSelected;
}

/*
 * @func:		LCD_GetLightValueSelected
 *
 * @brief:		Lấy giá trị ánh sáng được lựa chọn
 *
 * @param:		None
 *
 * @retval:		g_lightValueSelected
 *
 * @note:		None
 */
value_e LCD_GetLightValueSelected (void)
{
	return g_lightValueSelected;
}

/*
 * @func:		LCD_ResetCurrentSelect
 *
 * @brief:		Ham khoi tao lai gia tri duoc chon
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
uint8_t LCD_ResetItemSelect (void)
{
	return g_menuItemSelected = VALUE_1;
}

/* END FILE */
