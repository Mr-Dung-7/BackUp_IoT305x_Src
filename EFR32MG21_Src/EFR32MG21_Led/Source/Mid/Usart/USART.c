/*
 * USART.c
 *
 *  Created on: Sep 25, 2024
 *      Author: Mr.hDung
 */

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include "Source/Mid/Usart/USART.h"

/******************************************************************************/
/*                                 DEFINEs                                    */
/******************************************************************************/


/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/


/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/
EmberEventControl usart2ScanEventControl;

pUsartStateCallback usart2StateCallback = NULL;

// Variable storing the position of an element in the array holding data retrieved
// from the buffer
uint8_t 	g_IndexRxBuf = 0;

// Received data storage array
uint8_t 	g_strRxBuffer[RX_BUFFER_SIZE] = {0};

/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/


/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/
/*
 * @func:  		USART2_Init
 *
 * @brief:		Initialize USART
 *
 * @param:		callback
 *
 * @retval:		None
 *
 * @note:
 * 				+ USART2: Tx - PC01
 * 				+ USART2: Rx - PC02
 */
void USART2_Init (pUsartStateCallback callback)
{
	emberSerialInit(COM_USART2, 115200, PARITY_NONE, 1);

	usart2StateCallback = callback;

	// Start the scan event, with a delay on the first call: waiting for peripheral initialization
	emberEventControlSetDelayMS(usart2ScanEventControl, 1000);
}

/*
 * @func:  		usart2ScanEventHandler
 *
 * @brief:		Periodic data retrieval function
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void usart2ScanEventHandler (void)
{
	emberEventControlSetInactive(usart2ScanEventControl);

	uint8_t 	dataByte;
	uint16_t 	bytesReceived = 0;        // Variable storing the number of received bytes

	// Get the number of received bytes
	uint16_t numOfByteAvail = emberSerialReadAvailable(COM_USART2);

	if(numOfByteAvail > 0)
	{
		while (numOfByteAvail > 0 && bytesReceived < RX_BUFFER_SIZE)
		{
			emberSerialReadByte(COM_USART2, &dataByte);

			g_strRxBuffer[bytesReceived++] = dataByte;		// Store the received byte in the buffer
			numOfByteAvail--;
		}

		processSerialHandle();
	}

	emberEventControlSetDelayMS(usart2ScanEventControl, USART_PROCESS_SERIAL_INTERVAL);
}

/*
 * @func:  		processSerialHandle
 *
 * @brief:		The function retrieves and sends data to the main function for event execution
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void processSerialHandle (void)
{
	USART_STATE_e	RxState = PollRxBuff(COM_USART2);

	if (RxState == USART_STATE_DATA_RECEIVED || RxState == USART_STATE_ERROR)
	{
		if (usart2StateCallback != NULL)
		{
			usart2StateCallback(RxState);
		}
	}
}

/*
 * @func:  		PollRxBuff
 *
 * @brief:		The function to process received messages based on format
 *
 * @param:		port - USART port
 *
 * @retval:		byUartState - USART state
 *
 * @note:		None
 */
uint8_t PollRxBuff (uint8_t port)
{
	uint8_t 	byUartState = (uint8_t) USART_STATE_IDLE;
	uint8_t 	RxBufState = RX_STATE_START_BYTE;
	uint8_t 	byCheckXorRxBuf = 0;

	g_IndexRxBuf = 0;

	while ((port == COM_USART2) && (byUartState == USART_STATE_IDLE))
	{
		// Variable to store value after retrieved from buffer
		uint8_t 	byRxData = g_strRxBuffer[g_IndexRxBuf];

		switch (RxBufState)
		{
			case RX_STATE_START_BYTE:
			{
				if (byRxData == FRAME_START)
				{
					g_IndexRxBuf++;
					byCheckXorRxBuf = CXOR_INIT_VAL;
					RxBufState = RX_STATE_DATA_BYTES;
				}
				else
				{
					byUartState = USART_STATE_EMPTY;	// Buffer EMPTY
				}
			} break;

			case RX_STATE_DATA_BYTES:
			{
				if (g_IndexRxBuf < RX_BUFFER_SIZE)
				{
					if (g_IndexRxBuf > 1)
					{
						byCheckXorRxBuf ^= byRxData;		// Calculate CXOR
					}

					if (g_IndexRxBuf == *(g_strRxBuffer + 1))		// g_IndexRxBuf == FRAME_LENGTH
					{
						RxBufState = RX_STATE_CXOR_BYTE;
					}

					g_IndexRxBuf++;
				}
				else
				{
					RxBufState = RX_STATE_START_BYTE;
					byUartState = USART_STATE_DATA_ERROR;
				}
			} break;

			case RX_STATE_CXOR_BYTE:
			{
				if (byRxData == byCheckXorRxBuf)
				{
					byUartState = USART_STATE_DATA_RECEIVED;
				}
				else
				{
					byUartState = USART_STATE_ERROR;
				}

				RxBufState = RX_STATE_START_BYTE;
			} break;

			default:
				RxBufState = RX_STATE_START_BYTE;
				break;
		}
	}

	return byUartState;
}

