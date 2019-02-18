<?php

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickletCO2V2.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickletCO2V2;

const HOST = 'localhost';
const PORT = 4223;
const UID = 'XYZ'; // Change XYZ to the UID of your CO2 Bricklet 2.0

$ipcon = new IPConnection(); // Create IP connection
$co2 = new BrickletCO2V2(UID, $ipcon); // Create device object

$ipcon->connect(HOST, PORT); // Connect to brickd
// Don't use device before ipcon is connected

// Get current all values
$all_values = $co2->getAllValues();

echo "CO2 Concentration: " . $all_values['co2_concentration'] . " ppm\n";
echo "Temperature: " . $all_values['temperature']/100.0 . " °C\n";
echo "Humidity: " . $all_values['humidity']/100.0 . " %RH\n";

echo "Press key to exit\n";
fgetc(fopen('php://stdin', 'r'));
$ipcon->disconnect();

?>
