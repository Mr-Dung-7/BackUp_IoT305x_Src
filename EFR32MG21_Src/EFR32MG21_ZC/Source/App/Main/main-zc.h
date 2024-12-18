/*
 * main-zc.h
 *
 *  Created on: Dec 1, 2024
 *      Author: Mr.hDung
 */

#ifndef SOURCE_APP_MAIN_MAIN_ZC_H_
#define SOURCE_APP_MAIN_MAIN_ZC_H_

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include "app/framework/include/af.h"
#include "protocol/zigbee/stack/include/binding-table.h"
#include "Source/App/Network/nwk-user.h"
#include "Source/App/Receive/receive-user.h"
#include "Source/App/Send/send-user.h"
#include "Source/Mid/Button/button-user.h"
#include "Source/Mid/Led/led-user.h"
#include "Source/Mid/Timer/timer-user.h"
#include "Source/Mid/Usart/USART.h"
#include "Source/Utils/DeviceManager/DeviceManager.h"

/******************************************************************************/
/*                                 DEFINEs                                    */
/******************************************************************************/
#define ENDPOINT_1					1
#define ENDPOINT_2					2
#define ENDPOINT_3					3

#define CMD_ID						frame[5]
#define CMD_TYPE					frame[6]

#define THRESHOLD_LUX_REPORT		30
#define THRESHOLD_LUX_CONTROL_LED	500

#define DEVICE_OFFLINE_THRESHHOLD_INTERVAL			((uint32_t)(16 * 1000 * 60))

/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/
typedef enum
{
	POWER_ON_STATE,
	IDLE_STATE,
	REBOOT_STATE
} MainState_e;

/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/


/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/
void Main_NetworkHandle (NetworkState_e networkResult);
void Main_ButtonPressHandle (uint8_t button, uint8_t pressCount);
void Main_ButtonHoldHandle (uint8_t button, uint8_t holdCount);
void Main_ReceiveHandle(EmberNodeId nodeId, RECEIVE_CMD_ID_e receiveId, uint8_t *payload, uint8_t length);
void Main_Usart2RxHandle (USART_STATE_e UsartStateRx);

/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/


#endif /* SOURCE_APP_MAIN_MAIN_ZC_H_ */

/* END FILE */

