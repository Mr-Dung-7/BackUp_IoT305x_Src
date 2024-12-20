/*
 * main-displaycontrol.c
 *
 *  Created on: Dec 2, 2024
 *      Author: Mr.hDung
 */

/****************************************************************************************/
/*                                      INCLUDEs                                		*/
/****************************************************************************************/
#include "main-displaycontrol.h"


/****************************************************************************************/
/*                                       DEFINEs                        			    */
/****************************************************************************************/


/****************************************************************************************/
/*                                  STRUCTs AND ENUMs                           		*/
/****************************************************************************************/


/****************************************************************************************/
/*                                  GLOBAL VARIABLEs                 					*/
/****************************************************************************************/
// Variable representing the current state of the program
state_app_e 	g_eCurrentState;

uint8_t 		g_enable = 0;
uint8_t 		g_temperature, g_humidity;
uint16_t 		g_light;

screenType_e	g_curScreen;
//uint8_t			g_curSelection;
//value_e			g_distanceValue;
//value_e			g_sensitivityValue;
//value_e			g_latencyValue;
//value_e			g_lightValue;

uint8_t			g_idTimerCallScreenHome = NO_TIMER;

/****************************************************************************************/
/*                                 FUNCTIONs PROTOTYPE                                  */
/****************************************************************************************/


/****************************************************************************************/
/*                                      FUNCTIONs                                       */
/****************************************************************************************/
/*
 * @func:  		main
 *
 * @brief:		Main program
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
int main ()
{
	Main_AppInitManager();

	// Initializing the main program to the previous state of the device before power-off
	Main_SetStateApp(STATE_APP_STARTUP);

	// Adding initialization events when the system starts operating to the Queue
	EventSchedulerAdd(EVENT_APP_INIT);

	while (1)
	{
		processTimerScheduler();
		processEventScheduler();
		processSerialHandle();
	}
}

/*
 * @func:  		Main_AppInitManager
 *
 * @brief:		The function to initialize the resources needed for usage
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void Main_AppInitManager (void)
{
	SystemCoreClockUpdate();
	TimerInit();

	// Initializing the buffer to store the event list of the program
	EventSchedulerInit(Main_AppStateManager);

	EventButton_Init();
	Button_Init();
	USART6_FifoAndUsartInit();
	USART6_HandleEventCallback(Main_Usart2RxHandle);
	LCD_Init();
}

/*
 * @func:  		Main_AppStateManager
 *
 * @brief		Event handling function of the main program
 *
 * @param:		event - Event when pressing the button on the KIT board
 *
 * @retval:		None
 *
 * @note:		None
 */
void Main_AppStateManager (uint8_t event)
{
	switch (Main_GetStateApp())
	{
		case STATE_APP_STARTUP:
		{
			if (event == EVENT_APP_INIT)
			{
				LCD_LoadOnStartup();
				Main_SetStateApp(STATE_APP_IDLE);
//
//				if (g_idTimerCallScreenHome != NO_TIMER)
//				{
//					TimerStop(g_idTimerCallScreenHome);
//					g_idTimerCallScreenHome = NO_TIMER;
//				}
//
//				g_idTimerCallScreenHome = TimerStart("LCD_ScreenHome", 2000, 1, (void*)LCD_ScreenHome, NULL);
			}

			break;
		}

		case STATE_APP_IDLE:
		{
			Main_DeviceStateMachine(event);
			break;
		}

		case STATE_APP_RESET:
			break;

		default:
			break;
	}
}

/*
 * @func:  		Main_SetStateApp
 *
 * @brief:		The function to set the state of the program
 *
 * @param:		state - The state of the program
 *
 * @retval:		None
 *
 * @note:		None
 */
void Main_SetStateApp (state_app_e state)
{
	g_eCurrentState = state;
}

/*
 * @func:  		Main_GetStateApp
 *
 * @brief:		The function to retrieve the current state of the program
 *
 * @param:		None
 *
 * @retval:		g_eCurrentState - The current state of the program
 *
 * @note:		None
 */
state_app_e Main_GetStateApp (void)
{
	return g_eCurrentState;
}

