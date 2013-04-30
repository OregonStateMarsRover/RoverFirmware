Features
=====================

Switch between cameras
-----------------
This is pretty easy. It uses a demultiplexer, just like PORTE on the Wunderboard.


Related options
----------------------
(These are all just pulling a pin high or low)
* Enable/disable the mux
* Enable/disable the video buffers


Read preciscion 5v reference
---------------------
Uses SPI.
Read the datasheet to learn more (TLC548CD)
Make this interrupt-based, if possible.


Control the brightness of the cameras
-----------------------------
Prevents washout from bright spots, but still allows vision in shadows.
*	Use the info from the voltage reference as feedback.
*	Try to average the frame
*	Adjust the gain of the amplifier using the digital potentiometer
	(It just takes a pulse and a direction, but read the MCP4011 datasheet to find more)
*	Use PID (probably just P term) to make sure we get where we want.
	Wikipedia has a good article on that.
*	Update only once a frame, if possible
	(How to find the start of the frame?)


Communicate with Beaglebone
-------------------------
Using USART.c and SerialProtocol.c, as usual.
This code can be copied over from the Bogie stuff


GPIO?
-----------------
PORTA is just hooked up directly to a header.
These could be digital inputs or outputs, or analog inputs as well.
