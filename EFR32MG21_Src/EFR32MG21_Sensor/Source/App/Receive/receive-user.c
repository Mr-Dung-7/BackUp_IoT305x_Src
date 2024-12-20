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
static pReceiveHandler g_receiveHandler = NULL;

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
	g_receiveHandler = callback;
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
boolean emberAfPreCommandReceivedCallback (EmberAfClusterCommand* cmd)
{
	EmberNodeId nodeId = cmd->source;
	uint16_t clusterID = cmd->apsFrame->clusterId;
	uint8_t commandID = cmd->commandId;
	uint16_t attrID = (uint16_t)(cmd->buffer[cmd->payloadStartIndex] | cmd->buffer[cmd->payloadStartIndex + 1] << 8);
	uint16_t bufIndex = cmd->payloadStartIndex + 4; 	// buffer payload start index
//	uint8_t 	desEndpoint = cmd -> apsFrame -> destinationEndpoint;

	// Su dung dieu kien Switch la gi?
	// Su dung case nao
//	switch()
//	{
//		case EMBE:
//		{
//			break;
//		}
//
//		case EMBER:
//		{
//			break;
//		}
//
//		default:
//			break;
//	}

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
				if (g_receiveHandler)
				{
					g_receiveHandler(emberNodeId, DEVICE_LEAVE_NETWORK, NULL, 0);
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
