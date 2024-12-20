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
boolean emberAfPreMessageReceivedCallback (EmberAfIncomingMessage *incommingMessage)
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
boolean emberAfPreCommandReceivedCallback (EmberAfClusterCommand *cmd)
{
	EmberNodeId nodeId = cmd->source;
	uint16_t clusterID = cmd->apsFrame->clusterId;
	uint8_t commandID = cmd->commandId;
	uint16_t attrID = (uint16_t)(cmd->buffer[cmd->payloadStartIndex] | cmd->buffer[cmd->payloadStartIndex + 1] << 8);
	uint16_t bufIndex = cmd->payloadStartIndex + 4; 	// buffer payload start index
	uint8_t payload[1];
	payload[0] = cmd->buffer[bufIndex];

	switch(clusterID)
	{
		case ZCL_BASIC_CLUSTER_ID:
		{
			//if receive a model id from a device
			if(attrID == ZCL_MODEL_IDENTIFIER_ATTRIBUTE_ID)
			{
				// Get the model id from payload
				uint8_t modelID[20] = {0};
				uint8_t index = 0;

				for(; bufIndex < cmd->bufLen; index++, bufIndex++)
				{
					modelID[index] = cmd->buffer[bufIndex];
				}

				if(g_receiveHandler != NULL)
				{
					g_receiveHandler(nodeId, DEVICE_JOIN_NETWORK, modelID, index);
				}
			}
			return true;
		}

		case ZCL_TEMP_MEASUREMENT_CLUSTER_ID:
		{
			if(attrID == ZCL_TEMP_MEASURED_VALUE_ATTRIBUTE_ID)
			{
				if(g_receiveHandler != NULL)
				{
					g_receiveHandler(nodeId, DEVICE_SENSOR_TEMP_VALUE, payload, 1);
				}
			}

			break;
		}

		case ZCL_RELATIVE_HUMIDITY_MEASUREMENT_CLUSTER_ID:
		{
			if(attrID == ZCL_RELATIVE_HUMIDITY_MEASURED_VALUE_ATTRIBUTE_ID)
			{
				if(g_receiveHandler != NULL)
				{
					g_receiveHandler(nodeId, DEVICE_SENSOR_HUMI_VALUE, payload, 1);
				}
			}

			break;
		}

		case ZCL_ILLUM_MEASUREMENT_CLUSTER_ID:
		{
			if(attrID == ZCL_ILLUM_MEASURED_VALUE_ATTRIBUTE_ID)
			{
				if(g_receiveHandler != NULL)
				{
					g_receiveHandler(nodeId, DEVICE_SENSOR_LIGHT_VALUE, payload, 2);
				}
			}

			break;
		}

		case ZCL_IAS_ZONE_CLUSTER_ID:
		{
			if(attrID == ZCL_ZONE_STATE_ATTRIBUTE_ID)
			{
				if (payload[0] == 1)
				{
					if(g_receiveHandler != NULL)
					{
						g_receiveHandler(nodeId, DEVICE_SENSOR_LD2410_MOTION, payload, 1);
					}
				}
				else
				{
					if(g_receiveHandler != NULL)
					{
						g_receiveHandler(nodeId, DEVICE_SENSOR_LD2410_UNMOTION, payload, 1);
					}
				}
			}

			break;
		}

		case ZCL_ON_OFF_CLUSTER_ID:		// Xu ly khi nhan ban tin On/Off tu thiet bi LED
		{
			if (commandID == ZCL_ON_COMMAND_ID)
			{
				g_receiveHandler(nodeId, DEVICE_LED_ON, NULL, 0);
			}
			else if (commandID == ZCL_OFF_COMMAND_ID)
			{
				g_receiveHandler(nodeId, DEVICE_LED_OFF, NULL, 0);
			}

			break;
		}

		case ZCL_KEEPALIVE_CLUSTER_ID:
		{
			if(attrID == ZCL_KEEPALIVE_BASE_ATTRIBUTE_ID)
			{
				g_receiveHandler(nodeId, DEVICE_UPDATE_TIME, NULL, 0);
			}

			break;
		}

		default:
			break;
	}

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
