import com.tinkerforge.IPConnection;
import com.tinkerforge.BrickletCO2V2;
import com.tinkerforge.BrickletCO2V2.AllValues;

public class ExampleSimple {
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

		// Get current all values
		AllValues allValues = co2.getAllValues(); // Can throw com.tinkerforge.TimeoutException

		System.out.println("CO2 Concentration: " + allValues.co2Concentration + " ppm");
		System.out.println("Temperature: " + allValues.temperature/100.0 + " °C");
		System.out.println("Humidity: " + allValues.humidity/100.0 + " %RH");

		System.out.println("Press key to exit"); System.in.read();
		ipcon.disconnect();
	}
}
