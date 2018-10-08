//
// File: __UI__UI__setTime.cc
//
// UK Crown Copyright (c) 2018. All Rights Reserved
//
#include "UI_OOA/__UI.hh"
#include "UI_OOA/__UI_interface.hh"
#include "__UI__UI.hh"
#include <stdint.h>
#include "swa/Domain.hh"
#include "swa/Stack.hh"

extern "C" {
#include <stdlib.h>
#include <stdio.h>
}

#define CLK 23
#define DIO 24

namespace masld_UI
{
  void maslo_UI::masls_setTime ( int32_t maslp_time )
  {
    char buf[256];
    sprintf(buf, "sudo python3 tm1637.py %d %d displayTime %d", CLK, DIO, maslp_time);
    system(buf);
  }

}
