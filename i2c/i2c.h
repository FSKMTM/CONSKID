#ifndef _I2C_H_
#define _I2C_H_

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

#include "supervisor.h"

#ifndef DEBUG_I2C
#define DEBUG_I2C 0
#endif

/*
 * reads a byte from register of the deivce via i2c
 *  writes received byte in data
 *  returns 1 if read successful, else 0
 */
uint8_t i2c_read(uint8_t reg, uint8_t *data);

/*
 * writes a byte to register of the deivce via i2c
 *  writes data to reg
 *  returns 1 if write successful, else 0
 */
uint8_t i2c_write(uint8_t reg, uint8_t data);

/*
 * initialize i2c device located in path with I2C address addr
 * return 1 on successfull initialization, else 0
 *  path example: "/dev/i2c-1"	-location of the i2c device in linux - enable in /boot/config.txt and /etc/modules where insert i2c_dev
 *  addr - i2c address of the device
 */
uint8_t init_i2c(char *path, uint8_t addr);

#endif /*_I2C_H_*/
