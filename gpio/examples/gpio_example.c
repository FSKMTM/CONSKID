#include <wiringPi.h>
#include <stdint.h>
#include <stdio.h>

#define PIN 0
#define IN 1

int main(){

	uint8_t in=0;
	wiringPiSetup();
	pinMode(PIN, OUTPUT);
	pinMode(IN, INPUT);
	while(1){
		in = digitalRead(IN);
		if(in==1) printf("High\n");
		digitalWrite(PIN,HIGH);
		delay(500);
		digitalWrite(PIN,LOW);
		delay(500);
	}

	return 0;
}
