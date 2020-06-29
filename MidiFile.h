#ifndef MIDIFILE_H
#define MIDIFILE_H

#include <stdio.h>
#include <string.h>
#include <functional>

class StatusRunningBuffer
{
public:
   static const unsigned int bufsize = 3;

   StatusRunningBuffer() : type(0), size(0)
   {
   }
   void clear()
   {
      size = 0;
   }
   unsigned int get_size()
   {
      return size;
   }
   unsigned char operator[](unsigned int i)
   {
      return buf[i];
   }
   void push(unsigned char c)
   {
      if (size < bufsize)
      {
         buf[size++] = c;
      }
   }
   void clear_set(unsigned char tp)
   {
      clear();
      set_type(tp);
   }
   unsigned char pop()
   {
      if (size > 0)
      {
         size--;
         unsigned char c = buf[0];
         for (unsigned int i=0; i<size; i++)
         {
            buf[i] = buf[i + 1];
         }

         return c;
      }
      else
      {
         return 0;
      }
   }
   unsigned char get_type()
   {
      return type;
   }
   void set_type(unsigned char tp)
   {
      type = tp;
   }
private:
   unsigned char type;
   unsigned char buf[3];
   unsigned int  size;
};

enum ParserResult
{
   parser_ok,
   parser_headererror,
   parser_shorterror,
   parser_trackerror,
   parser_subtypeerror,
   parser_openerror
};

class MidiFile
{
public:
   MidiFile();
   ParserResult parse(const char *fn, std::function<void(unsigned int, unsigned char, unsigned char, unsigned char)> notefu);
   unsigned int delta_to_ms(unsigned int dlt);

private:
   static const bool debug;
   
   StatusRunningBuffer stabuf;
   
   // in header
   unsigned long  total_length;
   unsigned short format;
   unsigned short ntracks; // number of tracks
   unsigned short upb;     // units per beat

   // in track
   unsigned short seqnr;
   unsigned char text[1000];
   unsigned char track_name[1000];
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
};

#endif
