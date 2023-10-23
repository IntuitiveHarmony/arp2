// https://fortyseveneffects.github.io/arduino_midi_library/index.html
// https://github.com/mon/Arduino-USB-Rename

#include <MIDI.h>     // Add Midi Library for DIN connections
#include <MIDIUSB.h>  // MIDI library for... USB

#include "usb_rename.h"  // Rename the USB Device
// Product name, Manufacturer, serial
// Any argument can be left off, or NULL to keep the original Arduino name
USBRename dummy = USBRename(NULL, "Intuitive Harmony", "0001");

//Create an instance of the library with default name, serial port and settings
MIDI_CREATE_DEFAULT_INSTANCE();

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Arp Note (Uses MIDI library calbacks from the setup)
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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
  // Transpose the note by a certain number of semitones
  void transpose(int semitones) {
    noteOut = noteRoot + semitones;
  }
  // Reset noteOut to the noteRoot
  void resetNote() {
    noteOut = noteRoot;
  }

  // Change the velocity to a new value
  void changeVelocity(byte newVelocity) {
    velocityOut = newVelocity;
  }

  // Getter for the root note value
  byte getNoteRoot() {
    return noteRoot;
  }
};
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


// Held notes settings
const byte MAX_NOTES = 10;
ArpNote* notesHeld[MAX_NOTES];  // Array to store MIDI data for held notes to arpeggiate over
byte notesHeldCount = 0;        // Enables program to check if a note is held and used as index for the array

// Arpeggio settings
const int heldNotesLED = 8;      // LED pin for held notes visualization
const int tempoPin = A0;         // Pin where the Tempo potentiometer is connected
int arpeggioIntervalMin = 5;     // Minimum arpeggio interval (max fast)
int arpeggioIntervalMax = 1000;  // Maximum arpeggio interval (max slow)
unsigned long lastArpeggioTime = 0;
unsigned long arpeggioInterval = 250;
int arpeggioCounter = 0;  // Counter for the arpeggio sequence

// Probability settings
const int probabilityPin = A1;  // Pin where the probability potentiometer is connected
int noteProbability;

// Octave Settings
const int octavePin = A2;  // Pin where the octave potentiometer is connected
int octaveRange;
int octaveCounter;  // keep track of octave range when arp goes over more than the root


// ~~~~~~~~~~~~~
// Arduino Setup
// ~~~~~~~~~~~~~
void setup() {
  pinMode(heldNotesLED, OUTPUT);
  pinMode(tempoPin, INPUT);  // Set Arduino board pin to input

  MIDI.begin(MIDI_CHANNEL_OMNI);         // Initialize the Midi Library.
  MIDI.turnThruOff();                    // Turns MIDI through off
  MIDI.setHandleNoteOn(handleNoteOn);    // Set callback for the MIDI DIN handling
  MIDI.setHandleNoteOff(handleNoteOff);  // Set callback for the MIDI DIN handling
}

// ~~~~~~~~~~~~
// Arduino Loop
// ~~~~~~~~~~~~
void loop() {
  getPotStates();

  MIDI.read();  // Continuously check if Midi data has been received.
  if (notesHeldCount > 0) {
    digitalWrite(heldNotesLED, HIGH);
    playArp();  // Play the held notes
  } else {
    digitalWrite(heldNotesLED, LOW);
    resetArp();
  }
}

// ~~~~~~~~~~~~~~~~~
// Utility Functions
// ~~~~~~~~~~~~~~~~~
void getPotStates() {
  int tempoPotValue = analogRead(tempoPin);  // Read the tempo pot value and map it to the arpeggio interval range
  arpeggioInterval = map(tempoPotValue, 0, 1023, arpeggioIntervalMin, arpeggioIntervalMax);

  int probabilityValue = analogRead(probabilityPin);  // Read the probabilty pot and map it
  // this doesn't get to 0% or 100% when max an min are set to 0 and 100 :|
  noteProbability = map(probabilityValue, 0, 1023, -9, 106);

  int octaveValue = analogRead(octavePin);  // Read the octave pot and map it
  octaveRange = map(octaveValue, 0, 1023, 0, 4);
  Serial.print("octave. ");
  Serial.print(octaveRange);
  Serial.print(" octaveCounter: ");
  Serial.println(octaveCounter);
}

// ~~~~~~~~~~~~~~~~~~
// Arpeggio Functions
// ~~~~~~~~~~~~~~~~~~
void playArp() {
  // Loop over the held notes array
  for (int i = 0; i < notesHeldCount; i++) {
    Serial.print("i. ");
    Serial.print(i);
    Serial.print(" notesHeldCount: ");
    Serial.println(notesHeldCount - 1);

    // Check if it's time for the next arpeggio step
    if (millis() - lastArpeggioTime >= arpeggioInterval) {

      // Transpose the note based on the octaveRange and the octaveCounter
      // Root octave
      if (octaveRange == 0) {
        notesHeld[arpeggioCounter]->resetNote();
        // Octave Range One
      } else if (octaveRange == 1) {
        // Root Octave Level
        if (octaveCounter == 0) {
          notesHeld[arpeggioCounter]->resetNote();
          // Check if at the end off arp array
          if (arpeggioCounter == notesHeldCount - 1) {
            // Increment the octave level up
            octaveCounter = 1;
          }
          // First Octave Level
        } else {
          notesHeld[arpeggioCounter]->transpose(12);
          // Check if at the end off arp array
          if (arpeggioCounter == notesHeldCount - 1) {
            // Increment the octave level down
            octaveCounter = 0;
          }
        }
      }

      // Randomly decide whether to play a note based on probability
      if (random(0, 100) < noteProbability) {


        // Play the note in the arpeggio sequence
        notesHeld[arpeggioCounter]->on();
      }

      arpeggioCounter = (arpeggioCounter + 1) % notesHeldCount;  // Move to the next note in the arpeggio
      lastArpeggioTime = millis();                               // Update the last arpeggio time
    }
  }
}

void resetArp() {
  lastArpeggioTime = 0;  // Reset the arpeggio timer
  arpeggioCounter = 0;   // Reset the arpeggio counter
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
    notesHeldCount++;
  }
}

void handleNoteOff(byte channel, byte note, byte velocity) {
  // For live accompaniment
  MIDI.sendNoteOff(note, velocity, channel);

  // Find and remove the corresponding ArpNote from the notesHeld array
  for (byte i = 0; i < notesHeldCount; ++i) {
    if (notesHeld[i]->getNoteRoot() == note) {
      // This may only come later in the Arp for now leave it for sticky notes potential
      notesHeld[i]->off();
      delete notesHeld[i];  // Free memory allocated for ArpNote
      // Move the last element to the current position to maintain order
      notesHeld[i] = notesHeld[notesHeldCount - 1];
      notesHeldCount--;
      break;
    }
  }
}
