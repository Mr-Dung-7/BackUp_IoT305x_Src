/*
 * TemHumSensor.c
 *
 *  Created on: Jul 24, 2024
 *      Author: Mr.hDung
 */

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include "Source/Mid/Si7020/TemHumSensor.h"

/******************************************************************************/
/*                                 DEFINEs                                    */
/******************************************************************************/


/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/


/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/
EmberEventControl I2CSendDataEventControl;

/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/


/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/
/*
 * @func:		Si7020_Init
 *
 * @brief:		Initializes temperature / humidity sensor
 *
 * @params:		None
 *
 * @retVal:		True for successes task, false for fails
 *
 * @note:		None
 */
boolean Si7020_Init (void)
{
	I2C_TransferSeq_TypeDef seq;
	I2C_TransferReturn_TypeDef ret;
	uint8_t i2c_read_data[3];
	uint8_t i2c_write_data[2];

	i2cInit();

	seq.addr  = SI7020_ADDR;
	seq.flags = I2C_FLAG_WRITE_READ;

	// Select command to issue
	i2c_write_data[0] = SI7020_READ_ID_1;
	i2c_write_data[1] = SI7020_READ_ID_2;
	seq.buf[0].data   = i2c_write_data;
	seq.buf[0].len    = 2;

	// Select location/length of data to be read
	seq.buf[1].data = i2c_read_data;
	seq.buf[1].len  = 3;

	ret = I2C_TransferInit(I2C0, &seq);

	// Sending data
	while (ret == i2cTransferInProgress)
	{
		ret = I2C_Transfer(I2C0);
	}
	emberAfCorePrintln (" Detect Si7020--ret:%d",ret);

	if ((ret != i2cTransferDone) | (i2c_read_data[0] != SI7020_DEVICE_ID))
	{

		emberAfCorePrintln (" Detect Si7020 failure!--Device Id:%d ",i2c_read_data[0]);
		return -1;
	}

	emberAfCorePrintln (" Detect Si7020 success! ");

	emberEventControlSetActive(I2CSendDataEventControl);

	return ret;
}

/*
 * @func:		i2cInit
 *
 * @brief:		Initializes i2c communication
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void i2cInit (void)
{
  // Using default settings
  I2C_Init_TypeDef i2cInit = I2C_INIT_DEFAULT;

  // Enable I2C Clock
  CMU_ClockEnable(cmuClock_I2C0, true);

  // Using PB1 (SDA) and PB0 (SCL)
  GPIO_PinModeSet(I2C0_SCL_PORT, I2C0_SCL_PIN, gpioModeWiredAndPullUpFilter, 1);
  GPIO_PinModeSet(I2C0_SDA_PORT, I2C0_SDA_PIN, gpioModeWiredAndPullUpFilter, 1);

  // Route GPIO pins to I2C module
  GPIO->I2CROUTE[0].ROUTEEN = GPIO_I2C_ROUTEEN_SDAPEN | GPIO_I2C_ROUTEEN_SCLPEN;

  GPIO->I2CROUTE[0].SDAROUTE = (GPIO->I2CROUTE[0].SDAROUTE & ~_GPIO_I2C_SDAROUTE_MASK)
                        | (I2C0_SDA_PORT << _GPIO_I2C_SDAROUTE_PORT_SHIFT
                        | (I2C0_SDA_PIN << _GPIO_I2C_SDAROUTE_PIN_SHIFT));
  GPIO->I2CROUTE[0].SCLROUTE = (GPIO->I2CROUTE[0].SCLROUTE & ~_GPIO_I2C_SCLROUTE_MASK)
                        | (I2C0_SCL_PORT << _GPIO_I2C_SCLROUTE_PORT_SHIFT
                        | (I2C0_SCL_PIN << _GPIO_I2C_SCLROUTE_PIN_SHIFT));

  I2C_Init(I2C0, &i2cInit);
}

/*
 * @func:		Si7020_Measure
 *
 * @brief:		Send command and receive data from Si7020
 *
 * @params[1]:	buffer - pointer to Rxbuffer
 * @params[2]:	command - command code
 *
 * @retVal:		True for successes task, false for fails
 *
 * @note:		None
 */
