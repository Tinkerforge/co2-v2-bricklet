// This example is not self-contained.
// It requires usage of the example driver specific to your platform.
// See the HAL documentation.

#include "src/bindings/hal_common.h"
#include "src/bindings/bricklet_co2_v2.h"

void check(int rc, const char *msg);
void example_setup(TF_HAL *hal);
void example_loop(TF_HAL *hal);

// Callback function for all values callback
static void all_values_handler(TF_CO2V2 *device, uint16_t co2_concentration,
                               int16_t temperature, uint16_t humidity, void *user_data) {
	(void)device; (void)user_data; // avoid unused parameter warning

	tf_hal_printf("CO2 Concentration: %I16u ppm\n", co2_concentration);
	tf_hal_printf("Temperature: %d 1/%d °C\n", temperature, 100);
	tf_hal_printf("Humidity: %d 1/%d %%RH\n", humidity, 100);
	tf_hal_printf("\n");
}

static TF_CO2V2 co2;

void example_setup(TF_HAL *hal) {
	// Create device object
	check(tf_co2_v2_create(&co2, NULL, hal), "create device object");

	// Register all values callback to function all_values_handler
	tf_co2_v2_register_all_values_callback(&co2,
	                                       all_values_handler,
	                                       NULL);

	// Set period for all values callback to 1s (1000ms)
	tf_co2_v2_set_all_values_callback_configuration(&co2, 1000, false);
}

void example_loop(TF_HAL *hal) {
	// Poll for callbacks
	tf_hal_callback_tick(hal, 0);
}
