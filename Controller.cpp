/*******************************************************************************
 * Copyright (c) 2020 Rooi.
 *
 *******************************************************************************/


#include "Controller.h"
#include "AfterTime.h"

#include "RotaryButton.h"

#define CONTROLLER_DEBUG 1

#ifdef CONTROLLER_DEBUG
#define blog(fmt, args...)    printf(fmt, ## args)
#else
#define blog(fmt, args...)    /* Don't do anything in release builds */
#endif





DirList::DirList(int whh) : wh(whh), wpos(0), curr(0)
{
}

void DirList::add(char *fn)
{
   list.push_back(fn);
}

int DirList::size()
{
   if (list.size() > wh)
   {
      return wh;
   }
   else
   {
      return list.size();
   }
}

const char *DirList::get(int i)
{
   int j = wpos + i;
   return list[j].c_str();
}

bool DirList::isCurr(int i)
{
   int j = wpos + i;
   return j == curr;
}

const char *DirList::getCurr()
{
   return list[curr].c_str();
}

static char fn[100];

const char *DirList::getCurrFull()
{
   strcpy(fn, "/fs/");
   strcat(fn, list[curr].c_str());
   return fn;
}

bool DirList::isCurrMidi()
{
   const char *p = getCurr();
   printf("p %p\r\n", p);
   printf("s %s\r\n", p);
   
   int le = strlen(p);
   const char *q = p + le;
   if (le > 4)
   {
      q -= 4;
      if (strcmp(q, ".mid") == 0)
      {
         return true;
      }
   }
   return false;
}

void DirList::down()
{
   if (list.size() >= 2 && curr < list.size() - 1)
   {
      curr++;
      if (curr >= wpos + wh)
      {
         wpos++;
      }
   }
}

void DirList::up()
{
   if (curr > 0)
   {
      curr--;
      if (curr < wpos)
      {
         wpos--;
      }
   }
}


const uint8_t Controller::hline = 8;
const uint8_t Controller::wchar = 6;


/*!
   Constructor for Controller.
 */
Controller::Controller() : 
   fs("fs"),
   midiout(PA_0, PA_1),
   dirlist(6)
{
#if COMPONENT_SD
   bd = new SDBlockDevice(
        MBED_CONF_SD_SPI_MOSI,
        MBED_CONF_SD_SPI_MISO,
        MBED_CONF_SD_SPI_CLK,
        MBED_CONF_SD_SPI_CS);
#endif
}

void Controller::setRotaryButton(RotaryButton *bu)
{
   button = bu;
}

/*!
 */
void Controller::start()
{
   blog("Controller::start()\r\n");

   allsoundoff();
   allnotesoff();

   Actor::start();
}

void Controller::displayList()
{
   printf("root directory:\r\n");
   display.clearDisplay();
   display.setTextSize(1);
   
   int r = 0;
   display.setCursor(0, r*hline);
   display.println("/fs");
   r++;
   for (int i=0; i<dirlist.size(); i++)
   {
      // Show file name
      display.setCursor(2*wchar, r*hline);
      display.println(dirlist.get(i));
      
      if (dirlist.isCurr(i))
      {
         display.setCursor(0, r*hline);
         display.println(">");
      }
      printf("    %s\r\n", dirlist.get(i));

      r++;
   }
   display.display();
   
   if (dirlist.isCurrMidi())
   {
      button->setRGB(1, 1, 0); // yellow
   }
   else
   {
      button->setRGB(0, 0, 1); // blue
   }
   //printf("%s\r\n", dirlist.getCurrFull());
}

void Controller::allsoundoff()
{
   midiout.putc(0xb0);
   midiout.putc(120);
   midiout.putc(0);
}

void Controller::allnotesoff()
{
   midiout.putc(0xb0);
   midiout.putc(123);
   midiout.putc(0);
}

