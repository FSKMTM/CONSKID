// -- prevents double declaration of any identifiers such as
// -- types, enums and static variables
#ifndef tasks_H_
#define tasks_H_

#include <stdint.h>
#include "../fifo/fifo.h"
#include "supervisor.h"

#ifndef ENABLE_GNSS
#define ENABLE_GNSS 0
#endif

#ifndef ENABLE_IMU
#define ENABLE_IMU 0
#endif

#ifndef DEBUG_GNSS
#define DEBUG_GNSS 0
#endif

#ifndef DEBUG_GNSS_DATA
#define DEBUG_GNSS_DATA 0
#endif

#ifndef DEBUG_IMU
#define DEBUG_IMU 0
#endif


// -- task function pointer
typedef void (*_struct_func_t)(void);
// -- declare task structure
typedef struct {
  _struct_func_t function;
  int32_t last_call;
  int32_t deadline;
  int32_t priority;
  int32_t interval;
} rtos_task_t;

// -- extend timing variable
extern uint32_t elapsed_meas_2;

// -- import tasks and coresponding function prototypes -------------------

/* TASK GNSS READ
*/
extern rtos_task_t task_gnss_read;
extern str_fifo_t gnss_read_fifo;
#define GNSS_READ_FIFO_BUF_SIZE 10	//possible number of kept strings in fifo
#define GNSS_READ_FIFO_STR_SIZE 128	//size of string to be kept in fifo

/* driver for reading from gnss module
 *  read data writes to gnss_read_fifo fifo buffer
 */
void task_gnss_read_driver(void);

/* setup of driver for reading data from gnss module
 *  returns 0 if setup was ok, else 1
 */
int setup_gnss_read(void);


/* TASK IMU READ
 */
extern rtos_task_t task_imu_read;
extern motion_fifo_t imu_read_fifo;
#define IMU_READ_FIFO_BUF_SIZE 65536	//possible number of kept motion strutcts in fifo

/* driver for reading from imu module
 *  reads data and writes it to imu_read_fifo fifo buffer
 */
void task_imu_read_driver(void);

/* setup of driver for reading data from imu module
 *  returns 0 if setup was ok, else 1
 */
uint8_t setup_imu_read(void);


/* 
 * Reset elapsed timer
 *  used in 'main.c', when saving GNSS data
 */
void reset_elapsed_tt(void);
/* 
 * Calculate elapsed time betwene measurements, when:
 *  calling task is not GNSS (first measurement)
 *  after GNSS has already been written (1st mask in 'if')
 *  before data flush has begun (2nd mask in 'if')
 * To write elapsed time, read global 'elapsed_meas' variable
 */
void calc_elapsed_tt(void);

#endif //tasks_H_
