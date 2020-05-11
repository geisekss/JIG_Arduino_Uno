# JIG Arduino UNO

Three latency testers using Arduino Uno:
* JIG QTM: measures latency between and event and 3D marker processing in the Qualisys Track Manager (QTM 2019.3) software.
* JIG ESP: measures latency between and event and receiving the package from a ESP 32 microcontroller by OSC protocol.
* JIG USB: measures latency of the Serial communication in the Uno microcontroller.

### How to use:
* JIG QTM: install the firmware in the Arduino Uno, connect an event generator in an Analog port of it (e.g. push button), and capture the event with markers using the Qualisys system.
* JIG ESP: install the firmware in the Arduino Uno, and a firmware in an Arduino ESP 32 to send events by OSC, connect an event generator in Analog ports of both microcontrollers, and connect the Arduino Uno in a PC by USB to receive the JIG measurements whereas it also receives the ESP 32 measurements by WiFi.
* JIG USB: install the firmware in the Arduino Uno, and connect the Arduino Uno in a PC by USB to receive the JIG measurements.
