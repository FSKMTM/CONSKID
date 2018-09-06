#ifndef _MPU_9250_H_
#define _MPU_9250_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "../i2c/i2c.h"

//For debugging purposes
#ifndef DEBUG_MPU
#define DEBUG_MPU 0
#endif

//Location (path) of MPU 9250 device and it's I2C address
#define MPU_I2C_LOCATION  "/dev/i2c-1" //enable it in /boot/config.txt and /etc/modules where add i2c_dev
#define MPU_I2C_ADDRESS	  0x68	 //if AD0 pin to GND, 0x69 if AD0 to VCC

//STRUCT DEFINITONS
/* definiton of structs used for accel and gyro data in bits
 */
struct _struct_motion_bit{
	int16_t ax, ay, az, gx, gy, gz;
};
typedef struct _struct_motion_bit motion_bit;
/* definiton of structs used for accel and gyro data in floats
 */
struct _struct_motion_float{
	float ax, ay, az, gx, gy, gz;
};
typedef struct _struct_motion_float motion_float;


//MPU9250 Register map

#define MPU9250_DEFAULT_ADDRESS         0xD1
#define MPU9250_ALT_DEFAULT_ADDRESS     0xD2

#define MPU9250_SELF_TEST_X_GYRO        0x00
#define MPU9250_SELF_TEST_Y_GYRO        0x01
#define MPU9250_SELF_TEST_Z_GYRO        0x02

#define MPU9250_SELF_TEST_X_ACCEL       0x0D
#define MPU9250_SELF_TEST_Y_ACCEL       0x0E
#define MPU9250_SELF_TEST_Z_ACCEL       0x0F

#define MPU9250_XG_OFFSET_H             0x13
#define MPU9250_XG_OFFSET_L             0x14
#define MPU9250_YG_OFFSET_H             0x15
#define MPU9250_YG_OFFSET_L             0x16
#define MPU9250_ZG_OFFSET_H             0x17
#define MPU9250_ZG_OFFSET_L             0x18
#define MPU9250_SMPLRT_DIV              0x19
#define MPU9250_CONFIG                  0x1A
#define MPU9250_GYRO_CONFIG             0x1B
#define MPU9250_ACCEL_CONFIG            0x1C
#define MPU9250_ACCEL_CONFIG2           0x1D
#define MPU9250_LP_ACCEL_ODR            0x1E
#define MPU9250_WOM_THR                 0x1F

#define MPU9250_INT_PIN_CFG             0x37
#define MPU9250_INT_ENABLE              0x38

#define MPU9250_INT_STATUS              0x3A
#define MPU9250_ACCEL_XOUT_H            0x3B
#define MPU9250_ACCEL_XOUT_L            0x3C
#define MPU9250_ACCEL_YOUT_H            0x3D
#define MPU9250_ACCEL_YOUT_L            0x3E
#define MPU9250_ACCEL_ZOUT_H            0x3F
#define MPU9250_ACCEL_ZOUT_L            0x40
#define MPU9250_TEMP_OUT_H              0x41
#define MPU9250_TEMP_OUT_L              0x42
#define MPU9250_GYRO_XOUT_H             0x43
#define MPU9250_GYRO_XOUT_L             0x44
#define MPU9250_GYRO_YOUT_H             0x45
#define MPU9250_GYRO_YOUT_L             0x46
#define MPU9250_GYRO_ZOUT_H             0x47
#define MPU9250_GYRO_ZOUT_L             0x48

#define MPU9250_I2C_SLV0_DO             0x63
#define MPU9250_I2C_SLV1_DO             0x64
#define MPU9250_I2C_SLV2_DO             0x65
#define MPU9250_I2C_SLV3_DO             0x66
#define MPU9250_I2C_MST_DELAY_CTRL      0x67
#define MPU9250_SIGNAL_PATH_RESET       0x68
#define MPU9250_MOT_DETECT_CTRL         0x69
#define MPU9250_USER_CTRL               0x6A
#define MPU9250_PWR_MGMT_1              0x6B
#define MPU9250_PWR_MGMT_2              0x6C

