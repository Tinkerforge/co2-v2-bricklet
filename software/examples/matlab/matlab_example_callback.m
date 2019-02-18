function matlab_example_callback()
    import com.tinkerforge.IPConnection;
    import com.tinkerforge.BrickletCO2V2;

    HOST = 'localhost';
    PORT = 4223;
    UID = 'XYZ'; % Change XYZ to the UID of your CO2 Bricklet 2.0

    ipcon = IPConnection(); % Create IP connection
    co2 = handle(BrickletCO2V2(UID, ipcon), 'CallbackProperties'); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Register all values callback to function cb_all_values
    set(co2, 'AllValuesCallback', @(h, e) cb_all_values(e));

    % Set period for all values callback to 1s (1000ms)
    co2.setAllValuesCallbackConfiguration(1000, false);

    input('Press key to exit\n', 's');
    ipcon.disconnect();
end

% Callback function for all values callback
function cb_all_values(e)
    fprintf('CO2 Concentration: %i ppm\n', e.co2Concentration);
    fprintf('Temperature: %g °C\n', e.temperature/100.0);
    fprintf('Humidity: %g %%RH\n', e.humidity/100.0);
    fprintf('\n');
end
