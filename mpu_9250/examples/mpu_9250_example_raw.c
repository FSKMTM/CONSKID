#include "mpu_9250.h"

int main(int argc, char **argv)
{
	uint8_t data=0, ok=0;

	ok = init_mpu_9250();
	if(ok==0){
		printf("Error initializing IMU MPU 9250 device\n");
		return 1;
	}

	data = mpu_9250_getDeviceID();
	printf("WHO AM I: 0x%02x\n", data);

	int16_t temp = mpu_9250_getTemperature();
	float T = temp /333.87 + 21;
	printf("T[C]: %.1f\n",T);

	int16_t ax, ay, az, gx, gy, gz;
	mpu_9250_getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

	printf("XYZ A: %d,%d,%d    %d,%d,%d\n", ax,ay,az, gx,gy,gz);

	return 0;
}
