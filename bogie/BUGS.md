Bug file
=======================

CURRENT BUGS:
--------------------

Sabertooth is acting a little funny during my ramping; I think it's ramping
faster than it should.  (I suppose I could measure the time and make sure).
However, I suspect it has to do with my LEDs.  So, that's what I'm testing
right now.

TO BE TESTED:
-------------------

* Communcation over RS485
* Optical encooder ( position )
* Magnetic encoder ( speed )
* PID


TO BE IMPLEMENTED:
--------------------------

* Current measurement
* Limit switches and indexing
* Fault detection


FIXED BUGS:
---------------------

* System untested.  Can write code to the XMega, but the communication to the Sabertooth doesn't work.  This was fixed by adjusting the baud rate.  However, the system clock speed may have to be adjusted in order for the beaglebone communication to work properly.  (Note that only the setting for 9600 baud was adjusted.  The others are probably still wrong)


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