void Controller::play()
{
   printf("play\r\n");

   unsigned int notectr = 0;
   
   // start the timer
   t.start();
   const char *fln =  dirlist.getCurrFull();
   ParserResult r =
   mf.parse(fln, [this, &notectr](unsigned int delta, unsigned char type, unsigned char note, unsigned char velo)
   {
      //printf("note ooo %ld, %x, %d\r\n", delta, type, note);
      notectr++;
      
      unsigned int delta_ms = mf.delta_to_ms(delta);
      //printf("dms %d\r\n", delta_ms);
      if (delta_ms > 0)
      {
         rtos::ThisThread::sleep_for(delta_ms); // time in ms
      }

      // Send only Note off, Note on and Controller
      if (type == 0x80 || type == 0x90 || type == 0xb0)
      {
         midiout.putc(type);
         midiout.putc(note);
         midiout.putc(velo);
      }
   });

   if (r != parser_ok)
   {
      // go silent now
      allsoundoff();
      allnotesoff();
   }
   // stop the timer
   t.stop();

   printf("play time %f s\r\n", t.read());
   printf("notectr %u\r\n", notectr);
}

/*!
   This is the Controller thread.
 */
void Controller::run()
{
   blog("Controller::run()\r\n");

   /* 
     Dit werkt.
   
   AfterTime after;
   after.sendAfter(this, 3000, M_TEST);
   
   while (true)
   {
      Msg *m = get();
      blog("Controller::count %c\r\n", m->type);
      free(m);
   }
    */
   // doe();

   printf("--- Mbed OS midi player ---\r\n");
   midiout.baud(31250);

   display.begin(SH1106_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x64)
   display.clearDisplay();

   // Show the welcome text
   display.setTextSize(1);
   display.setTextColor(WHITE);
   display.setCursor(0, 0);
   display.println("Midiplayer 1/ 5/2020 voor Alver");

   display.setTextSize(2);
   display.setCursor(0, 3*hline);
   display.println("Welkom");

   display.display();

   ThisThread::sleep_for(2000);

   // Try to mount the filesystem
   printf("Mounting the filesystem... \r\n");
   fflush(stdout);
   int err = fs.mount(bd);
   printf("%s\r\n", (err ? "Fail :(" : "OK"));
   if (err)
   {
      printf("No filesystem found\r\n");
      fflush(stdout);
   }
    
   // Display the root directory
   printf("Opening the root directory... ");
   fflush(stdout);
   DIR *d = opendir("/fs/");
   printf("%s\r\n", (!d ? "Fail :(" : "OK"));
   if (!d)
   {
      error("error: %s (%d)\r\n", strerror(errno), -errno);
   }

   while (true)
   {
      struct dirent *e = readdir(d);
      if (!e)
      {
         break;
      }

      dirlist.add(e->d_name);
   }

   printf("Closing the root directory... ");
   fflush(stdout);
   err = closedir(d);
   printf("%s\r\n", (err < 0 ? "Fail :(" : "OK"));
   if (err < 0)
   {
      error("error: %s (%d)\r\n", strerror(errno), -errno);
   }

   /*
    only for test scroll
    
   dirlist.add("muz3");
   dirlist.add("muz4");
   dirlist.add("muz5");
   dirlist.add("muz6");
   dirlist.add("muz7");
   dirlist.add("muz8");
    */

   displayList();

   /*
     only for test
     
   for (int i=0; i<8; i++)
   {
      ThisThread::sleep_for(1000);
      dirlist.down();
      displayList();
   }
   
   for (int i=0; i<8; i++)
   {
      ThisThread::sleep_for(1000);
      dirlist.up();
      displayList();
   }
     */
     
   while (true)
   {
      Msg *m = get();
      MsgType t = m->type;
      free(m);
      
      if (t == M_DOWN)
      {
         printf("cdo\r\n");
         dirlist.down();
         displayList();
      }
      else
      if (t == M_UP)
      {
         printf("cup\r\n");
         dirlist.up();
         displayList();
      }
      else
      if (t == M_PUSH)
      {
         printf("push\r\n");

         if (dirlist.isCurrMidi())
         {
            printf("is midi\r\n");
            button->setRGB(0, 1, 0); // green
            play();
            button->setRGB(1, 1, 0); // yellow
         }
      }
      else
      if (t == M_REL)
      {
         printf("release\r\n");

         //button->setRGB(0, 0, 1);
      }
   }


   // Tidy up
   printf("Unmounting... ");
   fflush(stdout);
   err = fs.unmount();
   printf("%s\r\n", (err < 0 ? "Fail :(" : "OK"));
   if (err < 0)
   {
      error("error: %s (%d)\r\n", strerror(-err), err);
   }
        
   printf("Mbed OS midi player done!\r\n");
}

