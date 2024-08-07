/*
 * BME280.c
 *
 *  Created on: Nov 26, 2023
 *      Author: hollerller
 */

#include <stdint.h>
#include <stdio.h>
#include "stm32f4xx_hal.h"

#include "API_delay.h"
#include "driver_BME280.h"
#include "API_i2c.h"
#include "API_uart.h"

#define BME280_ADDRESS 0xEC	// Device Address
#define TIMEOUT 1000		// Timeout
#define BME_HAL_DELAY 100	// Delay used between initialization settings
#define MEMADDRESSSIZE	1	// Memory address Size -> Required in HAL_I2C_Mem_Read/Write Functions
#define CALIBMEMADD1 0x88	// First memory address of the first portion of calibration data
#define CALIBMEMADD2 0xE1	// First memory address of the second portion of calibration data
#define CALIBDATASIZE1 25	// Size of first portion of compensation data
#define CALIBDATASIZE2 7	// Size of second portion of compensation data
#define CMDWRITESIZE 1		// Size of the control parameters
#define RAWDATAREG1 0XF7	// First memory address of the first portion of raw data
#define RAWDATASIZE 8		// Size of the raw data to be read
#define CHIPIDREG 0xD0		// Memory address of the chip ID

// Initial settings registers

#define RESET_REG 0xE0
#define CTRL_HUM 0xF2
#define STATUS 0xF3
#define CTRL_MEAS 0xF4
#define CONFIG_REG 0xF5

// Variables to save the compensation values for later calculations

static uint16_t dig_T1, dig_P1, dig_H1, dig_H3;
static int16_t dig_T2, dig_T3, dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8,
		dig_P9, dig_H2, dig_H4, dig_H5, dig_H6;

// stores the raw data read by the sensor

static int32_t tADC, hADC;

typedef int32_t BME280_S32_t;
typedef uint32_t BME280_U32_t;

// stores the actual values for temperature and humidity

static float temp, hum;

/*
 * Reads the calibration data from the sensor
 *
 * This function uses I2C memory reads to get the calibration data from the sensor memory.
 * Once the uncompensated values for temperature and humidity are read,
 * this calibration parameters are used to calculate the actual values for T and H.
 * 		- Refer to datasheet page 24 >> 4.2.2 Trimming parameter readout
 *
 */

static void trimmingParametersRead(void) {

	// Store calibration data from sensor memory

	uint8_t calibData1[26];
	uint8_t calibData2[7];

	// Read the first portion of calibration data from memory address 0x88

	i2c_Mem_Read(BME280_ADDRESS, CALIBMEMADD1, MEMADDRESSSIZE, calibData1,
			CALIBDATASIZE1);

	// Read the second portion of calibration data from memory address 0x88

	i2c_Mem_Read(BME280_ADDRESS, CALIBMEMADD2, MEMADDRESSSIZE, calibData2,
			CALIBDATASIZE2);

	// Calculate the compensation words for later evaluations

	dig_T1 = (calibData1[1] << 8) | calibData1[0];
	dig_T2 = (calibData1[3] << 8) | calibData1[2];
	dig_T3 = (calibData1[5] << 8) | calibData1[4];
	dig_P1 = (calibData1[7] << 8) | calibData1[6];
	dig_P2 = (calibData1[9] << 8) | calibData1[8];
	dig_P3 = (calibData1[11] << 8) | calibData1[10];
	dig_P4 = (calibData1[13] << 8) | calibData1[12];
	dig_P5 = (calibData1[15] << 8) | calibData1[14];
	dig_P6 = (calibData1[17] << 8) | calibData1[16];
	dig_P7 = (calibData1[19] << 8) | calibData1[18];
	dig_P8 = (calibData1[21] << 8) | calibData1[20];
	dig_P9 = (calibData1[23] << 8) | calibData1[22];
	dig_H1 = calibData1[24];
	dig_H2 = (calibData2[1] << 8) | calibData2[0];
	dig_H3 = calibData2[2];
	dig_H4 = (calibData2[3] << 4) | (calibData2[4] & 0x0F);
	dig_H5 = (calibData2[4] << 4) | (calibData2[5] >> 4);
	dig_H6 = calibData2[6];

}

/* Initializes the BME280 sensor
 *
 * This function initializes the BME280 sensor
 * Uses the recommended mode of operation suggested for indoor navigation - Datasheet page 20 -> 3.5.3
 * 		- Refer to datasheet page 26 >> 5. Global memory map and register description
 *
 */

void BME280_init(void) {

	// Read and set the calibration parameters required for sensor compensation

	trimmingParametersRead();

	// Value required to reset the device
	uint8_t resetSeq = 0xB6;

	// Set the humidity data acquisition option (oversampling x 1)
	uint8_t ctrlHum = 0x01;

	/* Set the pressure and temperature data acquisition options
	 Bit 7, 6, 5 = temperature (oversampling x 16)
	 Bit 4, 3, 2 = pressure (skipped)
	 Bit 1, 0 = mode (normal mode)
	 */
	uint8_t ctrlMeas = 0b10100011;

	/* Set the rate, filter and interface options on the device
	 Bit 7, 6, 5 = controls inactive duration
	 in normal mode (tstandby 0.5 ms)
	 Bit 4, 3, 2 = IIR filter (filter coeficient 16)
	 Bit 0 = Enables 3-wire SPI (skipped)
	 */
	uint8_t config = 0b00010000;

	// Performs a soft reset of the device

	i2c_Mem_Write(BME280_ADDRESS, RESET_REG, MEMADDRESSSIZE, &resetSeq,
			CMDWRITESIZE);

	HAL_Delay(BME_HAL_DELAY);

	// Configure control humidity register

	i2c_Mem_Write(BME280_ADDRESS, CTRL_HUM, MEMADDRESSSIZE, &ctrlHum,
			CMDWRITESIZE);

	HAL_Delay(BME_HAL_DELAY);

	// Configure temperature and operation mode of the sensor

	i2c_Mem_Write(BME280_ADDRESS, CTRL_MEAS, MEMADDRESSSIZE, &ctrlMeas,
			CMDWRITESIZE);

	HAL_Delay(BME_HAL_DELAY);

	// Set the configuration registers

	i2c_Mem_Write(BME280_ADDRESS, CONFIG_REG, MEMADDRESSSIZE, &config,
			CMDWRITESIZE);

	HAL_Delay(BME_HAL_DELAY);

}

