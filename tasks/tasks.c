#include "../supervisor.h"
#include "tasks.h"
#include "../serial_read/serial_read.h"
#include "../mpu_9250/mpu_9250.h"
#include "../measurement_flow/measurement_flow.h"

#include <stdio.h>
#include <time.h>
#include <math.h>

// -- measure elapsed time betwene measurements
struct timespec t_meas_start_2={0,0}, t_meas_end_2={0,0};
uint32_t elapsed_meas_2 = 0;

// -- TASK 1 -------------------------------------------------------------
rtos_task_t task_gnss_read = {
  .function = task_gnss_read_driver,
  .last_call = 0,
};

static char **gnss_read_fifo_buf=NULL;
str_fifo_t gnss_read_fifo = {
	0,
	0,
	GNSS_READ_FIFO_BUF_SIZE+1,
	GNSS_READ_FIFO_STR_SIZE+1,
	NULL	//gnss_read_fifo_buf - if I put here directly fifo_buf I get compile error "initializer element is not constant"
};

/* driver for reading from gnss module
 */
void task_gnss_read_driver(void){
  #if(ENABLE_GNSS==1)

	char data[GNSS_READ_FIFO_STR_SIZE+1];
	int gnss_read_err=0;
	/* read output of GNSS
	 */
	#if(DEBUG_GNSS==1)
	printf("Starting to read data from GNSS.\n");
	#endif

	gnss_read_err = gnss_read(data, GNSS_READ_FIFO_STR_SIZE+1);
	if(gnss_read_err==0) return;

	#if(DEBUG_GNSS_DATA==1)
	printf("data from gnss: %s\n", data);
	#endif

	/* write output of GNSS to fifo
	 */
	#if(DEBUG_GNSS==1)
	printf("Writing read data from GNSS to fifo buf.\n");
	#endif
	uint32_t write_ok=0;
	write_ok=str_fifo_write(&gnss_read_fifo, data);
	if(write_ok==0) printf("GNSS READ BUF OVERFLOW!");
 
  #endif
}

/*  setup of driver for reading data from gnss module
 *   returns 0 if setup ok, else 1
 */
int setup_gnss_read(void){
	int init_err = init_gnss_read();
	if(init_err == 1) return 1;

	gnss_read_fifo_buf = (char **) malloc(sizeof(char *)*(GNSS_READ_FIFO_BUF_SIZE+1));
	int i = 0;
	for(i=0; i < GNSS_READ_FIFO_BUF_SIZE+1; i++){
		char *tmp_p = (char *) malloc(sizeof(char)*(GNSS_READ_FIFO_STR_SIZE+1));
		gnss_read_fifo_buf[i] = tmp_p;
	}

	gnss_read_fifo.buffer = gnss_read_fifo_buf;
	
    return 0;
}

// -- TASK 2 -------------------------------------------------------------
rtos_task_t task_imu_read = {
  .function = task_imu_read_driver,
  .last_call = 0,
};

static motion_bit *imu_read_fifo_buf=NULL;
motion_fifo_t imu_read_fifo = {
	0,
	0,
	IMU_READ_FIFO_BUF_SIZE+1,
	NULL	//imu_read_fifo_buf - if I put here directly fifo_buf I get compile error "initializer element is not constant"
};

/* driver for reading imu data
 */
void task_imu_read_driver(void){
  #if(ENABLE_IMU==1)

	#if(TIMING_TASKS==1)	
	static struct timespec tasks_timer_start={0,0}, tasks_timer_end={0,0};
	clock_gettime(CLOCK_MONOTONIC, &tasks_timer_start);
	#endif
	
	motion_bit b_in;
	mpu_9250_getMotion6(&(b_in.ax), &(b_in.ay), &(b_in.az),  &(b_in.gx), &(b_in.gy), &(b_in.gz));

	#if(TIMING_TASKS==1)
	clock_gettime(CLOCK_MONOTONIC, &tasks_timer_end);
	printf("'mpu_9250_getMotion6' took %.3f ms\n",
	((double)tasks_timer_end.tv_sec*1.0e3 + 1.0e-6*tasks_timer_end.tv_nsec) -
	((double)tasks_timer_start.tv_sec*1.0e3 + 1.0e-6*tasks_timer_start.tv_nsec));
	clock_gettime(CLOCK_MONOTONIC, &tasks_timer_start);
	#endif
	
	uint32_t write_ok = 0;
	write_ok = motion_fifo_write(&imu_read_fifo, &b_in);
	if(write_ok==0){
		#if(DEBUG_IMU==1)
		printf("IMU READ BUF OVERFLOW!\n");
		#endif
	}

	#if(TIMING_TASKS==1)
	clock_gettime(CLOCK_MONOTONIC, &tasks_timer_end);
	printf("'motion_fifo_write' took %.3f ms\n",
	((double)tasks_timer_end.tv_sec*1.0e3 + 1.0e-6*tasks_timer_end.tv_nsec) -
	((double)tasks_timer_start.tv_sec*1.0e3 + 1.0e-6*tasks_timer_start.tv_nsec));
	#endif
    
    // -- get elapsed measurement time
    calc_elapsed_tt();
  #endif
}

/*  setup of driver for reading data from imu module
 *   returns 0 if setup ok, else 1
 */
uint8_t setup_imu_read(void){
	uint8_t init_err = init_mpu_9250();
	if(init_err == 0) return 1;

	imu_read_fifo_buf = (motion_bit *) malloc(sizeof(motion_bit)*(IMU_READ_FIFO_BUF_SIZE+1));

	imu_read_fifo.buffer = imu_read_fifo_buf;
	return 0;
}



// -- TIMING ------------------------------------------------------------------

/* 
 * Reset elapsed timer
 *  used in 'main.c', when saving GNSS data
 */
void reset_elapsed_tt(void) {
    // -- set elapsed time to 0
    elapsed_meas_2 = 0;
    // -- initiate start timer 
    clock_gettime(CLOCK_MONOTONIC, &t_meas_start_2);
    //printf("elapsed_2_a: %d\n", elapsed_meas_2);
}


/* 
 * Calculate elapsed time betwene measurements, when:
 *  calling task is not GNSS (first measurement)
 *  after GNSS has already been written (1st mask in 'if')
 *  before data flush has begun (2nd mask in 'if')
 * To write elapsed time, read global 'elapsed_meas' variable
 */
void calc_elapsed_tt(void) {
    //printf("stat: %x\n", measure_status);
    if ((measure_status & GNSS_POSITION_ALREADY_WRITTEN_MASK) &&
        !(measure_status & FLUSH_DATA_SAVE_MASK)) {
    // -- calculate elapsed time since measurement start 
    clock_gettime(CLOCK_MONOTONIC, &t_meas_end_2);
    elapsed_meas_2 = (uint32_t) 
        (((double) t_meas_end_2.tv_sec*1.0e6 + 
          1.0e-3*t_meas_end_2.tv_nsec) -
        ((double) t_meas_start_2.tv_sec*1.0e6 + 
         1.0e-3*t_meas_start_2.tv_nsec));
    
    // -- lower two lines for displaying only rounded miliseconds
    float rounded = roundf((float) elapsed_meas_2/1000.0);
    elapsed_meas_2 = (uint32_t) rounded;
    
    //printf("elapsed_2_b: %d | stat: %d\n", elapsed_meas_2, measure_status);    

    /* (un)comment lower line to get:
     *  0 at start of measurement and keep adding elepsed times
     *  0 at beginning and reset timer on each elapsed calculation
     */
    //clock_gettime(CLOCK_MONOTONIC, &t_meas_start_2);
    }
}
