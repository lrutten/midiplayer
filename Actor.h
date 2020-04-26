/*******************************************************************************
 * Copyright (c) 2018 Rediscover.
 *
 *******************************************************************************/

#ifndef ACTOR_H
#define ACTOR_H

#include "mbed.h"
#include "rtos.h"

#include "Msg.h"

/*!
   \brief Base class for actors
   
   An actor is a functional element and is running extactly one thread.
   It can receive messages which are sent by other actor's.
   Arriving messages are stored in a fixed-length queue.
 */
class Actor
{
public:
   Actor();

   void start();
   virtual void run();
   void send(MsgType tpe);
   void send(MsgType tpe, double arg1);
   Msg *alloc();
   void put(Msg *m);
   Msg *get();
   void free(Msg *m);

protected:
   void startRun();

private:
   /*!
      Mailbox for received messages.
      The size is critical: 16 causes a crash at startup
      when too many actors are to started.
    */
   Mail<Msg, 64> mbox;
   
   /*!
      The single thread which consumes the received messages.
    */
   Thread        th;
};

#endif
