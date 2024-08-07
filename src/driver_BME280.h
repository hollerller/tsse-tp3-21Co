/*
 * BME280.h
 *
 *  Created on: Nov 26, 2023
 *      Author: hollerller
 */

#ifndef API_INC_DRIVER_BME280_H_
#define API_INC_DRIVER_BME280_H_


/* Initializes the BME280 sensor
 *
 * This function initializes the BME280 sensor
 * Uses the recommended mode of operation suggested for indoor navigation - Datasheet page 20 -> 3.5.3
 * 		- Refer to datasheet page 26 >> 5. Global memory map and register description
 *
 */
void BME280_init(void);


/*
 * Reads the sensor and use the compensation formulas to calculate the
 * actual temperature and humidity
 *
 */
void BME280_calculate(void);


/*
 * Functions created to encapsulate values of temperature and humidity
 */

float BME280_getTemp();
float BME280_getHum();


// Function declaration to send current values over UART

void BME280_uart();


#endif /* API_INC_DRIVER_BME280_H_ */
