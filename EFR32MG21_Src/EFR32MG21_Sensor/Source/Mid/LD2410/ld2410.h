/*
 * ld2410.h
 *
 *  Created on: Sep 8, 2023
 *      Author: Administrator
 */

#ifndef SOURCE_MID_LD2410_LD2410_H_
#define SOURCE_MID_LD2410_LD2410_H_

/******************************************************************************/
/*                              INCLUDEs                                      */
/******************************************************************************/
#include "app/framework/include/af.h"
#include "Source/App/Send/send-user.h"
#include "Source/Mid/Led/led-user.h"

/******************************************************************************/
/*                               DEFINEs                              		  */
/******************************************************************************/
#define LD2410_PORT				gpioPortC
#define LD2410_PIN				(3U)
#define LD2410_EXTI_CHANNEL		(LD2410_PIN)

#define ENDPOINT_3				3

/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/
typedef enum
{
	LD2410_STATE_DEBOUNCE,
	LD2410_STATE_WAIT_5S,
	LD2410_STATE_WAIT_30S
} LD2410_State_e;

typedef enum
{
	LD2410_UNMOTION,
	LD2410_MOTION
} LD2410_Action_e;

/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/
//typedef void (*pLd2410ActionHandle)(uint8_t);

/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/
void LD2410_Init (void);
void LD2410_Enable (boolean en);
boolean LD2410_isMotionSignal (void);
void LD2410_INTSignalHandle (uint8_t intNo);
void LD2410_Handle (LD2410_Action_e action);


/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/


#endif /* SOURCE_MID_LD2410_LD2410_H_ */

/* END FILE */