#define MPU9250_WHO_AM_I                0x75

#define MPU9250_XA_OFFSET_H             0x77
#define MPU9250_XA_OFFSET_L             0x78
#define MPU9250_YA_OFFSET_H             0x7A
#define MPU9250_YA_OFFSET_L             0x7B
#define MPU9250_ZA_OFFSET_H             0x7D
#define MPU9250_ZA_OFFSET_L             0x7E

//reset values
#define WHOAMI_RESET_VAL                0x71
#define POWER_MANAGMENT_1_RESET_VAL     0x01
#define DEFAULT_RESET_VALUE             0x00

/* for same sensors from different suppliers
   different values are possible */
#define WHOAMI_DEFAULT_VAL_1            0x71
#define WHOAMI_DEFAULT_VAL_2            0x73

//CONFIG register masks
#define MPU9250_DLPF_CFG_MASK           0x07

//GYRO_CONFIG register masks
#define MPU9250_XGYRO_CTEN_MASK         0x80
#define MPU9250_YGYRO_CTEN_MASK         0x40
#define MPU9250_ZGYRO_CTEN_MASK         0x20
#define MPU9250_GYRO_FS_SEL_MASK        0x18
#define MPU9250_GYRO_FCHOICE_B_MASK     0x03

#define MPU9250_FCHOICE_DISABLE_DLPF	0x0F

#define MPU9250_GYRO_FULL_SCALE_250DPS  0
#define MPU9250_GYRO_FULL_SCALE_500DPS  1
#define MPU9250_GYRO_FULL_SCALE_1000DPS 2
#define MPU9250_GYRO_FULL_SCALE_2000DPS 3

//ACCEL_CONFIG register masks
#define MPU9250_AX_ST_EN_MASK           0x80
#define MPU9250_AY_ST_EN_MASK           0x40
#define MPU9250_AZ_ST_EN_MASK           0x20
#define MPU9250_ACCEL_FS_SEL_MASK       0x18

#define MPU9250_FULL_SCALE_2G           0
#define MPU9250_FULL_SCALE_4G           1
#define MPU9250_FULL_SCALE_8G           2
#define MPU9250_FULL_SCALE_16G          3

//ACCEL_CONFIG_2 register masks
#define MPU9250_ACCEL_FCHOICE_B_MASK    0x08
#define MPU9250_A_DLPF_CFG_MASK         0x07

#define MPU9250_ACCEL_DLPF_1000HZ	0x08
#define MPU9250_ACCEL_DLPF_218HZ	0x00
#define MPU9250_ACCEL_DLPF_218HZ_2	0x01
#define MPU9250_ACCEL_DLPF_99HZ		0x02
#define MPU9250_ACCEL_DLPF_45HZ		0x03
#define MPU9250_ACCEL_DLPF_21HZ		0x04
#define MPU9250_ACCEL_DLPF_10HZ		0x05
#define MPU9250_ACCEL_DLPF_5HZ		0x06
#define MPU9250_ACCEL_DLPF_420HZ	0x07

//LP_ACCEL_ODR register masks
#define MPU9250_LPOSC_CLKSEL_MASK       0x0F

//INT_PIN_CFG register masks
#define MPU9250_ACTL_MASK               0x80
#define MPU9250_OPEN_MASK               0x40
#define MPU9250_LATCH_INT_EN_MASK       0x20
#define MPU9250_INT_ANYRD_2CLEAR_MASK   0x10
#define MPU9250_ACTL_FSYNC_MASK         0x08
#define MPU9250_FSYNC_INT_MODE_EN_MASK  0x04
#define MPU9250_BYPASS_EN_MASK          0x02

//INT_ENABLE register masks
#define MPU9250_WOM_EN_MASK             0x40
#define MPU9250_FIFO_OFLOW_EN_MASK      0x10
#define MPU9250_FSYNC_INT_EN_MASK       0x08
#define MPU9250_RAW_RDY_EN_MASK         0x01

