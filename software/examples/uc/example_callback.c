#include "bindings/hal_common.h"
#include "bindings/bricklet_co2_v2.h"

#define UID "XYZ" // Change XYZ to the UID of your CO2 Bricklet 2.0

void check(int rc, const char* msg);

void example_setup(TF_HalContext *hal);
void example_loop(TF_HalContext *hal);


// Callback function for all values callback
static void all_values_handler(TF_CO2V2 *device, uint16_t co2_concentration,
                               int16_t temperature, uint16_t humidity, void *user_data) {
	(void)device; (void)user_data; // avoid unused parameter warning

	tf_hal_printf("CO2 Concentration: %I16u ppm\n", co2_concentration);
	tf_hal_printf("Temperature: %d 1/%d °C\n", temperature, 100.0);
	tf_hal_printf("Humidity: %d 1/%d %%RH\n", humidity, 100.0);
	tf_hal_printf("\n");
}

static TF_CO2V2 co2;

void example_setup(TF_HalContext *hal) {
	// Create device object
	check(tf_co2_v2_create(&co2, UID, hal), "create device object");

	// Register all values callback to function all_values_handler
	tf_co2_v2_register_all_values_callback(&co2,
	                                       all_values_handler,
	                                       NULL);

	// Set period for all values callback to 1s (1000ms)
	tf_co2_v2_set_all_values_callback_configuration(&co2, 1000, false);
}

void example_loop(TF_HalContext *hal) {
	// Poll for callbacks
	tf_hal_callback_tick(hal, 0);
}
