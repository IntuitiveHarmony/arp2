#ifndef Arpeggio_h
#define Arpeggio_h

#include <Arduino.h>
#include <MIDI.h>     // Add Midi Library for DIN connections
#include <MIDIUSB.h>  // MIDI library for... USB

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
    // Initialize other members if needed
    // For now, assume that output data is the same as root data
    channelOut = channelRoot;
    noteOut = noteRoot;
    velocityOut = velocityRoot;
  }

  // This Handles both MIDI DIN and USB
  void sendMIDI(bye noteOut, byte velocityOut, byte channelOut) {
    // MIDI DIN
    MIDI.sendNoteOn(note, velocity, channel);
    // MIDI USB
    midiEventPacket_t midiEvent = { 0x09, 0x90 | channelOut - 1, noteOut, velocityOut };
    MidiUSB.sendMIDI(midiEvent);  // Send MIDI event to USB
    MidiUSB.flush();              // Send midi event immediately
  }

  // Methods for manipulation
  void transpose(int semitones) {
    noteOut = noteRoot + semitones;
  }

  void changeVelocity(byte newVelocity) {
    // Example: Change the velocity to a new value
    velocityOut = newVelocity;
  }
}

#endif