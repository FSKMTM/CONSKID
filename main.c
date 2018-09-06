// -- enostaven ciklicen "RTOS" z lastnim schedulerjem

// -- DEBUG PRINTS and MODULE ENABLE DEFINES -------------------------------------
#ifndef DEBUG_PRINTS
#define DEBUG_PRINTS 0	//to enable debug reports set to 1
#endif


// -- INCLUDES -----------------------------------------------------------------
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <sys/syscall.h>

#include <limits.h>
#include <sched.h>
#include <pthread.h>
#include <sys/mman.h>

#include "supervisor.h"
#include "tasks/tasks.h"
#include "rtos/rtos.h"
#include "fifo/fifo.h"
#include "serial_read/serial_read.h"
#include "requests/requests.h"
#include "gpio/gpio.h"
#include "mpu_9250/mpu_9250.h"
#include "i2c/i2c.h"
#include "measurement_flow/measurement_flow.h"
#include "data_save/data_save.h"

// -- SETTINGS ---------------------------------------------------------------
//#define RTOS_SLICE_US 250000
//#define RTOS_SLICE_US 100000
#define RTOS_SLICE_US 10000

rtos_task_t *rtos_task_list[] = { &task_gnss_read, &task_imu_read };

// -- must be defined in same file as task list, otherwise the compiler
// -- doesn't recognize task list (array) length
int8_t rtos_num_tasks = sizeof(rtos_task_list) / sizeof(rtos_task_list[0]);

//#define LED_BLINK_MAX	1000000/RTOS_SLICE_US	//counter number before toggling led if system active and not measuring (set to 1 second)

// Threshold of accel for starting saving measurements (in g)
#define MOTION_THRESHOLD_LEVEL 0.2
//#define MOTION_THRESHOLD_LEVEL 0.0

