# Pump controller for the wells

The [main.ino](main.ino) file contains the source code for the well pump controller. The controller cyclically switches the pumps in periods of 1, 7 or 28 days. As a result, the water is mixed with two wells and does not rottenness.

Components used
---------------

* ARDUINO PRO MINI ATMEGA328 (https://allegro.pl/promocja-na-modul-arduino-pro-mini-z-atmega328-avr-i5574675358.html)
* 2x Relay (https://allegro.pl/modul-2-kanalowy-przekaznik-5v-10a-avr-arm-arduino-i6573709399.html)
* 5x LED 5mm (3x yellow, 2x green)
* 1x Switch
* Surface mounted housing

Schematic diagram
-----------------

![Schematic diagram](schema.png)

Device presentation
-------------------

[![Presentation](https://img.youtube.com/vi/mslES55ITuQ/0.jpg)](https://youtu.be/mslES55ITuQ)

Usage
-----

Controller has one switch. A single short press switches the pumps - off, first pump, second pump, off, ... A single long press changes the pump switching interval - 1 day, 7 days, 28 days.
