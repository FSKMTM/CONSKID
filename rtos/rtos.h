// -- prevents double declaration of any identifiers such as
// -- types, enums and static variables
#ifndef rtos_H_
#define rtos_H_

#include "supervisor.h"

#include <stdint.h>
#include <time.h>


#ifndef DEBUG_RTOS
#define DEBUG_RTOS 0
#endif

#define USEC_IN_SEC 1000000

typedef struct {
        struct timespec next_period;
        long period_ns;
} period_info_t;


void rtos_init(int32_t rtos_slice_us);
void sigalarm_handler(int sig_num);
void schedule_tasks(void);

void periodic_task_init(period_info_t *pinfo, int32_t slice_us);
void inc_period(period_info_t *pinfo);
void wait_rest_of_period(period_info_t *pinfo);

#endif //rtos_H_
