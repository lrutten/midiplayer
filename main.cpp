/* 
   Midiplayer 28/ 4/2020
 */

#include "mbed.h"


#include "Controller.h"
#include "RotaryButtonPoll.h"

Controller       controller;
RotaryButtonPoll button;

int main()
{
   button.setController(&controller);
   controller.start();
   button.start();

   while (true)
   {
      ThisThread::sleep_for(3000);
   }
}
