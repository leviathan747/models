//
// File: __UI__UI__poll.cc
//
// UK Crown Copyright (c) 2018. All Rights Reserved
//
#include "UI_OOA/__UI.hh"
#include "UI_OOA/__UI_interface.hh"
#include "__UI__UI.hh"
#include <stdint.h>
#include "swa/Domain.hh"
#include "swa/Stack.hh"

#include <wiringPi.h>

#define START_STOP_PIN  2
#define LAP_RESET_PIN   0
#define MODE_PIN        1

namespace masld_UI
{
  int32_t maslo_UI::masls_poll ( )
  {
    static bool start_stop_pressed;
    static bool lap_reset_pressed;
    static bool mode_pressed;
    if ( !start_stop_pressed && LOW == digitalRead(START_STOP_PIN) ) {
      start_stop_pressed = true;
      return 1;
    }
    else if ( start_stop_pressed && HIGH == digitalRead(START_STOP_PIN) ) {
      start_stop_pressed = false;
    }
    else if ( !lap_reset_pressed && LOW == digitalRead(LAP_RESET_PIN) ) {
      lap_reset_pressed = true;
      return 3;
    }
    else if ( lap_reset_pressed && HIGH == digitalRead(LAP_RESET_PIN) ) {
      lap_reset_pressed = false;
    }
    else if ( !mode_pressed && LOW == digitalRead(MODE_PIN) ) {
      mode_pressed = true;
      return 5;
    }
    else if ( mode_pressed && HIGH == digitalRead(MODE_PIN) ) {
      mode_pressed = false;
    }
    return 0;
  }

}
