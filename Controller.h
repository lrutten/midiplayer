/*******************************************************************************
 * Copyright (c) 2018 Rediscover.
 *
 *******************************************************************************/

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <vector>
#include <string>

#include <mbed.h>
#include <Serial.h>

#include <stdio.h>
#include <errno.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>

#if COMPONENT_SD
#include "SDBlockDevice.h"
#endif 

#include "FATFileSystem.h"
#include "MidiFile.h"


#include "Actor.h"
//#include "RotaryButton.h"


class DirList
{
public:
   DirList(int whh);
   void        add(char * fn);
   int         size();
   const char *get(int i);
   bool        isCurr(int i);
   const char *getCurr();
   const char *getCurrFull();
   bool        isCurrMidi();
   void        down();
   void        up();
   
private:
   std::vector<std::string> list;
   int                      wh;    // window height
   int                      wpos;  // window position
   int                      curr;  // current file
};

class RotaryButton;

/*!
   \brief controller actor
   
   Controller is the main actor.
 */
class Controller : public Actor
{
public:
   Controller();

   void start();
   void run();
   // void doe();
   void play();
   void displayList();
   void setRotaryButton(RotaryButton *bu);

private:
   SDBlockDevice  *bd;
   FATFileSystem   fs;
   MidiFile        mf;
   Timer           t;
   Serial          midiout;  // Serial4
   Adafruit_SH1106 display;  // OLED display
   DirList         dirlist;
   RotaryButton   *button;
   
   static const uint8_t hline;
   static const uint8_t wchar;
};

#endif
