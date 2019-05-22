# Collection of ATMEL based projects

These projects show to use OpenHomeControl protocol for different kind of application.

## Receiver
based on Atmega328p (e.g. Arduino Nano)
transmits and receives data on CC1101 - module
usb - interface to connect to any computer

## Sensor
based on Atmega328p (e.g. Arduino Nano)
supports sensors:
* BH1750 light intensity sensor
* BME280 Integrated Environmental Sensor
* BMP180  Barometric Pressure Sensor 
* DHT22 Humidity sensor
* OneWireDevices (e.g. DS18S20, DS2401, DS2438 )
* Ventus W132 anemometer (directly connected to Pin2 of Arduino Nano, radio interface is not used )
transmits data over CC1101 - module

## Display
based on Atmega644
displays different data
receives data on CC1101 - module