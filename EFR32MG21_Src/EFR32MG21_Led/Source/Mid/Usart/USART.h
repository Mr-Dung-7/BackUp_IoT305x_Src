/*
 * USART.h
 *
 *  Created on: Sep 25, 2024
 *      Author: Mr.hDung
 */

#ifndef SOURCE_MID_USART_USART_H_
#define SOURCE_MID_USART_USART_H_

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include <stdbool.h>
#include <stdlib.h>
#include "app/framework/include/af.h"

/******************************************************************************/
/*                                 DEFINEs                                    */
/******************************************************************************/

// The size of the array storing data retrieved from the buffer
#define RX_BUFFER_SIZE 					256

#define FRAME_START 					0xB1

#define CMD_ID_NETWORK_CREATING			0x01
#define CMD_ID_NETWORK_OPENING			0x02
#define CMD_ID_NETWORK_CLOSING			0x03
#define CMD_ID_NETWORK_DELETE			0x04
#define CMD_ID_NETWORK_REPORT			0x05

#define CMD_ID_DEVICE_JOIN_NETWORK		0x06
#define CMD_ID_DEVICE_LEAVE_NETWORK		0x07
#define CMD_ID_DEVICE_CONNECTED			0x08
#define CMD_ID_DEVICE_DISCONNECTED		0x09
#define CMD_ID_DEVICE_DELETE			0x10

#define CMD_ID_LED						0x81
#define CMD_ID_TEMP_SENSOR 				0x82
#define CMD_ID_HUMI_SENSOR 				0x83
#define CMD_ID_LIGHT_SENSOR 			0x84
#define CMD_ID_DETEC					0x85

// Message retrieving information and status of the device from the host computer
#define CMD_TYPE_GET               		0x00

// Message responding with the device's information and status to the host computer
#define CMD_TYPE_RES               	 	0x01

// Message for controlling the device from the host computer
#define CMD_TYPE_SET               	 	0x02

#define CMD_DATA_UNMOTION				0x00
#define CMD_DATA_MOTION					0x01
#define CMD_DATA_LED_OFF				0x00
#define CMD_DATA_LED_ON					0x01

#define CMD_SEQUENCE					0x00
#define CXOR_INIT_VAL 					0xFF

#define NODE_ID_CONTROLLER				0x0000
#define NODE_ID_NOT_USE					0xFFEE
#define ENDPOINT_NOT_USE				0xFE

#define USART_PROCESS_SERIAL_INTERVAL	50

/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/
typedef enum
{
    RX_STATE_START_BYTE,
    RX_STATE_DATA_BYTES,
    RX_STATE_CXOR_BYTE
} RX_STATE_e;

typedef enum
{
    USART_STATE_IDLE,
	USART_STATE_EMPTY,
    USART_STATE_DATA_RECEIVED,
	USART_STATE_DATA_ERROR,
    USART_STATE_ERROR,
    USART_STATE_RX_TIMEOUT,
} USART_STATE_e;


/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/
typedef void (*pUsartStateCallback)(USART_STATE_e);

/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/
void USART2_Init (pUsartStateCallback callback);
void processSerialHandle (void);
uint8_t PollRxBuff (uint8_t port);
uint8_t* GetFrame (void);
void USART_SendPacket (EmberNodeId byNodeId,	\
					   uint8_t byEndpoint,		\
					   uint8_t byCmdId, 		\
					   uint8_t byCmdType,		\
					   uint8_t *pPayload,		\
					   uint8_t byLengthPayload);
void USART_CreateNetwork (void);
void USART_OpenNetwork (void);
void USART_CloseNetwork (void);
void USART_DeleteNetwork (void);
void USART_DeviceJoinNetwork (EmberNodeId nodeId, char *deviceType);
void USART_DeviceLeaveNetwork (EmberNodeId nodeId, char *deviceType);
void USART_DeviceDelete (EmberNodeId nodeId, char *deviceType);
void USART_DeviceConnected (EmberNodeId nodeId);
void USART_DeviceDisconnected (EmberNodeId nodeId);
void USART_TempValue (uint8_t temperature);
void USART_HumiValue (uint8_t humidity);
void USART_LightValue (uint16_t light);
void USART_DetectionState (uint8_t state);
void USART_LedControl (uint8_t state);

/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/


#endif /* SOURCE_MID_USART_USART_H_ */

/* END FILE */
