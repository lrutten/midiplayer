#ifndef MIDIFILE_H
#define MIDIFILE_H

#include <stdio.h>
#include <string.h>
#include <functional>

class MidiFile
{
private:
   static const bool debug = false;
   
   // in header
   unsigned long  total_length;
   unsigned short format;
   unsigned short ntracks; // number of tracks
   unsigned short upb;     // units per beat

   // in track
   unsigned short seqnr;
   unsigned char text[100];
   unsigned char track_name[100];
   unsigned long tempo;
   unsigned char sf;
   unsigned char mi;
   unsigned char tsig_num; // time signature numerator
   unsigned char tsig_den; // time signature denominator
   unsigned char mtro_tic; // clock tick per metronome
   unsigned char not_32nd; // number of 32nd in quarter note 
   
   unsigned short read_2b_integer(FILE *fp);
   unsigned long read_3b_integer(FILE *fp);
   unsigned long read_4b_length(FILE *fp);
   unsigned char read_char(FILE *fp);
   unsigned int  read_var_len_val(FILE *fp);

public:
   MidiFile();
   int parse(const char *fn, std::function<void(unsigned int, unsigned char, unsigned char, unsigned char)> notefu);
   unsigned int delta_to_ms(unsigned int dlt);
};

#endif