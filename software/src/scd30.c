/* co2-v2-bricklet
 * Copyright (C) 2019 Olaf Lüke <olaf@tinkerforge.com>
 *
 * scd30.c: Driver for SCD30 co2 sensor
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

#include "scd30.h"

#include "configs/config_scd30.h"

#include "bricklib2/os/coop_task.h"
#include "bricklib2/hal/i2c_fifo/i2c_fifo.h"

#include "bricklib2/logging/logging.h"

typedef enum XMC_I2C_CH_TDF {
	XMC_I2C_CH_TDF_MASTER_SEND         = 0,
	XMC_I2C_CH_TDF_SLAVE_SEND          = 1 << 8,
	XMC_I2C_CH_TDF_MASTER_RECEIVE_ACK  = 2 << 8,
	XMC_I2C_CH_TDF_MASTER_RECEIVE_NACK = 3 << 8,
	XMC_I2C_CH_TDF_MASTER_START        = 4 << 8,
	XMC_I2C_CH_TDF_MASTER_RESTART      = 5 << 8,
	XMC_I2C_CH_TDF_MASTER_STOP         = 6 << 8
} XMC_I2C_CH_TDF_t;

SCD30 scd30;
CoopTask scd30_task;

#define SCD30_POLYNOMIAL 0x31 // x^8 + x^5 + x^4 + 1 = 100110001

uint32_t scd30_read_data_task(uint8_t *data) {
	i2c_fifo_coop_write_register(&scd30.i2c_fifo, 0x0300, 0, NULL, true);

	if((scd30.i2c_fifo.state != I2C_FIFO_STATE_IDLE) && (!(scd30.i2c_fifo.state & I2C_FIFO_STATE_READY))) {
		scd30.i2c_fifo.state = I2C_FIFO_STATE_READ_DIRECT_ERROR;
		return scd30.i2c_fifo.state;
	}

	scd30.i2c_fifo.last_activity = system_timer_get_ms();

	scd30.i2c_fifo.state = I2C_FIFO_STATE_READ_DIRECT;
	uint8_t length = 10;

	// I2C Master start
	scd30.i2c_fifo.i2c->IN[0] = XMC_I2C_CH_TDF_MASTER_START | XMC_I2C_CH_CMD_READ | (scd30.i2c_fifo.address << 1);

	// I2C Master send ACK/NACK
	for(uint16_t i = 0; i < length; i++) {
		scd30.i2c_fifo.i2c->IN[0] = XMC_I2C_CH_TDF_MASTER_RECEIVE_ACK;
	}
	scd30.i2c_fifo.expected_fifo_level = length;
	while(true) {
		I2CFifoState state = i2c_fifo_next_state(&scd30.i2c_fifo);
		if(state & I2C_FIFO_STATE_ERROR) {
			loge("I2C FIFO COOP I2C error %d (state %d)\n\r", scd30.i2c_fifo.i2c_status, state);
			return scd30.i2c_fifo.i2c_status;
		}
		if(state != I2C_FIFO_STATE_READ_DIRECT_READY) {
			coop_task_yield();
			continue;
		}

		break;
	}

	uint8_t length_read = i2c_fifo_read_fifo(&scd30.i2c_fifo, data, length);
	if(length_read != length) {
		loge("I2C FIFO COOP unexpected I2C read length: %d vs %d\n\r", length_read, length);
		return XMC_I2C_CH_STATUS_FLAG_DATA_LOST_INDICATION;
	}

	scd30.i2c_fifo.state = I2C_FIFO_STATE_READ_DIRECT;
	length = 8;
	scd30.i2c_fifo.expected_fifo_level = length;

	// I2C Master send ACK/NACK
	for(uint16_t i = 0; i < length-1; i++) {
		scd30.i2c_fifo.i2c->IN[0] = XMC_I2C_CH_TDF_MASTER_RECEIVE_ACK;
	}
	scd30.i2c_fifo.i2c->IN[0] = XMC_I2C_CH_TDF_MASTER_RECEIVE_NACK;

	// I2C Master stop
	scd30.i2c_fifo.i2c->IN[0] = XMC_I2C_CH_TDF_MASTER_STOP;

	while(true) {
		I2CFifoState state = i2c_fifo_next_state(&scd30.i2c_fifo);
		if(state & I2C_FIFO_STATE_ERROR) {
			loge("I2C FIFO COOP I2C error %d (state %d)\n\r", scd30.i2c_fifo.i2c_status, state);
			return scd30.i2c_fifo.i2c_status;
		}
		if(state != I2C_FIFO_STATE_READ_DIRECT_READY) {
			coop_task_yield();
			continue;
		}

		break;
	}

	length_read = i2c_fifo_read_fifo(&scd30.i2c_fifo, &data[10], length);
	if(length_read != length) {
		loge("I2C FIFO COOP unexpected I2C read length: %d vs %d\n\r", length_read, length);
		return XMC_I2C_CH_STATUS_FLAG_DATA_LOST_INDICATION;
	}

	return 0;
}

uint8_t scd30_calculate_crc(uint8_t *data, uint8_t length) {
	uint8_t crc = 0xFF;

	for(uint8_t i = 0; i < length; i++) {
		crc ^= data[i];
		for(uint8_t bit = 0; bit < 8; bit++) {
			if(crc & 0x80) {
				crc = (crc << 1) ^ SCD30_POLYNOMIAL;
			} else {
				crc = (crc << 1);
			}
		}
	}

	return crc;
}

bool scd30_check_crc(uint8_t *data) {
	return data[2] == scd30_calculate_crc(data, 2);
}

void scd30_task_tick(void) {
	uint8_t data[20];

	coop_task_sleep_ms(2500);

#if LOGGING_LEVEL != LOGGING_DEBUG
	// Print SCD30 firmware version
	i2c_fifo_coop_write_register(&scd30.i2c_fifo, SCD30_REG_FIRMWARE_VERSION, 0, NULL, true);
	i2c_fifo_coop_read_direct(&scd30.i2c_fifo, 3, data, false);
	if(scd30_check_crc(data)) {
		logd("SCD30 Firmware: %x %x\n\r", data[0], data[1]);
	}
#endif

	// Read temperature offset
	i2c_fifo_coop_write_register(&scd30.i2c_fifo, SCD30_REG_TEMP_OFFSET, 0, NULL, true);
	i2c_fifo_coop_read_direct(&scd30.i2c_fifo, 3, data, false);
	if(scd30_check_crc(data)) {
		scd30.temperature_offset = (data[0] << 8) | data[1];
		logd("SCD30 Temperature Offset (startup): %d\n\r", scd30.temperature_offset);
	} else {
		logw("SCD30 SCD30_REG_TEMP_OFFSET CRC error: %x != %x\n\r", scd30_calculate_crc(data, 2), data[2]);
	}

	// Configure interval to 2 seconds
	data[0] = 0x00;
	data[1] = 0x02;
	data[2] = scd30_calculate_crc(data, 2);
	i2c_fifo_coop_write_register(&scd30.i2c_fifo, SCD30_REG_INTERVAL, 3, data, true);
	coop_task_sleep_ms(5);

	scd30.air_pressure_new = true;

	while(true) {
		if(scd30.air_pressure_new) {
			data[0] = scd30.air_pressure >> 8;
			data[1] = scd30.air_pressure & 0xFF;
			data[2] = scd30_calculate_crc(data, 2);

			// Set new air pressure and start continous measurement
			i2c_fifo_coop_write_register(&scd30.i2c_fifo, SCD30_REG_TRIGGER_CONT, 3, data, true);

			scd30.air_pressure_new = false;
		}

		if(scd30.temperature_offset_new) {
			data[0] = scd30.temperature_offset >> 8;
			data[1] = scd30.temperature_offset & 0xFF;
			data[2] = scd30_calculate_crc(data, 2);

			// Set new temperature offset
			i2c_fifo_coop_write_register(&scd30.i2c_fifo, SCD30_REG_TEMP_OFFSET, 3, data, true);

			scd30.temperature_offset_new = false;
		}

		coop_task_sleep_ms(500);
		i2c_fifo_coop_write_register(&scd30.i2c_fifo, SCD30_REG_DATA_READY, 0, NULL, true);
		i2c_fifo_coop_read_direct(&scd30.i2c_fifo, 3, data, false);
		if(scd30_check_crc(data)) {
			if(data[1] == 1) {
				scd30_read_data_task(data);
				if(scd30_check_crc(&data[0]) && scd30_check_crc(&data[3])) {
					uint32_t co2_uint = (data[0] << 24) | (data[1] << 16) | (data[3] << 8) | (data[4] << 0);
					float *co2 = (float*)&co2_uint;
					scd30.co2_concentration = (uint16_t)*co2;
				} else {
					logw("SCD30 CO2 CRC error: %x != %x or %x != %x\n\r", 
					     scd30_calculate_crc(&data[0], 2), data[2],
					     scd30_calculate_crc(&data[3], 2), data[5]);
				}

				if(scd30_check_crc(&data[6]) && scd30_check_crc(&data[9])) {
					uint32_t temperature_uint = (data[6] << 24) | (data[7] << 16) | (data[9] << 8) | (data[10] << 0);
					float *temperature = (float*)&temperature_uint;
					*temperature = *temperature*100;
					scd30.temperature = (int16_t)*temperature;
				} else {
					logw("SCD30 Temperature CRC error: %x != %x or %x != %x\n\r", 
					     scd30_calculate_crc(&data[6], 2), data[8],
					     scd30_calculate_crc(&data[9], 2), data[11]);
				}

				if(scd30_check_crc(&data[12]) && scd30_check_crc(&data[15])) {
					uint32_t humidity_uint = (data[12] << 24) | (data[13] << 16) | (data[15] << 8) | (data[16] << 0);
					float *humidity = (float*)&humidity_uint;
					*humidity = *humidity*100;
					scd30.humidity = (uint16_t)*humidity;
				} else {
					logw("SCD30 Temperature CRC error: %x != %x or %x != %x\n\r", 
					     scd30_calculate_crc(&data[12], 2), data[14],
					     scd30_calculate_crc(&data[15], 2), data[17]);
				}
			}
		} else {
			logw("SCD30 SCD30_REG_DATA_READY CRC error: %x != %x\n\r", scd30_calculate_crc(data, 2), data[2]);
		}
	}
}

void scd30_tick(void) {
	coop_task_tick(&scd30_task);
}

void scd30_init_i2c(void) {
	scd30.i2c_fifo.baudrate         = SCD30_I2C_BAUDRATE;
	scd30.i2c_fifo.address          = SCD30_I2C_ADDRESS;
	scd30.i2c_fifo.i2c              = SCD30_I2C;

	scd30.i2c_fifo.scl_port         = SCD30_SCL_PORT;
	scd30.i2c_fifo.scl_pin          = SCD30_SCL_PIN;
	scd30.i2c_fifo.scl_mode         = SCD30_SCL_PIN_MODE;
	scd30.i2c_fifo.scl_input        = SCD30_SCL_INPUT;
	scd30.i2c_fifo.scl_source       = SCD30_SCL_SOURCE;
	scd30.i2c_fifo.scl_fifo_size    = SCD30_SCL_FIFO_SIZE;
	scd30.i2c_fifo.scl_fifo_pointer = SCD30_SCL_FIFO_POINTER;

	scd30.i2c_fifo.sda_port         = SCD30_SDA_PORT;
	scd30.i2c_fifo.sda_pin          = SCD30_SDA_PIN;
	scd30.i2c_fifo.sda_mode         = SCD30_SDA_PIN_MODE;
	scd30.i2c_fifo.sda_input        = SCD30_SDA_INPUT;
	scd30.i2c_fifo.sda_source       = SCD30_SDA_SOURCE;
	scd30.i2c_fifo.sda_fifo_size    = SCD30_SDA_FIFO_SIZE;
	scd30.i2c_fifo.sda_fifo_pointer = SCD30_SDA_FIFO_POINTER;

	i2c_fifo_init(&scd30.i2c_fifo);
}

void scd30_init(void) {
	memset(&scd30, 0, sizeof(SCD30));
	scd30_init_i2c();

	coop_task_init(&scd30_task, scd30_task_tick);
}

uint16_t scd30_get_humidity(void) {
	return scd30.humidity;
}

int16_t scd30_get_temperature(void) {
	return scd30.temperature;
}

uint16_t scd30_get_co2_concentration(void) {
	return scd30.co2_concentration;
}