// https://fortyseveneffects.github.io/arduino_midi_library/index.html
// https://github.com/mon/Arduino-USB-Rename

#include <MIDI.h>     // Add Midi Library for DIN connections
#include <MIDIUSB.h>  // MIDI library for... USB

#include "usb_rename.h"  // Rename the USB Device
// Product name, Manufacturer, serial
// Any argument can be left off, or NULL to keep the original Arduino name
USBRename dummy = USBRename("Arp 2", "Intuitive Harmony", "0001");



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
  // Destructor
  ~ArpNote() {
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

  // Getter for the note value
  byte getNoteRoot() {
    return noteRoot;
  }
};


// Held notes settings
const byte MAX_NOTES = 10;
ArpNote* notesHeld[MAX_NOTES];  // Array to store MIDI data for held notes to arpeggiate over
byte notesHeldCount = 0;        // Enables program to check if a note is held

// Function to play held notes
void playArp() {
  for (byte i = 0; i < notesHeldCount; ++i) {
    notesHeld[i]->on();
  }
}

// Arpeggio settings
const int heldNotesLED = 8;                  // LED pin for held notes visualization
const unsigned long arpeggioInterval = 500;  // Time between arpeggio steps in milliseconds
const int noteProbability = 50;              // Probability of playing a note in the arpeggio sequence (0-100)
unsigned long lastArpeggioTime = 0;          // Time of the last arpeggio step
byte arpeggioCounter = 0;                    // Counter for the arpeggio sequence

// ~~~~~~~~~~~~~
// Arduino Setup
// ~~~~~~~~~~~~~
void setup() {
  pinMode(heldNotesLED, OUTPUT);
  MIDI.begin(MIDI_CHANNEL_OMNI);         // Initialize the Midi Library.
  MIDI.turnThruOff();                    // Turns MIDI through off
  MIDI.setHandleNoteOn(handleNoteOn);    // Set callback for the MIDI DIN handling
  MIDI.setHandleNoteOff(handleNoteOff);  // Set callback for the MIDI DIN handling
}

// ~~~~~~~~~~~~
// Arduino Loop
// ~~~~~~~~~~~~
void loop() {
  MIDI.read();  // Continuously check if Midi data has been received.
  if (notesHeldCount > 0) {
    digitalWrite(heldNotesLED, HIGH);
    // playArp();  // Play the held notes
  } else {
    digitalWrite(heldNotesLED, LOW);
  }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MIDI Input Callback Functions for MIDI Library Handling
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void handleNoteOn(byte channel, byte note, byte velocity) {
  // add logic for allowing accompaniment (or not) this will play the note immediedtly
  MIDI.sendNoteOn(note, velocity, channel);

  // Create a new ArpNote and store its pointer in the notesHeld array
  if (notesHeldCount < MAX_NOTES) {
    notesHeld[notesHeldCount] = new ArpNote(channel, note, velocity);
    // This will come later in the Arp
    // notesHeld[notesHeldCount]->on();
    notesHeldCount++;
  }
}

void handleNoteOff(byte channel, byte note, byte velocity) {
  // For live accompaniment
  MIDI.sendNoteOff(note, velocity, channel);

  // Find and remove the corresponding ArpNote from the notesHeld array
  for (byte i = 0; i < notesHeldCount; ++i) {
    if (notesHeld[i]->getNoteRoot() == note) {
      // This may only come later in the Arp for now lets leave it for sticky notes potential
      notesHeld[i]->off();
      delete notesHeld[i];  // Free memory allocated for ArpNote
      // Move the last element to the current position to maintain order
      notesHeld[i] = notesHeld[notesHeldCount - 1];
      notesHeldCount--;
      break;
    }
  }
}
