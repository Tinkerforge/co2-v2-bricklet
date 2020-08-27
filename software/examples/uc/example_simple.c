#include "bindings/hal_common.h"
#include "bindings/bricklet_co2_v2.h"

#define UID "XYZ" // Change XYZ to the UID of your CO2 Bricklet 2.0

void check(int rc, const char* msg);

void example_setup(TF_HalContext *hal);
void example_loop(TF_HalContext *hal);


static TF_CO2V2 co2;

void example_setup(TF_HalContext *hal) {
	// Create device object
	check(tf_co2_v2_create(&co2, UID, hal), "create device object");

	// Get current all values
	uint16_t co2_concentration, humidity; int16_t temperature;
	check(tf_co2_v2_get_all_values(&co2, &co2_concentration, &temperature,
	                               &humidity), "get all values");

	tf_hal_printf("CO2 Concentration: %u ppm\n", co2_concentration);
	tf_hal_printf("Temperature: %d 1/%d °C\n", temperature, 100.0);
	tf_hal_printf("Humidity: %d 1/%d %%RH\n", humidity, 100.0);
}

void example_loop(TF_HalContext *hal) {
	// Poll for callbacks
	tf_hal_callback_tick(hal, 0);
}
