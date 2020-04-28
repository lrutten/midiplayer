# Midi player on NUCLEO-F446RE

This project implements a midi player on a NUCLEO-F446RE board.

At the start of the program it reads the root directory of the sdcard
of shows the list of files on the 128x64 OLED display.
Turning the knob allows one to choose the file to be played.
A push on the knob starts the playing.

The MIDI events are sent to the UART which is connected to the 5 pin DIN connector.


## Connection sd card reader

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


## OLED display

## Rotary button decoder

## Links

* [https://os.mbed.com/users/Padman/code/MIDIparser/](https://os.mbed.com/users/Padman/code/MIDIparser/)

    This is a partial midi parser.

* [http://www.ccarh.org/courses/253/handout/smf/](http://www.ccarh.org/courses/253/handout/smf/)

    The MIDI format explained.

* [https://github.com/ncu571633/midi_parser](https://github.com/ncu571633/midi_parser)

    A parser for `iostream`, not suited for embed-os.

* [https://github.com/craigsapp/midifile](https://github.com/craigsapp/midifile)

    A parser for `iostream`. This parser is interessing  for the analysis of a midi file.
    One of the exampes dumps a complete midi bestand into readable text.

* [https://github.com/ARMmbed/mbed-os-example-filesystem.git](https://github.com/ARMmbed/mbed-os-example-filesystem.git)

    A `mbed-os` example that reads the sd card.


