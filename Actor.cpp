/*******************************************************************************
 * Copyright (c) 2018 Rediscover.
 *
 *******************************************************************************/

#include "Actor.h"

// #define ACTOR_DEBUG 1


#ifdef ACTOR_DEBUG
#define alog(fmt, args...)    printf(fmt, ## args)
#else
#define alog(fmt, args...)    /* Don't do anything in release builds */
#endif

/*!
   Constructor for `Actor`
 */
Actor::Actor()
{
}

/*!
   Every actor must be started.
   The only action to be executed is starting the thread.
 */
void Actor::start()
{
   alog("Actor::start()\r\n");
   th.start(mbed::callback(this, &Actor::startRun));
}

/*!
   This is the default thread method.
   This method does nothing and has to be redefined
   in the subclasses.
 */
void Actor::run()
{
   alog("Actor::run()\r\n");
   while (true)
   {
      ThisThread::sleep_for(2000);
   }
}

/*!
  Send a message without parameter to this actor.
  
  \param `tpe` type of the message
 */
void Actor::send(MsgType tpe)
{
   Msg *msg = alloc();
   if (msg != NULL)
   {
      msg->type = tpe;
      alog("Actor::send() %c\r\n", msg->type);
      put(msg);
   }
}


/*!
  Send a message with a `double` parameter to this actor.

  \param `tpe` type of the message
  \param `par1` first paremeter
 */
void Actor::send(MsgType tpe, double par1)
{
   Msg *msg = alloc();
   if (msg != NULL)
   {
      msg->type = tpe;
      msg->arg1 = par1;
      alog("Actor::send() %c %lf\r\n", msg->type, msg->arg1);
      put(msg);
   }
}

/*!
   Allocate a message in the `Mail` mailbox.
 */
Msg *Actor::alloc()
{
   Msg *m = mbox.alloc();
   /*
   if (m == NULL)
   {
      alog("m null\r\n");
   }
    */
   return m;
}

/*!
   Add a message to the mailbox.
 */
void Actor::put(Msg *m)
{
   mbox.put(m);
}

/*
osEvent Actor::get()
{
   return mbox.get();
}
 */

/*
osEvent Actor::get()
{
   while (true)
   {
      osEvent ev = mbox.get();
      if (ev.status == osEventMail)
      {
         return ev;
      }
   }
}
 */

/*!
  Wait for a message to arrive in the mailbox.
 */
Msg *Actor::get()
{
   while (true)
   {
      osEvent ev = mbox.get();
      if (ev.status == osEventMail)
      {
         return (Msg *)ev.value.p;
      }
   }
}

/*!
  Release a message.
 */
void Actor::free(Msg *m)
{
   mbox.free(m);
}

/*!
   This is a small helper method which is started as the
   thread function. When started, it will start the virtual `run()` method.
 */
void Actor::startRun()
{
   alog("Actor::startRun()\r\n");
   run();
}

