function octave_example_simple()
    more off;

    HOST = "localhost";
    PORT = 4223;
    UID = "XYZ"; % Change XYZ to the UID of your CO2 Bricklet 2.0

    ipcon = javaObject("com.tinkerforge.IPConnection"); % Create IP connection
    co2 = javaObject("com.tinkerforge.BrickletCO2V2", UID, ipcon); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Get current all values
    allValues = co2.getAllValues();

    fprintf("CO2 Concentration: %d ppm\n", allValues.co2Concentration);
    fprintf("Temperature: %g °C\n", allValues.temperature/100.0);
    fprintf("Humidity: %g %%RH\n", allValues.humidity/100.0);

    input("Press key to exit\n", "s");
    ipcon.disconnect();
end
