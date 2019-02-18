using System;
using Tinkerforge;

class Example
{
	private static string HOST = "localhost";
	private static int PORT = 4223;
	private static string UID = "XYZ"; // Change XYZ to the UID of your CO2 Bricklet 2.0

	static void Main()
	{
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickletCO2V2 co2 = new BrickletCO2V2(UID, ipcon); // Create device object

		ipcon.Connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Get current all values
		int co2Concentration, humidity; short temperature;
		co2.GetAllValues(out co2Concentration, out temperature, out humidity);

		Console.WriteLine("CO2 Concentration: " + co2Concentration + " ppm");
		Console.WriteLine("Temperature: " + temperature/100.0 + " °C");
		Console.WriteLine("Humidity: " + humidity/100.0 + " %RH");

		Console.WriteLine("Press enter to exit");
		Console.ReadLine();
		ipcon.Disconnect();
	}
}
