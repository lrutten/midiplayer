/*******************************************************************************
 * Copyright (c) 2020 Rooi.
 *
 *******************************************************************************/

#ifndef ROTARYBUTTON_H
#define ROTARYBUTTON_H

#include "Actor.h"
#include "Controller.h"

/*!
   \brief rotary button actor
   
   RotaryButton is the polling actor for the rotary button.
 */
class RotaryButton : public Actor
{
public:
   RotaryButton();

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
