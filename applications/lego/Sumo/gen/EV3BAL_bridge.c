/*----------------------------------------------------------------------------
 * Description:   Methods for bridging to an external entity.
 *
 * External Entity:  EV3Balancer (EV3BAL)
 * 
 * your copyright statement can go here (from te_copyright.body)
 *--------------------------------------------------------------------------*/

#include "Sumo_sys_types.h"
#include "EV3BAL_bridge.h"
#include "EV3GYR_bridge.h"
#include "EV3M_bridge.h"
#include "LOG_bridge.h"

/**
 * This sample program balances a two-wheeled Segway type robot such as Gyroboy in EV3 core set.
 *
 * References:
 * http://www.hitechnic.com/blog/gyro-sensor/htway/
 * http://www.cs.bgu.ac.il/~ami/teaching/Lejos-2013/classes/src/lejos/robotics/navigation/Segoway.java
 */

#include "ev3api.h"
#include "app.h"

#define DEBUG

#ifdef DEBUG
#define _debug(x) (x)
#else
#define _debug(x)
#endif

/**
 * Constants for the self-balance control algorithm.
 */
const float KSTEER=-0.25;
const float EMAOFFSET = 0.0005f, KGYROANGLE = 7.5f, KGYROSPEED = 1.15f, KPOS = 0.07f, KSPEED = 0.1f, KDRIVE = -0.02f;
const float WHEEL_DIAMETER = 5.6;
const uint32_t WAIT_TIME_MS = 1;
const uint32_t FALL_TIME_MS = 1000;
const float INIT_INTERVAL_TIME = 0.014;
const int MAX_ACCEL = 8;

/**
 * Global variables used by the self-balance control algorithm.
 */
static int motor_diff;
static int loop_count, motor_control_drive, motor_control_steer, target_speed;
static float gyro_offset, gyro_speed, gyro_angle, interval_time;
static float motor_pos, motor_speed, motor_diff_target;

/**
 * Calculate the initial gyro offset for calibration.
 */
static ER calibrate_gyro_sensor() {
  int gMn = 1000, gMx = -100, gSum = 0;
  for (int i = 0; i < 200; ++i) {
    int gyro = EV3GYR_get_rate();
    gSum += gyro;
    if (gyro > gMx)
      gMx = gyro;
    if (gyro < gMn)
      gMn = gyro;
    tslp_tsk(4);
  }
  if(!(gMx - gMn < 2)) {
    gyro_offset = gSum / 200.0f;
    return E_OK;
  } else {
    return E_OBJ;
  }
}

/**
 * Calculate the average interval time of the main loop for the self-balance control algorithm.
 * Units: seconds
 */
static void update_interval_time() {
  static SYSTIM start_time;

  if(loop_count++ == 0) { // Interval time for the first time (use 6ms as a magic number)
    interval_time = INIT_INTERVAL_TIME;
    ER ercd = get_tim(&start_time);
    assert(ercd == E_OK);
  } else {
    SYSTIM now;
    ER ercd = get_tim(&now);
    assert(ercd == E_OK);
    interval_time = ((float)(now - start_time)) / loop_count / 1000;
  }
}

/**
 * Update data of the gyro sensor.
 * gyro_offset: the offset for calibration.
 * gyro_speed: the speed of the gyro sensor after calibration.
 * gyro_angle: the angle of the robot.
 */
static void update_gyro_data() {
  int gyro = EV3GYR_get_rate();
  gyro_offset = EMAOFFSET * gyro + (1 - EMAOFFSET) * gyro_offset;
  gyro_speed = gyro - gyro_offset;
  gyro_angle += gyro_speed * interval_time;
}

/**
 * Update data of the motors
 */
static void update_motor_data() {
  static int32_t prev_motor_cnt_sum, motor_cnt_deltas[4];

  if(loop_count == 1) { // Reset
    motor_pos = 0;
    prev_motor_cnt_sum = 0;
    motor_cnt_deltas[0] = motor_cnt_deltas[1] = motor_cnt_deltas[2] = motor_cnt_deltas[3] = 0;
  }

  int32_t left_cnt = EV3M_get_count(DEV_MOTOR_LEFT);
  int32_t right_cnt = EV3M_get_count(DEV_MOTOR_RIGHT);
  int32_t motor_cnt_sum = left_cnt + right_cnt;
  motor_diff = right_cnt - left_cnt;
  int32_t motor_cnt_delta = motor_cnt_sum - prev_motor_cnt_sum;

  prev_motor_cnt_sum = motor_cnt_sum;
  motor_pos += motor_cnt_delta;
  motor_cnt_deltas[loop_count % 4] = motor_cnt_delta;
  motor_speed = (motor_cnt_deltas[0] + motor_cnt_deltas[1] + motor_cnt_deltas[2] + motor_cnt_deltas[3]) / 4.0f / interval_time;
}

