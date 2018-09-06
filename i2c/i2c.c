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

#include <time.h>
#include "supervisor.h"

#include <unistd.h>

#include <wiringPiI2C.h>


// location of the i2c device in linux - enable in /boot/config.txt and 
// etc/modules where insert i2c_dev
#define DEVICE_LOCATION "/dev/i2c-1"
#define DEVICE_ADDRESS 0x68		//either 0x68 if AD0 to GND, or 0x69 if VDD

/* LOCAL VARIABLES */
static int device_file;	//file descriptor of device

/* DEFINITIONS OF STATIC FUNCTIONS */
/*
 * internal function for i2c support
 */
static inline __s32 i2c_smbus_access(int file, char read_write, __u8 command, int size, union i2c_smbus_data *data);

/*
 * internal function for i2c support
 * reads a byte from the device
 * if error, returns negative errno
 * else a data byte received from the device
 */
static inline __s32 i2c_smbus_read_byte_data(int file, __u8 command);

/*
 * internal function for i2c support
 * writes value to device
 * if error, returns negative errno
 * else returns 0
 */
static inline __s32 i2c_smbus_write_byte_data(int file, __u8 command, __u8 value);
/* ################################################## */

/*
 * internal function for i2c support
 */
static inline __s32 i2c_smbus_access(int file, char read_write, __u8 command, int size, union i2c_smbus_data *data){
	struct i2c_smbus_ioctl_data args;

	args.read_write = read_write;
	args.command = command;
	args.size = size;
	args.data = data;

	#if(TIMING_I2C==1)	
	static struct timespec tasks_timer_start={0,0}, tasks_timer_end={0,0};
	clock_gettime(CLOCK_MONOTONIC, &tasks_timer_start);
	
	ioctl(file,I2C_SMBUS,&args);
	
	clock_gettime(CLOCK_MONOTONIC, &tasks_timer_end);
	printf("'ioctl(file,I2C_SMBUS,&args);' took %.3f ms\n",
	((double)tasks_timer_end.tv_sec*1.0e3 + 1.0e-6*tasks_timer_end.tv_nsec) -
	((double)tasks_timer_start.tv_sec*1.0e3 + 1.0e-6*tasks_timer_start.tv_nsec));
	#endif

	return ioctl(file,I2C_SMBUS,&args);
}

/*
 * internal function for i2c support
 * reads a byte from the device
 * if error, returns negative errno
 * else a data byte received from the device
 */
static inline __s32 i2c_smbus_read_byte_data(int file, __u8 command){
	union i2c_smbus_data data;
	if (i2c_smbus_access(file,I2C_SMBUS_READ,command,
	                     I2C_SMBUS_BYTE_DATA,&data))
		return -1;
	else
		return 0x00FF & data.byte;
}

/*
 * internal function for i2c support
 * writes value to device
 * if error, returns negative errno
 * else returns 0
 */
static inline __s32 i2c_smbus_write_byte_data(int file, __u8 command, __u8 value){
	union i2c_smbus_data data;
	data.byte = value;
	if (i2c_smbus_access(file,I2C_SMBUS_WRITE,command,
	                     I2C_SMBUS_BYTE_DATA,&data))
		return -1;
	else
		return 0x0000 & data.byte;
}

/*
 * reads a byte from register of the deivce via i2c
 *  writes received byte in data
 *  returns 1 if read successful, else 0
 */
uint8_t i2c_read(uint8_t reg, uint8_t *data){

	int tmp = -1;

	#if(TIMING_I2C==1)	
	static struct timespec tasks_timer_start={0,0}, tasks_timer_end={0,0};
	clock_gettime(CLOCK_MONOTONIC, &tasks_timer_start);
	#endif

	tmp = wiringPiI2CReadReg8(device_file, reg);

	#if(TIMING_I2C==1)	
	clock_gettime(CLOCK_MONOTONIC, &tasks_timer_end);
	printf("'i2c_read' took %.3f ms\n",
	((double)tasks_timer_end.tv_sec*1.0e3 + 1.0e-6*tasks_timer_end.tv_nsec) -
	((double)tasks_timer_start.tv_sec*1.0e3 + 1.0e-6*tasks_timer_start.tv_nsec));
	#endif

	#if(DEBUG_I2C==1)
	printf("Read byte from reg via i2c: 0x%04x\n", tmp);
	#endif
	if(tmp != -1){
		*data = (uint8_t)(tmp & 0x00FF);
		#if(DEBUG_I2C==1)
		printf("Converted read byte from reg via i2c: 0x%02x\n", *data);
		#endif
		return 1;
	}
	else{
		return 0;
	}
}

/*
 * writes a byte to register of the deivce via i2c
 *  writes data to reg
 *  returns 1 if write successful, else 0
 */
uint8_t i2c_write(uint8_t reg, uint8_t data){

	int8_t success=0;

	#if(TIMING_I2C==1)
	static struct timespec tasks_timer_start={0,0}, tasks_timer_end={0,0};
	clock_gettime(CLOCK_MONOTONIC, &tasks_timer_start);
	#endif

	success = wiringPiI2CWriteReg8(device_file, reg, data);

	#if(TIMING_I2C==1)
	clock_gettime(CLOCK_MONOTONIC, &tasks_timer_end);
	printf("'i2c_write' took %.3f ms\n",
	((double)tasks_timer_end.tv_sec*1.0e3 + 1.0e-6*tasks_timer_end.tv_nsec) -
	((double)tasks_timer_start.tv_sec*1.0e3 + 1.0e-6*tasks_timer_start.tv_nsec));
	#endif


	#if(DEBUG_I2C==1)
	printf("Successful write of data to reg via i2c: 0x%02x\n", success);
	#endif
	if(success != -1){
		return 1;
	}
	else{
		return 0;
	}
}

/*
 * initialize i2c device
 * return 1 on successful initialization, else 0
 */
uint8_t init_i2c(char *path, uint8_t addr){
	device_file = wiringPiI2CSetup(addr);
	if(device_file<0){
		printf("Cannot open i2c device at '%s'\n", path);
		return 0;
	}

	return 1;
}
