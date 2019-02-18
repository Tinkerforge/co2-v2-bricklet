Imports System
Imports Tinkerforge

Module ExampleCallback
    Const HOST As String = "localhost"
    Const PORT As Integer = 4223
    Const UID As String = "XYZ" ' Change XYZ to the UID of your CO2 Bricklet 2.0

    ' Callback subroutine for all values callback
    Sub AllValuesCB(ByVal sender As BrickletCO2V2, ByVal co2Concentration As Integer, _
                    ByVal temperature As Short, ByVal humidity As Integer)
        Console.WriteLine("CO2 Concentration: " + co2Concentration.ToString() + " ppm")
        Console.WriteLine("Temperature: " + (temperature/100.0).ToString() + " °C")
        Console.WriteLine("Humidity: " + (humidity/100.0).ToString() + " %RH")
        Console.WriteLine("")
    End Sub

    Sub Main()
        Dim ipcon As New IPConnection() ' Create IP connection
        Dim co2 As New BrickletCO2V2(UID, ipcon) ' Create device object

        ipcon.Connect(HOST, PORT) ' Connect to brickd
        ' Don't use device before ipcon is connected

        ' Register all values callback to subroutine AllValuesCB
        AddHandler co2.AllValuesCallback, AddressOf AllValuesCB

        ' Set period for all values callback to 1s (1000ms)
        co2.SetAllValuesCallbackConfiguration(1000, False)

        Console.WriteLine("Press key to exit")
        Console.ReadLine()
        ipcon.Disconnect()
    End Sub
End Module
