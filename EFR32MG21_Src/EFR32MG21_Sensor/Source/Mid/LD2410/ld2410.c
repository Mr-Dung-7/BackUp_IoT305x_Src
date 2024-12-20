/*
 * ld2410.c
 *
 *  Created on: Sep 8, 2023
 *      Author: Administrator
 */

/******************************************************************************/
/*                              INCLUDEs                                      */
/******************************************************************************/
#include "Source/Mid/LD2410/ld2410.h"

/******************************************************************************/
/*                               DEFINEs                              		  */
/******************************************************************************/


/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/


/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/
EmberEventControl Ld2410DetectEventControl;

LD2410_State_e g_ld2410State = LD2410_STATE_DEBOUNCE;

static uint8_t g_frame[50] = {0};
static uint16_t g_length = 0;

/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/


/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/
/*
 * @func:		LD2410_Init
 *
 * @brief:		Ham khoi tao LD2410
 *
 * @params:		handle
 *
 * @retVal:		None
 *
 * @note:		None
 */
void LD2410_Init (void)
{
	CMU_ClockEnable(cmuClock_GPIO, true);

	GPIO_PinModeSet(LD2410_PORT, LD2410_PIN, gpioModeInputPull, 1);

	// Register callbacks before setting up and enabling pin interrupt
	GPIOINT_CallbackRegister(LD2410_EXTI_CHANNEL, LD2410_INTSignalHandle);

	emberEventControlSetActive(Ld2410DetectEventControl);

	LD2410_Enable(true);
}

/*
 * @func:		LD2410_Enable
 *
 * @brief:		Ham kich hoat cam bien LD2410
 *
 * @params:		en
 *
 * @retVal:		None
 *
 * @note:		None
 */
void LD2410_Enable (boolean en)
{
	if(en)
	{
		GPIO_ExtIntConfig(LD2410_PORT, LD2410_PIN, LD2410_EXTI_CHANNEL, true, false, true);
	}
	else
	{
		GPIO_ExtIntConfig(LD2410_PORT, LD2410_PIN, LD2410_EXTI_CHANNEL, false, false, false);
	}
}

/*
 * @func:		LD2410_isMotionSignal
 *
 * @brief:		Ham lay tin hieu tu GPIO Pin khi co chuyen dong
 *
 * @params:		None
 *
 * @retVal:		true / false
 *
 * @note:		None
 */
boolean LD2410_isMotionSignal (void)
{
	uint8_t ret = (uint8_t)(GPIO_PinOutGet(LD2410_PORT, LD2410_PIN));
	return (ret & 0x01);
}

/*
 * @func:		LD2410_INTSignalHandle
 *
 * @brief:		Ham xu ly tin hieu ngat tu chan cam bien chuyen dong
 *
 * @params:		intNo
 *
 * @retVal:		None
 *
 * @note:		None
 */
void LD2410_INTSignalHandle (uint8_t intNo)
{
//	emberAfCorePrintln("LD2410_INTSignalHandle");

	if (intNo != (LD2410_EXTI_CHANNEL))
	{
		return;
	}

	if (LD2410_isMotionSignal())
	{
		g_ld2410State = LD2410_STATE_DEBOUNCE;
		LD2410_Enable(false);

		emberEventControlSetInactive(Ld2410DetectEventControl);
		emberEventControlSetDelayMS(Ld2410DetectEventControl, 200);	// Delay 200ms de tranh nhieu
	}
}

/*
 * @func:		LD2410_Handle
 *
 * @brief:		Ham xu ly cac su kien cua LD2410 va gui thong tin ve ZC
 *
 * @params:		action
 *
 * @retVal:		None
 *
 * @note:		None
 */