void *_loop_thread (void *data) {

	#if(DEBUG_MAIN==1)
	printf("Entered thread.\n");
	#endif
	
    // -- INIT -------------------------------------------------------------------
    //period_info_t pinfo;
	//periodic_task_init(&pinfo, RTOS_SLICE_US);

	#if(DEBUG_RTOS==1)
	printf("Timer interrupt init successful.\n");
	#endif


    /* setup of GPIO module
	 */
 	#if(ENABLE_GPIO==1)
	init_gpio();

	  #if(DEBUG_BTN==1)
	  uint8_t debug_stop_btn_output=0;
	  #endif

	  #if(DEBUG_MAIN==1)
	  printf("GPIO init successful.\n");
	  #endif
	#endif


	#if(ENABLE_GNSS==1 || ENABLE_IMU==1)
	int setup_err = 1;
	uint32_t read_ok = 0;
	#endif
	/* setup of GNSS module
	 */
 	#if(ENABLE_GNSS==1)
	setup_err = setup_gnss_read();
	//if(setup_err == 1) return 1;
	//if(setup_err == 1) return NULL;
	if(setup_err == 1) {
        turn_led_on(R_LED_PIN);
        turn_led_off(G_LED_PIN);
        return NULL;
    }

	char gnss_data[GNSS_READ_FIFO_STR_SIZE+1];
	int gnss_fix = 0;
	int gnss_first_fix = 0;

	gnss gnss_position;

	gnss_position.fix = '-';
	memcpy(gnss_position.time, "\0", 1);
	memcpy(gnss_position.lat, "\0", 1);
	gnss_position.ni = '\0';
	memcpy(gnss_position.lon, "\0", 1);
	gnss_position.ei = '\0';
    	gnss_position.speed = 0.0;
	memcpy(gnss_position.date, "\0", 1);

        memcpy(data_buf.time, gnss_position.time, 10);
	  #if(DEBUG_MAIN==1)
	  printf("GNSS init successful.\n");
	  #endif
	#endif


	/* setup of IMU module
	 */
 	#if(ENABLE_IMU==1)
	setup_err = setup_imu_read();
	//if(setup_err == 1) return 1;
	//if(setup_err == 1) return NULL;
	if(setup_err == 1) {
        turn_led_on(R_LED_PIN);
        turn_led_off(G_LED_PIN);
        return NULL;
    }
	motion_bit motion_bit_data;
	motion_float motion_float_data, motion_float_first;

	  #if(DEBUG_MAIN==1)
	  printf("IMU init successful.\n");
	  #endif
	#endif


	// -- setup socket and related request buffer
	#if(ENABLE_REQUESTS==1)
	if (setup_socket()) {
		setup_request_buffer();
		dummy_request();
	}

	  #if(DEBUG_REQUESTS==1)
	  printf("Requests init successful.\n");
	  #endif
	#endif
    
    
    // -- setup data buffer for saving to file
    #if(ENABLE_SAVE==1)
    setup_data_buffer();
      #if(DEBUG_MAIN==1)
      printf("Data save init successful\n");
      #endif
    #endif


	/* setup of low battery output interrupt
	 */
	#if (ENABLE_LBO==1)
	init_lbo_isr();
	  #if(DEBUG_MAIN==1)
	  printf("Low battery output init successful.\n");
	  #endif
	#endif

 	/* setup of safe shutdown interrupt
        */
        #if (ENABLE_BSS==1)
        init_bss_isr();
          #if(DEBUG_MAIN==1)
          printf("Safe shutdown button init successful.\n");
          #endif
        #endif


	/* initialize measurement flow module */
	init_measurement_flow();

    // -- start green heartbeat
    g_led_hbt_status();

	// -- start RTOS tick intervals
    rtos_init(RTOS_SLICE_US);

	#if(DEBUG_MAIN==1)
	printf("Initializations complete.\n");
	#endif


	// -- LOOP -----------------------------------------------------------------
	while (1) {
       
        // -- REQUESTS ---------------------------------------------------------
      
		// -- new request pending
		#if(ENABLE_REQUESTS==1)
		switch (socket_state) {
	        // -- create
	        case SOCKET_CLOSED:
			// -- check for pending request in buffer and load it
			if (str_fifo_read(&request_fifo, request_data)) {
	        		if (create_socket()) {
					request_data_len = strlen(request_data);
			        	sprintf(request, request_fmt, 
                                host, request_data_len, request_data);
				}
				#if(DEBUG_REQUESTS==1)
				printf("%d\n",socket_state);
				#endif
			}
		        break;
	        // -- connect
		case SOCKET_CREATED:
			connect_socket();
			#if(DEBUG_REQUEST==1)
			printf("%d\n",socket_state);
			#endif
		        break;
	        // -- write
	        case SOCKET_CONNECTED:
	        	write_socket();
			#if(DEBUG_REQUESTS==1)
			printf("%d\n",socket_state);
			#endif
        		break;
	        // -- read
	        case SOCKET_WRITE_FINISHED:
	        	read_socket();
			#if(DEBUG_REQUESTS==1)
			printf("%d\n",socket_state);
			#endif
        		break;
      		// -- close
      		case SOCKET_READ_FINISHED:
        		close_socket();
			#if(DEBUG_REQUESTS==1)
			printf("%d\n",socket_state);
			#endif
	                break;
	        default:
        	// -- do nothing
        		break;
		}
		#endif


        // -- GNSS FIX --------------------------------------------------------

        /* check if there is new data from gnss to read
		 */
 		#if(ENABLE_GNSS==1)
		read_ok = str_fifo_read(&gnss_read_fifo, gnss_data);
		if(read_ok == 1){
			gnss_fix = gnss_format_output(gnss_data);
			if(gnss_fix==1){
				/* update GNSS position */
				gnss_position = get_curr_gnss_position();

				/* when GNSS position gets first fix */
				if(gnss_first_fix == 0){
					/* update system time from gnss */
        	                        #if(ENABLE_TIME_FROM_GNSS==1)
                	                char time[48] = "sudo timedatectl set-time \'20";
                        	        time[29] = gnss_position.date[4];
                                	time[30] = gnss_position.date[5];
	                                time[31] = '-';
	                                time[32] = gnss_position.date[2];
        		                time[33] = gnss_position.date[3];
                        	        time[34] = '-';
	                                time[35] = gnss_position.date[0];
        	                        time[36] = gnss_position.date[1];
                	                time[37] = ' ';
                        	        time[38] = gnss_position.time[0];
                                	time[39] = gnss_position.time[1];
	                                time[40] = ':';
        	                        time[41] = gnss_position.time[2];
                	                time[42] = gnss_position.time[3];
                        	        time[43] = ':';
                                	time[44] = gnss_position.time[4];
	                                time[45] = gnss_position.time[5];
        	                        time[46] = '\'';

	                                system(time);

        	                        #if(DEBUG_TIME_SYNCH==1)
                	                printf("System time command: %s\n",time);
                        	        printf("Time changed according to GNSS fix.\n");
                                	system("timedatectl");
	                                #endif

        	                        #endif

					gnss_first_fix = 1;
					turn_led_off(R_LED_PIN);
                			#if(DEBUG_MAIN==1 || DEBUG_GNSS_DATA==1)
        			        printf("First fix.\n");
			                #endif
				}


                #if(DEBUG_GNSS_DATA==1)
                printf("Fix.\n");
                #endif
			}
		}
		#endif


        // -- MEASUREMENT -----------------------------------------------------

		/* if there was interrupt for button press 
	         * (button_pressed_flag is set to 1, else to 0)
		 * then start with measurement of imu data
		 */
		if(button_pressed_flag==1 && gnss_first_fix==1){
			/* if measurement hasn't yet started, start it */
			if(!(measure_status & MEASURE_MASK) &&
			!(measure_status & FLUSH_DATA_SAVE_MASK)){
				imu_read_fifo.write_idx = 0;
				imu_read_fifo.read_idx = 0;
				start_measuring_imu();
				// -- turn on green led continuously
		                turn_led_on(G_LED_PIN);
		                g_led_on_status();
				#if (DEBUG_BTN==1)
				printf("Starting measurement.\n");
				debug_stop_btn_output=0;
				#endif
			}
			else{
				/* if measurement has already started and there was button press
				 * to stop measurement, stop it
				 */
				if(stop_button_pressed_flag==1){
					//stop_measuring_imu();
					flush_data_save_buffer();
			                #if(DEBUG_BTN==1)
					if(debug_stop_btn_output==0){
						printf("Ending measurement beacuse of button press.\n");
					}
					debug_stop_btn_output=1;
					#endif
				}
			}
		}
		else{
			//led_blink_c++;
		}

		/* check whether there are measurements to be made
		 */
		if(measure_status & MEASURE_MASK){
			/* write last know GNSS position to fifo buffer for file saving at
             		 * start of measurement */
			if(!(measure_status & GNSS_POSITION_ALREADY_WRITTEN_MASK)){

				#if (DEBUG_SAVE==1)
				printf("Writing GNSS to save file buffer.\n");
				#endif

                reset_elapsed_tt();

                // -- only write gnss data, IMU data is initiated as 0.0
                //data_buf.elapsed = elapsed_meas;
                data_buf.elapsed = elapsed_meas_2;
                data_buf.fix = gnss_position.fix;
                memcpy(data_buf.time, gnss_position.time, 10);
                memcpy(data_buf.lat, gnss_position.lat, 11);
                data_buf.ni = gnss_position.ni;
                memcpy(data_buf.lon, gnss_position.lon, 12);
                data_buf.ei = gnss_position.ei;
                data_buf.speed = gnss_position.speed;
                memcpy(data_buf.date, gnss_position.date, 7);
                
		        data_fifo_write(&meas_fifo, &data_buf);

				set_gnss_position_already_written();
			}

			/* check if there is new data from imu to read
			 */
			#if (ENABLE_IMU==1)
				#if (DEBUG_IMU==1)
				printf("Reading IMU data.\n");
				#endif
			read_ok = motion_fifo_read(&imu_read_fifo, &motion_bit_data);
			//#endif
			if(read_ok == 1){
				/* convert read imu data from bits to float */
				mpu_9250_motion2float(&motion_bit_data, &motion_float_data);

				#if (DEBUG_IMU==1)
				printf("XYZ bit Accel: %x,%x,%x  Gyro: %x,%x,%x\n ", motion_bit_data.ax,
		 motion_bit_data.ay, motion_bit_data.az, motion_bit_data.gx, motion_bit_data.gy, motion_bit_data.gz);
				printf("XYZ Accel: %.3f,%.3f,%.3f  Gyro: %.1f,%.1f,%.1f\n ", motion_float_data.ax,
		 motion_float_data.ay, motion_float_data.az, motion_float_data.gx, motion_float_data.gy, motion_float_data.gz);
				printf("Converting measured imu data.\n");
				#endif

				/* check if there already was a measurement above the threshold level 
				 */
				if( !(measure_status & FIRST_MEASUREMENT_MASK) && 
                (motion_above_threshold(motion_float_data, MOTION_THRESHOLD_LEVEL)) ){

					#if (DEBUG_MAIN==1)
					printf("First measurement greater than threshold.\n");
					#endif

					/* save the first read data for calculating sign change */
					motion_float_first.ax = motion_float_data.ax;
					motion_float_first.ay = motion_float_data.ay;
					motion_float_first.az = motion_float_data.az;

					#if(ENABLE_GYRO==1)
					motion_float_first.gx = motion_float_data.gx;
					motion_float_first.gy = motion_float_data.gy;
					motion_float_first.gz = motion_float_data.gz;
					#endif
		            
		            // -- stop saving position
                    data_buf.fix = 255;

                    set_first_measurement();
				}

				/* if there was already first measurement, write next data to save file */
				if( measure_status & FIRST_MEASUREMENT_MASK ){
					/* set bit for writing measured data */
					write_measured_imu_data();

					/* check if there is a change in sign of imu data
					 * if yes, than stop measureming
					 */
					if(motion_different_sign(motion_float_first, motion_float_data)){
						#if(ENABLE_IMU_SIGN_STOP==1)
						//stop_measuring_imu();
	   				        flush_data_save_buffer();
						  #if (DEBUG_IMU==1)
						  printf("Ending measurement because of sign change.\n");
						  #endif
						//stop_writing_measured_imu_data();
						#endif
					}
		        }
				else{
					stop_writing_measured_imu_data();
				}
			}
			else{
				stop_writing_measured_imu_data();
			}

			/* check if there is imu measured data to be saved into file
			 */
			if(measure_status & WRITE_MEASURED_DATA_MASK){
				/* write read imu data converted to float to fifo buffer
                 		 * for file saving. This includes that last data with changed
		                 * sign - to disable this uncomment line in
		                 * motion_different_sign if clause  */
				#if (DEBUG_SAVE==1)
				printf("Writing imu data to save file buffer.\n");
				#endif

				#if (DEBUG_SAVE==1)
               		printf("Writing IMU to save file buffer.\n");
                #endif
                
                // -- only wirte IMU data ("fix=255" -> empty GNSS data)
                //data_buf.elapsed = elapsed_meas;
                data_buf.elapsed = elapsed_meas_2;
           		data_buf.ax = motion_float_data.ax;
				data_buf.ay = motion_float_data.ay;
				data_buf.az = motion_float_data.az;
				
				#if(ENABLE_GYRO==1)
				data_buf.gx = motion_float_data.gx;
				data_buf.gy = motion_float_data.gy;
				data_buf.gz = motion_float_data.gz;
				#endif

            	data_fifo_write(&meas_fifo, &data_buf);

			}
		}
		if (measure_status & FLUSH_DATA_SAVE_MASK) {
            switch (file_state) {
                case FILE_CLOSED:
                    stop_button_pressed_flag = 0;
                    char *outputFilename = get_filename();
                    data_file = fopen(outputFilename, "a");
                    if (data_file != NULL) file_state = FILE_OPEN;
                    // -- substract elapsed time offset between GPS and IMU slice
                    data_fifo_elapsed_sub (&meas_fifo);
                    #if(DEBUG_SAVE==1)
                    printf("Data saving started: %d\n", file_state);
                    #endif
                    break;
                case FILE_OPEN:
                    // -- Check for positive read index
                    // -- Try writing data to file, if available
                    if (write_file(data_file) == -1) {
                        fclose(data_file);
                        file_state = FILE_CLOSED;
                        stop_measuring_imu();
                        #if(DEBUG_SAVE==1)
                        printf("Data saving finished: %d\n", file_state);
                        #endif
                    }
                    break;
            }
        }  
        /* check whether to stop measuring */
		if(measure_status & STOP_MEASUREMENT_MASK){
			/* reset all flags */
			reset_measurement();

			/* enable new measurement with clearing button_pressed_flag */
			button_pressed_flag = 0;

			/* enable new measurement button stop */
			stop_button_pressed_flag = 0;

			//led_blink_c = 0;
			// -- turn off green led and initiate red led status
            turn_led_off(G_LED_PIN);
            g_led_off_status();
            r_led_on_status();

			#if (DEBUG_MAIN==1)
			printf("Ended measurement.\n");
			#endif
		}
		#endif

        // -- LEDS -----------------------------------------------------
	if(gnss_first_fix == 1){
	        if (led_status & G_LED_ON) {
        	    if (led_status & G_LED_HEARTBEAT) {
	               led_heartbeat(G_LED_PIN);
	            } 
	        }
	        else if (led_status & R_LED_ON){
	            red_led_on();
	        }
	}
	else{
	        led_heartbeat_searching_for_fix(R_LED_PIN);
	}
    }

    return NULL; 
}

