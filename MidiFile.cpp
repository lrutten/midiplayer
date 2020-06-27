#include <stdio.h>
#include <string.h>

#include "MidiFile.h"


MidiFile::MidiFile()
{
}

unsigned short MidiFile::read_2b_integer(FILE *fp)
{
   unsigned char buf[2];
   unsigned char bufb[2];
   int r = fread(buf, 1, 2, fp);
   if (r == 2)
   {
      if (debug) printf("integer in 2 bytes: %x %x\r\n", buf[0], buf[1]);
      bufb[0] = buf[1];
      bufb[1] = buf[0];
      unsigned short *ps = (unsigned short *) bufb;
      if (debug) printf("integer %u\n", *ps);
      return *ps;
   }
   return 0;
}

unsigned long MidiFile::read_3b_integer(FILE *fp)
{
   unsigned char buf[3];
   //unsigned char bufb[4];
   int r = fread(buf, 1, 3, fp);
   if (r == 3)
   {
      if (debug) printf("length in 3 bytes: %x %x %x\r\n", buf[0], buf[1], buf[2]);
      /*
      bufb[0] = buf[2];
      bufb[1] = buf[1];
      bufb[2] = buf[0];
      bufb[3] = 0;
      unsigned long *pl = (unsigned long *) bufb;
       */
      
      // This conversion is architecture safe.
      unsigned long pl = 256*256*buf[0] + 256*buf[1] + buf[2];
      if (debug) printf("length %lu\n", pl);
      return pl;
   }
   return 0;
}

unsigned long MidiFile::read_4b_length(FILE *fp)
{
   char buf[4];
   char bufb[4];
   int r = fread(buf, 1, 4, fp);
   if (r == 4)
   {
      if (debug) printf("length in 4 bytes: %x %x %x %x\r\n", buf[0], buf[1], buf[2], buf[3]);
      bufb[0] = buf[3];
      bufb[1] = buf[2];
      bufb[2] = buf[1];
      bufb[3] = buf[0];
      unsigned long *pl = (unsigned long *) bufb;
      if (debug) printf("length %lu\n", *pl);
      return *pl;
   }
   return 0;
}


unsigned char MidiFile::read_char(FILE *fp)
{
   unsigned char c;

   int r = 0;
   if (stabuf.get_size() > 0)
   {
      c = stabuf.pop();
      r = 1;
   }
   else
   {
      r = fread(&c, 1, 1, fp);
   }
   if (debug) printf("char in %d byte: %x\r\n", r, c);
   return c;
}


unsigned int MidiFile::read_var_len_val(FILE *fp)
{
   unsigned int  value = 0;
   unsigned char c;
   bool          reading  = true;

   fread(&c, 1, 1, fp);
   if (debug) printf("   bytea %x\r\n", c);
   while (reading)
   {
      value = (value << 7) + (c & 0x7f);

      if ((c & 0x80) != 0)
      {
         fread(&c, 1, 1, fp);
         if (debug) printf("   byteb %x\r\n", c);
      }
      else
      {
         reading = false;
      }
   }

   if (debug) printf("read var len value %d\r\n", value);

   return value;
}


unsigned int MidiFile::delta_to_ms(unsigned int dlt)
{
   //printf("tempo upb %d %d\r\n", tempo, upb);
   double tick_ms = ((double) tempo)/((double) upb);
   tick_ms /= 1000.0; // convert µs to ms
   return (unsigned int)(((double) dlt) * tick_ms);
}

