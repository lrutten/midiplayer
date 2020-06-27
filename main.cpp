/* 
   Midiplayer 28/ 4/2020
 */

#include "mbed.h"


#include "Controller.h"
#include "RotaryButton.h"

const bool MidiFile::debug = false;

Controller   controller;
RotaryButton button;

int main()
{
   button.setController(&controller);
   controller.setRotaryButton(&button);
   
   controller.start();
   button.start();

   while (true)
   {
      ThisThread::sleep_for(3000);
   }
}
