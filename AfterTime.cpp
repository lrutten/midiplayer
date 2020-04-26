/*******************************************************************************
 * Copyright (c) 2018 Rediscover.
 *
 *******************************************************************************/

#include "AfterTime.h"
#include "Msg.h"

//#define AFTERTIME_DEBUG 1

#ifdef AFTERTIME_DEBUG
#define atlog(fmt, args...)    printf(fmt, ## args)
#else
#define atlog(fmt, args...)    /* Don't do anything in release builds */
#endif

/*!
   Constructor for afterTime
 */
AfterTime::AfterTime()
{
   atlog("AfterTime::AfterTime()\r\n");
   th.start(mbed::callback(this, &AfterTime::doDispatch));
}

/*!
   Destructor for afterTime, terminates the running dispatch thread.
 */
AfterTime::~AfterTime()
{
   atlog("AfterTime::~AfterTime()\r\n");
   th.terminate();
}

/*!
   This method is called by the client actor to start 
   the timer.
 */
void AfterTime::sendAfter(Actor *act, int time, MsgType type)
{
   atlog("AfterTime::sendAfter() %d %c\r\n", time, type);

   actor = act;
   // ok
   //queue.call_in(time, callback(this, &AfterTime::timeHandler));

   // niet ok
   //queue.call_in(time, callback(this, &AfterTime::timeHandler), 5);
   
   queue.call_in(time, mbed::Callback<void(MsgType)>(this, &AfterTime::timeHandler), type);
}

/*!
   This callback will be called at timeout.
   The chosen message is sent to the client actor.
   This method is called within a ISR context, so printf() is forbidden.
 */
void AfterTime::timeHandler(MsgType c)
{
   // ISR context
   //atlog("After::timeHandler()\r\n");
   Msg *msg = actor->alloc();
   if (msg != NULL)
   {
      msg->type = c;
      actor->put(msg);
   }
}

/*!
   The dispatch thread for the queue.
 */
void AfterTime::doDispatch()
{
   queue.dispatch();
}

