/*----------------------------------------------------------------------------
 * Description:   Methods for bridging to an external entity.
 *
 * External Entity:  Graphical_User_Interface (GuiBridge)
 * 
 * your copyright statement can go here (from te_copyright.body)
 *--------------------------------------------------------------------------*/

#include "GPS_Watch_sys_types.h"
#include "GuiBridge_bridge.h"
#include "TIM_bridge.h"

/*
 * Bridge:  setData
 */
void
GuiBridge_setData( const i_t p_unit, const r_t p_value )
{
  /* Replace/Insert your implementation code here... */
}


/*
 * Bridge:  setTime
 */
void
GuiBridge_setTime( const i_t p_time )
{
  if ( p_time & 0xfff8 ) digitalWrite(7, HIGH);
  else digitalWrite(7, LOW);
  if ( p_time & 0xfff4 ) digitalWrite(8, HIGH);
  else digitalWrite(8, LOW);
  if ( p_time & 0xfff2 ) digitalWrite(9, HIGH);
  else digitalWrite(9, LOW);
  if ( p_time & 0xfff1 ) digitalWrite(10, HIGH);
  else digitalWrite(10, LOW);
}


/*
 * Bridge:  connect
 */
i_t
GuiBridge_connect()
{
  i_t result = 0;
  /* Insert your implementation code here... */
  return result;
}


/*
 * Bridge:  setIndicator
 */
void
GuiBridge_setIndicator( const i_t p_value )
{
  /* Replace/Insert your implementation code here... */
}


/*
 * Bridge:  poll
 */
i_t
GuiBridge_poll()
{
  i_t result = 0;
  /* Insert your implementation code here... */
  return result;
}

