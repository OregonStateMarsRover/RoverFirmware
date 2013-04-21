Features
=================

Talk to the two AX12 servos (rs232 half-duplex)
----------------------------------------
This means the tri-state buffer should only be enabled when we're sending data;
otherwise, we should be listening for replies from the servos.
This should be done through the USART.c functions, and maybe a modified SerialProtocol.c
Read the servo datasheet to find out what commands they take and how. (AX-12)

Configure camera settings (J1) (USART)
------------------
Again, use USART.c for this.  Read the camera datasheet to find out what commands it takes and how.

Measure acceleromter (U35) (spi)
--------------------------
Learn about SPI on wikipedia.
Read about how to implement it on the Xmega in the datasheet.  Try configuring it.
Read about what commands the accelerometer takes on its datasheet. (KXUD9-4100)

Measure barometric pressure (U3) (i2c)
-------------------------
Learn about i2c on wikipedia.
Read about how to implement it on the Xmega in the datasheet.  Try configuring it.
Read about the commands the barometric pressure sensor takes on its datasheet. (BMP085)

Talk to Beaglebone (RS485)
-----------------------------
This can pretty much be copied over from the bogie code. It uses serialprotocol.h.
However, we need to decide the commands between the tripod and the beaglebone.
We can figure that out after we flesh out this document more.

Display status (LEDs)
--------------------------
This one is really easy, but it's a good place to start to make sure you know how to program
the tripod board.
