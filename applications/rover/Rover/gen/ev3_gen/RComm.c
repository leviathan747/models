/*----------------------------------------------------------------------------
 * File:  RComm.c
 *
 * UML Component Port Messages
 * Component/Module Name:  RComm
 *
 * your copyright statement can go here (from te_copyright.body)
 *--------------------------------------------------------------------------*/

#include "Rover_sys_types.h"
#include "RComm.h"
#include "Rover.h"
#include "EV3BAL_bridge.h"
#include "ev3api.h"

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

static FILE * bt;
static int32_t fontw, fonth;

extern int target_speed;

void RComm_send_bt_message(const char * restrict format, ...) {
  // make sure bluetooth is connected
  assert(ev3_bluetooth_is_connected());
  if (NULL == bt) bt = ev3_serial_open_file(EV3_SERIAL_BT);

  // send message
  va_list args;
  va_start(args, format);
  char buf[256]; buf[0] = '\0';
  vsnprintf(buf, 256, format, args);
  va_end(args);
  fprintf(bt, "%s\n", buf);
  fflush(bt);
}

/*
 * Interface:  RComm
 * Provided Port:  RComm
 * To Provider Message:  brake
 */
void
RComm_RComm_brake( const i_t p_power )
{
  //RComm_send_bt_message("Rover,brake(%d)", p_power);
  EV3BAL_run(0, 0);
}

/*
 * Interface:  RComm
 * Provided Port:  RComm
 * To Provider Message:  incrementPower
 */
void
RComm_RComm_incrementPower( const i_t p_power )
{
  //RComm_send_bt_message("Rover,incrementPower(%d)", p_power);
  EV3BAL_run(target_speed + p_power, 0);
}

/*
 * Interface:  RComm
 * Provided Port:  RComm
 * To Provider Message:  pollLeaderDistance
 */
void
RComm_RComm_pollLeaderDistance()
{
  RComm_send_bt_message("Leader,Distance()");
}

/*
 * Interface:  RComm
 * Provided Port:  RComm
 * To Provider Message:  pollLeaderGPS
 */
void
RComm_RComm_pollLeaderGPS()
{
  RComm_send_bt_message("Leader,GPS()");
}

/*
 * Interface:  RComm
 * Provided Port:  RComm
 * To Provider Message:  pollRoverCompass
 */
void
RComm_RComm_pollRoverCompass()
{
  RComm_send_bt_message("Rover,getCompass()");
}

/*
 * Interface:  RComm
 * Provided Port:  RComm
 * To Provider Message:  pollRoverGPS
 */
void
RComm_RComm_pollRoverGPS()
{
  RComm_send_bt_message("Rover,GPS()");
}

/*
 * Interface:  RComm
 * Provided Port:  RComm
 * To Provider Message:  ready
 */
void
RComm_RComm_ready()
{
  RComm_send_bt_message("ready");
}

/*
 * Interface:  RComm
 * Provided Port:  RComm
 * To Provider Message:  setForwardPower
 */
void
RComm_RComm_setForwardPower( const i_t p_power )
{
  //RComm_send_bt_message("Rover,setForwardPower(%d)", p_power);
  EV3BAL_run(p_power, 0);
}

/*
 * Interface:  RComm
 * Provided Port:  RComm
 * To Provider Message:  setLRPower
 */
void
RComm_RComm_setLRPower( const i_t p_lpower, const i_t p_rpower )
{
  //RComm_send_bt_message("Rover,setLRPower(%d,%d)", p_lpower, p_rpower);
  EV3BAL_run((p_lpower + p_rpower) / 2, (p_lpower - p_rpower) / 2);
}

/*
 * Interface:  LocationData
 * Required Port:  Location
 * To Provider Message:  leaderDistance
 */
void
RComm_Location_leaderDistance( const r_t p_dist )
{
  char buf[256];
  sprintf(buf, "Dist: %.2f              ", p_dist);
  ev3_lcd_draw_string(buf, 0, fonth*1);
  Rover_RComm__Location_leaderDistance(p_dist);
}

/*
 * Interface:  LocationData
 * Required Port:  Location
 * To Provider Message:  leaderGPS
 */
void
RComm_Location_leaderGPS( const r_t p_x, const r_t p_z )
{
  char buf[256];
  sprintf(buf, "Lead: %.2f, %.2f              ", p_x, p_z);
  ev3_lcd_draw_string(buf, 0, fonth*2);
  Rover_RComm__Location_leaderGPS(p_x, p_z);
}

/*
 * Interface:  LocationData
 * Required Port:  Location
 * To Provider Message:  roverCompass
 */
void
RComm_Location_roverCompass( const r_t p_degrees )
{
  char buf[256];
  sprintf(buf, "Comp: %.2f              ", p_degrees);
  ev3_lcd_draw_string(buf, 0, fonth*3);
  Rover_RComm__Location_roverCompass(p_degrees);
}

/*
 * Interface:  LocationData
 * Required Port:  Location
 * To Provider Message:  roverGPS
 */
void
RComm_Location_roverGPS( const r_t p_x, const r_t p_z )
{
  char buf[256];
  sprintf(buf, "Rov:  %.2f, %.2f              ", p_x, p_z);
  ev3_lcd_draw_string(buf, 0, fonth*4);
  Rover_RComm__Location_roverGPS(p_x, p_z);
}

void bluetooth_task(intptr_t extinf) {
  static char buf[256];
  if (NULL == bt) bt = ev3_serial_open_file(EV3_SERIAL_BT);
  while (fgets(buf, 256, bt)) {
    // parse response
    char * ptr;
    r_t arg1 = 0.0;
    r_t arg2 = 0.0;
    char * numStart;
    numStart = strstr(buf, "Leader,");
    if (numStart != NULL) {
      numStart = buf + strlen("Leader,");
      arg1 = strtod(numStart, &ptr);
      numStart = ptr + strlen(",");
      if (',' == *ptr) {
        arg2 = strtod(numStart, &ptr);
        RComm_Location_leaderGPS(arg1, arg2);
      }
      else {
        RComm_Location_leaderDistance(arg1);
      }
    }
    else {
      numStart = strstr(buf, "Rover,");
      if (numStart != NULL) {
        numStart = buf + strlen("Rover,");
        arg1 = strtod(numStart, &ptr);
        numStart = ptr + strlen(",");
        if (',' == *ptr) {
          arg2 = strtod(numStart, &ptr);
          RComm_Location_roverGPS(arg1, arg2);
        }
        else {
          RComm_Location_roverCompass(arg1);
        }
      }
    }

  }
  assert(!ev3_bluetooth_is_connected());
}

void RComm_execute_initialization() {
  // setup font
  lcdfont_t font = EV3_FONT_MEDIUM;
  ev3_font_get_size(font, &fontw, &fonth);
  ev3_lcd_set_font(font);

  // initialize balance
  EV3BAL_init(0);
}
