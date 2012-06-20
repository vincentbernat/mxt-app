//------------------------------------------------------------------------------
/// \file   mxt_app.c
/// \brief  Command line tool for Atmel maXTouch chips.
/// \author Srivalli Ineni & Iiro Valkonen.
//------------------------------------------------------------------------------
// Copyright 2011 Atmel Corporation. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//
//    2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY ATMEL ''AS IS'' AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
// EVENT SHALL ATMEL OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
// OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>

#include "libmaxtouch/libmaxtouch.h"
#include "touch_app.h"
#include "utilfuncs.h"
#include "self_test.h"

//******************************************************************************
/// \brief Load config from file
static void load_config(void)
{
   char cfg_file[255];

   /* Load config file */
   printf("Give cfg file name: ");
   scanf("%255s", cfg_file);
   printf("Trying to open %s...\n", cfg_file);

   if (mxt_load_config_file(cfg_file, false) == 0)
   {
      printf("Successfully uploaded the configuration file\n");
   }
   else
   {
      printf("Failed to upload the configuration\n");
   }
}

//******************************************************************************
/// \brief Save config to file
static void save_config(void)
{
   char cfg_file[255];

   /* Save config file */
   printf("Give cfg file name: ");
   scanf("%255s", cfg_file);

   if (mxt_save_config_file(cfg_file) == 0)
   {
      printf("Successfully saved configuration to file\n");
   }
   else
   {
      printf("Failed to save configuration\n");
   }
}

//******************************************************************************
/// \brief Read objects according to the input value
static void read_object_command(void)
{
   int obj_num;

   printf("Objects on the chip:\n");

   while(1)
   {
      print_objs();

      printf("Enter the object number to read the object's "
             "field values; Enter 255 to return to main menu\n");
      scanf("%d",&obj_num);

      if ((obj_num >= 0) && (obj_num < 255))
      {
         read_object(obj_num);
      }
      else if (obj_num == 255)
      {
         break;
      }
      else
      {
         printf("Please enter a valid object number\n");
         printf("Coming out of objects space...\n");
         break;
      }
   }
}

//******************************************************************************
/// \brief Write objects
static void write_object_command(void)
{
   int obj_num;

   printf("Objects on the chip:\n");
   while(1)
   {
     print_objs();
     printf("Enter the object number to modify the object's "
       "field values; or 255 to return to main menu\n");
     scanf("%d",&obj_num);

     if((obj_num >= 0) && (obj_num < 255))
     {
       write_to_object(obj_num);
     }
     else if(obj_num == 255)
     {
       break;
     }
     else
     {
       printf("Please enter a valid object number\n");
       printf("Coming out of objects space...\n");
       break;
     }
   }
}

//******************************************************************************
/// \brief Handle command
static int mxt_app_command(char selection)
{
   int exit_loop = 0;

   switch(selection)
   {
   case 'l':
      load_config();
      break;
   case 's':
      save_config();
   case 'i':
      /* Print info block */
      printf("Reading info block.....\n");
      print_info_block();
      break;
   case 'd':
      read_object_command();
      break;
    case 'w':
      write_object_command();
      break;
    case 'f':
      /* Run the self-test */
      self_test_handler();
      break;
    case 'b':
      /* Backup the config data */
      if (mxt_backup_config() == 0)
      {
        printf("Settings successfully backed up to non-volatile memory\n");
      }
      else
      {
        printf("Failed to back up settings\n");
      }
      break;
    case 'r':
      /* Reset the chip */
      if (mxt_reset_chip(false) == 0)
      {
        printf("Successfully forced a reset of the device\n");
      }
      else
      {
        printf("Failed to force a reset\n");
      }
      break;
    case 'a':
      /* Calibrate the device*/
      if (mxt_calibrate_chip() == 0)
      {
        printf("Successfully performed a global recalibration on all channels\n");
      }
      else
      {
        printf("Failed to perform a global recalibration\n");
      }
      break;
    case 'e':
      /* Read the events generated */
      event_printer();
      break;
    case 'm':
      /* Display raw messages */
      if (mxt_get_device_type() == E_USB)
      {
        printf("to be implemented...\n\n");
      }
      else
      {
        print_raw_messages();
      }
      break;
    case 'q':
      printf("Quitting the maxtouch application\n");
      exit_loop = 1;
      break;
    default:
      printf("Invalid menu option\n");
      break;
  }

  return exit_loop;
}

//******************************************************************************
/// \brief Menu function for mxt-app
static int mxt_menu(void)
{
   char menu_input;
   int exit_loop;

   printf("Command line tool for Atmel maXTouch chips\n");
   exit_loop = 0;

   while(!exit_loop)
   {
     printf("\nSelect one of the options:\n\n"
       "Enter L:   (L)oad config file\n"
       "Enter S:   (S)ave config file\n"
       "Enter I:   Read (I)nfo block\n"
       "Enter D:   Rea(D) individual object config\n"
       "Enter W:   (W)rite individual object\n"
       "Enter F:   Run sel(F)-test\n"
       "Enter B:   (B)ackup the config data to NVM\n"
       "Enter R:   (R)eset the maxtouch device\n"
       "Enter A:   C(A)librate the maxtouch device\n"
       "Enter E:   Display the input (E)vents from the device\n"
       "Enter M:   Display raw (M)essages\n"
       "Enter Q:   (Q)uit the application\n");

     scanf("%1s", &menu_input);

     exit_loop = mxt_app_command(menu_input);
   }

   return 0;
}

//******************************************************************************
/// \brief Main function for mxt-app
int main (int argc, char *argv[])
{
   int opt;
   int ret;
   bool test = false;

   /*! Handle command line options */
   while ((opt = getopt(argc, argv, "t")) != -1)
   {
      switch (opt)
      {
      case 't':
         test = true;
         break;
      }
   }

   /*! Find an mXT device and read the info block */
   ret = mxt_scan();
   if (ret == 0)
   {
     printf("Unable to find any maXTouch devices - exiting the application\n");
     return -1;
   }
   else if (ret < 0)
   {
     printf("Failed to init device - exiting the application\n");
     return -1;
   }

   if (mxt_get_info() < 0)
   {
     printf("Error reading info block, exiting...\n");
     return -1;
   }

   /*! Turn on kernel dmesg output of MSG */
   mxt_set_debug(true);

   /*! Run tests, otherwise display menu */
   if (test)
   {
      run_self_tests(SELF_TEST_ALL);
   }
   else
   {
      return mxt_menu();
   }

   mxt_set_debug(false);
   mxt_release();

   return 0;
}
