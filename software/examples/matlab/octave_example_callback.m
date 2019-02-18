function octave_example_callback()
    more off;

    HOST = "localhost";
    PORT = 4223;
    UID = "XYZ"; % Change XYZ to the UID of your CO2 Bricklet 2.0

    ipcon = javaObject("com.tinkerforge.IPConnection"); % Create IP connection
    co2 = javaObject("com.tinkerforge.BrickletCO2V2", UID, ipcon); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Register all values callback to function cb_all_values
    co2.addAllValuesCallback(@cb_all_values);

    % Set period for all values callback to 1s (1000ms)
    co2.setAllValuesCallbackConfiguration(1000, false);

    input("Press key to exit\n", "s");
    ipcon.disconnect();
end

% Callback function for all values callback
function cb_all_values(e)
    fprintf("CO2 Concentration: %d ppm\n", e.co2Concentration);
    fprintf("Temperature: %g °C\n", e.temperature/100.0);
    fprintf("Humidity: %g %%RH\n", e.humidity/100.0);
    fprintf("\n");
end
