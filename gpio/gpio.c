#include "gpio.h"
#include "supervisor.h"

#include <wiringPi.h>

#include <time.h>


uint8_t button_pressed_flag;	//flag for interrupt status
uint8_t led_status;

/* interrupt service routine for button press
 *  only sets button_pressed_flag
 *  and other time critical things
 */
void button_isr(){
	//so that same multiple interrupts don't cause multiple calls - against button debouncing
	if(button_pressed_flag==0){
		button_pressed_flag=1;
		#if (DEBUG_PRINTS==1)
		printf("New interrupt for starting measurements.\n");
		#endif
	}
}

uint8_t stop_button_pressed_flag; //flag for interrupt status

/* interrupt service routine for  button press for stopping measurements
 *  only sets stop_button_pressed_flag if there was already a button press to start measurement
 *  and other time critical things
 */
void stop_button_isr(){
	//so that same multiple interrupts don't cause multiple calls - against button debouncing
	if(stop_button_pressed_flag==0){
		if(button_pressed_flag==1){
			stop_button_pressed_flag=1;
		}
		#if (DEBUG_PRINTS==1)
		printf("New interrupt on button stop.\n");
		#endif
	}
}

/* interrupt service routine for low battery output
 * calls system function shutdown 0
 */
void lbo_isr(){
	static uint8_t lbo = 0;
	if(lbo == 0){
		printf("System shutdown due to low battery.\n");
		system("sudo shutdown 0");
		lbo = 1;
	}
}

/* initialize low battery output interrupt
 */
void init_lbo_isr(){
	wiringPiSetup();
	wiringPiISR(LOW_BATTERY_OUTPUT_PIN, INT_EDGE_RISING, lbo_isr);
}

/* interrupt service routine for safe shutdown button
 * calls system function shutdown 0
 */
void bss_isr(){
	static uint8_t bss = 0;
	if(bss == 0){
		printf("System shutdown due to button press.\n");
		system("sudo shutdown 0");
		bss = 1;
	}
}

/* initialize safe shutdown button
 */
void init_bss_isr(){
	wiringPiSetup();
	wiringPiISR(SAFE_SHUTDOWN_PIN, INT_EDGE_FALLING, bss_isr);
}

/* initialize gpio pins
 *  for output led
 *  input button -> interrupt: button_isr
 */
void init_gpio(){
	wiringPiSetup();
	pinMode(G_LED_PIN, OUTPUT);
	pinMode(R_LED_PIN, OUTPUT);
    turn_led_off(G_LED_PIN);
    turn_led_off(R_LED_PIN);
    rg_led_off_status();    

    wiringPiISR(BUTTON_PIN, INT_EDGE_RISING, button_isr);
	button_pressed_flag = 0;

	wiringPiISR(STOP_BUTTON_PIN, INT_EDGE_RISING, stop_button_isr);
	stop_button_pressed_flag = 0;
}

/* for turning on led
 */
void turn_led_on(uint8_t led_pin){
	digitalWrite(led_pin, HIGH);
}

/* for turning off led
 */
void turn_led_off(uint8_t led_pin){
	digitalWrite(led_pin, LOW);
}

/* toggle led
 */
void led_heartbeat(uint8_t led_pin) {
    static int8_t c = 0;
    static struct timespec tstart={0,0}, tend={0,0};
    volatile double elapsed, end;
    static double start;
    
    clock_gettime(CLOCK_MONOTONIC, &tend);

    elapsed = (double)(tend.tv_sec + 1.0e-9*tend.tv_nsec -
              tstart.tv_sec + 1.0e-9*tstart.tv_nsec); 
    end = tend.tv_sec + 1.0e-9*tend.tv_nsec;
   
    elapsed = end-start;

    if (elapsed > HEARTBEAT_ON_TIME){
        if (c == 0 || c == 2){
            turn_led_on(led_pin);
        }
        else {
            turn_led_off(led_pin);;
        }
        #if(DEBUG_LED==1)
        printf("** %f, %d\n", elapsed, c);
        #endif  
        clock_gettime(CLOCK_MONOTONIC, &tstart);
        start = tstart.tv_sec + 1.0e-9*tstart.tv_nsec;
        c++;
        if (c == HEARTBEAT_CYCLES) c = 0;
    }
}

/* toggle led
 */
void led_heartbeat_searching_for_fix(uint8_t led_pin) {
    static int8_t c = 0;
    static struct timespec tstart={0,0}, tend={0,0};
    volatile double elapsed, end;
    static double start;
    
    clock_gettime(CLOCK_MONOTONIC, &tend);

    elapsed = (double)(tend.tv_sec + 1.0e-9*tend.tv_nsec -
              tstart.tv_sec + 1.0e-9*tstart.tv_nsec); 
    end = tend.tv_sec + 1.0e-9*tend.tv_nsec;
   
    elapsed = end-start;

    if (elapsed > HEARTBEAT_ON_TIME_FIX){
        if (c == 0 || c == 2){
            turn_led_on(led_pin);
        }
        else {
            turn_led_off(led_pin);;
        }
        #if(DEBUG_LED==1)
        printf("** %f, %d\n", elapsed, c);
        #endif  
        clock_gettime(CLOCK_MONOTONIC, &tstart);
        start = tstart.tv_sec + 1.0e-9*tstart.tv_nsec;
        c++;
        if (c == HEARTBEAT_CYCLES_FIX) c = 0;
    }
}

// -- turn red led on continuously for given amount of time
void red_led_on (void) {
    static struct timespec tstart={0,0}, tend={0,0};
    volatile double elapsed, end;
    static double start = 0;
    
    if (start == 0) {
        clock_gettime(CLOCK_MONOTONIC, &tstart);
        start = tstart.tv_sec + 1.0e-9*tstart.tv_nsec;
        turn_led_on(R_LED_PIN);
    }

    clock_gettime(CLOCK_MONOTONIC, &tend);

    elapsed = (double)(tend.tv_sec + 1.0e-9*tend.tv_nsec -
              tstart.tv_sec + 1.0e-9*tstart.tv_nsec); 
    end = tend.tv_sec + 1.0e-9*tend.tv_nsec;
   
    elapsed = end-start;
 
    // -- turn off after given amoubnt of time
    if (elapsed > R_LED_ON_TIME){
        turn_led_off(R_LED_PIN);
        start=0;
        #if(DEBUG_LED==1)
        printf("** %f\n", elapsed);
        #endif  
        r_led_off_status();
        g_led_hbt_status();
    }
    
}


// -- turn both led's off
void rg_led_off_status(void) {
    led_status &= LED_OFF;
}

// -- turn green led on, turn green hartbeat off
void g_led_on_status(void) {
    led_status |= G_LED_ON;
    led_status &= (~G_LED_HEARTBEAT);
}
// -- turn green led off
void g_led_off_status(void) {
    led_status &= (~G_LED_ON);
}
// -- turn green hartbeat on
void g_led_hbt_status(void) {
    led_status |= G_LED_ON;
    led_status |= G_LED_HEARTBEAT;
}

// -- turn red led on, turn red hartbeat off
void r_led_on_status(void) {
    led_status |= R_LED_ON;
    led_status &= (~R_LED_HEARTBEAT);
}
// -- turn red led off
void r_led_off_status(void) {
    led_status &= (~R_LED_ON);
}
// -- turn red hartbeat on
void r_led_hbt_status(void) {
    led_status |= R_LED_ON;
    led_status |= R_LED_HEARTBEAT;
}
