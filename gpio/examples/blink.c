#include <wiringPi.h>

#define PIN 0

int main(){

	wiringPiSetup();
	pinMode(PIN, OUTPUT);
	while(1){
		digitalWrite(PIN,HIGH);
		delay(500);
		digitalWrite(PIN,LOW);
		delay(500);
	}

	return 0;
}
