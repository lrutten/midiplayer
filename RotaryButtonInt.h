/*******************************************************************************
 * Copyright (c) 2020 Rooi.
 *
 *******************************************************************************/

#ifndef ROTARYBUTTONINT_H
#define ROTARYBUTTONINT_H

#include "Actor.h"
#include "Controller.h"

/*!
   \brief rotary button actor
   
   RotaryButtonInt is an actor for the rotary button.
 */
class RotaryButtonInt : public Actor
{
public:
   RotaryButtonInt();

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
