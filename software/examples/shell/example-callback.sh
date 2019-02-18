#!/bin/sh
# Connects to localhost:4223 by default, use --host and --port to change this

uid=XYZ # Change XYZ to the UID of your CO2 Bricklet 2.0

# Handle incoming all values callbacks
tinkerforge dispatch co2-v2-bricklet $uid all-values &

# Set period for all values callback to 1s (1000ms)
tinkerforge call co2-v2-bricklet $uid set-all-values-callback-configuration 1000 false

echo "Press key to exit"; read dummy

kill -- -$$ # Stop callback dispatch in background
