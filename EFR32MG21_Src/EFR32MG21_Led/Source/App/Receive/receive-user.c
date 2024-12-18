/*
 * receive-user.c
 *
 *  Created on: Jul 30, 2024
 *      Author: Mr.hDung
 */

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include "Source/App/Receive/receive-user.h"

/******************************************************************************/
/*                                 DEFINEs                                    */
/******************************************************************************/


/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/


/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/
static pReceiveHandler receiveHandler = NULL;

/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/


/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/
/*
 * @func:		Receive_Init
 *
 * @brief:		Initialize the function to receive values
 *
 * @params:		callback
 *
 * @retVal:		None
 *
 * @note:		None
 */
void RECEIVE_Init (pReceiveHandler callback)
{
	receiveHandler = callback;
}

/*
 * @func:		Receive_OnOffClusterHandle
 *
 * @brief:		The function executes ZCL on/off
 *
 * @params:		cmd - Pointer to the received command
 *
 * @retVal:		None
 *
 * @note:		None
 */
void RECEIVE_OnOffClusterHandle (EmberAfClusterCommand* cmd)
{
	uint8_t commandID = cmd->commandId;
	uint8_t desEndpoint = cmd->apsFrame->destinationEndpoint;
	uint16_t attrID = (uint16_t)(cmd->buffer[cmd->payloadStartIndex] | cmd->buffer[cmd->payloadStartIndex + 1] << 8);
	uint16_t bufIndex = cmd->payloadStartIndex + 4; 	// buffer payload start index

	switch(commandID)
	{
		case ZCL_ON_COMMAND_ID:		// LED nhan ban tin ON tu SENSOR
		{
			if (desEndpoint == ENDPOINT_1)
			{
				led_turnOn(LED_1, BLUE);
				SEND_OnOffStateReport(desEndpoint, 1);
				USART_LedControl(CMD_DATA_LED_ON);
			}
		} break;

		case ZCL_OFF_COMMAND_ID:	// LED nhan ban tin OFF tu SENSOR
		{
			if (desEndpoint == ENDPOINT_1)
			{
				led_turnOff(LED_1);
				SEND_OnOffStateReport(desEndpoint, 0);
				USART_LedControl(CMD_DATA_LED_OFF);
			}
		} break;

		case ZCL_WRITE_ATTRIBUTES_COMMAND_ID: //LED nhan ban tin dieu khien tu ZC (Dieu khien On/Off tu Web)
		{
			if (attrID == ZCL_ON_OFF_ATTRIBUTE_ID)
			{
				uint8_t state = cmd->buffer[bufIndex];

				if (state == 1)
				{
					led_turnOn(LED_1, BLUE);
					SEND_OnOffStateReport(desEndpoint, 1);
					USART_LedControl(CMD_DATA_LED_ON);
				}
				else
				{
					led_turnOff(LED_1);
					SEND_OnOffStateReport(desEndpoint, 0);
					USART_LedControl(CMD_DATA_LED_OFF);
				}
			}
		} break;

		default:
			break;
	}
}

/*
 * @func:		emberAfPreMessageReceivedCallback
 *
 * @brief:		This function is called when any Zigbee message is received and
 * 				indicates whether the message has been processed or not
 *
 * @params:		incommingMessage
 *
 * @retVal:		true / false
 *
 * @note:		None
 */
boolean emberAfPreMessageReceivedCallback (EmberAfIncomingMessage* incommingMessage)
{
	return false;
}

/*
 * @func:		emberAfPreZDOMessageReceivedCallback
 *
 * @brief:		The function receives ZDO messages and forwards them to the application
 *
 * @params[1]:	emberNodeId
 * @params[2]:	apsFrame
 * @params[3]:	message
 * @params[4]:	length
 *
 * @retVal:		true / false
 *
 * @note:		None
 */
bool emberAfPreZDOMessageReceivedCallback (EmberNodeId emberNodeId,
										   EmberApsFrame* apsFrame,
										   uint8_t* message,
										   uint16_t length)
{
	if (apsFrame->profileId == EMBER_ZDO_PROFILE_ID)
	{
		switch(apsFrame->clusterId)
		{
			case LEAVE_RESPONSE:
			{
				if (receiveHandler)
				{
					receiveHandler(emberNodeId, DEVICE_LEAVE_NETWORK);
				}
				return true;
			} break;

			default:
				break;
		}
	}

	return false;
}

/* END FILE */