void LD2410_Handle (LD2410_Action_e action)
{
//	static boolean sendFlag = true;
//
//	switch(action)
//	{
//		case LD2410_MOTION:
//		{
//			if(sendFlag)
//			{
//				sendFlag = false;
//				SEND_LD2410StateReport(ENDPOINT_3, LD2410_MOTION);
//				SEND_ResendZclCommandViaBinding(ENDPOINT_3, 1, 1, emberAfGetNodeId());
//			}
//
//			led_turnOn(LED_2, CYAN);
//		} break;
//
//		case LD2410_UNMOTION:
//		{
//			sendFlag = true;
//			led_turnOff(LED_2);
//			SEND_LD2410StateReport(ENDPOINT_3, LD2410_UNMOTION);
//			SEND_ResendZclCommandViaBinding(ENDPOINT_3, 1, 0, emberAfGetNodeId());
//		} break;
//
//		default:
//			break;
//	}
}

/*
 * @func:		Ld2410DetectEventHandler
 *
 * @brief:		Ham xu ly cac trang thai cua cam bien chuyen dong
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void Ld2410DetectEventHandler (void)
{
//	emberEventControlSetInactive(Ld2410DetectEventControl);
//
//	switch(g_ld2410State)
//	{
//		case LD2410_STATE_DEBOUNCE:
//		{
//			if (LD2410_isMotionSignal())
//			{
//				emberAfCorePrintln("LD2410_DETECT_MOTION");
//				g_ld2410State = LD2410_STATE_WAIT_5S;
//
//				LD2410_Handle(LD2410_MOTION);
//
//				emberEventControlSetDelayMS(Ld2410DetectEventControl, 5000);
//			}
//			else
//			{
//				LD2410_Enable(true);
//			}
//		} break;
//
//		case LD2410_STATE_WAIT_5S:
//		{
//			g_ld2410State = LD2410_STATE_WAIT_30S;
//			LD2410_Enable(true);
//
//			emberEventControlSetDelayMS(Ld2410DetectEventControl, 30000);
//		} break;
//
//		case LD2410_STATE_WAIT_30S:
//		{
//			emberAfCorePrintln("LD2410_DETECT_UNMOTION");
//			LD2410_Handle(LD2410_UNMOTION);
//		} break;
//
//		default:
//			break;
//	}
}

/*
 * @func:		LD2410_HeaderSendAndACK
 *
 * @brief:		Header cho ban tin commands va ACK
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void LD2410_HeaderSendAndACK (void)
{
	g_frame[g_length++] = 0xFD;
	g_frame[g_length++] = 0xFC;
	g_frame[g_length++] = 0xFB;
	g_frame[g_length++] = 0xFA;
}

/*
 * @func:		LD2410_EndFrameSendAndACK
 *
 * @brief:		EndFrame cho ban tin commands va ACK
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void LD2410_EndFrameSendAndACK (void)
{
	g_frame[g_length++] = 0x04;
	g_frame[g_length++] = 0x03;
	g_frame[g_length++] = 0x02;
	g_frame[g_length++] = 0x01;
}

/*
 * @func:		LD2410_FrameSetConfiguration
 *
 * @brief:		Frame Configuration
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void LD2410_FrameSetConfiguration (uint8_t enable)
{
	if (enable == 1)
	{
		// Enable configuration command
		g_length = 0;

		LD2410_HeaderSendAndACK();

		g_frame[g_length++] = 0x04;
		g_frame[g_length++] = 0x00;

		g_frame[g_length++] = 0xFF;
		g_frame[g_length++] = 0x00;

		g_frame[g_length++] = 0x01;
		g_frame[g_length++] = 0x00;

		LD2410_EndFrameSendAndACK();
	}
	else
	{
		// End configuration command
		g_length = 0;

		LD2410_HeaderSendAndACK();

		g_frame[g_length++] = 0x02;
		g_frame[g_length++] = 0x00;

		g_frame[g_length++] = 0xFE;
		g_frame[g_length++] = 0x00;

		LD2410_EndFrameSendAndACK();
	}

	USART_SendFrame(g_frame, g_length);
}

/*
 * @func:		LD2410_SetDistanceLatency
 *
 * @brief:		Ham cai dat gia tri khoang cach phat hien va do tre
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void LD2410_SetDistanceLatency (uint8_t valueMovingDistance,	\
								uint8_t valueRestingDistance,	\
								uint8_t valueLatency)
{
	LD2410_FrameSetConfiguration(CONFIG_ENABLE);

	g_length = 0;

	LD2410_HeaderSendAndACK();

	g_frame[g_length++] = 0x14;
	g_frame[g_length++] = 0x00;

	// Command word
	g_frame[g_length++] = 0x60;
	g_frame[g_length++] = 0x00;

	// Moving distance
	g_frame[g_length++] = 0x00;
	g_frame[g_length++] = 0x00;

	g_frame[g_length++] = valueMovingDistance;
	g_frame[g_length++] = 0x00;
	g_frame[g_length++] = 0x00;
	g_frame[g_length++] = 0x00;

	// Resting distance
	g_frame[g_length++] = 0x01;
	g_frame[g_length++] = 0x00;

	g_frame[g_length++] = valueRestingDistance;
	g_frame[g_length++] = 0x00;
	g_frame[g_length++] = 0x00;
	g_frame[g_length++] = 0x00;

	// Unmanned duration (Latency)
	g_frame[g_length++] = 0x02;
	g_frame[g_length++] = 0x00;

	g_frame[g_length++] = valueLatency;
	g_frame[g_length++] = 0x00;
	g_frame[g_length++] = 0x00;
	g_frame[g_length++] = 0x00;

	LD2410_EndFrameSendAndACK();

	USART_SendFrame(g_frame, g_length);

	LD2410_FrameSetConfiguration(CONFIG_END);
}

/*
 * @func:		LD2410_SetSensitivity
 *
 * @brief:		Ham cai dat gia tri do nhay
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void LD2410_SetSensitivity (uint8_t valueMotionSensitivity, uint8_t valueStaticSensitivity)
{
	LD2410_FrameSetConfiguration(CONFIG_ENABLE);

	g_length = 0;

	LD2410_HeaderSendAndACK();

	g_frame[g_length++] = 0x14;
	g_frame[g_length++] = 0x00;

	// Command word
	g_frame[g_length++] = 0x64;
	g_frame[g_length++] = 0x00;

	// Distance gate
	g_frame[g_length++] = 0x00;
	g_frame[g_length++] = 0x00;

	g_frame[g_length++] = 0xFF;
	g_frame[g_length++] = 0xFF;
	g_frame[g_length++] = 0x00;
	g_frame[g_length++] = 0x00;

	// Motion sensitivity
	g_frame[g_length++] = 0x01;
	g_frame[g_length++] = 0x00;

	g_frame[g_length++] = valueMotionSensitivity;
	g_frame[g_length++] = 0x00;
	g_frame[g_length++] = 0x00;
	g_frame[g_length++] = 0x00;

	// Static sensitivity
	g_frame[g_length++] = 0x02;
	g_frame[g_length++] = 0x00;

	g_frame[g_length++] = valueStaticSensitivity;
	g_frame[g_length++] = 0x00;
	g_frame[g_length++] = 0x00;
	g_frame[g_length++] = 0x00;

	LD2410_EndFrameSendAndACK();

	USART_SendFrame(g_frame, g_length);

	LD2410_FrameSetConfiguration(CONFIG_END);
}

/*
 * @func:		LD2410_ReadParameterCommand
 *
 * @brief:		Frame Read parameter command
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void LD2410_ReadParameterCommand (void)
{
	LD2410_FrameSetConfiguration(CONFIG_ENABLE);

	g_length = 0;

	LD2410_HeaderSendAndACK();

	g_frame[g_length++] = 0x02;
	g_frame[g_length++] = 0x00;

	// Command word
	g_frame[g_length++] = 0x61;
	g_frame[g_length++] = 0x00;

	LD2410_EndFrameSendAndACK();

	USART_SendFrame(g_frame, g_length);

	LD2410_FrameSetConfiguration(CONFIG_END);
}

/*
 * @func:		LD2410_FrameAckDistanceLatency
 *
 * @brief:		Frame ACK khi SET gia tri Distance va Latency thanh cong
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
uint8_t* LD2410_FrameAckDistanceLatency (void)
{
	g_length = 0;

	LD2410_HeaderSendAndACK();

	g_frame[g_length++] = 0x04;
	g_frame[g_length++] = 0x00;

	g_frame[g_length++] = 0x60;
	g_frame[g_length++] = 0x01;

	g_frame[g_length++] = 0x00;
	g_frame[g_length++] = 0x00;

	LD2410_EndFrameSendAndACK();

	return g_frame;
}

/*
 * @func:		LD2410_FrameAckSensitivity
 *
 * @brief:		Frame ACK khi SET gia tri Sensitivity thanh cong
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
uint8_t* LD2410_FrameAckSensitivity (void)
{
	g_length = 0;

	LD2410_HeaderSendAndACK();

	g_frame[g_length++] = 0x04;
	g_frame[g_length++] = 0x00;

	g_frame[g_length++] = 0x64;
	g_frame[g_length++] = 0x01;

	g_frame[g_length++] = 0x00;
	g_frame[g_length++] = 0x00;

	LD2410_EndFrameSendAndACK();

	return g_frame;
}

/*
 * @func:		LD2410_CompareFrameAckDistanceLatencyWithBuffer
 *
 * @brief:		Ham so sanh Frame ACK (trong buffer) khi SET gia tri Distance va Latency thanh cong voi frame ACK mac dinh tuong ung
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
uint8_t LD2410_CompareFrameAckDistanceLatencyWithBuffer (void)
{
	uint8_t difference = 1; 	// Mac dinh 2 khung truyen giong nhau
	uint8_t *ackDistanceLatency = LD2410_FrameAckDistanceLatency();
	uint8_t *frameRxBuffer = USART_GetFrame();
	uint8_t length = USART_GetFrameLength();

	for (uint8_t i = 0; i < length; i++)
	{
		if (*(frameRxBuffer + LD2410_SKIP_HEADER + i) != *(ackDistanceLatency + LD2410_SKIP_HEADER + i))
		{
			difference = 0;
			return difference;
		}
	}

	// Neu 2 khung truyen giong nhau
	return difference;
}

/*
 * @func:		LD2410_CompareFrameAckSensitivityWithBuffer
 *
 * @brief:		Ham so sanh Frame ACK (trong buffer) khi SET gia tri Sensitivity thanh cong voi frame ACK mac dinh tuong ung
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
uint8_t LD2410_CompareFrameAckSensitivityWithBuffer (void)
{
	uint8_t difference = 1; 	// Mac dinh 2 khung truyen giong nhau
	uint8_t *ackSensitivity = LD2410_FrameAckSensitivity();
	uint8_t *frameRxBuffer = USART_GetFrame();
	uint8_t length = USART_GetFrameLength();

	for (uint8_t i = 0; i < length; i++)
	{
		if (*(frameRxBuffer + LD2410_SKIP_HEADER + i) != *(ackSensitivity + LD2410_SKIP_HEADER + i))
		{
			difference = 0;
			return difference;
		}
	}

	// Neu 2 khung truyen giong nhau
	return difference;
}

/*
 * @func:  		LD2410_SetValueSuccess
 *
 * @brief:
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LD2410_SetValueSuccess (void)
{
	uint8_t AckDistanceLatencySuccess = LD2410_CompareFrameAckDistanceLatencyWithBuffer();
	uint8_t AckSensitivitySuccess = LD2410_CompareFrameAckSensitivityWithBuffer();

	if (AckDistanceLatencySuccess == 1 || AckSensitivitySuccess == 1)
	{
		toggleLed(LED_1, PINK, 2, 300, 300);
		emberAfCorePrintln("Blink pink led");
	}
}

/* END FILE */
