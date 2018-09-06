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

	motion_bit test;
	mpu_9250_getMotion6(&test.ax, &test.ay, &test.az, &test.gx, &test.gy, &test.gz);

	motion_float testf;
	uint8_t con_ok = mpu_9250_motion2float(&test,&testf);
	printf("Conversion status: %d\n", con_ok);

	printf("XYZ A[g]: %.3f,%.3f,%.3f   G[dps]: %.1f,%.1f,%.1f\n", testf.ax,testf.ay,testf.az, testf.gx,testf.gy,testf.gz);

	return 0;
}
