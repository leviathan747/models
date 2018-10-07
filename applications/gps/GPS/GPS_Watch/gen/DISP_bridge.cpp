/*----------------------------------------------------------------------------
 * Description:   Methods for bridging to an external entity.
 *
 * External Entity:  Display (DISP)
 *
 * your copyright statement can go here (from te_copyright.body)
 *--------------------------------------------------------------------------*/

#include "GPS_Watch_sys_types.h"
#include "DISP_bridge.h"

#include <TM1637Display.h>

#define CLK1 8
#define DIO1 7
#define CLK2 10
#define DIO2 9

TM1637Display display1(CLK1, DIO1);
TM1637Display display2(CLK2, DIO2);


/*
 * Bridge:  showNumber
 */
void
DISP_showNumber( const i_t p_num )
{
  display2.setBrightness(0x0f);
  display2.showNumberDec(p_num);
}


/*
 * Bridge:  showTime
 */
void
DISP_showTime( const i_t p_time )
{
  i_t time = p_time >= 60 ? ((p_time / 60) * 100) + (p_time % 60) : p_time;
  display1.setBrightness(0x0f);
  display1.showNumberDecEx(time, 0b01000000, true);
}

