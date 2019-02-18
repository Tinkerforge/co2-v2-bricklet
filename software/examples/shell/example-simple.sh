#!/bin/sh
# Connects to localhost:4223 by default, use --host and --port to change this

uid=XYZ # Change XYZ to the UID of your CO2 Bricklet 2.0

# Get current all values
tinkerforge call co2-v2-bricklet $uid get-all-values