//INT_STATUS register masks
#define MPU9250_WOM_INT_MASK            0x40
#define MPU9250_FIFO_OFLOW_INT_MASK     0x10
#define MPU9250_FSYNC_INT_MASK          0x08
#define MPU9250_RAW_DATA_RDY_INT_MASK   0x01

//SIGNAL_PATH_RESET register masks
#define MPU9250_GYRO_RST_MASK           0x04
#define MPU9250_ACCEL_RST_MASK          0x02
#define MPU9250_TEMP_RST_MASK           0x01

//MOT_DETECT_CTRL register masks
#define MPU9250_ACCEL_INTEL_EN_MASK     0x80
#define MPU9250_ACCEL_INTEL_MODE_MASK   0x40

//USER_CTRL register masks
#define MPU9250_FIFO_EN_MASK            0x40
#define MPU9250_I2C_MST_EN_MASK         0x20
#define MPU9250_I2C_IF_DIS_MASK         0x10
#define MPU9250_FIFO_RST_MASK           0x04
#define MPU9250_I2C_MST_RST_MASK        0x02
#define MPU9250_SIG_COND_RST_MASK       0x01

//PWR_MGMT_1 register masks
#define MPU9250_H_RESET_MASK            0x80
#define MPU9250_SLEEP_MASK              0x40
#define MPU9250_CYCLE_MASK              0x20
#define MPU9250_GYRO_STANDBY_CYCLE_MASK 0x10
#define MPU9250_PD_PTAT_MASK            0x08
#define MPU9250_CLKSEL_MASK             0x07

//PWR_MGMT_2 register masks
#define MPU9250_DISABLE_XA_MASK         0x20
#define MPU9250_DISABLE_YA_MASK         0x10
#define MPU9250_DISABLE_ZA_MASK         0x08
#define MPU9250_DISABLE_XG_MASK         0x04
#define MPU9250_DISABLE_YG_MASK         0x02
#define MPU9250_DISABLE_ZG_MASK         0x01

#define MPU9250_DISABLE_XYZA_MASK       0x38
#define MPU9250_DISABLE_XYZG_MASK       0x07

//Magnetometer register maps
#define MPU9250_MAG_ADDRESS             0x0C

#define MPU9250_MAG_WIA                 0x00
#define MPU9250_MAG_INFO                0x01
#define MPU9250_MAG_ST1                 0x02
#define MPU9250_MAG_XOUT_L              0x03
#define MPU9250_MAG_XOUT_H              0x04
#define MPU9250_MAG_YOUT_L              0x05
#define MPU9250_MAG_YOUT_H              0x06
#define MPU9250_MAG_ZOUT_L              0x07
#define MPU9250_MAG_ZOUT_H              0x08
#define MPU9250_MAG_ST2                 0x09
#define MPU9250_MAG_CNTL                0x0A
#define MPU9250_MAG_RSV                 0x0B //reserved mystery meat
#define MPU9250_MAG_ASTC                0x0C
#define MPU9250_MAG_TS1                 0x0D
#define MPU9250_MAG_TS2                 0x0E
#define MPU9250_MAG_I2CDIS              0x0F
#define MPU9250_MAG_ASAX                0x10
#define MPU9250_MAG_ASAY                0x11
#define MPU9250_MAG_ASAZ                0x12

//Magnetometer register masks
#define MPU9250_WIA_MASK 0x48

//FUNCTIONS
/* initialization of mpu 9250 with disabled DLPF and min accell and gyro scale range */
uint8_t init_mpu_9250(void);

uint8_t mpu_9250_getMasterClockSpeed(void);

uint8_t mpu_9250_resetSensors(void);
uint8_t mpu_9250_reset(void);

uint8_t mpu_9250_getDeviceID(void);
uint8_t mpu_9250_testConnection();

