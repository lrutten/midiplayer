/* 
   Midiplayer 21/ 8/2019
 */

#include "mbed.h"

#include <stdio.h>
#include <errno.h>

#if COMPONENT_SD
#include "SDBlockDevice.h"
#endif 

#include "MidiFile.h"

/*
DigitalOut led1(LED1);


int main()
{
   int  count = 0;

   while (true)
   {
      led1 = !led1;
      wait_ms(500);
   }
}
 */



// This will take the system's default block device
//BlockDevice *bd = BlockDevice::get_default_instance();

#if COMPONENT_SD
SDBlockDevice *bd = new SDBlockDevice(
        MBED_CONF_SD_SPI_MOSI,
        MBED_CONF_SD_SPI_MISO,
        MBED_CONF_SD_SPI_CLK,
        MBED_CONF_SD_SPI_CS);
#endif

// Instead of the default block device, you can define your own block device.
// For example: HeapBlockDevice with size of 2048 bytes, read size 1, write size 1 and erase size 512.
// #include "HeapBlockDevice.h"
// BlockDevice *bd = new HeapBlockDevice(2048, 1, 1, 512);


// This example uses LittleFileSystem as the default file system
//#include "LittleFileSystem.h"
//LittleFileSystem fs("fs");

// Uncomment the following two lines and comment the previous two to use FAT file system.
#include "FATFileSystem.h"
FATFileSystem fs("fs");

MidiFile mf;
Timer    t;


// Entry point for the example
int main()
{
   printf("--- Mbed OS midi player ---\r\n");

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

      printf("    %s\r\n", e->d_name);
      int le = strlen(e->d_name);
      char *p = e->d_name + le;
      if (le > 4)
      {
         char *p = e->d_name + le;
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

            mf.parse(fn, [&notectr]()
            {
               //printf("note on/off");
               notectr++;
               rtos::ThisThread::sleep_for(20); // time in ms
            });

            // stop the timer
            t.stop();
            printf("The time taken was %f seconds\n", t.read());
            
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



