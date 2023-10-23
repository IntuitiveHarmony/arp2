# Arp2
### 🚧 Project is a work in progress 🚧

To see this arp in action please visit my [instagram page](https://www.instagram.com/intuitive.harmony/) for a [demo](https://www.instagram.com/p/CyIHmlrBUzx/)

Welcome to another arpeggiator exploration.  It is essentially the same arpeggeatior as [this one](https://github.com/IntuitiveHarmony/MIDI_ARP_1).  The code is just cleaned up and refactored to use a class for the held arpeggeatior notes.

<br>

I am using an Arduino Pro Micro the FortySevenEffects' [MIDI Library](https://github.com/FortySevenEffects/arduino_midi_library/tree/dev), Arduino's [MIDIUSB library](https://github.com/arduino-libraries/MIDIUSB) and [custom code](https://github.com/IntuitiveHarmony/arp2/blob/main/arp2.ino) in order to loop over any MIDI notes that are held down.  



## Parameters

The player is able manipulate the tempo, octave range and note on probability.  

## Wiring Diagram

This circuit follows MIDI standard and uses a 6n137 optocoupler in order to isolate the incoming MIDI instrument from my device.   The primary function of this is to prevent noisy ground loops. I relied on [this page](https://www.kieranreck.co.uk/blog/midi-input-with-6n137-6n138-or-6n139#:~:text=The%206N138%20and%206N139%20use,to%20achieve%20the%20same%20output.) by Kieran Reck, in order to help myself understand that part of the circuit.

The rest is of it is simply a couple potentiometers to adjust the parameters, a button to reset the program and the LED indicates that notes are being held down.

![Arp2 Diagram](./Arp2%20Diagram.png)
