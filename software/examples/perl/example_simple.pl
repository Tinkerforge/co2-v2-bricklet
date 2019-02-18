#!/usr/bin/perl

use strict;
use Tinkerforge::IPConnection;
use Tinkerforge::BrickletCO2V2;

use constant HOST => 'localhost';
use constant PORT => 4223;
use constant UID => 'XYZ'; # Change XYZ to the UID of your CO2 Bricklet 2.0

my $ipcon = Tinkerforge::IPConnection->new(); # Create IP connection
my $co2 = Tinkerforge::BrickletCO2V2->new(&UID, $ipcon); # Create device object

$ipcon->connect(&HOST, &PORT); # Connect to brickd
# Don't use device before ipcon is connected

# Get current all values
my ($co2_concentration, $temperature, $humidity) = $co2->get_all_values();

print "CO2 Concentration: $co2_concentration ppm\n";
print "Temperature: " . $temperature/100.0 . " °C\n";
print "Humidity: " . $humidity/100.0 . " %RH\n";

print "Press key to exit\n";
<STDIN>;
$ipcon->disconnect();
