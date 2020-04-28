/*******************************************************************************
 * Copyright (c) 2020 Rooi.
 *
 *******************************************************************************/

/*
   This version of the rotary button decoder uses interrupts for the A and B inputs.
   Unfortunately the approach results in an overrun error on the ISR queue.
 */

#include "RotaryButtonInt.h"
#include "AfterTime.h"

#define ROTARYBUTTONINT_DEBUG 1

#ifdef ROTARYBUTTONINT_DEBUG
#define blog(fmt, args...)    printf(fmt, ## args)
#else
#define blog(fmt, args...)    /* Don't do anything in release builds */
#endif

/*!
   Constructor for Button.
 */
RotaryButtonInt::RotaryButtonInt() : controller(NULL),
   buttonA(PC_0), 
   buttonB(PC_1), 
   pos(0),
   lastpos(0),
   ar(0),
   af(0),
   br(0),
   bf(0)
{
}

void RotaryButtonInt::setController(Controller *cnt)
{
   controller = cnt;
}

void RotaryButtonInt::enableA()
{
   buttonA.rise(mbed::callback(this, &RotaryButtonInt::buttonARise));
   buttonA.fall(mbed::callback(this, &RotaryButtonInt::buttonAFall));
}

void RotaryButtonInt::disableA()
{
   buttonA.rise(NULL);
   buttonA.fall(NULL);
}

void RotaryButtonInt::enableB()
{
   buttonB.rise(mbed::callback(this, &RotaryButtonInt::buttonBRise));
   buttonB.fall(mbed::callback(this, &RotaryButtonInt::buttonBFall));
}

void RotaryButtonInt::disableB()
{
   buttonB.rise(NULL);
   buttonB.fall(NULL);
}


/*!
   At the start a callback is registered for the rising edge
   and the thread is started.
 */
void RotaryButtonInt::start()
{
   blog("RotaryButtonInt::start()\r\n");

   enableA();
   enableB();
   
   Actor::start();
}

/*!
   Callback for the rising edge of the user button input.
   It is not allowed to call printf() as this method will
   be running in ISR mode. Instead of calling printf() a message
   is sent to the RotaryButton mailbox.
 */
void RotaryButtonInt::buttonARise()
{
   //disableA();
   //enableB();
   ar++;

   Msg *msg = alloc();
   if (msg != NULL)
   {
      msg->type = M_AUP;
      put(msg);
   }
}

void RotaryButtonInt::buttonAFall()
{
   //disableA();
   //enableB();
   af++;

   Msg *msg = alloc();
   if (msg != NULL)
   {
      msg->type = M_ADN;
      put(msg);
   }
}

void RotaryButtonInt::buttonBRise()
{
   //disableB();
   //enableA();
   br++;

   Msg *msg = alloc();
   if (msg != NULL)
   {
      msg->type = M_BUP;
      put(msg);
   }
}

void RotaryButtonInt::buttonBFall()
{
   //disableB();
   //enableA();
   bf++;

   Msg *msg = alloc();
   if (msg != NULL)
   {
      msg->type = M_BDN;
      put(msg);
   }
}

/*
int RotaryButton::getCount()
{
   return count;
}
 */

uint8_t RotaryButtonInt::getButtons()
{
   return (buttonA.read() & 0x01) | ((buttonB.read() & 0x01) << 1 );
}

uint8_t RotaryButtonInt::getStableButtons()
{
   const int     time = 1;
   const uint8_t n    = 2;

   //blog("RotaryButton::getStableButtons()\r\n");
   
   uint8_t v = getButtons();
   uint8_t w;
   uint8_t i = 0;
   while (i < n)
   {
      w = getButtons();
      if (w == v)
      {
         i++;
         if (i < n)
         {
            ThisThread::sleep_for(time);
         }
      }
      else
      {
         i = 0;
         v = w;
         ThisThread::sleep_for(time);
      }
   }
   //blog("   stable w %d\r\n", w);
   
   return w;
}

void RotaryButtonInt::send(MsgType t)
{
   const int diff = 2;
   //printf("send %c %d %d\r\n", t, lastpos, pos);

   /*
   bool snd = false;
   if (t == M_UP)
   {
      if (pos > lastpos + diff)
      {
         snd = true;
      }
   }
   else
   if (t == M_DOWN)
   {
      if (pos < lastpos - diff)
      {
         snd = true;
      }
   }
    */
   if (controller != NULL)
   {
      //lastpos = pos;
      controller->send(t);
   }
}

/*!
   This is the RotaryButton thread.
   It will wait for M_COUNT messages.
   It will also receive a M_TEST message after 20 s.
   This is a test for the AfterTime class.
 */
 

void RotaryButtonInt::run()
{
   blog("RotaryButtonInt::run()\r\n");

   AfterTime after;
   //after.sendAfter(this, 2000, M_UNB);

   bool    block = false;
   MsgType t;
   uint8_t v   = getButtons();
   uint8_t w   = v;
   bool    snd = false;
   while (true)
   {
      Msg *m = get();
      t = m->type;
      free(m);

      if (t == M_UNB)
      {
         enableA();
         enableB();
         block = false;
      }
      else
      {
         switch (t)
         {
         case M_AUP:
            w |= 1;
            break;
         case M_ADN:
            w &= ~1;
            break;
         case M_BUP:
            w |= 2;
            break;
         case M_BDN:
            w &= ~2;
            break;
         }

         if (!block)
         {
            w = getButtons();
            if (w != v)
            {
               switch (v)
               {
               case 0:
                  if (w == 1)
                  {
                     snd = true;
                     pos++;
                     send(M_UP);
                  }
                  else
                  if (w == 2)
                  {
                     snd = true;
                     pos--;
                     send(M_DOWN);
                  }
                  break;
               case 1:
                  if (w == 0)
                  {
                     snd = true;
                     pos--;
                     send(M_DOWN);
                  }
                  else
                  if (w == 3)
                  {
                     snd = true;
                     pos++;
                     send(M_UP);
                  }
                  break;
               case 2:
                  if (w == 0)
                  {
                     snd = true;
                     pos++;
                     send(M_UP);
                  }
                  else
                  if (w == 3)
                  {
                     snd = true;
                     pos--;
                     send(M_DOWN);
                  }
                  break;
               case 3:
                  if (w == 1)
                  {
                     snd = true;
                     pos--;
                     send(M_DOWN);
                  }
                  else
                  if (w == 2)
                  {
                     snd = true;
                     pos++;
                     send(M_UP);
                  }
                  break;
               }

               if (snd)
               {
                  snd = false;
                  blog("%c: %d->%d - %3d - %d %d %d %d\r\n", t, v, w, pos, ar, af, br, bf);
                  //ar = 0;
                  //af = 0;
                  //br = 0;
                  //bf = 0;
               }
               v = w;
   
               
               //block = true;
               //after.sendAfter(this, 3, M_UNB);
            }
         }
      }
   }
}
