# Midi player on NUCLEO-F446RE


This project implements a midi player on a NUCLEO-F446RE board.

At the start of the program it reads the root directory of the sdcard
of shows the list of files on the 128x64 OLED display.
Turning the knob allows one to choose the file to be played.
A push on the knob starts the playing.

The MIDI events are sent to the UART which is connected to the 5 pin DIN connector.

## Software

The software is is built as an actor system. A class becomes an actor while
inheriting from the `Actor` class.

These are the actors:

* `Controller`
* `RotayButton`


### MIDI Parser

The parser is given an name of a MIDI file and starts readding the file.
The extension of the file must be `.mid`.
As the a NUCLEO-F446RE board has only 128kb of static RAM the parser avoids the buffering
of the midi events in memory while reading the MIDI file. In order to make the parser
more generally usefull it is given a lambda function which is used as a callback.

Not on every event this lambda function is called. The following table shows how each
event is handled by the parser.

| Meaning             | Code    | Action             |
|---------------------|---------|--------------------|
| Sequence number     | `FF 00` | skip               |
| Text                | `FF 01` | skip               |
| Copyright notice    | `FF 02` | skip               |
| Track name          | `FF 03` | buffered, not used |
| Instrument name     | `FF 04` | skip               |
| Lyric text          | `FF 05` | skip               |
| Marker text         | `FF 06` | skip, error?       |
| Cue point           | `FF 07` | skip, error?       |
| Program name        | `FF 08` | not detected       |
| Device name         | `FF 09` | not detected       |
| MIDI channel prefix | `FF 20` | skip               |
| MIDI port           | `FF 21` | skip               |
| Track end           | `FF 2F` | stop               |
| Tempo               | `FF 51` | buffered, not used |
| SMPTE offset        | `FF 54` | skip, error?       |
| Time signature      | `FF 58` | buffered, not used |
| Key signature       | `FF 59` | buffered, not used |
| Key signature       | `FF 7F` | not detected       |
| Note off            | `80`    | lambda called      |
| Note on             | `90`    | lambda called      |
| Polyphonic pressure | `F0 A0` | skip               |
| Controller          | `F0 B0` | lambda called      |
| Program change      | `F0 C0` | skip               |
| Channel pressure    | `F0 D0` | skip               |
| Pitch bend          | `F0 E0` | skip               |
| Sys Ex event        | `F0 LL` | not detected       |
| Escape sequence     | `F3`    | not detected       |
| Escape sequence     | `F7`    | not detected       |

The *Running status* mechanism is handled correctly.

## Hardware

### Connection sd card reader

In the source header file:

~~~~
mbed-os/targets/TARGET_STM/TARGET_STM32F4/TARGET_STM32F446xE/TARGET_NUCLEO_F446RE/PinNames.h
~~~~

the general names definitions for the for the SPI port can be found.
For the `F446RE` these are the SPI definitions:

~~~~
typedef enum {
    SPI_MOSI    = PA_7,
    SPI_MISO    = PA_6,
    SPI_SCK     = PA_5,
    SPI_CS      = PB_6,
} PinName;
~~~~

Only the SPI definitions are mentioned in the preceding example.


### OLED display

Source code from Adafruit is used to drive the display.

### Rotary button decoder

The `RotaryButton` class is responsable for handling the reoary button.
Due to contact bounce an interrupt solution was not possible. 
The `RotaryButton` object runs as an `Actor` and has its own thread.
This way a polling solution is made.


## Links

* [https://os.mbed.com/users/Padman/code/MIDIparser/](https://os.mbed.com/users/Padman/code/MIDIparser/)

    This is a partial midi parser.

* [http://www.ccarh.org/courses/253/handout/smf/](http://www.ccarh.org/courses/253/handout/smf/)

    The MIDI format explained.

* [http://www.personal.kent.edu/~sbirch/Music_Production/MP-II/MIDI/an_introduction_to_midi_contents.htm](http://www.personal.kent.edu/~sbirch/Music_Production/MP-II/MIDI/an_introduction_to_midi_contents.htm)

    Another MIDI format description.

* [https://tttapa.github.io/Arduino/MIDI/Chap00-Arduino-MIDI.html](https://tttapa.github.io/Arduino/MIDI/Chap00-Arduino-MIDI.html)

   A Arduino Midi project with Midi, hardware and software description.

* [https://github.com/ncu571633/midi_parser](https://github.com/ncu571633/midi_parser)

    A parser for `iostream`, not suited for mbed-os.

* [https://github.com/craigsapp/midifile](https://github.com/craigsapp/midifile)

    A parser for `iostream`. This parser is interessing  for the analysis of a midi file.
    One of the exampes dumps a complete midi bestand into readable text.

* [https://github.com/ARMmbed/mbed-os-example-filesystem.git](https://github.com/ARMmbed/mbed-os-example-filesystem.git)

    A `mbed-os` example that reads the sd card.