// -- MAIN -------------------------------------------------------------------
int main (int argc, char **argv) {

	#if(DEBUG_MAIN==1)
	printf("Starting main.\n");
	#endif

    /*
    struct sched_param param;
    pthread_attr_t attr;
    pthread_t thread;
    int ret;
    
    // -- lock memory
    if (mlockall(MCL_CURRENT|MCL_FUTURE) == -1) {
        printf ("init pthread attributes failed\n");
        return 1;
    }

    // -- Initialize pthread attributes (default values)
    ret = pthread_attr_init(&attr);
    if (ret) {
        printf("init pthread attributes failed\n");
        return 1;
    }                                                            
   
    // -- set stack size
    ret = pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);
    if (ret) {
        printf("pthread setstacksize failed\n");
        return 1;
    }

    // -- set scheduling policy
    ret = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    if (ret) {
        printf("pthread setschedpolicy failed\n");
        return 1;
    }

    // -- set scheduling priority
    param.sched_priority = 10;
    ret = pthread_attr_setschedparam(&attr, &param);
    if (ret) {
        printf("pthread setschedparam failed\n");
        return 1;
    }

    // -- Use scheduling parameters of attr
    ret = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    if (ret) {
        printf("pthread setinheritsched failed\n");
        return 1;
    }
    
    // -- Create a pthread with specified attributes
    ret = pthread_create(&thread, &attr, _loop_thread, NULL);
    if (ret) {
        printf("create pthread failed\n");
        return 1;
    }

    // -- Join the thread and wait until it is done
    ret = pthread_join(thread, NULL);
    if (ret) {
        printf("join pthread failed: %m\n");
    }
    */

    // -- test without thread
    int my_pointed_int = 10;
    _loop_thread(&my_pointed_int);

    return 1;
}
