package main

import (
	"fmt"
	"github.com/Tinkerforge/go-api-bindings/co2_v2_bricklet"
	"github.com/Tinkerforge/go-api-bindings/ipconnection"
)

const ADDR string = "localhost:4223"
const UID string = "XYZ" // Change XYZ to the UID of your CO2 Bricklet 2.0.

func main() {
	ipcon := ipconnection.New()
	defer ipcon.Close()
	co2, _ := co2_v2_bricklet.New(UID, &ipcon) // Create device object.

	ipcon.Connect(ADDR) // Connect to brickd.
	defer ipcon.Disconnect()
	// Don't use device before ipcon is connected.

	// Get current all values.
	co2Concentration, temperature, humidity, _ := co2.GetAllValues()

	fmt.Printf("CO2 Concentration:  ppm\n", co2Concentration)
	fmt.Printf("Temperature: %f °C\n", float64(temperature)/100.0)
	fmt.Printf("Humidity: %f %%RH\n", float64(humidity)/100.0)

	fmt.Print("Press enter to exit.")
	fmt.Scanln()
}
