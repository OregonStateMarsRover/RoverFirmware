Features
=====================

Switch between cameras
-----------------
This is pretty easy. It uses a demultiplexer, just like PORTE on the Wunderboard.

Related options
----------------------
(These are all just pulling a pin high or low)
* Manipulate UD and CS on the MCP4011 chip
* Enable/disable the mux
* Enable/disable the video buffers


Read preciscion 5v reference
---------------------
Uses SPI.
Read the datasheet to learn more (TLC548CD)


Communicate with Beaglebone
-------------------------
Using USART.c and SerialProtocol.c, as usual

GPIO?
-----------------
PORTA is just hooked up directly to a header.
These could be digital inputs or outputs, or analog inputs as well.
