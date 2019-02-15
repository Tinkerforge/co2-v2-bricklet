/* co2-v2-bricklet
 * Copyright (C) 2019 Olaf Lüke <olaf@tinkerforge.com>
 *
 * communication.h: TFP protocol message handling
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <stdint.h>
#include <stdbool.h>

#include "bricklib2/protocols/tfp/tfp.h"
#include "bricklib2/bootloader/bootloader.h"

// Default functions
BootloaderHandleMessageResponse handle_message(const void *data, void *response);
void communication_tick(void);
void communication_init(void);

// Constants
#define CO2_V2_THRESHOLD_OPTION_OFF 'x'
#define CO2_V2_THRESHOLD_OPTION_OUTSIDE 'o'
#define CO2_V2_THRESHOLD_OPTION_INSIDE 'i'
#define CO2_V2_THRESHOLD_OPTION_SMALLER '<'
#define CO2_V2_THRESHOLD_OPTION_GREATER '>'

#define CO2_V2_BOOTLOADER_MODE_BOOTLOADER 0
#define CO2_V2_BOOTLOADER_MODE_FIRMWARE 1
#define CO2_V2_BOOTLOADER_MODE_BOOTLOADER_WAIT_FOR_REBOOT 2
#define CO2_V2_BOOTLOADER_MODE_FIRMWARE_WAIT_FOR_REBOOT 3
#define CO2_V2_BOOTLOADER_MODE_FIRMWARE_WAIT_FOR_ERASE_AND_REBOOT 4

#define CO2_V2_BOOTLOADER_STATUS_OK 0
#define CO2_V2_BOOTLOADER_STATUS_INVALID_MODE 1
#define CO2_V2_BOOTLOADER_STATUS_NO_CHANGE 2
#define CO2_V2_BOOTLOADER_STATUS_ENTRY_FUNCTION_NOT_PRESENT 3
#define CO2_V2_BOOTLOADER_STATUS_DEVICE_IDENTIFIER_INCORRECT 4
#define CO2_V2_BOOTLOADER_STATUS_CRC_MISMATCH 5

#define CO2_V2_STATUS_LED_CONFIG_OFF 0
#define CO2_V2_STATUS_LED_CONFIG_ON 1
#define CO2_V2_STATUS_LED_CONFIG_SHOW_HEARTBEAT 2
#define CO2_V2_STATUS_LED_CONFIG_SHOW_STATUS 3

// Function and callback IDs and structs
#define FID_GET_ALL_VALUES 1
#define FID_SET_AIR_PRESSURE 2
#define FID_GET_AIR_PRESSURE 3
#define FID_SET_TEMPERATURE_OFFSET 4
#define FID_GET_TEMPERATURE_OFFSET 5
#define FID_SET_ALL_VALUES_CALLBACK_CONFIGURATION 6
#define FID_GET_ALL_VALUES_CALLBACK_CONFIGURATION 7
#define FID_GET_CO2_CONCENTRATION 9
#define FID_SET_CO2_CONCENTRATION_CALLBACK_CONFIGURATION 10
#define FID_GET_CO2_CONCENTRATION_CALLBACK_CONFIGURATION 11
#define FID_GET_TEMPERATURE 13
#define FID_SET_TEMPERATURE_CALLBACK_CONFIGURATION 14
#define FID_GET_TEMPERATURE_CALLBACK_CONFIGURATION 15
#define FID_GET_HUMIDITY 17
#define FID_SET_HUMIDITY_CALLBACK_CONFIGURATION 18
#define FID_GET_HUMIDITY_CALLBACK_CONFIGURATION 19

#define FID_CALLBACK_ALL_VALUES 8
#define FID_CALLBACK_CO2_CONCENTRATION 12
#define FID_CALLBACK_TEMPERATURE 16
#define FID_CALLBACK_HUMIDITY 20

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) GetAllValues;

typedef struct {
	TFPMessageHeader header;
	uint16_t co2_concentration;
	int16_t temperature;
	uint16_t humidity;
} __attribute__((__packed__)) GetAllValues_Response;

typedef struct {
	TFPMessageHeader header;
	uint16_t air_pressure;
} __attribute__((__packed__)) SetAirPressure;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) GetAirPressure;

typedef struct {
	TFPMessageHeader header;
	uint16_t air_pressure;
} __attribute__((__packed__)) GetAirPressure_Response;

typedef struct {
	TFPMessageHeader header;
	uint16_t offset;
} __attribute__((__packed__)) SetTemperatureOffset;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) GetTemperatureOffset;

typedef struct {
	TFPMessageHeader header;
	uint16_t offset;
} __attribute__((__packed__)) GetTemperatureOffset_Response;

typedef struct {
	TFPMessageHeader header;
	uint32_t period;
	bool value_has_to_change;
} __attribute__((__packed__)) SetAllValuesCallbackConfiguration;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) GetAllValuesCallbackConfiguration;

typedef struct {
	TFPMessageHeader header;
	uint32_t period;
	bool value_has_to_change;
} __attribute__((__packed__)) GetAllValuesCallbackConfiguration_Response;

typedef struct {
	TFPMessageHeader header;
	uint16_t co2_concentration;
	int16_t temperature;
	uint16_t humidity;
} __attribute__((__packed__)) AllValues_Callback;


// Function prototypes
BootloaderHandleMessageResponse get_all_values(const GetAllValues *data, GetAllValues_Response *response);
BootloaderHandleMessageResponse set_air_pressure(const SetAirPressure *data);
BootloaderHandleMessageResponse get_air_pressure(const GetAirPressure *data, GetAirPressure_Response *response);
BootloaderHandleMessageResponse set_temperature_offset(const SetTemperatureOffset *data);
BootloaderHandleMessageResponse get_temperature_offset(const GetTemperatureOffset *data, GetTemperatureOffset_Response *response);
BootloaderHandleMessageResponse set_all_values_callback_configuration(const SetAllValuesCallbackConfiguration *data);
BootloaderHandleMessageResponse get_all_values_callback_configuration(const GetAllValuesCallbackConfiguration *data, GetAllValuesCallbackConfiguration_Response *response);

// Callbacks
bool handle_all_values_callback(void);
bool handle_co2_concentration_callback(void);
bool handle_temperature_callback(void);
bool handle_humidity_callback(void);

#define COMMUNICATION_CALLBACK_TICK_WAIT_MS 1
#define COMMUNICATION_CALLBACK_HANDLER_NUM 4
#define COMMUNICATION_CALLBACK_LIST_INIT \
	handle_all_values_callback, \
	handle_co2_concentration_callback, \
	handle_temperature_callback, \
	handle_humidity_callback, \


#endif
