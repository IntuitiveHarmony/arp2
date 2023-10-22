// https://fortyseveneffects.github.io/arduino_midi_library/index.html
// https://github.com/mon/Arduino-USB-Rename

#include <MIDI.h>     // Add Midi Library for DIN connections
#include <MIDIUSB.h>  // MIDI library for... USB

#include "usb_rename.h"  // Rename the USB Device
// Product name, Manufacturer, serial
// Any argument can be left off, or NULL to keep the original Arduino name
USBRename dummy = USBRename("Arp 1", "Intuitive Harmony", "0001");


//Create an instance of the library with default name, serial port and settings
MIDI_CREATE_DEFAULT_INSTANCE();




void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
