/*
 * main-zc.c
 *
 *  Created on: Dec 1, 2024
 *      Author: Mr.hDung
 */

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include "Source/App/Main/main-zc.h"

/******************************************************************************/
/*                                 DEFINEs                                    */
/******************************************************************************/


/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/


/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/
EmberEventControl mainStateEventControl;
EmberEventControl updateConnectStatusEventControl;

MainState_e systemState;

/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/


/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/
/** @brief Main Init
 *
 * This function is called from the application's main function. It gives the
 * application a chance to do any initialization required at system startup.
 * Any code that you would normally put into the top of the application's
 * main() routine should be put into this function.
        Note: No callback
 * in the Application Framework is associated with resource cleanup. If you
 * are implementing your application on a Unix host where resource cleanup is
 * a consideration, we expect that you will use the standard Posix system
 * calls, including the use of atexit() and handlers for signals such as
 * SIGTERM, SIGINT, SIGCHLD, SIGPIPE and so on. If you use the signal()
 * function to register your signal handler, please mind the returned value
 * which may be an Application Framework function. If the return value is
 * non-null, please make sure that you call the returned function from your
 * handler to avoid negating the resource cleanup of the Application Framework
 * itself.
 *
 */
void emberAfMainInitCallback (void)
{
	emberAfCorePrintln("emberAfMainInitCallback");

	NETWORK_Init(Main_NetworkHandle);
	RECEIVE_Init(Main_ReceiveHandle);
	Button_Init(Main_ButtonPressHandle, Main_ButtonHoldHandle);
	led_Init();
	Timer_Init(10);
	USART2_Init(Main_Usart2RxHandle);

	systemState = POWER_ON_STATE;

	emberEventControlSetActive(mainStateEventControl);
}

/*
 * @func:		mainStateEventHandler
 *
 * @brief:		The function handles the program states
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void mainStateEventHandler (void)
{
	emberAfCorePrintln("mainStateEventHandler");
	emberEventControlSetInactive(mainStateEventControl);

	switch(systemState)
	{
		case POWER_ON_STATE:
		{
			systemState = IDLE_STATE;
			// Khi duoc cap nguon thi lam gi???
		} break;

		case IDLE_STATE:
			break;

		case REBOOT_STATE:
		{
			systemState = IDLE_STATE;
			halReboot();
		} break;

		default:
			break;
	}
}

/*
 * @func:		Main_NetworkHandle
 *
 * @brief:		The function handles events at the Network layer
 *
 * @params:		networkResult - Network states
 *
 * @retVal:		None
 *
 * @note:		None
 */
void Main_NetworkHandle (NetworkState_e networkResult)
{
	emberAfCorePrintln("USER_NetworkHandle");

	switch(networkResult)
	{
		case NETWORK_CREATING:
		{
			emberAfCorePrintln("NETWORK_CREATING");
			toggleLed(LED_1, PINK, 3, 300, 300);
		} break;

		case NETWORK_OPENING:
		{
			emberAfCorePrintln("NETWORK_OPENING");
			toggleLed(LED_1, GREEN, 3, 300, 300);

		} break;

		case NETWORK_CLOSING:
		{
			emberAfCorePrintln("NETWORK_CLOSING");
			toggleLed(LED_1, RED, 2, 500, 500);
		} break;

		default:
			break;
	}
}

/*
 * @func:		Main_ButtonPressHandle
 *
 * @brief:		The function handles events when a button is pressed
 *
 * @params[1]:	button - Button to be used
 * @params[2]:	pressCount - Number of button presses
 *
 * @retVal:		None
 *
 * @note:		None
 */
void Main_ButtonPressHandle (uint8_t button, uint8_t pressCount)
{
	if(button == SW1)
	{
		switch(pressCount)
		{
			case press_1:
			{
				NETWORK_CreateNetwork();
			} break;

			case press_2:
			{
				NETWORK_OpenNetwork();
			} break;

			case press_3:
			{
				NETWORK_CloseNetwork();
			} break;

			case press_4:
			{
				NETWORK_DeleteNetwork();
			} break;

			case press_5:

				break;

			default:
				break;
		}
	}
	else if (button == SW2)
	{
		switch(pressCount)
		{
			case press_1:
				break;

			default:
				break;
		}
	}
}

/*
 * @func:		Main_ButtonHoldHandle
 *
 * @brief:		The function handles events when a button is held down
 *
 * @params[1]:	button - Button to be used
 * @params[2]:	holdCount - Button hold time
 *
 * @retVal:		None
 *
 * @note:		None
 */
void Main_ButtonHoldHandle (uint8_t button, uint8_t holdCount)
{
	if(button == SW1)
	{
		switch(holdCount)
		{
			case hold_1s:
				break;

			default:
				break;
		}
	}
	else if (button == SW2)
	{
		switch(holdCount)
		{
			case hold_1s:
				break;

			default:
				break;
		}
	}
}

