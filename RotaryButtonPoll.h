/*******************************************************************************
 * Copyright (c) 2018 Rediscover.
 *
 *******************************************************************************/

#ifndef ROTARYBUTTON_H
#define ROTARYBUTTON_H

#include "Actor.h"
#include "Controller.h"

/*!
   \brief rotary button actor
   
   RotaryButton is an actor for testing the user button.
 */
class RotaryButtonPoll : public Actor
{
public:
   RotaryButtonPoll();

   void    start();
   void    run();
   uint8_t getButtons();
   uint8_t getStableButtons();
   void    setController(Controller *cnt);
   void    send(MsgType t);
   void    timeHandler();

private:
   /*!
      Input for A.
    */
   DigitalIn  buttonA;
   /*!
      Input for B.
    */
   DigitalIn  buttonB;
   DigitalOut led;
   Ticker     timeout;
   int        count;
   uint16_t   stateA;
   uint8_t    lastA;
   uint16_t   stateB;
   uint8_t    lastB;
   Controller *controller;
};

#endif
