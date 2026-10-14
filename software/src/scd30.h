/* co2-v2-bricklet
 * Copyright (C) 2019 Olaf Lüke <olaf@tinkerforge.com>
 *
 * scd30.h: Driver for SCD30 co2 sensor
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

#ifndef SCD30_H
#define SCD30_H

#include <stdint.h>

#include "bricklib2/hal/i2c_fifo/i2c_fifo.h"

typedef struct {
	I2CFifo i2c_fifo;

	uint16_t humidity;
	int16_t temperature;
	uint16_t co2_concentration;

	bool temperature_offset_new;
	uint16_t temperature_offset;
	bool air_pressure_new;
	uint16_t air_pressure;

	bool asc_enabled;
	bool asc_new;

	bool frc_new;
	uint16_t frc_co2_concentration;
	uint16_t frc_co2_concentration_sensor;

	uint32_t measurement_start_time;

	bool all_values_callback_value_has_to_change;
	uint32_t all_values_callback_period;
} SCD30;

extern SCD30 scd30;

void scd30_tick(void);
void scd30_init(void);
void scd30_write_config(void);

uint16_t scd30_get_humidity(void);
int16_t scd30_get_temperature(void);
uint16_t scd30_get_co2_concentration(void);

#define SCD30_CONFIG_PAGE          1
#define SCD30_CONFIG_MAGIC         0x12345678
#define SCD30_CONFIG_MAGIC_POS     0
#define SCD30_CONFIG_ASC_POS       1

// The SCD30 has to measure continuously for at least 2 minutes before FRC is applied
#define SCD30_FRC_MIN_MEASUREMENT_TIME (2*60*1000)

// Reference value reported by the SCD30 if no FRC was applied since power-up
#define SCD30_FRC_DEFAULT_CO2_CONCENTRATION 400

#define SCD30_REG_TRIGGER_CONT     0x0010
#define SCD30_REG_STOP_CONT        0x0104
#define SCD30_REG_INTERVAL         0x4600
#define SCD30_REG_DATA_READY       0x0202
#define SCD30_REG_READ_MEASUREMENT 0x0300
#define SCD30_REG_ASC              0x5306
#define SCD30_REG_FRC              0x5204
#define SCD30_REG_TEMP_OFFSET      0x5403
#define SCD30_REG_ALTITUDE_COMP    0x5102
#define SCD30_REG_FIRMWARE_VERSION 0xD100
#define SCD30_REG_SOFT_RESET       0xD304

#endif