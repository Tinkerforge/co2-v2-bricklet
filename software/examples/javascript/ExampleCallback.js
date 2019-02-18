var Tinkerforge = require('tinkerforge');

var HOST = 'localhost';
var PORT = 4223;
var UID = 'XYZ'; // Change XYZ to the UID of your CO2 Bricklet 2.0

var ipcon = new Tinkerforge.IPConnection(); // Create IP connection
var co2 = new Tinkerforge.BrickletCO2V2(UID, ipcon); // Create device object

ipcon.connect(HOST, PORT,
    function (error) {
        console.log('Error: ' + error);
    }
); // Connect to brickd
// Don't use device before ipcon is connected

ipcon.on(Tinkerforge.IPConnection.CALLBACK_CONNECTED,
    function (connectReason) {
        // Set period for all values callback to 1s (1000ms)
        co2.setAllValuesCallbackConfiguration(1000, false);
    }
);

// Register all values callback
co2.on(Tinkerforge.BrickletCO2V2.CALLBACK_ALL_VALUES,
    // Callback function for all values callback
    function (co2Concentration, temperature, humidity) {
        console.log('CO2 Concentration: ' + co2Concentration + ' ppm');
        console.log('Temperature: ' + temperature/100.0 + ' °C');
        console.log('Humidity: ' + humidity/100.0 + ' %RH');
        console.log();
    }
);

console.log('Press key to exit');
process.stdin.on('data',
    function (data) {
        ipcon.disconnect();
        process.exit(0);
    }
);
