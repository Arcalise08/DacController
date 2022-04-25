# DacController
This is a c++ implementation for MCP4725 DAC using raspberry pi pico

Please refer to MCP4725 documentation for any issues. 
http://ww1.microchip.com/downloads/en/devicedoc/22039d.pdf

This was wired using I2Cs fast mode specification, Ensure you have both i2c wires tethered to a pull up resistor. I used 470 ohms for fast performance. They recommend
> 1000 for speed but more energy and around 2k for normal mode with decent energy consumption.
