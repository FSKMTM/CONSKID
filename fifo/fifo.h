#ifndef FIFO_H_
#define FIFO_H_

#include <stdint.h>
#include "../mpu_9250/mpu_9250.h"

/* FIFO FOR STRINGS */
struct _str_fifo {
	uint32_t read_idx;
	uint32_t write_idx;
	uint32_t buf_size;
	uint32_t str_size;
	char **buffer;
};

typedef struct _str_fifo str_fifo_t;

/* str_fifo_read(fifo_t *fifo, char *data);
 *  function for reading from fifo buffer of strings
 *   fifo - address of fifo for reading
 *   data - address of where read data are stored
 *
 *   returns 1 if data was successfully read, else 0 (buffer empty)
 */
uint32_t str_fifo_read(str_fifo_t *fifo, char *data);

/* str_fifo_write(fifo_t *fifo, char *data);
 *  function for writing to fifo buffer of strings
 *   fifo - address of fifo for writing
 *   data - address of data to be written into fifo
 *
 *   returns 1 if data was successfully written, else 0
 */
uint32_t str_fifo_write(str_fifo_t *fifo, char *data);

/* FIFO FOR MOTION STRUCTS */
struct _motion_fifo {
	uint32_t read_idx;
	uint32_t write_idx;
	uint32_t buf_size;
	motion_bit *buffer;
};

typedef struct _motion_fifo motion_fifo_t;

/* motion_fifo_read(motion_fifo_t *fifo, motion_bit *data);
 *  function for reading from fifo buffer of motion structs
 *   fifo - address of fifo for reading
 *   data - address of where read data are stored
 *
 *   returns 1 if data was successfully read, else 0 (buffer empty)
 */
uint32_t motion_fifo_read(motion_fifo_t *fifo, motion_bit *data);

/* motion_fifo_write(motion_fifo_t *fifo, motion_bit *data);
 *  function for writing to fifo buffer of motion structs
 *   fifo - address of fifo for writing
 *   data - address of data to be written into fifo
 *
 *   returns 1 if data was successfully written, else 0
 */
uint32_t motion_fifo_write(motion_fifo_t *fifo, motion_bit *data);

#endif //FIFO_H_
