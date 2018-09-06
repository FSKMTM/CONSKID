#include "../supervisor.h"
#include "../measurement_flow/measurement_flow.h"
#include "rtos.h"
#include "../tasks/tasks.h"
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

extern rtos_task_t *rtos_task_list[];
extern int8_t rtos_num_tasks;

int32_t slice_us;

void rtos_init(int32_t rtos_slice_us) {
	slice_us = (int32_t)rtos_slice_us;
    signal(SIGALRM, sigalarm_handler);
    ualarm(slice_us, slice_us);   
	return;
}

void sigalarm_handler(int sig_num) {
	#if(TIMING_RTOS==1)	
	static int32_t iter = 0;
	static struct timespec rtos_timer_start={0,0}, rtos_timer_end={0,0};
	clock_gettime(CLOCK_MONOTONIC, &rtos_timer_start);
	#endif

	schedule_tasks();
	
	#if(TIMING_RTOS==1)	
	clock_gettime(CLOCK_MONOTONIC, &rtos_timer_end);
	printf("Sched. + task took %.3f ms\t|\titer: %d\n",
	((double)rtos_timer_end.tv_sec*1.0e3 + 1.0e-6*rtos_timer_end.tv_nsec) -
	((double)rtos_timer_start.tv_sec*1.0e3 + 1.0e-6*rtos_timer_start.tv_nsec), iter);
	iter++;
	if (iter == USEC_IN_SEC/slice_us) iter=0;
	#endif

	return;
}

void schedule_tasks(void)
{
	static uint32_t i = 0;
    static uint8_t k = 0;	
	/*
	#if(DEBUG_RTOS==1)
	printf("Starting task.\n");
	#endif

	rtos_task_list[i] -> function();

	#if(DEBUG_RTOS==1)
	printf("Finished with task.\n");
	#endif

	i++;
	if (i == rtos_num_tasks) i = 0;
	*/
	
	// only schedule IMU (repeatedly) and GNSS (once per second)
	if (i < USEC_IN_SEC/slice_us - 1){ 
		//task_imu_read.function();
		i++;
        k=1;
	} else {
		//task_gnss_read.function();
		i=0;
        k=0;
	}
   
    /* during measurement do not schedule GNSS task -> avoid skipping
     * each 100th line in output file 
     * */
    k = k || (measure_status & GNSS_POSITION_ALREADY_WRITTEN_MASK); 
    rtos_task_list[k]->function();    
    
    #if(DEBUG_RTOS==1)
    printf("k: %d\n", k);
    #endif
        
	return;
}

void periodic_task_init(period_info_t *pinfo, int32_t slice_us)
{
	pinfo->period_ns = slice_us * 1000;

	clock_gettime(CLOCK_MONOTONIC, &(pinfo->next_period));
}

//static void inc_period(struct period_info *pinfo)
void inc_period(period_info_t *pinfo)
{
        pinfo->next_period.tv_nsec += pinfo->period_ns;

        while (pinfo->next_period.tv_nsec >= 1000000000) {
                // timespec nsec overflow
                pinfo->next_period.tv_sec++;
                pinfo->next_period.tv_nsec -= 1000000000;
        }
}

//static void wait_rest_of_period(struct period_info *pinfo)
void wait_rest_of_period(period_info_t *pinfo)
{
        inc_period(pinfo);

        // for simplicity, ignoring possibilities of signal wakes
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &pinfo->next_period, NULL);
}
