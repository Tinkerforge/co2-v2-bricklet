<?php

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickletCO2V2.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickletCO2V2;

const HOST = 'localhost';
const PORT = 4223;
const UID = 'XYZ'; // Change XYZ to the UID of your CO2 Bricklet 2.0

// Callback function for all values callback
function cb_allValues($co2_concentration, $temperature, $humidity)
{
    echo "CO2 Concentration: $co2_concentration ppm\n";
    echo "Temperature: " . $temperature/100.0 . " °C\n";
    echo "Humidity: " . $humidity/100.0 . " %RH\n";
    echo "\n";
}

$ipcon = new IPConnection(); // Create IP connection
$co2 = new BrickletCO2V2(UID, $ipcon); // Create device object

$ipcon->connect(HOST, PORT); // Connect to brickd
// Don't use device before ipcon is connected

// Register all values callback to function cb_allValues
$co2->registerCallback(BrickletCO2V2::CALLBACK_ALL_VALUES, 'cb_allValues');

// Set period for all values callback to 1s (1000ms)
$co2->setAllValuesCallbackConfiguration(1000, FALSE);

echo "Press ctrl+c to exit\n";
$ipcon->dispatchCallbacks(-1); // Dispatch callbacks forever

?>