int MidiFile::parse(const char *fn, std::function<void(unsigned int, unsigned char, unsigned char, unsigned char)> notefu)
{
   if (debug) printf("start midireader\r\n");

   FILE *fp = fopen(fn, "r");
   if (fp == NULL)
   {
      if (debug) printf("error at fopen\r\n");
   }

   if (debug) printf("fopen ok\r\n");

   char buf[20];
   int r = fread(buf, 1, 4, fp);
   if (r == 4)
   {
      if (debug) printf("fread first 4 bytes ok\r\n");
      buf[4] = '\0';
      if (debug) printf("type %s\r\n", buf);
      
      if (strcmp(buf, "MThd") == 0)
      {
         if (debug) printf("is header\r\n");
         
         
         total_length = read_4b_length(fp);
         format       = read_2b_integer(fp);
         ntracks      = read_2b_integer(fp);
         upb          = read_2b_integer(fp); // units per beat
         
         if (debug) printf("format  %d\r\n", format);
         if (debug) printf("ntracks %d\r\n", ntracks);
         if (debug) printf("upb     %d\r\n", upb);
         
         int r = fread(buf, 1, 4, fp);
         if (r == 4)
         {
            if (debug) printf("fread track 4 bytes ok\r\n");
            buf[4] = '\0';
            if (debug) printf("type %s\r\n", buf);
            
            if (strcmp(buf, "MTrk") == 0)
            {
               if (debug) printf("is track\r\n");

               unsigned long  trlen = read_4b_length(fp);
               if (debug) printf("track length in bytes %ld\r\n", trlen);
               
               unsigned int  delta = read_var_len_val(fp);
               unsigned char type  = read_char(fp);
               
               bool reading = true;
               while (reading)
               {
                  if (debug) printf("\r\n\r\ntype %x\r\n", type);
                  if (type == 0xff)
                  {
                     stabuf.clear();
                     
                     if (debug) printf("meta type\r\n");
                     unsigned char subtype = read_char(fp);
                     if (debug) printf("subtype %x\r\n", subtype);
                     
                     unsigned int  melen;
                     melen = read_var_len_val(fp);
                     if (debug) printf("melen %d\r\n", melen);

                     switch (subtype)
                     {
                        case 0x00:
                           // sequence number
                           if (melen == 2)
                           {
                              // read only if present
                              seqnr = read_2b_integer(fp);
                           }
                           break;
                        case 0x01:
                           // text
                           for (unsigned int i = 0; i<melen; i++)
                           {
                              text[i] = read_char(fp);
                           }
                           text[melen] = '\0';
                           if (debug) printf("text %s\r\n", text);
                           break;
                        case 0x02:
                           // copyright notice
                           for (unsigned int i = 0; i<melen; i++)
                           {
                              read_char(fp);
                           }
                           break;
                        case 0x03:
                           // track name
                           for (unsigned int i = 0; i<melen; i++)
                           {
                              track_name[i] = read_char(fp);
                           }
                           track_name[melen] = '\0';
                           if (debug) printf("track name %s\r\n", track_name);
                           break;
                        case 0x04:
                           // instrument name
                           for (unsigned int i = 0; i<melen; i++)
                           {
                              read_char(fp);
                           }
                           break;
                        case 0x05:
                           // lyric text
                           for (unsigned int i = 0; i<melen; i++)
                           {
                              read_char(fp);
                           }
                           break;
                        case 0x06:
                           // marker text
                           break;
                        case 0x07:
                           // cue point
                           break;
                        case 0x20:
                           // midi channel prefix assignment
                           read_char(fp);
                           break;
                        case 0x21:
                           // midi port
                           read_char(fp);
                           break;
                        case 0x2f:
                           // track end
                           reading = false;
                           break;
                        case 0x51:
                           // tempo
                           tempo = read_3b_integer(fp);
                           if (debug) printf("tempo %lu\r\n", tempo);
                           break;
                        case 0x54:
                           // SMPTE offset
                           break;
                        case 0x58:
                           // time signature
                           if (debug) printf("time signature\r\n");
                           tsig_num = read_char(fp);
                           tsig_den = read_char(fp);
                           mtro_tic = read_char(fp); // number of clock ticks per metronome
                           not_32nd = read_char(fp);
                           if (debug) printf("time signature %d %d %d %d\r\n",
                              tsig_num, tsig_den, mtro_tic, not_32nd);

                           //reading = false;
                           break;
                        case 0x59:
                           // key signature
                           sf = read_char(fp);
                           mi = read_char(fp);
                           break;
                     }
                  }
                  else
                  if ((type & 0xf0) == 0x80 || (type & 0xf0) == 0x90)
                  {
                     // note on/off
                     unsigned char note = read_char(fp);
                     unsigned char velo = read_char(fp);
                     if (debug) printf("note on/off %d %d\r\n", note, velo);
                     notefu(delta, type, note, velo);
                     
                     stabuf.clear_set(type);
                  }
                  else
                  if ((type & 0xf0) == 0xa0)
                  {
                     // polyphonic pressure
                     read_char(fp);
                     read_char(fp);

                     stabuf.clear_set(type);
                  }
                  else
                  if ((type & 0xf0) == 0xb0)
                  {
                     // controller
                     unsigned char note = read_char(fp);
                     unsigned char velo = read_char(fp);
                     if (debug) printf("controller %d %d\r\n", note, velo);
                     notefu(delta, type, note, velo);

                     stabuf.clear_set(type);
                  }
                  else
                  if ((type & 0xf0) == 0xc0)
                  {
                     // program change
                     unsigned char cha = read_char(fp);
                     if (debug) printf("program change %x\r\n", cha);

                     stabuf.clear_set(type);
                  }
                  else
                  if ((type & 0xf0) == 0xd0)
                  {
                     // channel pressure
                     read_char(fp);

                     stabuf.clear_set(type);
                  }
                  else
                  if ((type & 0xf0) == 0xe0)
                  {
                     // pitch bend
                     read_char(fp);
                     read_char(fp);

                     stabuf.clear_set(type);
                  }
                  else
                  {
                     // This is the Running Status situation.
                     // Keep the type as the 1st char.
                     stabuf.push(type);
                     
                     if (debug) printf("reuse type\r\n");
                     //reading = false;
                  }

                  if (reading)
                  {
                     if (stabuf.get_size() == 0)
                     {
                        // Normal situation
                        delta = read_var_len_val(fp);
                        if (debug) printf("next delta %u\r\n", delta);
                        type  = read_char(fp);
                        if (debug) printf("next type %x\r\n", type);
                     }
                     else
                     {
                        // Status Running situation
                        
                        // delta is not changed.

                        // Take the previous type.
                        type = stabuf.get_type();
                     }
                  }
               }
            }
         }
         else
         {
            if (debug) printf("wrong track in first 4 bytes\r\n");
         }
      }
      else
      {
         printf("wrong header\n");
         return 0;
      }
   }   
   fclose(fp);
   return 0;
}
