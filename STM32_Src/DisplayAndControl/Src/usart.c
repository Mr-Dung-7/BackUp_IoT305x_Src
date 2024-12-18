/*
 * usart.c
 *
 *  Created on: Dec 2, 2024
 *      Author: Mr.hDung
 */

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include "usart.h"

/******************************************************************************/
/*                                 DEFINEs                                    */
/******************************************************************************/


/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/


/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/
pUsartStateCallback USART6StateCallback = NULL;

// Variable storing the position of an element in the array holding data retrieved
// from the queue
uint8_t 		g_IndexRxBuf = 0;

// Array storing data passed to FIFO (input data)
uint8_t 		g_strRxBufData[SIZE_QUEUE_DATA_RX];

// Received data storage array
uint16_t 		g_strRxBuffer[RX_BUFFER_SIZE] = {0};

// Pointer to reference the FIFO object
buffqueue_t 	g_serialQueueRx;

uint8_t 		g_Data_Receive = 0;

/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/


/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/
/*
 * @func:  		USART6_FifoAndUsartInit
 *
 * @brief:		The function to initialize the FIFO buffer and USART6
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART6_FifoAndUsartInit (void)
{
	bufInit(g_strRxBufData, &g_serialQueueRx, sizeof(g_strRxBufData[0]), SIZE_QUEUE_DATA_RX);
	USART6_Init();
}

/*
 * @func:  		USART6_Init
 *
 * @brief:		The function to configure USART6 communication
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART6_Init (void)
{
	// Declare variables of data type struct GPIO, USART, NVIC-----------------------------
	GPIO_InitTypeDef	GPIO_InitStruct;
	USART_InitTypeDef	USART_InitStruct;
	NVIC_InitTypeDef	NVIC_InitStruct;

	/* Initialize GPIO with alternate function in Open-drain mode--------------------------*/
	// Clock supply for GPIO---------------------------------------------------------------
	RCC_AHB1PeriphClockCmd(USART6_GPIO_RCC, ENABLE);

	// Configure alternate function for GPIO pin-------------------------------------------
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;

	GPIO_InitStruct.GPIO_Pin = USART6_TX_GPIO_PIN;
	GPIO_Init(USART6_TX_GPIO_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = USART6_RX_GPIO_PIN;
	GPIO_Init(USART6_RX_GPIO_PORT, &GPIO_InitStruct);

	// Mapping Pin AF----------------------------------------------------------------------
	GPIO_PinAFConfig(USART6_TX_GPIO_PORT, GPIO_PinSource6, GPIO_AF_USART6);
	GPIO_PinAFConfig(USART6_RX_GPIO_PORT, GPIO_PinSource12, GPIO_AF_USART6);

	/* Configure USART6--------------------------------------------------------------------*/
	// Clock supply for USART6-------------------------------------------------------------
	RCC_APB2PeriphClockCmd(USART6_RCC, ENABLE);

	// Configure USART6--------------------------------------------------------------------
	USART_InitStruct.USART_BaudRate = USART_BAUDRATE;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;

	USART_Init(USART6, &USART_InitStruct);

	USART_Cmd(USART6, ENABLE);

	/* Configure interrupts for USART------------------------------------------------------*/
	// Clock supply for NVIC---------------------------------------------------------------
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);

	// NVIC Configuration------------------------------------------------------------------
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	// Configure NVIC----------------------------------------------------------------------
	NVIC_InitStruct.NVIC_IRQChannel = USART6_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;

	NVIC_Init(&NVIC_InitStruct);
}

/*
 * @func:  		USART6Modify_IRQHandler
 *
 * @brief:		The function to handle USART6 interrupt
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART6Modify_IRQHandler (void)
{
	if (USART_GetITStatus(USART6, USART_IT_RXNE) == SET)
	{
		g_Data_Receive = (uint8_t)USART_ReceiveData(USART6);
		bufEnDat(&g_serialQueueRx, &g_Data_Receive);
	}

	USART_ClearITPendingBit(USART6, USART_IT_RXNE);
}

/*
 * @func:  		USART6_HandleEventCallback
 *
 * @brief:
 *
 * @param:		callback
 *
 * @retval:		None
 *
 * @note:		None
 *
 */