boolean Si7020_Measure (uint32_t *buffer, uint8_t command)
{
	I2C_TransferSeq_TypeDef    seq;
	I2C_TransferReturn_TypeDef retVal;
	uint8_t i2c_read_data[2];
	uint8_t i2c_write_data[1];

	seq.addr  = SI7020_ADDR;
	seq.flags = I2C_FLAG_WRITE_READ;

	// Select command to issue
	i2c_write_data[0] = command;
	seq.buf[0].data   = i2c_write_data;
	seq.buf[0].len    = 1;

	// Select location/length of data to be read
	seq.buf[1].data = i2c_read_data;
	seq.buf[1].len  = 2;

	retVal = I2C_TransferInit(I2C0, &seq);
	uint32_t timeout = 10000;

	// Sending data
	while (retVal == i2cTransferInProgress && (--timeout) != 0)
	{
		retVal = I2C_Transfer(I2C0);
	}

	if(timeout == 0) return false;

	*buffer = ((uint32_t) i2c_read_data[0] << 8) + (i2c_read_data[1] & 0xfc);

	return true;
}

/*
 * @func:		Si7020_MeasureTempAndHumi
 *
 * @brief:		Measures temperature and humidity
 *
 * @params[1]:	tempData - Pointer to temp value
 * @params[2]:	humiData - Pointer to humi value
 *
 * @retVal:		True if both value are received, false otherwise
 *
 * @note:		None
 */
boolean Si7020_MeasureTempAndHumi (uint32_t *tempData, uint32_t *humiData)
{
	// Get temperature value
	boolean retVal = Si7020_Measure (tempData, SI7020_READ_TEMP);

	if (retVal)
	{
		*tempData = ((((*tempData) * 21965) >> 13) - 46850 )/1000;
	}
	else
	{
		return false;
	}

	// Get humidity value
	retVal = Si7020_Measure (humiData, SI7020_READ_RH);

	if (retVal)
	{
		*humiData = ((((*humiData) * 15625L) >> 13) - 6000 )/1000;
	}
	else
	{
		return false;
	}

	return true;
}

/*
 * @func:		Si7020_UpdateValueTempHumi
 *
 * @brief:		The function to update temperature and humidity values
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void Si7020_UpdateValueTempHumi (void)
{
	static float tempCurEst = 0, humiCurEst = 0;
	static uint8_t countTemp = 0, countHumi = 0;
	static uint32_t prevTemp = 0, prevHumi = 0;
	uint32_t currentTemp, currentHumi;

	countTemp++;
	countHumi++;

	// Get value from sensor Si7020
	Si7020_MeasureTempAndHumi(&currentTemp, &currentHumi);

	// Filter noise from the measured value
	currentTemp = (uint32_t)KalmanFilter(&tempCurEst,
										(float)currentTemp,
										 MEASURE_NOISE_INIT,
										 PROCESS_NOISE_INIT);

	currentHumi = (uint32_t)KalmanFilter(&humiCurEst,
										(float)currentHumi,
										 MEASURE_NOISE_INIT,
										 PROCESS_NOISE_INIT);


	/* Update temperature value */
	// Update temperature value to ZC every 5 second
	if (countTemp >= PERIOD_UPDATE_TO_HC / 1000)
	{
		SEND_TempValueReport(ENDPOINT_1, currentTemp);
//		emberAfCorePrintln("Temp: %"PRIu32" oC", currentTemp);

		countTemp = 0;
	}

	// Update temperature value immediately when a great change occurs
	if (((currentTemp > prevTemp) && (currentTemp - prevTemp >= CHANGE_VALUE_TEMP)) ||
		((currentTemp < prevTemp) && (prevTemp - currentTemp >= CHANGE_VALUE_TEMP)))
	{
		SEND_TempValueReport(ENDPOINT_1, currentTemp);
//		emberAfCorePrintln("Temp value changes greatly: %"PRIu32" oC", g_temp);
	}

	/* Update humidity value */
	// Update humidity value to ZC every 5 second
	if (countHumi >= PERIOD_UPDATE_TO_HC / 1000)
	{
		SEND_HumiValueReport(ENDPOINT_1, currentHumi);
//		emberAfCorePrintln("Humi: %"PRIu32" %%", g_humi);

		countHumi = 0;
	}

	// Update humidity value immediately when a great change occurs
	if (((currentHumi > prevHumi) && (currentHumi - prevHumi >= CHANGE_VALUE_HUMI)) ||
		((currentHumi < prevHumi) && (prevHumi - currentHumi >= CHANGE_VALUE_HUMI)))
	{
		SEND_HumiValueReport(ENDPOINT_1, currentHumi);
//		emberAfCorePrintln("Humi value changes greatly: %"PRIu32" %%", g_humi);
	}

	prevTemp = currentTemp;
	prevHumi = currentHumi;
}

/*
 * @func:		I2CSendDataEventHandler
 *
 * @brief:		Poilling read Humidity and Temperature value
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void I2CSendDataEventHandler (void)
{
	emberEventControlSetInactive(I2CSendDataEventControl);

	Si7020_UpdateValueTempHumi();

	emberEventControlSetDelayMS(I2CSendDataEventControl, 1000);
}

/* END_FILE */
