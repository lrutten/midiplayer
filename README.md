# Midi player op NUCLEO-F446RE

Nog te documenteren.

## Links

* [https://os.mbed.com/users/Padman/code/MIDIparser/](https://os.mbed.com/users/Padman/code/MIDIparser/)

    Deze code is in gebruik.

* [http://www.ccarh.org/courses/253/handout/smf/](http://www.ccarh.org/courses/253/handout/smf/)

    Het MIDI formaat uitgelegd.

* [https://github.com/ncu571633/midi_parser](https://github.com/ncu571633/midi_parser)

    Een parser voor `iostream`.

* [https://github.com/craigsapp/midifile](https://github.com/craigsapp/midifile)

    Een parser voor `iostream`. Deze parser is interessant voor de analyse van een midibestand.
    Het voorbeeld dumpt een midibestand in leesbare tekst.

* [https://github.com/ARMmbed/mbed-os-example-filesystem.git](https://github.com/ARMmbed/mbed-os-example-filesystem.git)

    Een `mbed-os` voorbeeld dat de sd card leest.


## Aansluiting sdcard lezer

In dit bestand

~~~~
mbed-os/targets/TARGET_STM/TARGET_STM32F4/TARGET_STM32F446xE/TARGET_NUCLEO_F446RE/PinNames.h
~~~~

staan de definities van algemene namen voor de SPI poort.
Voor de `F446RE` zijn dit de SPI definities:

~~~~
typedef enum {
    SPI_MOSI    = PA_7,
    SPI_MISO    = PA_6,
    SPI_SCK     = PA_5,
    SPI_CS      = PB_6,
} PinName;
~~~~

Alleen de SPI definities worden hierboven opgesomd.