void USART6_HandleEventCallback (pUsartStateCallback callback)
{
	USART6StateCallback = callback;
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
	USART_STATE_e	RxState = PollRxBuff();

	if (RxState == USART_STATE_DATA_RECEIVED || RxState == USART_STATE_ERROR)
	{
		if (USART6StateCallback != NULL)
		{
			USART6StateCallback(RxState);
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
uint8_t PollRxBuff (void)
{
	// Variable to store value after retrieved from Queue
	uint8_t byRxData;

	uint8_t byUartState = (uint8_t) USART_STATE_IDLE;
	uint8_t RxBufState = RX_STATE_START_BYTE;
	uint8_t byCheckXorRxBuf = 0;

	while ((bufNumItems(&g_serialQueueRx) != 0) && (byUartState == USART_STATE_IDLE))
	{
		bufDeDat(&g_serialQueueRx, &byRxData);			// Retrieve data from the queue (FIFO)

		switch (RxBufState)
		{
			case RX_STATE_START_BYTE:
			{
				if (byRxData == FRAME_START)
				{
					g_IndexRxBuf = 0;
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
					// Retrieve the bytes in the transmission frame
					g_strRxBuffer[g_IndexRxBuf] = byRxData;

					if (g_IndexRxBuf > 0)
					{
						byCheckXorRxBuf ^= byRxData;		// Calculator CXOR
					}

					g_IndexRxBuf++;

					if (g_IndexRxBuf == *g_strRxBuffer)
					{
						RxBufState = RX_STATE_CXOR_BYTE;
					}
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
uint8_t* USART_GetFrame (void)
{
	return g_strRxBuffer;
}

/*
 * @func:  		USART_SendPacket
 *
 * @brief:		The function sends data in the specified format
 *
 * @param[1]:	byNodeId - Byte nodeid of the frame
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
void USART_SendPacket (uint16_t byNodeId,		\
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

	for (uint8_t k = 0; k < index; k++)
	{
		while(USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET);
		USART_SendData(USART6, Frame[k]);
		while(USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET);
	}
}

/*
 * @func:		USART_SendCreateNetwork
 *
 * @brief:		The function sends the network creating message
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART_SendCreateNetwork (void)
{
	USART_SendPacket(NODE_ID_CONTROLLER,
					 0x01,
					 CMD_ID_NETWORK_CREATING,
					 CMD_TYPE_SET,
					 NULL,
					 0);
}

/*
 * @func:		USART_SendDeleteNetwork
 *
 * @brief:		The function sends the message to remove the device from the network
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART_SendDeleteNetwork (void)
{
	USART_SendPacket(NODE_ID_CONTROLLER,
					 0x01,
					 CMD_ID_NETWORK_DELETE,
					 CMD_TYPE_SET,
					 NULL,
					 0);
}

/*
 * @func:		USART_SendOpenNetwork
 *
 * @brief:		The function sends the network opening message
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART_SendOpenNetwork (void)
{
	USART_SendPacket(NODE_ID_CONTROLLER,
					 0x01,
					 CMD_ID_NETWORK_OPENING,
					 CMD_TYPE_SET,
					 NULL,
					 0);
}

/*
 * @func:		USART_SendCloseNetwork
 *
 * @brief:		The function sends the network closing message
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART_SendCloseNetwork (void)
{
	USART_SendPacket(NODE_ID_CONTROLLER,
					 0x01,
					 CMD_ID_NETWORK_CLOSING,
					 CMD_TYPE_SET,
					 NULL,
					 0);
}

/*
 * @func:		USART_SendDistanceValue
 *
 * @brief:
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART_SendDistanceValue (uint8_t value)
{
	USART_SendPacket(NODE_ID_CONTROLLER,
					 0x01,
					 CMD_ID_DISTANCE,
					 CMD_TYPE_SET,
					 (uint8_t*)value,
					 1);
}

/*
 * @func:		USART_SendSensitivityValue
 *
 * @brief:
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART_SendSensitivityValue (uint8_t value)
{
	USART_SendPacket(NODE_ID_CONTROLLER,
					 0x01,
					 CMD_ID_SENSITIVITY,
					 CMD_TYPE_SET,
					 (uint8_t*)value,
					 1);
}

/*
 * @func:		USART_SendLatencyValue
 *
 * @brief:
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART_SendLatencyValue (uint8_t value)
{
	USART_SendPacket(NODE_ID_CONTROLLER,
					 0x01,
					 CMD_ID_LATENCY,
					 CMD_TYPE_SET,
					 (uint8_t*)value,
					 1);
}

/*
 * @func:		USART_SendLightValue
 *
 * @brief:
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART_SendLightValue (uint16_t value)
{
	USART_SendPacket(NODE_ID_CONTROLLER,
					 0x01,
					 CMD_ID_LIGHT,
					 CMD_TYPE_SET,
					 (uint16_t*)value,
					 2);
}

/*
 * @func:		USART_SendLedState
 *
 * @brief:
 *
 * @param:		state
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART_SendLedState (uint8_t state)
{
//	uint8_t payload[1] = {state};

	USART_SendPacket(NODE_ID_NOT_USE,
					 ENDPOINT_NOT_USE,
					 CMD_ID_LED,
					 CMD_TYPE_SET,
					 (uint8_t*)state,	// payload
					 1);
}

/*
 * @func:		USART_ReceivedData
 *
 * @brief:
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART_ReceivedData (void)
{
	uint8_t *frame = USART_GetFrame();

	switch (CMD_ID)
	{
		case CMD_ID_TEMP_SENSOR:
		{
			if (CMD_TYPE == CMD_TYPE_RES)
			{
				LCD_DisplayTempValue(CMD_DATA_TEMP);
			}

			break;
		}

		case CMD_ID_HUMI_SENSOR:
		{
			if (CMD_TYPE == CMD_TYPE_RES)
			{
				LCD_DisplayHumiValue(CMD_DATA_HUMI);
			}

			break;
		}

		case CMD_ID_LIGHT_SENSOR:
		{
			if (CMD_TYPE == CMD_TYPE_RES)
			{
				LCD_DisplayLightValue(CMD_DATA_LIGHT);
			}

			break;
		}

		case CMD_ID_DETEC:
		{
			if (CMD_TYPE == CMD_TYPE_RES && CMD_DATA_DETEC == CMD_DATA_MOTION)
			{
				LCD_DisplayDetectionState(CMD_DATA_MOTION);
				LCD_DisplayLedState(CMD_DATA_LED_ON);
			}
			else if (CMD_TYPE == CMD_TYPE_RES && CMD_DATA_DETEC == CMD_DATA_UNMOTION)
			{
				LCD_DisplayDetectionState(CMD_DATA_UNMOTION);
				LCD_DisplayLedState(CMD_DATA_LED_OFF);
			}

			break;
		}

		case CMD_ID_LED:
		{
			if (CMD_TYPE == CMD_TYPE_RES && CMD_DATA_LED == CMD_DATA_LED_ON)
			{
				LCD_DisplayLedState(CMD_DATA_LED_ON);
			}
			else if (CMD_TYPE == CMD_TYPE_RES && CMD_DATA_LED == CMD_DATA_LED_OFF)
			{
				LCD_DisplayLedState(CMD_DATA_LED_OFF);
			}

			break;
		}

		default:
			break;
	}
}


void USART_SendFrame(uint8_t* frame, uint16_t length){
	for (uint8_t k = 0; k < length; k++)
	{
		while(USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET);
		USART_SendData(USART6, frame[k]);
		while(USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET);
	}
}


/* END FILE */
