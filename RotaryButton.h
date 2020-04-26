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
class RotaryButton : public Actor
{
public:
   RotaryButton();

   void    start();
   void    buttonARise();
   void    buttonAFall();
   void    buttonBRise();
   void    buttonBFall();
   //int     getCount();
   void    run();
   uint8_t getButtons();
   uint8_t getStableButtons();
   void    setController(Controller *cnt);
   void    send(MsgType t);
   void    enableA();
   void    disableA();
   void    enableB();
   void    disableB();

private:
   /*!
      Input for A.
    */
   InterruptIn buttonA;
   /*!
      Input for B.
    */
   InterruptIn buttonB;
   
   /*!
      Counter for the number of rising edges on the user button input.
    */
   int         lastpos;
   int         pos;
   int         ar;
   int         af;
   int         br;
   int         bf;
   
   Controller *controller;
};

#endif
