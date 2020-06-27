#include <iostream>
#include <functional>

#include "../MidiFile.h"

class Play
{
public:
   Play()
   {
      mf = new MidiFile();
   }

   ~Play()
   {
      delete mf;
   }

   void play(const char *fln)
   {
      int notectr = 0;   
      mf->parse(fln, [this, &notectr](unsigned int delta, unsigned char type, unsigned char note, unsigned char velo)
      {
         printf("---> note ooo %ld, %x, %d\r\n", delta, type, note);
         notectr++;
       
         unsigned int delta_ms = mf->delta_to_ms(delta);
         printf("---> dms %d\r\n", delta_ms);
         if (delta_ms > 0)
         {
            //rtos::ThisThread::sleep_for(delta_ms); // time in ms
         }
      });
   }

private:
   MidiFile *mf;
};

const bool MidiFile::debug = true;  

int main(int argc, char **argv)
{
   std::cout << "testmidi\n";
   
   Play *pl = new Play();
   pl->play(argv[1]);

   delete pl;
}
