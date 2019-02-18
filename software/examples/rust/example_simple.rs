use std::{error::Error, io};

use tinkerforge::{co2_v2_bricklet::*, ip_connection::IpConnection};

const HOST: &str = "localhost";
const PORT: u16 = 4223;
const UID: &str = "XYZ"; // Change XYZ to the UID of your CO2 Bricklet 2.0.

fn main() -> Result<(), Box<dyn Error>> {
    let ipcon = IpConnection::new(); // Create IP connection.
    let co2 = Co2V2Bricklet::new(UID, &ipcon); // Create device object.

    ipcon.connect((HOST, PORT)).recv()??; // Connect to brickd.
                                          // Don't use device before ipcon is connected.

    // Get current all values.
    let all_values = co2.get_all_values().recv()?;

    println!("CO2 Concentration: {} ppm", all_values.co2_concentration);
    println!("Temperature: {} °C", all_values.temperature as f32 / 100.0);
    println!("Humidity: {} %RH", all_values.humidity as f32 / 100.0);

    println!("Press enter to exit.");
    let mut _input = String::new();
    io::stdin().read_line(&mut _input)?;
    ipcon.disconnect();
    Ok(())
}
