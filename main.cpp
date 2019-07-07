/* 
   Midiplayer 7/ 7/2019
 */

#include "mbed.h"

DigitalOut led1(LED1);


int main()
{
   int  count = 0;

   while (true)
   {
      led1 = !led1;
      wait_ms(500);
   }
}
