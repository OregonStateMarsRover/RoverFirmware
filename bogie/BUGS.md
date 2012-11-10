Bug file
=======================

CURRENT BUGS:
--------------------
* The Sabertooth needs some time for initialization before it is ready to
take configuration.  (In this case, the timeout)
* Periphrial clock is too slow for 115.2 kbaud uart.  Clock configuration change is untested.
(For now, the idea is to leave the CPU on the 2Mhz clock, and just run the periphrials on the
external 16Mhz crystal.  It is also possible to run them on the internal 32Mhz clock, but the 
16Mhz crystal should be more accurate)


TO BE TESTED:
-------------------

* Communcation over RS485
* Optical encooder ( position )
* Magnetic encoder ( speed )
* PID


TO BE IMPLEMENTED:
--------------------------

* Current measurement
* Limit switches and indexing (working on it)
* Fault detection


FIXED BUGS:
---------------------

* System untested.  Can write code to the XMega, but the communication to the Sabertooth doesn't work.  This was fixed by adjusting the baud rate.  However, the system clock speed may have to be adjusted in order for the beaglebone communication to work properly.  (Note that only the setting for 9600 baud was adjusted.  The others are probably still wrong)

* Sabertooth is acting a little funny during my ramping; I think it's ramping
faster than it should.  (I suppose I could measure the time and make sure).
However, I suspect it has to do with my LEDs.  This is fixed by correcting the
DIP switches on the Sabertooths, and making sure the #define macros don't have
semicolons in them.  Also, the LEDs are on when pulled low, not high.


CONFIRMED WORKING:
-------------------------

* Communication to Sabertooth (at least, the sabertooth responds and the baud rate is correct)
* Ring buffer (It's a circular queue)



EXPLANATION:
-----------------------

I'm too lazy to use the bug tracking integrated into Github, so this 
is what I do instead for small projects.
This file is tracked along with the rest of the repository.  So, as 
we find bugs and fix bugs, we make note of that in this file.
