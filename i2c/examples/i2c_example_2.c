#include "i2c.h"

#define DEVICE_LOCATION "/dev/i2c-1"
#define DEVICE_ADDRESS 0x68

int main(int argc, char **argv)
{
	uint8_t data=0, ok=0, reg = 0x75;

	ok = init_i2c(DEVICE_LOCATION, DEVICE_ADDRESS);
	if(ok==0){
		printf("Error initializing i2c device\n");
		return 1;
	}

	ok = i2c_read(reg, &data);
	printf("WHO I AM: 0x%02x\n",data);

	return 0;
}
