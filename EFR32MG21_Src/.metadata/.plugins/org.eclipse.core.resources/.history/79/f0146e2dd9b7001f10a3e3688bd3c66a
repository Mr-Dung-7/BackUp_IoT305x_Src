/*
 * DeviceManager.c
 *
 *  Created on: Oct 30, 2024
 *      Author: Mr.hDung
 */

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include "Source/Utils/DeviceManager/DeviceManager.h"

/******************************************************************************/
/*                                 DEFINEs                                    */
/******************************************************************************/


/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/


/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/
static uint8_t g_index = 0;
Device_t deviceList[DEVICE_NUMBER_MAX];

/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/


/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/
/*
 * @func:		DeviceManager_AddDeviceToList
 *
 * @brief:
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void DeviceManager_AddDeviceToList (EmberNodeId nodeId, char *modelId)
{
	// Check if the array is full (If the array is full, no new devices can be added)
	if (g_index >= DEVICE_NUMBER_MAX)
	{
		return;
	}

	// Check if the device already exists in the list
	uint8_t indexDevice = DeviceManager_GetDeviceIndex(nodeId);
	if (indexDevice != DEVICE_INDEX_INVALID)
	{
		return;		// The device already exists, not adding
	}

	// Create a new device
	Device_t newDevice;

	newDevice.nodeID = nodeId;
	newDevice.ConnectState = ONLINE;

	strncpy(newDevice.modelID, modelId, sizeof(newDevice.modelID) - 1);
	newDevice.modelID[sizeof(newDevice.modelID) - 1] = '\0';

	// Add the new device to the list
	deviceList[g_index] = newDevice;
	g_index++;
}

/*
 * @func:		DeviceManager_RemoveDeviceFromList
 *
 * @brief:
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void DeviceManager_RemoveDeviceFromList (EmberNodeId nodeId)
{
	uint8_t indexDevice = DeviceManager_GetDeviceIndex(nodeId);

	if (indexDevice != DEVICE_INDEX_INVALID)
	{
        for (uint8_t i = indexDevice; i < g_index - 1; i++)
        {
        	deviceList[i] = deviceList[i + 1];
        }

        g_index--;
	}
}

/*
 * @func:		DeviceManager_RemoveDeviceFromNetwork
 *
 * @brief:
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void DeviceManager_RemoveDeviceFromNetwork (EmberNodeId nodeId)
{
	emberSendZigbeeLeave(nodeId, EMBER_ZIGBEE_LEAVE_WITHOUT_REJOIN);
	DeviceManager_RemoveDeviceFromList(nodeId);
}

/*
 * @func:		DeviceManager_GetDeviceIndex
 *
 * @brief:		Ham kiem tra thiet bi co trong danh sach hay khong va lay vi tri luu thiet bi
 *
 * @params:		nodeId
 *
 * @retVal:		DEVICE_INDEX_INVALID - device not found
 *
 * @note:		None
 */
uint8_t DeviceManager_GetDeviceIndex (EmberNodeId nodeId)
{
	for (uint8_t i = 0; i < g_index; i++)
	{
		if (deviceList[i].nodeID == nodeId)
		{
			return i;
		}
	}

	return DEVICE_INDEX_INVALID;
}

/*
 * @func:		DeviceManager_GetDeviceIndex
 *
 * @brief:		Ham lay so luong thiet bi duoc luu trong mang
 *
 * @params:		nodeId
 *
 * @retVal:		g_index
 *
 * @note:		None
 */
uint8_t DeviceManager_GetDeviceCountToList (void)
{
	return g_index;
}

/*
 * @func:		DeviceManager_UpdateLastTime
 *
 * @brief:
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void DeviceManager_UpdateLastTime (EmberNodeId nodeId, uint32_t time)
{
	uint8_t indexDevice = DeviceManager_GetDeviceIndex(nodeId);

	if(indexDevice != DEVICE_INDEX_INVALID)
	{
		deviceList[g_index].lastTime = time;
	}
}

/*
 * @func:		DeviceManager_SetDeviceOnline
 *
 * @brief:
 *
 * @params:		nodeId
 *
 * @retVal:		return true if set online success (is currently in the list), false otherwise
 *
 * @note:		None
 */
bool DeviceManager_SetDeviceOnline(EmberNodeId nodeId)
{
	uint8_t indexDevice = DeviceManager_GetDeviceIndex(nodeId);

	if(indexDevice != DEVICE_INDEX_INVALID)
	{
		deviceList[indexDevice].ConnectState = ONLINE;
		return true;
	}

	return false;
}

/*
 * @func:		DeviceManager_SetDeviceOffline
 *
 * @brief:
 *
 * @params:		nodeId
 *
 * @retVal:		return true if set online success (is currently in the list), false otherwise
 *
 * @note:		None
 */
bool DeviceManager_SetDeviceOffline(EmberNodeId nodeId)
{
	uint8_t indexDevice = DeviceManager_GetDeviceIndex(nodeId);

	if(indexDevice != DEVICE_INDEX_INVALID)
	{
		deviceList[indexDevice].ConnectState = OFFLINE;
		return true;
	}

	return false;
}

/*
 * @func:		DeviceManager_SetLedOn
 *
 * @brief:
 *
 * @params:		nodeId
 *
 * @retVal:		return true if set online success (is currently in the list), false otherwise
 *
 * @note:		None
 */
bool DeviceManager_SetStateLedOn(EmberNodeId nodeId)
{
	uint8_t indexDevice = DeviceManager_GetDeviceIndex(nodeId);

	if(indexDevice != DEVICE_INDEX_INVALID)
	{
		deviceList[indexDevice].LedState = LED_ON;
		return true;
	}

	return false;
}

/*
 * @func:		DeviceManager_SetLedOff
 *
 * @brief:
 *
 * @params:		nodeId
 *
 * @retVal:		return true if set online success (is currently in the list), false otherwise
 *
 * @note:		None
 */
bool DeviceManager_SetStateLedOff(EmberNodeId nodeId)
{
	uint8_t indexDevice = DeviceManager_GetDeviceIndex(nodeId);

	if(indexDevice != DEVICE_INDEX_INVALID)
	{
		deviceList[indexDevice].LedState = LED_OFF;
		return true;
	}

	return false;
}

/*
 * @func:		DeviceManager_GetDeviceList
 *
 * @brief:
 *
 * @params:		None
 *
 * @retVal:
 *
 * @note:		None
 */
Device_t* DeviceManager_GetDeviceList (void)
{
	return deviceList;
}

/* END FILE */