//acceleration functions
uint8_t mpu_9250_getAccelXSelfTest(void);
uint8_t mpu_9250_setAccelXSelfTest(uint8_t enabled);
uint8_t mpu_9250_getAccelYSelfTest(void);
uint8_t mpu_9250_setAccelYSelfTest(uint8_t enabled);
uint8_t mpu_9250_getAccelZSelfTest(void);
uint8_t mpu_9250_setAccelZSelfTest(uint8_t enabled);
uint8_t mpu_9250_getFullScaleAccelRange(void);
uint8_t mpu_9250_setFullScaleAccelRange(uint8_t range);
uint8_t mpu_9250_configAccelDLPF(uint8_t fchoice);

int16_t mpu_9250_getAccelerationX(void);
int16_t mpu_9250_getAccelerationY(void);
int16_t mpu_9250_getAccelerationZ(void);
void mpu_9250_getAcceleration(int16_t* ax, int16_t* ay, int16_t* az);

uint8_t mpu_9250_resetAccelerometerPath(void);
uint8_t mpu_9250_getAccelerometerPowerOnDelay(void);
uint8_t mpu_9250_setAccelerometerPowerOnDelay(uint8_t delay);

int16_t mpu_9250_getXAccelOffset(void);
uint8_t mpu_9250_setXAccelOffset(int16_t offset);
int16_t mpu_9250_getYAccelOffset(void);
uint8_t mpu_9250_setYAccelOffset(int16_t offset);
int16_t mpu_9250_getZAccelOffset(void);
uint8_t mpu_9250_setZAccelOffset(int16_t offset);

void mpu_9250_getAccelerometerTestData(uint8_t *ax, uint8_t *ay, uint8_t *az);

uint8_t mpu_9250_accelerometerXIsEnabled(void);
uint8_t mpu_9250_accelerometerYIsEnabled(void);
uint8_t mpu_9250_accelerometerZIsEnabled(void);

//gyroscope functions
uint8_t mpu_9250_getFullScaleGyroRange(void);
uint8_t mpu_9250_setFullScaleGyroRange(uint8_t range);
uint8_t mpu_9250_configGyroDLPF(uint8_t fchoice);

uint8_t mpu_9250_resetGyroscopePath(void);

int16_t mpu_9250_getXGyroOffset(void);
uint8_t mpu_9250_setXGyroOffset(int16_t offset);
int16_t mpu_9250_getYGyroOffset(void);
uint8_t mpu_9250_setYGyroOffset(int16_t offset);
int16_t mpu_9250_getZGyroOffset(void);
uint8_t mpu_9250_setZGyroOffset(int16_t offset);

int16_t mpu_9250_getRotationX(void);
int16_t mpu_9250_getRotationY(void);
int16_t mpu_9250_getRotationZ(void);
void mpu_9250_getRotation(int16_t* gx, int16_t* gy, int16_t* gz);

void mpu_9250_getGyroTestData(uint8_t *gx, uint8_t *gy, uint8_t *gz);

void mpu_9250_getMotion6(int16_t* ax, int16_t* ay, int16_t* az, int16_t* gx, int16_t* gy, int16_t* gz);

/** converts accel and gyro data in bits from X,Y,Z axes to arrayfloat values in g
 * gets pointer b_in to struct of type motion_bit and converts data into struct of type motion_float pointed on by f_out
 * returns 1 if conversion is successful, else returns 0
 */
uint8_t mpu_9250_motion2float(motion_bit *b_in, motion_float *f_out);

//temperature functions
int16_t mpu_9250_getTemperature(void);

/* converts temp in bits to float value in Celzius */
float mpu_9250_temperature2float(int16_t temp_bits);

/** checks if there is a change between sign of motion_float sturucts
 *  returns 1 if there is, else 0
 */
uint8_t motion_different_sign(motion_float f1, motion_float f2);

/* checks if the motion's accel absolute data is above threshold level
 * returns 1 if above, else 0
 *
 * takes into account only X & Y direction!
 */
uint8_t motion_above_threshold(motion_float data, float threshold_level);

#endif /* _MPU9_250_H_ */
