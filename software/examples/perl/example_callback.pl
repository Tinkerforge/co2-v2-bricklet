#!/usr/bin/perl

use strict;
use Tinkerforge::IPConnection;
use Tinkerforge::BrickletCO2V2;

use constant HOST => 'localhost';
use constant PORT => 4223;
use constant UID => 'XYZ'; # Change XYZ to the UID of your CO2 Bricklet 2.0

# Callback subroutine for all values callback
sub cb_all_values
{
    my ($co2_concentration, $temperature, $humidity) = @_;

    print "CO2 Concentration: $co2_concentration ppm\n";
    print "Temperature: " . $temperature/100.0 . " °C\n";
    print "Humidity: " . $humidity/100.0 . " %RH\n";
    print "\n";
}

my $ipcon = Tinkerforge::IPConnection->new(); # Create IP connection
my $co2 = Tinkerforge::BrickletCO2V2->new(&UID, $ipcon); # Create device object

$ipcon->connect(&HOST, &PORT); # Connect to brickd
# Don't use device before ipcon is connected

# Register all values callback to subroutine cb_all_values
$co2->register_callback($co2->CALLBACK_ALL_VALUES, 'cb_all_values');

# Set period for all values callback to 1s (1000ms)
$co2->set_all_values_callback_configuration(1000, 0);

print "Press key to exit\n";
<STDIN>;
$ipcon->disconnect();
