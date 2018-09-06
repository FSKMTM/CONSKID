#ifndef _GPIO_H_
#define _GPIO_H_

#include <wiringPi.h> //from wiringpi.com
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define G_LED_PIN 1
#define R_LED_PIN 4
#define BUTTON_PIN 0
#define STOP_BUTTON_PIN 3
#define LOW_BATTERY_OUTPUT_PIN 2
#define SAFE_SHUTDOWN_PIN 5

#define HEARTBEAT_ON_TIME     0.1
#define HEARTBEAT_OFF_TIME    13*HEARTBEAT_ON_TIME
#define HEARTBEAT_CYCLES      (2*HEARTBEAT_ON_TIME + HEARTBEAT_OFF_TIME) / HEARTBEAT_ON_TIME

#define HEARTBEAT_ON_TIME_FIX     2.0
#define HEARTBEAT_OFF_TIME_FIX    1*HEARTBEAT_ON_TIME_FIX
#define HEARTBEAT_CYCLES_FIX      (HEARTBEAT_ON_TIME_FIX + HEARTBEAT_OFF_TIME_FIX) / HEARTBEAT_ON_TIME_FIX

#define R_LED_ON_TIME          1     // -- seconds

#define LED_OFF         0x00
#define G_LED_ON        0x01
#define G_LED_HEARTBEAT 0x02
#define R_LED_ON        0x04
#define R_LED_HEARTBEAT 0x08

extern uint8_t button_pressed_flag;	//flag for interrupt status
extern uint8_t led_status;

/* interrupt service routine for button press
 *  only sets button_pressed_flag
 *  and other time critical things
 */
void button_isr();

extern uint8_t stop_button_pressed_flag;	//flag for interrupt status

/* interrupt service routine for button press to stop measurement
 *  only sets stop_button_pressed_flag if there was already
 *  a button press for starting measurement
 */
void stop_button_isr();

/* interrupt service routine for low battery output
 * calls system function shutdown 0
 */
void lbo_isr();

/* initialize low battery output interrupt
 */
void init_lbo_isr();

/* interrupt service routine for safe shutdown button
 * calls system function shutdown 0
 */
void bss_isr();

/* initialize safe shutdown button
 */
void init_bss_isr();

/* initialize gpio pins
 *  for output led
 *  and input button -> interrupt: button_isr
 */
void init_gpio();

/* for turning on led
 */
void turn_led_on(uint8_t led_pin);

/* for turning off led
 */
void turn_led_off(uint8_t led_pin);

/* toggle led state
 */
void toggle_led();

/* toggle led
 */
void led_heartbeat(uint8_t led_pin);

/* toggle led
 */
void led_heartbeat_searching_for_fix(uint8_t led_pin);

// -- turn red led on continuously for given amount of time
void red_led_on (void);

// -- turn both led's off
void rg_led_off_status(void);// -- turn green led on, turn green hartbeat off
void g_led_on_status(void);
// -- turn green led off
void g_led_off_status(void);
// -- turn green hartbeat on
void g_led_hbt_status(void);
// -- turn red led on, turn red hartbeat off
void r_led_on_status(void);
// -- turn red led off
void r_led_off_status(void);
// -- turn red hartbeat on
void r_led_hbt_status(void);

#endif /*_GPIO_H_*/
