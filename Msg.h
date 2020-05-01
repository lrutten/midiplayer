/*******************************************************************************
 * Copyright (c) 2020 Rooi.
 *
 *******************************************************************************/

#ifndef MSG_H
#define MSG_H

#include "mbed.h"
#include "rtos.h"

/*!
   \brief Types for messages
 */
enum MsgType : char
{
   M_AUP     = 'a',
   M_ADN     = 'b',
   M_BUP     = 'c',
   M_BDN     = 'd',
   M_UNB     = 'e',
   M_UP      = 'f',
   M_DOWN    = 'g',
   M_PUSH    = 'h',
   M_REL     = 'i',
   M_COUNT   = 'u',
   M_TEST    = 't',   // only for test
};

class Msg
{
public:
   MsgType type;
   double  arg1;
};

#endif
