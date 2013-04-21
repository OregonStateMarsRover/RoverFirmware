Features
=========================

Control two motors
------------------
Using a sabertooth, just like the bogies

Get feedback from magnetic sensors
-----------------------
I believe it's SPI, but it doesn't require a command from us.
That means we only have 8 bits of resuolution. Read the datasheet to make sure.

Get feedback from limit switches; stop movement appropriately
------------------------
These are just digital inputs

PID
----------
Using our magnetic encoders, of course.

Measure current
--------------
It's just an analog input

Communicate to the bogie controller
---------------------
We need to figure out what the command will be.
It's probably a vertical and horizontal position. However, it could instead be two angles.
