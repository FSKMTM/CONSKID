#include "../supervisor.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// -- MACROS -------------------------------------------------------------------

// -- number of data_buffer structures inside of fifo buffer
// -- actual size = DATA_FIFO_BUF_SIZE * sizeof(data_buffer_t)
// -- estimated about 65536 * 84 B = 5.25 MB
// -- running at 100 Hz it is enough for about 10 min of measuring 
#define DATA_FIFO_BUF_SIZE 65536

#define FILE_CLOSED 0
#define FILE_OPEN 1


// -- TYPEDEFS -----------------------------------------------------------------

// -- single data package BUFFER
struct data_buffer {
    // -- elapsed time in 'ms'
    uint32_t elapsed;
	// -- gnss
    char fix;
	char time[10];
	char lat[11];
	char ni;
	char lon[12];
	char ei;
	float speed;
	char date[7];
    // -- imu/mpu
	float ax, ay, az, gx, gy, gz;
};
typedef struct data_buffer data_buffer_t;

// -- FIFO of data buffer packages
struct data_fifo {
    int32_t write_idx;
    int32_t read_idx;
	int32_t buf_size;
    data_buffer_t **buffer;
    //char **buffer;
};
typedef struct data_fifo data_fifo_t;


// -- GLOBALS ------------------------------------------------------------------

extern data_fifo_t meas_fifo;
extern data_buffer_t data_buf;
extern int32_t file_state;
extern FILE *data_file;


// -- FUNS ---------------------------------------------------------------------

/* char *get_filename(void)
 *	return: unique filename
 */
char *get_filename(void);

/* int32_t write_file()
 *	write to given file, if buffer isn't empty (non-negative read_idx)
 *	return: read index (-1 for empty buffer)
 */
int32_t write_file(FILE *ofp);

/* int32_t data_fifo_read(data_fifo_t *fifo, data_buffer_t *data)
*  returns:
*   non-negative 	- write index
*   -1 						- fifo full
 */
int32_t data_fifo_write(data_fifo_t *fifo, data_buffer_t *data);

/* int32_t data_fifo_read(data_fifo_t *fifo, data_buffer_t *data)
*  returns:
*   non-negative 	- fifo->read_idx
*   -1 						- fifo empty
 */
int32_t data_fifo_read(data_fifo_t *fifo, data_buffer_t *data);

/* int32_t data_fifo_elapsed_sub (data_fifo_t *fifo) {
*   Iterate 'data_save' fifo buffer and substract elapsed time between GNSS and
*   first IMU measurement from all IMU measurements.
*    returns:
*     non-negative 	- fifo->read_idx
*     -1 		    - fifo empty
 */
int32_t data_fifo_elapsed_sub (data_fifo_t *fifo);

/* int setup_data_buffer(void)
 *  set up data buffer
 *  returns:
 *   1 - buffer setup successful
 *   0 - error
 */
int32_t setup_data_buffer(void);