/*
 * Reads raw sensor data from the device
 *
 * This function reads the raw data from the sensor
 * Used to calculate the actual values for temperature and humidity
 * 		- Refer to datasheet page 30 >> 5.4.8 Register 0xFA...0xFC “temp” (_msb, _lsb, _xlsb)
 * 									    5.4.9 Register 0xFD...0xFE “hum” (_msb, _lsb)
 *
 */


static uint8_t BME280_read(void) {

	// Array to store the raw sensor data
	uint8_t sensorData[8];
	// Variable to save the chipID read in the sensor
	uint8_t chipID;

	// Read the chip id
	i2c_Mem_Read(BME280_ADDRESS, CHIPIDREG, MEMADDRESSSIZE, &chipID,
			MEMADDRESSSIZE);

	if (chipID == 0x60) {// If the chip ID is 0x60, the device is read and perform the raw data reading

		i2c_Mem_Read(BME280_ADDRESS, RAWDATAREG1, MEMADDRESSSIZE, sensorData,
				RAWDATASIZE);

		tADC = (sensorData[3] << 12) | (sensorData[4] << 4)
				| (sensorData[5] >> 4);
		hADC = (sensorData[6] << 8) | sensorData[7];

		return 0;
	}

	else
		return 1;

}

/*
 * Code provided to calculate the actual values using the trimming parameters
 * and sensor read values for Temperature and Humidity
 * 		- Refer to datasheet page 25 -> 4.2.3 Compensation formulas
 *
 */

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// t_fine carries fine temperature as global value
static BME280_S32_t t_fine;

static BME280_S32_t BME280_compensate_T_int32(BME280_S32_t adc_T) {
	BME280_S32_t var1, var2, T;
	var1 = ((((adc_T >> 3) - ((BME280_S32_t) dig_T1 << 1)))
			* ((BME280_S32_t) dig_T2)) >> 11;
	var2 = (((((adc_T >> 4) - ((BME280_S32_t) dig_T1))
			* ((adc_T >> 4) - ((BME280_S32_t) dig_T1))) >> 12)
			* ((BME280_S32_t) dig_T3)) >> 14;
	t_fine = var1 + var2;
	T = (t_fine * 5 + 128) >> 8;
	return T;
}

// Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format (22 integer and 10 fractional bits).
// Output value of “47445” represents 47445/1024 = 46.333 %RH

static BME280_U32_t bme280_compensate_H_int32(BME280_S32_t adc_H) {
	BME280_S32_t v_x1_u32r;
	v_x1_u32r = (t_fine - ((BME280_S32_t) 76800));
	v_x1_u32r =
			(((((adc_H << 14) - (((BME280_S32_t) dig_H4) << 20)
					- (((BME280_S32_t) dig_H5) * v_x1_u32r))
					+ ((BME280_S32_t) 16384)) >> 15)
					* (((((((v_x1_u32r * ((BME280_S32_t) dig_H6)) >> 10)
							* (((v_x1_u32r * ((BME280_S32_t) dig_H3)) >> 11)
									+ ((BME280_S32_t) 32768))) >> 10)
							+ ((BME280_S32_t) 2097152))
							* ((BME280_S32_t) dig_H2) + 8192) >> 14));
	v_x1_u32r = (v_x1_u32r
			- (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7)
					* ((BME280_S32_t) dig_H1)) >> 4));
	v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
	v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
	return (BME280_U32_t) (v_x1_u32r >> 12);
}

/*
 * Reads the sensor and use the compensation formulas to calculate the
 * actual temperature and humidity
 *
 */

void BME280_calculate(void) {

	if (BME280_read() == 0) {// Calls BME280 function if return 0 (device was read)

		temp = BME280_compensate_T_int32(tADC) / 100.0;
		hum = bme280_compensate_H_int32(hADC) / 1024.0;

	}

	else {	// send error message when device is disconnected
		temp = 0;
		hum = 0;
		uint8_t errorMessage[] = "Device not ready. Check device connection\r\n";

		uartSendString(errorMessage);
	}

}

/*
 * Function to get the current temperature
 *
 */

float BME280_getTemp() {
	return temp;
}

/*
 * Function to get the current humidity
 *
 */

float BME280_getHum() {
	return hum;
}

/*
 * Uses uart API to send current values
 *
 */

void BME280_uart() {

	char dataStr[255] = "";

	sprintf(dataStr, "Temperature: %.2f Humidity: %.2f \r\n", temp, hum);

	uartSendString((uint8_t*) dataStr);

	HAL_Delay(500);
}

