import com.tinkerforge.IPConnection;
import com.tinkerforge.BrickletCO2V2;

public class ExampleCallback {
	private static final String HOST = "localhost";
	private static final int PORT = 4223;

	// Change XYZ to the UID of your CO2 Bricklet 2.0
	private static final String UID = "XYZ";

	// Note: To make the example code cleaner we do not handle exceptions. Exceptions
	//       you might normally want to catch are described in the documentation
	public static void main(String args[]) throws Exception {
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickletCO2V2 co2 = new BrickletCO2V2(UID, ipcon); // Create device object

		ipcon.connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Add all values listener
		co2.addAllValuesListener(new BrickletCO2V2.AllValuesListener() {
			public void allValues(int co2Concentration, int temperature, int humidity) {
				System.out.println("CO2 Concentration: " + co2Concentration + " ppm");
				System.out.println("Temperature: " + temperature/100.0 + " °C");
				System.out.println("Humidity: " + humidity/100.0 + " %RH");
				System.out.println("");
			}
		});

		// Set period for all values callback to 1s (1000ms)
		co2.setAllValuesCallbackConfiguration(1000, false);

		System.out.println("Press key to exit"); System.in.read();
		ipcon.disconnect();
	}
}
