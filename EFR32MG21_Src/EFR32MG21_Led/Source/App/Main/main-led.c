/*
 * main-led.c
 *
 *  Created on: Dec 1, 2024
 *      Author: Mr.hDung
 */

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include "Source/App/Main/main-led.h"

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
EmberEventControl networkLeaveEventControl;
EmberEventControl keepAliveEventControl;

MainState_e g_systemState;

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

	RECEIVE_Init(Main_ReceiveLeaveHandle);

	Button_Init(Main_ButtonPressHandle, Main_ButtonHoldHandle);

	led_Init();

	Timer_Init(10);

	USART2_Init(Main_Usart2RxHandle);

	g_systemState = POWER_ON_STATE;

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

	EmberNetworkStatus nwkCurrentStatus;

	switch(g_systemState)
	{
		case POWER_ON_STATE:
		{
			g_systemState = IDLE_STATE;

			nwkCurrentStatus = emberAfNetworkState();	// Check the network status

			if(nwkCurrentStatus == EMBER_NO_NETWORK)
			{
				toggleLed(LED_1, RED, 3, 300, 300);
			}
		} break;

		case REPORT_STATE:
		{
			g_systemState = IDLE_STATE;
			SEND_ReportInfoToHC();
			emberAfCorePrintln("Report State\n");
		} break;

		case IDLE_STATE:
			break;

		case LEAVE_NETWORK:
		{
			// Send the Leave Response message to the HC
			SEND_LeaveResponse();

			// Need create event timer for leave network produce
			// because after send msg leave network, it must need coor's response
			emberEventControlSetDelayMS(networkLeaveEventControl, 2000);
		} break;

		case REBOOT_STATE:
		{
			g_systemState = IDLE_STATE;
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

	static bool networkReady = false;

	switch(networkResult)
	{
		case NETWORK_HAS_PARENT:
		{
			emberAfCorePrintln("NETWORK_HAS_PARENT");
			toggleLed(LED_1, PINK, 3, 300, 300);
			networkReady = true;
			g_systemState = REPORT_STATE;

			emberEventControlSetDelayMS(mainStateEventControl, 1000);
		} break;

		case NETWORK_JOIN_SUCCESS:
		{
			emberAfCorePrintln("NETWORK_JOIN_SUCCESS");
			toggleLed(LED_1, PINK, 3, 300, 300);
			networkReady = true;
			g_systemState = REPORT_STATE;

			emberEventControlSetDelayMS(mainStateEventControl, 1000);
		} break;

		case NETWORK_JOIN_FAIL:
		{
			emberAfCorePrintln("NETWORK_JOIN_FAIL");
			toggleLed(LED_1, RED, 2, 500, 500);
			g_systemState = IDLE_STATE;

			emberEventControlSetDelayMS(mainStateEventControl, 1000);
		} break;

		case NETWORK_LOST_PARENT:
		{
			emberAfCorePrintln("NETWORK_LOST_PARENT");
			toggleLed(LED_1, YELLOW, 3, 300, 300);
			g_systemState = IDLE_STATE;

			emberEventControlSetDelayMS(mainStateEventControl, 1000);
		} break;

		case NETWORK_OUT_NETWORK:
		{
			if(networkReady)
			{
				emberAfCorePrintln("NETWORK_OUT_NETWORK");
				toggleLed(LED_1, BLUE, 3, 300, 300);
				g_systemState = LEAVE_NETWORK;

				emberEventControlSetDelayMS(mainStateEventControl, 1000);
			}
		} break;

		default:
			break;
	}
}

/*
 * @func:		networkLeaveEventHandler
 *
 * @brief:		The function handles when the device leaves the network
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void networkLeaveEventHandler (void)
{
	emberEventControlSetInactive(networkLeaveEventControl);

	emberAfCorePrintln("Leaving network");
	g_systemState = REBOOT_STATE;

	emberClearBindingTable();
	emberLeaveNetwork();

	emberEventControlSetDelayMS(mainStateEventControl, 2000);
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
			case press_2:
			{
				NETWORK_FindAndJoin();
				emberAfCorePrintln("SW1_press_2");
			} break;

			case press_3:
			{
				emberAfPluginFindAndBindTargetStart(1);
				emberAfCorePrintln("SW1_press_3");
			} break;

			case press_5:
			{
				g_systemState = LEAVE_NETWORK;
				emberEventControlSetActive(mainStateEventControl);
				emberAfCorePrintln("SW1_press_5");
			} break;

			default:
				break;
		}
	}
	else if (button == SW2)
	{
		switch(pressCount)
		{
			case press_1:	// Viet de test truyen qua UART
			{
				emberAfCorePrintln("SW2_press_1");
				led_turnOn(LED_2, PINK);
				USART_DetectionState(CMD_DATA_MOTION);
				USART_LedControl(CMD_DATA_LED_ON);
			} break;

			case press_2:
			{
				emberAfCorePrintln("SW2_press_2");
				led_turnOff(LED_2);
				USART_DetectionState(CMD_DATA_UNMOTION);
				USART_LedControl(CMD_DATA_LED_OFF);
			} break;

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
 * @func:		emberAfPreCommandReceivedCallback
 *
 * @brief:		The function handles incoming messages
 *
 * @params:		cmd - Pointer to the received command
 *
 * @retVal:		true / false
 *
 * @note:		None
 */
boolean emberAfPreCommandReceivedCallback (EmberAfClusterCommand *cmd)
{
	uint16_t 	clusterID = cmd -> apsFrame -> clusterId;

	switch(cmd->type)
	{
		case EMBER_INCOMING_UNICAST:
		{
			if (clusterID == ZCL_ON_OFF_CLUSTER_ID)
			{
				RECEIVE_OnOffClusterHandle(cmd);
				return true;
			}
		} break;

		default:
			break;
	}

	return false;
}

/*
 * @func:		Main_ReceiveLeaveHandle
 *
 * @brief:		The function handles device removal messages from the Home Controller
 *
 * @params[1]:	nodeId - ZigBee network address
 * @params[2]:	receiveId - Received command
 *
 * @retVal:		None
 *
 * @note:		None
 */
void Main_ReceiveLeaveHandle (EmberNodeId nodeId, RECEIVE_CMD_ID_e receiveId)
{
	switch (receiveId)
	{
		case DEVICE_LEAVE_NETWORK:
		{
			emberAfCorePrintln("DEVICE_LEAVE_NETWORK");
			g_systemState = LEAVE_NETWORK;
			emberEventControlSetActive(mainStateEventControl);
		} break;

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
 * @note:		None
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
			} break;

			case USART_STATE_DATA_RECEIVED:
			{
				emberAfCorePrintln("USART_STATE_DATA_RECEIVED\n");
				uint8_t	*g_frame = GetFrame();

				if (CMD_ID == CMD_ID_LED && CMD_TYPE == CMD_TYPE_SET && CMD_DATA_LED == CMD_DATA_LED_ON)
				{
					led_turnOn(LED_2, PINK);
					SEND_OnOffStateReport(CMD_ENDPOINT, 1);
				}
				else if (CMD_ID == CMD_ID_LED && CMD_TYPE == CMD_TYPE_SET && CMD_DATA_LED == CMD_DATA_LED_OFF)
				{
					led_turnOff(LED_2);
					SEND_OnOffStateReport(CMD_ENDPOINT, 0);
				}
			} break;

			case USART_STATE_DATA_ERROR:
			{
				emberAfCorePrintln("USART_STATE_DATA_ERROR\n");
			} break;

			case USART_STATE_ERROR:
			case USART_STATE_RX_TIMEOUT:
			{
				emberAfCorePrintln("USART_STATE_ERROR_OR_RX_TIMEOUT\n");
			} break;

			default:
				break;
		}
	}
}

/*
 * @func:  		keepAliveEventHandler
 *
 * @brief:
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void keepAliveEventHandler (void)
{
	emberEventControlSetInactive(keepAliveEventControl);

	SEND_KeepAliveToZc();
	
	emberEventControlSetDelayMS(keepAliveEventControl, 1000 * 60 * 15);
}

/* END FILE */

