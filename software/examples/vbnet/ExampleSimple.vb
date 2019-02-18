Imports System
Imports Tinkerforge

Module ExampleSimple
    Const HOST As String = "localhost"
    Const PORT As Integer = 4223
    Const UID As String = "XYZ" ' Change XYZ to the UID of your CO2 Bricklet 2.0

    Sub Main()
        Dim ipcon As New IPConnection() ' Create IP connection
        Dim co2 As New BrickletCO2V2(UID, ipcon) ' Create device object

        ipcon.Connect(HOST, PORT) ' Connect to brickd
        ' Don't use device before ipcon is connected

        ' Get current all values
        Dim co2Concentration, humidity As Integer
        Dim temperature As Short

        co2.GetAllValues(co2Concentration, temperature, humidity)

        Console.WriteLine("CO2 Concentration: " + co2Concentration.ToString() + " ppm")
        Console.WriteLine("Temperature: " + (temperature/100.0).ToString() + " °C")
        Console.WriteLine("Humidity: " + (humidity/100.0).ToString() + " %RH")

        Console.WriteLine("Press key to exit")
        Console.ReadLine()
        ipcon.Disconnect()
    End Sub
End Module
