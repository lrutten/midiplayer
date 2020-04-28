/*******************************************************************************
 * Copyright (c) 2018 Rediscover.
 *
 *******************************************************************************/

#include "RotaryButtonPoll.h"

#define ROTARYBUTTONPOLL_DEBUG 1
#ifdef ROTARYBUTTONPOLL_DEBUG
#define blog(fmt, args...)    printf(fmt, ## args)
#else
#define blog(fmt, args...)    /* Don't do anything in release builds */
#endif

/*!
   Constructor for Button.
 */
RotaryButtonPoll::RotaryButtonPoll() : controller(NULL),
   buttonA(PC_0), 
   buttonB(PC_1),
   led(LED1),
   count(0),
   stateA(0xffff),
   lastA(1),
   stateB(0xffff),
   lastB(1)
{
}

void RotaryButtonPoll::setController(Controller *cnt)
{
   controller = cnt;
}

/*!
   At the start a callback is registered for the rising edge
   and the thread is started.
 */
void RotaryButtonPoll::start()
{
   blog("RotaryButtonPoll::start()\r\n");

   timeout.attach_us(mbed::callback(this, &RotaryButtonPoll::timeHandler), 5000);
   
   Actor::start();
}

/* 
void RotaryButton::buttonARise()
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
 */

void RotaryButtonPoll::timeHandler()
{
   //led = !led;
   //count++;
   
   // buttonA
   stateA = (stateA << 1) | (buttonA & 1) | 0xfe00;
   if (stateA == 0xff00)
   {
      // rising edge
      led = 1;
      count++;

      Msg *msg = alloc();
      if (msg != NULL)
      {
         msg->type = M_AUP;
         put(msg);
      }
   }
   else
   if (stateA == 0xfeff)
   {
      // falling edge
      led = 0;

      Msg *msg = alloc();
      if (msg != NULL)
      {
         msg->type = M_ADN;
         put(msg);
      }
   }
   
   // buttonB
   stateB = (stateB << 1) | (buttonB & 1) | 0xfe00;
   if (stateB == 0xff00)
   {
      // rising edge
      Msg *msg = alloc();
      if (msg != NULL)
      {
         msg->type = M_BUP;
         put(msg);
      }
   }
   else
   if (stateB == 0xfeff)
   {
      // falling edge
      Msg *msg = alloc();
      if (msg != NULL)
      {
         msg->type = M_BDN;
         put(msg);
      }
   }
}
 
 

uint8_t RotaryButtonPoll::getButtons()
{
   return (buttonA.read() & 0x01) | ((buttonB.read() & 0x01) << 1 );
}


void RotaryButtonPoll::send(MsgType t)
{
   if (controller != NULL)
   {
      controller->send(t);
   }
}

/*!
   This is the RotaryButtonPoll thread.
 */
void RotaryButtonPoll::run()
{
   blog("RotaryButtonPoll::run()\r\n");

   int     pos = 0;
   uint8_t v   = 3;
   uint8_t w   = v;
   MsgType t;
   while (true)
   {
      Msg *m = get();
      t = m->type;
      free(m);

      //blog("%c %d\r\n", t, count); 

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

      switch (v)
      {
      case 0:
         if (w == 1)
         {
            pos++;
            send(M_UP);
         }
         else
         if (w == 2)
         {
            pos--;
            send(M_DOWN);
         }
         break;
      case 1:
         if (w == 0)
         {
            //pos--;
            //send(M_DOWN);
         }
         else
         if (w == 3)
         {
            //pos++;
            //send(M_UP);
         }
         break;
      case 2:
         if (w == 0)
         {
            //pos++;
            //send(M_UP);
         }
         else
         if (w == 3)
         {
            //pos--;
            //send(M_DOWN);
         }
         break;
      case 3:
         if (w == 1)
         {
            //pos--;
            //send(M_DOWN);
         }
         else
         if (w == 2)
         {
            //pos++;
            //send(M_UP);
         }
         break;
      }

      blog("%d %c: %d->%d - %d\r\n", count, t, v, w, pos);
      v = w;
   }
}
