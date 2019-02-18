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

# Register all values callback
co2.register_callback(BrickletCO2V2::CALLBACK_ALL_VALUES) do |co2_concentration,
                                                              temperature, humidity|
  puts "CO2 Concentration: #{co2_concentration} ppm"
  puts "Temperature: #{temperature/100.0} °C"
  puts "Humidity: #{humidity/100.0} %RH"
  puts ''
end

# Set period for all values callback to 1s (1000ms)
co2.set_all_values_callback_configuration 1000, false

puts 'Press key to exit'
$stdin.gets
ipcon.disconnect
