#include <wiringPi.h>
#include <stdint.h>
#include <stdio.h>

#define PIN 0
#define IN 1

uint8_t flag;	//flag for interrupt status

void in_isr(){
	//so that same multiple interrupts don't cause multiple calls - against button debouncing
	if(flag==0){
		flag=1;
		printf("New interrupt.\n");
	}
}

int main(){

	wiringPiSetup();
	pinMode(PIN, OUTPUT);
	wiringPiISR(IN, INT_EDGE_RISING, in_isr);

	flag=0;

	while(1){
		if(flag==0){
			digitalWrite(PIN,HIGH);
			delay(500);
			digitalWrite(PIN,LOW);
			delay(500);
		}
		else{
			digitalWrite(PIN,HIGH);
			delay(2000);
			flag = 0; //clear flag for next interrupt
		}
	}

	return 0;
}