/*
void Controller::doe()
{
   display.begin(SH1106_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x64)
   display.clearDisplay();

   // Show the welcome text
   display.setTextSize(1);
   display.setTextColor(WHITE);
   display.setCursor(0, 0);
   display.println("Midiplayer 25/ 4/2020 voor Alver");

   display.setTextSize(2);
   display.setCursor(0, 3*hline);
   display.println("Welkom");

   display.display();
   
   ThisThread::sleep_for(2000);
   
   printf("--- Mbed OS midi player ---\r\n");
   midiout.baud(31250);
   
   // Try to mount the filesystem
   printf("Mounting the filesystem... \r\n");
   fflush(stdout);
   int err = fs.mount(bd);
   printf("%s\r\n", (err ? "Fail :(" : "OK"));
   if (err)
   {
      printf("No filesystem found\r\n");
      fflush(stdout);
   }
    
   // Display the root directory
   printf("Opening the root directory... ");
   fflush(stdout);
   DIR *d = opendir("/fs/");
   printf("%s\r\n", (!d ? "Fail :(" : "OK"));
   if (!d)
   {
      error("error: %s (%d)\r\n", strerror(errno), -errno);
   }

   printf("root directory:\r\n");
   while (true)
   {
      struct dirent *e = readdir(d);
      if (!e)
      {
         break;
      }

      // Show dir. name
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, 5*hline);
      display.println(e->d_name);
      display.display();
      
      printf("    %s\r\n", e->d_name);
      int le = strlen(e->d_name);
      char *p = e->d_name + le;
      if (le > 4)
      {
         //char *p = e->d_name + le;
         p -= 4;
         if (strcmp(p, ".mid") == 0)
         {
            printf("midi file\r\n");

            unsigned int notectr = 0;
            char fn[100];
            strcpy(fn, "/fs/");
            strcat(fn, e->d_name);
            
            // start the timer
            t.start();

            mf.parse(fn, [this, &notectr](unsigned int delta, unsigned char type, unsigned char note, unsigned char velo)
            {
               printf("note on/off %ld, %x, %d, %d\r\n", delta, type, note, velo);
               notectr++;
               
               unsigned int delta_ms = mf.delta_to_ms(delta);
               printf("delta_ms %d\r\n", delta_ms);
               if (delta_ms > 0)
               {
                  rtos::ThisThread::sleep_for(delta_ms); // time in ms
               }

               midiout.putc(type);
               midiout.putc(note);
               midiout.putc(velo);
            });

            // stop the timer
            t.stop();

            printf("The time taken was %f seconds\r\n", t.read());
            printf("notectr %u\r\n", notectr);
         }
      }
   }

   printf("Closing the root directory... ");
   fflush(stdout);
   err = closedir(d);
   printf("%s\r\n", (err < 0 ? "Fail :(" : "OK"));
   if (err < 0)
   {
      error("error: %s (%d)\r\n", strerror(errno), -errno);
   }

   // Tidy up
   printf("Unmounting... ");
   fflush(stdout);
   err = fs.unmount();
   printf("%s\r\n", (err < 0 ? "Fail :(" : "OK"));
   if (err < 0)
   {
      error("error: %s (%d)\r\n", strerror(-err), err);
   }
        
   printf("Mbed OS midi player done!\r\n");
}
 */


