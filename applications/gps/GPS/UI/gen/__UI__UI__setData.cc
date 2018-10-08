//
// File: __UI__UI__setData.cc
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

#define CLK 5
#define DIO 6

namespace masld_UI
{
  void maslo_UI::masls_setData ( double  maslp_value,
                                 int32_t maslp_unit )
  {
    char buf[256];
    sprintf(buf, "sudo python3 tm1637.py %d %d displayNumber %d", CLK, DIO, maslp_value);
    system(buf);
  }

}
