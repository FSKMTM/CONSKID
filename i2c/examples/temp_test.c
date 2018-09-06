/*
 * Simple I2C example
 *
 * Copyright 2017 Joel Stanley <joel@jms.id.au>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>

#include <linux/types.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define DEVICE_LOCATION "/dev/i2c-1"	//location of the i2c device in linux - enable in /boot/config.txt and /etc/modules where insert i2c_dev
#define DEVICE_ADDRESS 0x68		//either 0x68 if AD0 to GND, or 0x69 if VDD


static inline __s32 i2c_smbus_access(int file, char read_write, __u8 command,
                                     int size, union i2c_smbus_data *data)
{
	struct i2c_smbus_ioctl_data args;

	args.read_write = read_write;
	args.command = command;
	args.size = size;
	args.data = data;
	return ioctl(file,I2C_SMBUS,&args);
}

/*
 * reads a byte from the device
 * if error, returns negative errno
 * else a data byte received from the device
 */
static inline __s32 i2c_smbus_read_byte_data(int file, __u8 command)
{
	union i2c_smbus_data data;
	if (i2c_smbus_access(file,I2C_SMBUS_READ,command,
	                     I2C_SMBUS_BYTE_DATA,&data))
		return -1;
	else
		return 0x0FF & data.byte;
}

/*
 * writes value to device
 * if error, returns negative errno
 * else returns 0
 */
static inline __s32 i2c_smbus_write_byte_data(int file, __u8 command, __u8 value)
{
	union i2c_smbus_data data;
	data.byte = value;
	if (i2c_smbus_access(file,I2C_SMBUS_WRITE,command,
	                     I2C_SMBUS_BYTE_DATA,&data))
		return -1;
	else
		return 0x000 & data.byte;
}

int main(int argc, char **argv)
{
	uint8_t data, reg = 0x41;
	uint16_t temp = 0x0000, temp_room = 0x0000, temp_sens = 0xffff;
	float temp_c = 0;
	const char *path = DEVICE_LOCATION;
	int file, rc;

	file = open(path, O_RDWR);
	if (file < 0)
		err(errno, "Tried to open '%s'", path);

	rc = ioctl(file, I2C_SLAVE, DEVICE_ADDRESS);
	if (rc < 0)
		err(errno, "Tried to set device address '0x%02x'", DEVICE_ADDRESS);


	data = i2c_smbus_read_byte_data(file, reg);
	temp |= (data << 8);
	printf("dh: 0x%02x\n",data);
	data = i2c_smbus_read_byte_data(file, reg+1);
	printf("dl: 0x%02x\n",data);
	temp |= data;
	printf("bit_temp: 0x%04x\n",temp);

//	temp_c = ((temp-temp_room)*125.0/temp_sens) -40;
	temp_c = ((temp-temp_room)/333.87) +21;
	printf("Temp: %.1f[C]\n", temp_c);

	reg = 0x75;
	data = i2c_smbus_read_byte_data(file, reg);
	printf("WHO I AM: 0x%02x\n",data);

	reg = 0x23;
	data = i2c_smbus_read_byte_data(file, reg);
	printf("WHO I AM: 0x%02x\n",data);


	return 0;
}
