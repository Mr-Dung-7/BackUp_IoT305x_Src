/*
 * DeviceManager.h
 *
 *  Created on: Oct 30, 2024
 *      Author: Mr.hDung
 */

#ifndef SOURCE_UTILS_DEVICEMANAGER_DEVICEMANAGER_H_
#define SOURCE_UTILS_DEVICEMANAGER_DEVICEMANAGER_H_

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include "app/framework/include/af.h"

/******************************************************************************/
/*                                 DEFINEs                                    */
/******************************************************************************/
#define DEVICE_NUMBER_MAX			20
#define DEVICE_INDEX_INVALID		0xFF

/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/
typedef enum
{
	OFFLINE,
	ONLINE
} ConnectState_e;

typedef enum
{
	LED_OFF,
	LED_ON
} LedState_e;

typedef struct
{
	EmberNodeId nodeID;
	ConnectState_e ConnectState;
	LedState_e LedState;
	uint32_t lastTime;
	char modelID[10];
} Device_t;

/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/


/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/
void DeviceManager_AddDeviceToList (EmberNodeId nodeId, char *modelId);
void DeviceManager_RemoveDeviceFromList (EmberNodeId nodeId);
void DeviceManager_RemoveDeviceFromNetwork (EmberNodeId nodeId);
uint8_t DeviceManager_GetDeviceIndex (EmberNodeId nodeId);
uint8_t DeviceManager_GetDeviceCountToList (void);
void DeviceManager_UpdateLastTime (EmberNodeId nodeId, uint32_t time);
bool DeviceManager_SetDeviceOnline(EmberNodeId nodeId);
bool DeviceManager_SetDeviceOffline(EmberNodeId nodeId);
bool DeviceManager_SetStateLedOn(EmberNodeId nodeId);
bool DeviceManager_SetStateLedOff(EmberNodeId nodeId);
Device_t* DeviceManager_GetDeviceList (void);

/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/


#endif /* SOURCE_UTILS_DEVICEMANAGER_DEVICEMANAGER_H_ */

/* END FILE */

