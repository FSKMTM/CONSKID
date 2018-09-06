#ifndef _MEASUREMENT_FLOW_H_
#define _MEASUREMENT_FLOW_H_

#include <stdint.h>

#define MEASURE_MASK 				        0x01
#define STOP_MEASUREMENT_MASK 			    0x02
#define GNSS_POSITION_ALREADY_WRITTEN_MASK	0x04
#define WRITE_MEASURED_DATA_MASK		    0x08
#define FIRST_MEASUREMENT_MASK			    0x10
#define FLUSH_DATA_SAVE_MASK                0x20

/* variable containing measure status:
 * b0 - measure
 * b1 - stop measurement
 * b2 - gnss already written (variable for storing if current GNSS position has already been written to the save file at the beginning of a measurement)
 * b3 - write measured data
 */
extern uint8_t measure_status;

void init_measurement_flow(void);
void reset_measurement(void);
void start_measuring_imu(void);
void stop_measuring_imu(void);
void set_gnss_position_already_written(void);
void clear_gnss_position_already_written(void);
void write_measured_imu_data(void);
void stop_writing_measured_imu_data(void);
void set_first_measurement(void);
void clear_first_measurement(void);
void flush_data_save_buffer(void);

#endif /* _MEASUREMENT_FLOW_H_ */
