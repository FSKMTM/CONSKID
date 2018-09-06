
#ifndef supervisor_H_
#define supervisor_H_

/*
 * used for :
 *  enabling separate modules based on connected hardware.
 *  enabling debu prints of individual block of code
 */

// -- ENABLE -------------------------------------------------------------------
#define ENABLE_GNSS         1
#define ENABLE_IMU          1
#define ENABLE_REQUESTS     0
#define ENABLE_SAVE         1
#define ENABLE_GPIO         1
#define ENABLE_LBO          0
#define ENABLE_BSS	    0
#define ENABLE_GYRO	    0
#define ENABLE_TIME_FROM_GNSS 1

/* ENABLE_IMU_SIGN_STOP: 1 - stops measurement of imu when change
 * in accel sign or button pressed, 0 - stops when button pressed
 */
#define ENABLE_IMU_SIGN_STOP 1

// -- DEBUG --------------------------------------------------------------------
#define DEBUG_ALL           0
#define TIMING_ALL			0
// main
#define DEBUG_MAIN          DEBUG_ALL || 1
// rtos
#define DEBUG_RTOS          DEBUG_ALL || 0
#define TIMING_RTOS			TIMING_ALL || 0 
// tasks
#define DEBUG_TASKS			DEBUG_ALL || 0
#define TIMING_TASKS		TIMING_ALL || 0
// gnss
#define DEBUG_GNSS          DEBUG_ALL || 0
#define DEBUG_GNSS_DATA     DEBUG_ALL || 0
// i2c
#define DEBUG_I2C           DEBUG_ALL || 0
#define TIMING_I2C			TIMINIG_ALL || 0
// imu
#define DEBUG_IMU           DEBUG_ALL || 0 
#define TIMING_MPU			TIMINIG_ALL || 0
// requests
#define DEBUG_REQUESTS      DEBUG_ALL || 0
// gpio
#define DEBUG_GPIO          DEBUG_ALL || 0
// battery
#define DEBUG_LBO           DEBUG_ALL || 0
// button
#define DEBUG_BTN           DEBUG_ALL || 1
// led
#define DEBUG_LED           DEBUG_ALL || 0
// data save
#define DEBUG_SAVE          DEBUG_ALL || 0
// time synch from gnss
#define DEBUG_TIME_SYNCH    DEBUG_ALL || 1
// mpu
#define DEBUG_MPU	    DEBUG_ALL || 0
#endif
