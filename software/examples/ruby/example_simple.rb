#!/usr/bin/env ruby
# -*- ruby encoding: utf-8 -*-

require 'tinkerforge/ip_connection'
require 'tinkerforge/bricklet_co2_v2'

include Tinkerforge

HOST = 'localhost'
PORT = 4223
UID = 'XYZ' # Change XYZ to the UID of your CO2 Bricklet 2.0

ipcon = IPConnection.new # Create IP connection
co2 = BrickletCO2V2.new UID, ipcon # Create device object

ipcon.connect HOST, PORT # Connect to brickd
# Don't use device before ipcon is connected

# Get current all values as [co2_concentration, temperature, humidity]
all_values = co2.get_all_values

puts "CO2 Concentration: #{all_values[0]} ppm"
puts "Temperature: #{all_values[1]/100.0} °C"
puts "Humidity: #{all_values[2]/100.0} %RH"

puts 'Press key to exit'
$stdin.gets
ipcon.disconnect
