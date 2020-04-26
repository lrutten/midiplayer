/*******************************************************************************
 * Copyright (c) 2018 Rediscover.
 *
 *******************************************************************************/

#ifndef AFTERTIME_H
#define AFTERTIME_H

#include "mbed.h"
#include "rtos.h"

#include "Msg.h"
#include "Actor.h"

/*!
   AfterTime can be used by an actor to have a messag sent after
   a specified period of time. The type of the message to be sent is 
   given with the call to sendAfter().
 */
class AfterTime
{
public:
   AfterTime();
   ~AfterTime();
   void sendAfter(Actor *act, int time, MsgType type);
   void timeHandler(MsgType c);
   void doDispatch();

private:
   /*!
      The EventQueue which will call timeHandler() at a later moment.
    */
   EventQueue queue;
   
   /*!
     This threat must call the dispatch() at the queue regularly.
    */
   Thread     th;
   
   /*!
     The client of this class
    */
   Actor     *actor;
};

#endif
