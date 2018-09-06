#include "i2c.h"

/*Test i2c komunikacije preko branja temperaturnega senzorja tmp102*/

#define DEVICE_LOCATION "/dev/i2c-1"
#define DEVICE_ADDRESS 0x48

int main(int argc, char **argv)
{
	uint8_t data=0, ok=0, reg = 0x00;

	ok = init_i2c(DEVICE_LOCATION, DEVICE_ADDRESS);
	if(ok==0){
		printf("Error initializing i2c device\n");
		return 1;
	}

	uint16_t temp = 0x0000;
	temp = 0x0000;

	reg = 0x00;
	data = 0x00;
	ok = i2c_write(reg, data);
	reg = 0x00;
	ok = i2c_read(reg, &data);
	printf("TH: 0x%02x\n",data);

	temp = temp | data;
	printf("T: 0x%04x\n",temp);
	temp = temp << 4;
	printf("T: 0x%04x\n",temp);

	reg = 0x00;
	data = 0x00;
	ok = i2c_write(reg, data);
	reg = 0x00;
	ok = i2c_read(reg, &data);
	printf("TL: 0x%02x\n",data);

	temp = temp | (data>>4);
	temp = temp & 0x0FFF;
	printf("T: 0x%04x, %d\n",temp, temp);

	out = temp*0.0625;

	printf("Temp: %.2f\n",out);


	return 0;
}
