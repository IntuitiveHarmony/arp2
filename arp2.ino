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

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Arp Note (Uses MIDI library calbacks fro the setup)
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class ArpNote {
private:
  byte channelRoot;
  byte channelOut;
  byte noteRoot;
  byte noteOut;
  byte velocityRoot;
  byte velocityOut;

public:
  // Constructor
  ArpNote(byte _channelRoot, byte _noteRoot, byte _velocityRoot)
    : channelRoot(_channelRoot), noteRoot(_noteRoot), velocityRoot(_velocityRoot) {
    // For now, assume that output data is the same as root data
    channelOut = channelRoot;
    noteOut = noteRoot;
    velocityOut = velocityRoot;
  }

  // Send the note on to DIN and USB
  void on() {
    MIDI.sendNoteOn(noteOut, velocityOut, channelOut);  // Send to DIN

    midiEventPacket_t noteOn = { 0x09, 0x90 | channelOut - 1, noteOut, velocityOut };  // create MIDI event for USB library
    MidiUSB.sendMIDI(noteOn);                                                          // Send MIDI event to USB
    MidiUSB.flush();                                                                   // Send midi event immediately
  }

  // Send the note off to DIN and USB
  void off() {
    MIDI.sendNoteOff(noteOut, velocityOut, channelOut);  // Send to DIN

    midiEventPacket_t noteOff = { 0x08, 0x80 | channelOut - 1, noteOut, 0 };  // create MIDI event for USB library
    MidiUSB.sendMIDI(noteOff);                                                // Send MIDI event to USB
    MidiUSB.flush();                                                          // Send midi event immediately
  }

  // Methods for manipulation
  void transpose(int semitones) {
    // Example: Transpose the note by a certain number of semitones
    noteOut = noteRoot + semitones;
  }

  void changeVelocity(byte newVelocity) {
    // Example: Change the velocity to a new value
    velocityOut = newVelocity;
  }
};


// ~~~~~~~~~~~~~
// Arduino Setup
// ~~~~~~~~~~~~~
void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);  // Initialize the Midi Library.
  MIDI.turnThruOff();                    // Turns MIDI through off
  MIDI.setHandleNoteOn(handleNoteOn);    // Set callback for the MIDI DIN handling
  MIDI.setHandleNoteOff(handleNoteOff);  // Set callback for the MIDI DIN handling
}

// ~~~~~~~~~~~~
// Arduino Loop
// ~~~~~~~~~~~~
void loop() {
  MIDI.read(); // Continuously check if Midi data has been received.
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MIDI Input Callback Functions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void handleNoteOn(byte channel, byte note, byte velocity) {
  // add logic for allowing accompaniment this will play the note immediedtly
  MIDI.sendNoteOn(note, velocity, channel);
}

void handleNoteOff(byte channel, byte note, byte velocity) {
  MIDI.sendNoteOff(note, velocity, channel);
}
