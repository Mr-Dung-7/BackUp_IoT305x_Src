/*
 * nwk-user.c
 *
 *  Created on: Jul 30, 2024
 *      Author: Mr.hDung
 */

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include "Source/App/Network/nwk-user.h"

/******************************************************************************/
/*                                 DEFINEs                                    */
/******************************************************************************/


/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/


/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/
uint32_t g_timeFindAndJoin = 0;
pNetworkHandle networkHandlerCallback = NULL;

/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/


/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/
/*
 * @func:		NETWORK_Init
 *
 * @brief:		The function initializes the network
 *
 * @params:		callback
 *
 * @retVal:		None
 *
 * @note:		None
 */
void NETWORK_Init (pNetworkHandle callback)
{
	networkHandlerCallback = callback;
}

/*
 * @func:		NETWORK_CreateNetwork
 *
 * @brief:		The function creates a network with random parameters
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void NETWORK_CreateNetwork (void)
{
	emberAfPluginNetworkCreatorStart(true);

	if(networkHandlerCallback != NULL)
	{
		networkHandlerCallback(NETWORK_CREATING);
	}
}

/*
 * @func:		NETWORK_CreateNetwork
 *
 * @brief:		The function creates a network with user-configured parameters
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void Network_CreateNetworkForm(EmberPanId panId,uint8_t radioTxPower, uint8_t channel)
{
	emberAfPluginNetworkCreatorNetworkForm(1, panId, radioTxPower, channel);

  	if(networkHandlerCallback != NULL)
  	{
  		networkHandlerCallback(NETWORK_CREATING);
  	}
}

/*
 * @func:		NETWORK_OpenNetwork
 *
 * @brief:		The function opens the network
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void NETWORK_OpenNetwork (void)
{
	emberAfPluginNetworkCreatorSecurityOpenNetwork();

	if(networkHandlerCallback != NULL)
	{
		networkHandlerCallback(NETWORK_OPENING);
	}
}

/*
 * @func:		NETWORK_CloseNetwork
 *
 * @brief:		The function closes the network
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void NETWORK_CloseNetwork (void)
{
	emberAfPluginNetworkCreatorSecurityCloseNetwork();

	if(networkHandlerCallback != NULL)
	{
		networkHandlerCallback(NETWORK_CLOSING);
	}
}

/*
 * @func:		NETWORK_DeleteNetwork
 *
 * @brief:		The function delete the network
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void NETWORK_DeleteNetwork (void)
{
	emberLeaveNetwork();
}

/* END FILE */