/*
 * @func:  		Main_DeviceStateMachine
 *
 * @brief:
 *
 * @param:		event - Event when pressing a button on the KIT board
 *
 * @retval:		None
 *
 * @note:		None
 */

void Main_DeviceStateMachine (uint8_t event)
{
	static uint8_t nwkCreateDeleteToggle = 0;
	static uint8_t nwkOpenCloseToggle = 0;
	static uint8_t ledOnOffToggle = 0;

	switch (event)
	{
		case EVENT_OF_BUTTON_3_PRESS_LOGIC:
		{
			LCD_ScreenHome();
//			g_curScreen = LCD_GetCurrentScreen();
//
//			if (g_curScreen != SCREEN_HOME)
//			{
//				LCD_ScreenHome();
//			}

			break;
		}

		case EVENT_OF_BUTTON_3_PRESS_2_TIMES:
		{
			LCD_ResetItemSelect();
			LCD_ScreenMenuConfig();
			break;
		}

		case EVENT_OF_BUTTON_3_HOLD_1S:
		{
			ledOnOffToggle = !ledOnOffToggle;

			if (ledOnOffToggle)
			{
				// Gui ban tin On LED cho ZC
				USART_SendLedState(1);
			}
			else
			{
				// Gui ban tin Off LED cho ZC
				USART_SendLedState(0);
			}

			break;
		}

		case EVENT_OF_BUTTON_1_PRESS_LOGIC:
		{
			LCD_DisplayDetectionState(1);
//			LCD_ScreenMoveUp();
			break;
		}

		case EVENT_OF_BUTTON_5_PRESS_LOGIC:
		{
			LCD_DisplayDetectionState(0);
//			LCD_ScreenMoveDown();
			break;
		}

		case EVENT_OF_BUTTON_2_PRESS_LOGIC:
		{
			LCD_DisplayLedState(1);
//			LCD_ScreenBack();
			break;
		}

		case EVENT_OF_BUTTON_2_HOLD_1S:
		{
			nwkCreateDeleteToggle = !nwkCreateDeleteToggle;

			if (nwkCreateDeleteToggle)
			{
				// Gui ban tin tao mang cho ZC
				USART_SendCreateNetwork();
			}
			else
			{
				// Gui ban tin xoa mang cho ZC
				USART_SendDeleteNetwork();
			}

			break;
		}

		case EVENT_OF_BUTTON_4_PRESS_LOGIC:
		{
			LCD_DisplayLedState(0);
//			LCD_ScreenNextAndSet();
			break;
		}

		case EVENT_OF_BUTTON_4_HOLD_1S:
		{
			nwkOpenCloseToggle = !nwkOpenCloseToggle;

			if (nwkOpenCloseToggle)
			{
				// Gui ban tin mo mang cho ZC
				USART_SendOpenNetwork();
			}
			else
			{
				// Gui ban tin dong mang cho ZC
				USART_SendCloseNetwork();
			}

			break;
		}

		default:
			break;
	}
}

/*
 * @func:  		Main_Usart2RxHandle
 *
 * @brief:		The function executes the event upon receiving the corresponding message
 * 				in the specified format
 *
 * @param:		UsartStateRx - Received status
 *
 * @retval:		None
 *
 * @note:		Receive control messages from the Web Server (ESP32)
 */
void Main_Usart2RxHandle (USART_STATE_e UsartStateRx)
{
	if (UsartStateRx != USART_STATE_IDLE)
	{
		switch (UsartStateRx)
		{
			case USART_STATE_EMPTY:
			{
				printf("USART_STATE_EMPTY\n");
				break;
			}

			case USART_STATE_DATA_RECEIVED:
			{
				printf("USART_STATE_DATA_RECEIVED\n");
				USART_ReceivedData();
				break;
			}

			case USART_STATE_DATA_ERROR:
			{
				printf("USART_STATE_DATA_ERROR\n");
				break;
			}

			case USART_STATE_ERROR:
			case USART_STATE_RX_TIMEOUT:
			{
				printf("USART_STATE_ERROR_OR_RX_TIMEOUT\n");
				break;
			}

			default:
				break;
		}
	}
}

/* END FILE */
