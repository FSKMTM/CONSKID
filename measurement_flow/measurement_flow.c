#include "measurement_flow.h"

#include <stdio.h>

uint8_t measure_status;

void init_measurement_flow(void){
	measure_status = 0x00;
}

void reset_measurement(void){
	measure_status = 0x00;
}

void start_measuring_imu(void){
	measure_status |= MEASURE_MASK;
	measure_status &= (~STOP_MEASUREMENT_MASK);
}

void stop_measuring_imu(void){
	measure_status &= (~MEASURE_MASK);
	measure_status &= (~FLUSH_DATA_SAVE_MASK);
	measure_status |= STOP_MEASUREMENT_MASK;
}

void set_gnss_position_already_written(void){
	measure_status |= GNSS_POSITION_ALREADY_WRITTEN_MASK;
}

void clear_gnss_position_already_written(void){
	measure_status &= (~GNSS_POSITION_ALREADY_WRITTEN_MASK);
}

void write_measured_imu_data(void){
	measure_status |= WRITE_MEASURED_DATA_MASK;
}

void stop_writing_measured_imu_data(void){
	measure_status &= (~WRITE_MEASURED_DATA_MASK);
}

void set_first_measurement(void){
	measure_status |= FIRST_MEASUREMENT_MASK;

}

void clear_first_measurement(void){
	measure_status &= (~FIRST_MEASUREMENT_MASK);
}

void flush_data_save_buffer(void){
	measure_status &= (~MEASURE_MASK);
	measure_status &= (~STOP_MEASUREMENT_MASK);
    measure_status |= FLUSH_DATA_SAVE_MASK;
}
