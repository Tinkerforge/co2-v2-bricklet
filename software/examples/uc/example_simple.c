// This example is not self-contained.
// It requires usage of the example driver specific to your platform.
// See the HAL documentation.

#include "src/bindings/hal_common.h"
#include "src/bindings/bricklet_co2_v2.h"

void check(int rc, const char *msg);
void example_setup(TF_HAL *hal);
void example_loop(TF_HAL *hal);

static TF_CO2V2 co2;

void example_setup(TF_HAL *hal) {
	// Create device object
	check(tf_co2_v2_create(&co2, NULL, hal), "create device object");

	// Get current all values
	uint16_t co2_concentration, humidity; int16_t temperature;
	check(tf_co2_v2_get_all_values(&co2, &co2_concentration, &temperature,
	                               &humidity), "get all values");

	tf_hal_printf("CO2 Concentration: %I16u ppm\n", co2_concentration);
	tf_hal_printf("Temperature: %d 1/%d °C\n", temperature, 100);
	tf_hal_printf("Humidity: %d 1/%d %%RH\n", humidity, 100);
}

void example_loop(TF_HAL *hal) {
	// Poll for callbacks
	tf_hal_callback_tick(hal, 0);
}
