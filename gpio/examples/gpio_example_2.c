#include "gpio.h"

int main(){

	init_gpio();

	while(1){
		if(button_pressed_flag==0){
			turn_led_on();
			delay(500);
			turn_led_off();
			delay(500);
		}
		else{
			turn_led_on();
			delay(2000);
			button_pressed_flag = 0;
		}
	}

}