/**
 * Control the power to keep balance.
 * Return false when the robot has fallen.
 */
static bool_t keep_balance() {
  static SYSTIM ok_time;

  if(loop_count == 1) // Reset ok_time
    get_tim(&ok_time);

  float ratio_wheel = WHEEL_DIAMETER / 5.6;

  // Apply the drive control value to the motor position to get robot to move.
  motor_pos -= motor_control_drive * interval_time;

  // This is the main balancing equation
  int power = (int)((KGYROSPEED * gyro_speed +               // Deg/Sec from Gyro sensor
                     KGYROANGLE * gyro_angle) / ratio_wheel + // Deg from integral of gyro
                     KPOS       * motor_pos +                // From MotorRotaionCount of both motors
                     KDRIVE     * motor_control_drive +       // To improve start/stop performance
                     KSPEED     * motor_speed);              // Motor speed in Deg/Sec

  // Check fallen
  SYSTIM time;
  get_tim(&time);
  if(power > -100 && power < 100)
    ok_time = time;
  else if(time - ok_time >= FALL_TIME_MS)
    return false;

  // Steering control
  motor_diff_target += motor_control_steer * interval_time;

  int left_power, right_power;

  int power_steer = (int)(KSTEER * (motor_diff_target - motor_diff));
  left_power = power + power_steer;
  right_power = power - power_steer;
  if(left_power > 100)
    left_power = 100;
  if(left_power < -100)
    left_power = -100;
  if(right_power > 100)
    right_power = 100;
  if(right_power < -100)
    right_power = -100;

  EV3M_set_power(DEV_MOTOR_LEFT, (int)left_power);
  EV3M_set_power(DEV_MOTOR_RIGHT, (int)right_power);

  return true;
}

void balance_task(intptr_t unused) {
  ER ercd;

  /**
   * Reset
   */
  loop_count = 0;
  motor_control_drive = 0;
  motor_control_steer = 0;
  target_speed = 0;
  EV3M_reset_count(DEV_MOTOR_LEFT);
  EV3M_reset_count(DEV_MOTOR_RIGHT);
  EV3GYR_reset();

  /**
   * Calibrate the gyro sensor and set the led to green if succeeded.
   */
  _debug(LOG_LogInfo("Start calibration of the gyro sensor."));
  for(int i = 10; i > 0; --i) { // Max retries: 10 times.
    ercd = calibrate_gyro_sensor();
    if(ercd == E_OK) break;
    if(i != 1)
      LOG_LogFailure("Calibration failed, retry.");
    else {
      LOG_LogFailure("Max retries for calibration exceeded, exit.");
      return;
    }
  }
  char buf[256]; buf[0] = '\0';
  sprintf(buf, "Calibration succeed, offset is %de-3.", (int)(gyro_offset * 1000));
  _debug(LOG_LogInfo(buf));

  /**
   * Main loop for the self-balance control algorithm
   */
  while(1) {
    // Update speed
    if (motor_control_drive < target_speed) {
      if (motor_control_drive + MAX_ACCEL > target_speed)
        motor_control_drive = target_speed;
      else
        motor_control_drive += MAX_ACCEL;
    }
    if (motor_control_drive > target_speed) {
      if (motor_control_drive - MAX_ACCEL < target_speed)
        motor_control_drive = target_speed;
      else
        motor_control_drive -= MAX_ACCEL;
    }

    // Update the interval time
    update_interval_time();

    // Update data of the gyro sensor
    update_gyro_data();

    // Update data of the motors
    update_motor_data();

    // Keep balance
    if(!keep_balance()) {
      EV3M_stop(false, DEV_MOTOR_LEFT);
      EV3M_stop(false, DEV_MOTOR_RIGHT);
      LOG_LogInfo("Knock out!");
      return;
    }

    tslp_tsk(WAIT_TIME_MS);
  }
}

/*
 * Bridge:  init
 */
void
EV3BAL_init( const i_t p_gyro_offset )
{
  gyro_angle = p_gyro_offset;
  act_tsk(BALANCE_TASK);
}


/*
 * Bridge:  run
 */
void
EV3BAL_run( const i_t p_power, const i_t p_turn )
{
  target_speed = p_power * 10;         // +- 1300
  motor_control_steer = p_turn * 5;    // +- 500
}