/*
 * @func:  		Main_ReceiveHandle
 *
 * @brief:		Ham xu ly cac ban tin ZCL tu thiet bi gui ve
 *
 * @param:
 *
 * @retval:		None
 *
 * @note:		None
 */
void Main_ReceiveHandle(EmberNodeId nodeId, RECEIVE_CMD_ID_e receiveId,
						uint8_t *payload, uint8_t length)
{
	bool checkTime	= false;

	switch (receiveId)
	{
		case DEVICE_JOIN_NETWORK:
		{
			checkTime = true;
			DeviceManager_AddDeviceToList(nodeId, (char*)payload);
			break;
		}

		case DEVICE_LEAVE_NETWORK:
		{
			DeviceManager_RemoveDeviceFromList(nodeId);
			break;
		}

		case DEVICE_CONNECTED:	// Chua co callback truyen ve
		{
			checkTime = true;
			DeviceManager_SetDeviceOnline(nodeId);
			break;
		}

		case DEVICE_DISCONNECTED:	// Chua co callback truyen ve
		{
			checkTime = true;
			DeviceManager_SetDeviceOffline(nodeId);
			break;
		}

		case DEVICE_SENSOR_TEMP_VALUE:
		{
			checkTime = true;
			// Gui ban tin UART gia tri Temp cho STM32
			emberAfCorePrintln("Temp: %d", *payload);
			break;
		}

		case DEVICE_SENSOR_HUMI_VALUE:
		{
			checkTime = true;
			// Gui ban tin UART gia tri Humi cho STM32
			emberAfCorePrintln("Humi: %d", *payload);
			break;
		}

		case DEVICE_SENSOR_LIGHT_VALUE:
		{
			checkTime = true;
			// Gui ban tin UART gia tri Light cho STM32
			emberAfCorePrintln("Light: %d", *payload);
			break;
		}

		case DEVICE_SENSOR_LD2410_MOTION:
		{
			checkTime = true;
			// Gui ban tin UART MOTION cho STM32
			emberAfCorePrintln("MOTION: %d", *payload);
			break;
		}

		case DEVICE_SENSOR_LD2410_UNMOTION:
		{
			checkTime = true;
			// Gui ban tin UART UNMOTION cho STM32
			emberAfCorePrintln("UNMOTION: %d", *payload);
			break;
		}

		case DEVICE_LED_ON:
		{
			checkTime = true;
			// Gui ban tin UART LED On cho STM32
			emberAfCorePrintln("LED ON: %d", *payload);
			DeviceManager_SetStateLedOn(nodeId);
			break;
		}

		case DEVICE_LED_OFF:
		{
			checkTime = true;
			// Gui ban tin UART LED Off cho STM32
			emberAfCorePrintln("LED OFF: %d", *payload);
			DeviceManager_SetStateLedOff(nodeId);
			break;
		}

		case DEVICE_UPDATE_TIME:
		{
			checkTime = true;
			break;
		}

		default:
			break;
	}

	// Save last communicate time
	if(checkTime)
	{
		uint32_t time = Timer_GetMilSecTick();
		DeviceManager_UpdateLastTime(nodeId, time);
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
				emberAfCorePrintln("USART_STATE_EMPTY\n");
				break;
			}

			case USART_STATE_DATA_RECEIVED:
			{
				emberAfCorePrintln("USART_STATE_DATA_RECEIVED\n");
//				USART_ReceivedData();
				break;
			}

			case USART_STATE_DATA_ERROR:
			{
				emberAfCorePrintln("USART_STATE_DATA_ERROR\n");
				break;
			}

			case USART_STATE_ERROR:
			case USART_STATE_RX_TIMEOUT:
			{
				emberAfCorePrintln("USART_STATE_ERROR_OR_RX_TIMEOUT\n");
				break;
			}

			default:
				break;
		}
	}
}

/*
 * @func:  		updateConnectStatusEventHandler
 *
 * @brief:		The function checks the time to update the device's connection status
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void updateConnectStatusEventHandler (void)
{
	emberEventControlSetInactive(updateConnectStatusEventControl);

	Device_t* deviceList = DeviceManager_GetDeviceList();

	for (uint8_t i = 0; i < DeviceManager_GetDeviceCountToList(); i++)
	{
		uint32_t lastTime = deviceList[i].lastTime;
		uint32_t currentTime = Timer_GetMilSecTick();
		ConnectState_e connection = deviceList[i].ConnectState;

		if (Timer_CalculatorTime(lastTime, currentTime) >= DEVICE_OFFLINE_THRESHHOLD_INTERVAL && 	\
			connection == ONLINE)
		{
			DeviceManager_SetDeviceOffline(deviceList[i].nodeID);

			// Send response to ESP32
//			USART_DeviceDisconnected(deviceList[i].nodeID);
		}
	}

	emberEventControlSetDelayMS(updateConnectStatusEventControl, 1000);
}

/* END FILE */
