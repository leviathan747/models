/*----------------------------------------------------------------------------
 * File:  Caesar_rot13_bridge.c
 *
 * Description:
 * Methods for bridging to an external entity.
 *
 * External Entity:  rot13 (rot13)
 * 
 * your copyright statement can go here (from te_copyright.body)
 *--------------------------------------------------------------------------*/

#include "webchat_sys_types.h"
#include "ARCH_bridge.h"
#include "CSV_bridge.h"
#include "LOG_bridge.h"
#include "TIM_bridge.h"
#include "STRING_bridge.h"
#include "TRACE_bridge.h"
#include "Caesar_rot13_bridge.h"
#include "Caesar_rot13_bridge.h"
#include "webchat_sys_types.h"

/*
 * Bridge:  rot13
 */
void *
Caesar_rot13_rot13( const void * p_data, const i_t p_size )
{
  c_t * letters = (c_t*)p_data;
  for ( i_t i = 0; i < p_size; i++ ) {
    c_t letter = letters[i];
    if ( letter >= 'A' && letter <= 'Z' ) {
      letters[i] = 'A' + ( ( letter - 'A' + 13 ) % 26 );
    }
    else if ( letter >= 'a' && letter <= 'z' ) {
      letters[i] = 'a' + ( ( letter - 'a' + 13 ) % 26 );
    }
  }
  return letters;
}