/*
 * @func:  		GetFrame
 *
 * @brief:		The function get frame
 *
 * @param:		None
 *
 * @retval:		g_strRxBuffer
 *
 * @note:		None
 */
uint8_t* GetFrame (void)
{
	return g_strRxBuffer;
}

/*
 * @func:  		USART_SendPacket
 *
 * @brief:		The function sends data in the specified format
 *
 * @param[1]:	byNodeId - Byte nodeID of the frame
 * @param[2]:	byEndpoint - Byte endpoint of the frame
 * @param[3]:	byCmdId - Byte cmdId of the frame
 * @param[4]:	byCmdType - Byte cmdType of the frame
 * @param[5]:	pPayload - Byte Data of the frame
 * @param[6]:	byLengthPayload - Data length
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART_SendPacket (EmberNodeId byNodeId,	\
					   uint8_t byEndpoint,		\
					   uint8_t byCmdId, 		\
					   uint8_t byCmdType,		\
					   uint8_t *pPayload,		\
					   uint8_t byLengthPayload)
{
	uint8_t index = 0;
	uint8_t byLength = byLengthPayload + 7;		// nodeid, endpoint, id, type, seq, cxor
	static uint8_t bySequence = CMD_SEQUENCE;
	uint8_t byCXOR = CXOR_INIT_VAL;
	uint8_t Frame[byLength + 2];

	Frame[index++] = FRAME_START;
	Frame[index++] = byLength;
	Frame[index++] = (byNodeId >> 8) & 0xFF;
	Frame[index++] = byNodeId & 0xFF;
	Frame[index++] = byEndpoint;
	Frame[index++] = byCmdId;
	Frame[index++] = byCmdType;

	for (uint8_t i = 0; i < byLengthPayload; i++)
	{
		Frame[index++] = pPayload[i];
	}

	Frame[index++] = bySequence++;

	for (uint8_t j = 2; j < index; j++)
	{
		byCXOR ^= Frame[j];
	}
	Frame[index++] = byCXOR;

	emberSerialWriteData(COM_USART2, Frame, sizeof(Frame));
}

/*
 * @func:		USART_CreateNetwork
 *
 * @brief:		The function sends the network creating message
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART_CreateNetwork (void)
{
	USART_SendPacket(NODE_ID_CONTROLLER,
					 0x01,
					 CMD_ID_NETWORK_CREATING,
					 CMD_TYPE_RES,
					 NULL,
					 0);
}

/*
 * @func:		USART_OpenNetwork
 *
 * @brief:		The function sends the network opening message
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART_OpenNetwork (void)
{
	USART_SendPacket(NODE_ID_CONTROLLER,
					 0x01,
					 CMD_ID_NETWORK_OPENING,
					 CMD_TYPE_RES,
					 NULL,
					 0);
}

/*
 * @func:		USART_CloseNetwork
 *
 * @brief:		The function sends the network closing message
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART_CloseNetwork (void)
{
	USART_SendPacket(NODE_ID_CONTROLLER,
					 0x01,
					 CMD_ID_NETWORK_CLOSING,
					 CMD_TYPE_RES,
					 NULL,
					 0);
}

/*
 * @func:		USART_DeleteNetwork
 *
 * @brief:		The function sends the message to remove the device from the network
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART_DeleteNetwork (void)
{
	USART_SendPacket(NODE_ID_CONTROLLER,
					 0x01,
					 CMD_ID_NETWORK_DELETE,
					 CMD_TYPE_RES,
					 NULL,
					 0);
}

/*
 * @func:		USART_DeviceJoinNetwork
 *
 * @brief:		The function sends the device joining network message
 *
 * @param[1]:	nodeId
 * @param[2]:	deviceType
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART_DeviceJoinNetwork (EmberNodeId nodeId, char *deviceType)
{
	USART_SendPacket(nodeId,
					 0x01,
					 CMD_ID_DEVICE_JOIN_NETWORK,
					 CMD_TYPE_RES,
					 (uint8_t*)deviceType,
					 sizeof(deviceType));
}

/*
 * @func:		USART_DeviceLeaveNetwork
 *
 * @brief:		The function sends the device leaving network message
 *
 * @param[1]:	nodeId
 * @param[2]:	deviceType
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART_DeviceLeaveNetwork (EmberNodeId nodeId, char *deviceType)
{
	USART_SendPacket(nodeId,
					 0x01,
					 CMD_ID_DEVICE_LEAVE_NETWORK,
					 CMD_TYPE_RES,
					 (uint8_t*)deviceType,
					 sizeof(deviceType));
}

/*
 * @func:		USART_DeviceDelete
 *
 * @brief:		The function sends the message to remove the device from the network
 *
 * @param[1]:	nodeId
 * @param[2]:	deviceType
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART_DeviceDelete (EmberNodeId nodeId, char *deviceType)
{
	USART_SendPacket(nodeId,
					 0x01,
					 CMD_ID_DEVICE_DELETE,
					 CMD_TYPE_RES,
					 (uint8_t*)deviceType,
					 sizeof(deviceType));
}

/*
 * @func:		USART_DeviceConnected
 *
 * @brief:		The function sends the device's connection status message
 *
 * @param:		nodeId
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART_DeviceConnected (EmberNodeId nodeId)
{
	USART_SendPacket(nodeId, 0x01, CMD_ID_DEVICE_CONNECTED, CMD_TYPE_RES, NULL, 0);
}

/*
 * @func:		USART_DeviceDisconnected
 *
 * @brief:		The function sends the device's disconnection status message
 *
 * @param:		nodeId
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART_DeviceDisconnected (EmberNodeId nodeId)
{
	USART_SendPacket(nodeId, 0x01, CMD_ID_DEVICE_DISCONNECTED, CMD_TYPE_RES, NULL, 0);
}

/*
 * @func:		USART_TempValue
 *
 * @brief:
 *
 * @param:		nodeId
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART_TempValue (uint8_t temperature)
{
	uint8_t payload[1] = {temperature};

	USART_SendPacket(NODE_ID_NOT_USE,		\
					 ENDPOINT_NOT_USE, 		\
					 CMD_ID_TEMP_SENSOR, 	\
					 CMD_TYPE_RES, 			\
					 payload, 				\
					 1);
}

/*
 * @func:		USART_HumiValue
 *
 * @brief:
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART_HumiValue (uint8_t humidity)
{
	uint8_t payload[1] = {humidity};

	USART_SendPacket(NODE_ID_NOT_USE,		\
					 ENDPOINT_NOT_USE, 		\
					 CMD_ID_HUMI_SENSOR, 	\
					 CMD_TYPE_RES, 			\
					 payload, 				\
					 1);
}

/*
 * @func:		USART_LightValue
 *
 * @brief:
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART_LightValue (uint16_t light)
{
	uint16_t payload[1] = {light};

	USART_SendPacket(NODE_ID_NOT_USE,		\
					 ENDPOINT_NOT_USE, 		\
					 CMD_ID_LIGHT_SENSOR, 	\
					 CMD_TYPE_RES, 			\
					 payload, 				\
					 2);
}

/*
 * @func:		USART_DetectionState
 *
 * @brief:
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART_DetectionState (uint8_t state)
{
	uint8_t payload[1] = {state};

	USART_SendPacket(NODE_ID_NOT_USE,		\
					 ENDPOINT_NOT_USE, 		\
					 CMD_ID_DETEC, 			\
					 CMD_TYPE_RES, 			\
					 payload, 				\
					 1);
}

/*
 * @func:		USART_LedControl
 *
 * @brief:
 *
 * @param:		nodeId
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART_LedControl (uint8_t state)
{
	uint8_t payload[1] = {state};

	USART_SendPacket(NODE_ID_NOT_USE,		\
					 ENDPOINT_NOT_USE, 		\
					 CMD_ID_LED, 			\
					 CMD_TYPE_RES, 			\
					 payload, 				\
					 1);
}

/* END FILE */
