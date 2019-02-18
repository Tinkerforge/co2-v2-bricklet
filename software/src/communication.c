/* co2-v2-bricklet
 * Copyright (C) 2019 Olaf Lüke <olaf@tinkerforge.com>
 *
 * communication.c: TFP protocol message handling
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

#include "communication.h"

#include "bricklib2/utility/communication_callback.h"
#include "bricklib2/protocols/tfp/tfp.h"

#include "scd30.h"

#define CALLBACK_VALUE_TYPE CALLBACK_VALUE_TYPE_INT16
#include "bricklib2/utility/callback_value.h"
CallbackValue_int16_t callback_value_temperature;

#undef CALLBACK_VALUE_TYPE
#define CALLBACK_VALUE_TYPE CALLBACK_VALUE_TYPE_UINT16
#include "bricklib2/utility/callback_value.h"
CallbackValue_uint16_t callback_value_humidity;
CallbackValue_uint16_t callback_value_co2_concentration;


BootloaderHandleMessageResponse handle_message(const void *message, void *response) {
	switch(tfp_get_fid_from_message(message)) {
		case FID_GET_ALL_VALUES: return get_all_values(message, response);
		case FID_SET_AIR_PRESSURE: return set_air_pressure(message);
		case FID_GET_AIR_PRESSURE: return get_air_pressure(message, response);
		case FID_SET_TEMPERATURE_OFFSET: return set_temperature_offset(message);
		case FID_GET_TEMPERATURE_OFFSET: return get_temperature_offset(message, response);
		case FID_SET_ALL_VALUES_CALLBACK_CONFIGURATION: return set_all_values_callback_configuration(message);
		case FID_GET_ALL_VALUES_CALLBACK_CONFIGURATION: return get_all_values_callback_configuration(message, response);
		case FID_GET_CO2_CONCENTRATION: return get_callback_value_uint16_t(message, response, &callback_value_co2_concentration);
		case FID_SET_CO2_CONCENTRATION_CALLBACK_CONFIGURATION: return set_callback_value_callback_configuration_uint16_t(message, &callback_value_co2_concentration);
		case FID_GET_CO2_CONCENTRATION_CALLBACK_CONFIGURATION: return get_callback_value_callback_configuration_uint16_t(message, response, &callback_value_co2_concentration);
		case FID_GET_TEMPERATURE: return get_callback_value_int16_t(message, response, &callback_value_temperature);
		case FID_SET_TEMPERATURE_CALLBACK_CONFIGURATION: return set_callback_value_callback_configuration_int16_t(message, &callback_value_temperature);
		case FID_GET_TEMPERATURE_CALLBACK_CONFIGURATION: return get_callback_value_callback_configuration_int16_t(message, response, &callback_value_temperature);
		case FID_GET_HUMIDITY: return get_callback_value_uint16_t(message, response, &callback_value_humidity);
		case FID_SET_HUMIDITY_CALLBACK_CONFIGURATION: return set_callback_value_callback_configuration_uint16_t(message, &callback_value_humidity);
		case FID_GET_HUMIDITY_CALLBACK_CONFIGURATION: return get_callback_value_callback_configuration_uint16_t(message, response, &callback_value_humidity);
		default: return HANDLE_MESSAGE_RESPONSE_NOT_SUPPORTED;
	}
}


BootloaderHandleMessageResponse get_all_values(const GetAllValues *data, GetAllValues_Response *response) {
	response->header.length     = sizeof(GetAllValues_Response);
	response->co2_concentration = scd30_get_co2_concentration();
	response->temperature       = scd30_get_temperature();
	response->humidity          = scd30_get_humidity();

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse set_air_pressure(const SetAirPressure *data) {
	if((data->air_pressure != 0) && ((data->air_pressure < 700) || (data->air_pressure > 1200))) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	scd30.air_pressure     = data->air_pressure;
	scd30.air_pressure_new = true;

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_air_pressure(const GetAirPressure *data, GetAirPressure_Response *response) {
	response->header.length = sizeof(GetAirPressure_Response);
	response->air_pressure  = scd30.air_pressure;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse set_temperature_offset(const SetTemperatureOffset *data) {
	scd30.temperature_offset      = data->offset;
	scd30.temperature_offset_new  = true;

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_temperature_offset(const GetTemperatureOffset *data, GetTemperatureOffset_Response *response) {
	response->header.length = sizeof(GetTemperatureOffset_Response);
	response->offset        = scd30.temperature_offset;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse set_all_values_callback_configuration(const SetAllValuesCallbackConfiguration *data) {
	scd30.all_values_callback_value_has_to_change = data->value_has_to_change;
	scd30.all_values_callback_period              = data->period;

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_all_values_callback_configuration(const GetAllValuesCallbackConfiguration *data, GetAllValuesCallbackConfiguration_Response *response) {
	response->header.length = sizeof(GetAllValuesCallbackConfiguration_Response);
	response->value_has_to_change = scd30.all_values_callback_value_has_to_change;
	response->period              = scd30.all_values_callback_period;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}


bool handle_all_values_callback(void) {
	static bool is_buffered = false;
	static AllValues_Callback cb;
	static uint16_t last_humidity = 0;
	static int16_t  last_temperature = 0;
	static uint16_t last_co2_concentration = 0;
	static uint32_t last_time = 0;

	if(!is_buffered) {
		if(scd30.all_values_callback_period == 0 ||
		    !system_timer_is_time_elapsed_ms(last_time, scd30.all_values_callback_period)) {
			return false;
		}
		tfp_make_default_header(&cb.header, bootloader_get_uid(), sizeof(AllValues_Callback), FID_CALLBACK_ALL_VALUES);
		cb.humidity          = scd30_get_humidity();
		cb.temperature       = scd30_get_temperature();
		cb.co2_concentration = scd30_get_co2_concentration();

		if(scd30.all_values_callback_value_has_to_change && (last_humidity == cb.humidity) && (last_temperature == cb.temperature) && (last_co2_concentration == cb.co2_concentration)) {
			return false;
		}

		last_humidity          = cb.humidity;
		last_temperature       = cb.temperature;
		last_co2_concentration = cb.co2_concentration;
		last_time              = system_timer_get_ms();
	}

	if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
		bootloader_spitfp_send_ack_and_message(&bootloader_status, (uint8_t*)&cb, sizeof(AllValues_Callback));
		is_buffered = false;
		return true;
	} else {
		is_buffered = true;
	}

	return false;
}

bool handle_co2_concentration_callback(void) {
	return handle_callback_value_callback_uint16_t(&callback_value_co2_concentration, FID_CALLBACK_CO2_CONCENTRATION);
}

bool handle_temperature_callback(void) {
	return handle_callback_value_callback_int16_t(&callback_value_temperature, FID_CALLBACK_TEMPERATURE);
}

bool handle_humidity_callback(void) {
	return handle_callback_value_callback_uint16_t(&callback_value_humidity, FID_CALLBACK_HUMIDITY);
}

void communication_tick(void) {
	communication_callback_tick();
}

void communication_init(void) {
	callback_value_init_int16_t(&callback_value_temperature, scd30_get_temperature);
	callback_value_init_uint16_t(&callback_value_humidity, scd30_get_humidity);
	callback_value_init_uint16_t(&callback_value_co2_concentration, scd30_get_co2_concentration);

	communication_callback_init();
}
