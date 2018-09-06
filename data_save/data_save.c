#include "data_save.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// -- GLOBALS ------------------------------------------------------------------

data_fifo_t meas_fifo = {
    0,
    0,
	DATA_FIFO_BUF_SIZE+1,
    NULL
};

int32_t file_state;

FILE *data_file;

// -- TEST
//data_buffer_t my_buffer1;
data_buffer_t data_buf = {
    0,
    '\0',
    "\0",
    "\0",
    '\0',
    "\0",
    '\0',
    0.0,
    "\0",
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0
};

// -- LOCALS -------------------------------------------------------------------

static data_buffer_t tmp_buf;


// -- FUNS ---------------------------------------------------------------------

/* char *get_filename(void)
 *	return: unique filename
 */
char *get_filename(void) {
	//char *filename="test.txt";
	char *filename = malloc(256);
    // -- calendar time (Epoch time) - usually signed int
    time_t time_epoch;
    // -- pointer to time structure, for specifications view 'ctime' at 'man7
    struct tm *time_human;
    
    // -- save current time since Unix Epoch in seconds
    time_epoch = time(NULL);
    //time(&time_epoch);
    
    // -- save current broken down time to time structure
    time_human = localtime(&time_epoch);
 
    sprintf(filename, "measurements/meas-%04d-%02d-%02d_%02d-%02d-%02d.csv",
            time_human->tm_year+1900,time_human->tm_mon+1, time_human->tm_mday,
            time_human->tm_hour,time_human->tm_min, time_human->tm_sec);
    //sprintf(filename, "measurements/meas-%ld.txt", time(NULL));
	
    return filename;
}


/* int32_t write_file()
 *	write to given file, if buffer isn't empty (non-negative read_idx)
 *	 return: read index (-1 for empty buffer)
 */
int32_t write_file(FILE *ofp) {
	// -- read single line of data into temporary buffer and get read index
    int32_t read_idx = data_fifo_read(&meas_fifo, &tmp_buf);

	if (read_idx >= 0) {
        fprintf(ofp, 
        "%d,%c,%s,%s,%c,%s,%c,%f,%s,%f,%f,%f,%f,%f,%f\n",
	    tmp_buf.elapsed,
        tmp_buf.fix,
	    tmp_buf.time,
        tmp_buf.lat,
        tmp_buf.ni,
        tmp_buf.lon,
        tmp_buf.ei,
        tmp_buf.speed,
        tmp_buf.date,
        tmp_buf.ax, tmp_buf.ay, tmp_buf.az, 
        tmp_buf.gx, tmp_buf.gy, tmp_buf.gz);
	}
	return read_idx;
}


/* int32_t data_fifo_read(data_fifo_t *fifo, data_buffer_t *data)
*  copy from struct 'data' to new struct, part of 'fifo'
*  returns:
*   non-negative 	- write index
*   -1 				- fifo full
 */
int32_t data_fifo_write(data_fifo_t *fifo, data_buffer_t *data){
    int32_t tmp_write_idx = fifo->write_idx+1;
    
    // -- if not full
	if(tmp_write_idx != fifo->read_idx){
        // -- write elapsed time
		fifo->buffer[fifo->write_idx]->elapsed = data->elapsed;
        // -- if GPS writing enabled
        // -- only first measurement carries GPS data
        if ((int)(data->fix) == 255){
            data->fix = data->ni = data->ei = '\0';
            data->speed = -1.0;
            memcpy(data->time,"\0",1);
            memcpy(data->lat,"\0",1);
            memcpy(data->lon,"\0",1);
            memcpy(data->date,"\0",1);
        }
		fifo->buffer[fifo->write_idx]->fix = data->fix;
		memcpy(fifo->buffer[fifo->write_idx]->time, data->time, 10);
		memcpy(fifo->buffer[fifo->write_idx]->lat, data->lat, 11);
		fifo->buffer[fifo->write_idx]->ni = data->ni;
		memcpy(fifo->buffer[fifo->write_idx]->lon, data->lon, 12);
		fifo->buffer[fifo->write_idx]->ei = data->ei;
		fifo->buffer[fifo->write_idx]->speed = data->speed;
		memcpy(fifo->buffer[fifo->write_idx]->date, data->date, 7);
		fifo->buffer[fifo->write_idx]->ax = data->ax;
		fifo->buffer[fifo->write_idx]->ay = data->ay;
		fifo->buffer[fifo->write_idx]->az = data->az;
		fifo->buffer[fifo->write_idx]->gx = data->gx;
		fifo->buffer[fifo->write_idx]->gy = data->gy;
		fifo->buffer[fifo->write_idx]->gz = data->gz;
        fifo->write_idx = tmp_write_idx;
        return fifo->write_idx;
	}
	else{
		return -1;
	}
}


/* int32_t data_fifo_read(data_fifo_t *fifo, data_buffer_t *data)
*  returns:
*   non-negative 	- fifo->read_idx
*   -1 		    	- fifo empty
 */
int32_t data_fifo_read(data_fifo_t *fifo, data_buffer_t *data){
	if(fifo->read_idx != fifo->write_idx){
		data->elapsed = fifo->buffer[fifo->read_idx]->elapsed;
		data->fix = fifo->buffer[fifo->read_idx]->fix;
		memcpy(data->time, fifo->buffer[fifo->read_idx]->time, 10);
		memcpy(data->lat, fifo->buffer[fifo->read_idx]->lat, 11);
		data->ni = fifo->buffer[fifo->read_idx]->ni;
		memcpy(data->lon, fifo->buffer[fifo->read_idx]->lon, 12);
		data->ei = fifo->buffer[fifo->read_idx]->ei;
		data->speed = fifo->buffer[fifo->read_idx]->speed;
		memcpy(data->date, fifo->buffer[fifo->read_idx]->date, 7);
		data->ax = fifo->buffer[fifo->read_idx]->ax;
		data->ay = fifo->buffer[fifo->read_idx]->ay;
		data->az = fifo->buffer[fifo->read_idx]->az;
		data->gx = fifo->buffer[fifo->read_idx]->gx;
		data->gy = fifo->buffer[fifo->read_idx]->gy;
		data->gz = fifo->buffer[fifo->read_idx]->gz;
		fifo->read_idx = (fifo->read_idx+1)%fifo->buf_size;
		return fifo->read_idx;
	}
	else{
		return -1;
	}
}


/* int32_t data_fifo_elapsed_sub (data_fifo_t *fifo) {
*   Iterate 'data_save' fifo buffer and substract elapsed time between GNSS and
*   first IMU measurement from all IMU measurements.
*    returns:
*     non-negative 	- fifo->read_idx
*     -1 		    - fifo empty
 */
int32_t data_fifo_elapsed_sub (data_fifo_t *fifo) {
	if(fifo->read_idx != fifo->write_idx){
        // -- first IMU measurement elapsed time after GNSS slice
        uint32_t substract = fifo->buffer[fifo->read_idx + 1]->elapsed;
        // -- start and stop index for substracting elapsed time
        uint32_t i_start = fifo->read_idx + 1;
        uint32_t i_stop = fifo->write_idx;
        uint32_t i = i_start;
        // -- substract elapsed time. Stop when you hit write_idx
        while (i != i_stop) {
            fifo->buffer[i]->elapsed -= substract;
            i = (i + 1) % fifo->buf_size; 
        }
		return fifo->read_idx;
	}
	else{
		return -1;
	}
}


/* int setup_data_buffer(void)
 *  set up data buffer
 *  returns:
 *   1 - buffer setup successful
 *   0 - error
 */
int32_t setup_data_buffer(void) {
	// -- pointer to array of pointers to structures "data_buffer_t"
	data_buffer_t **data_fifo_buff =
    (data_buffer_t **) malloc(sizeof(data_buffer_t *)*DATA_FIFO_BUF_SIZE+1);

    for(int32_t i=0; i < DATA_FIFO_BUF_SIZE+1; i++) {
		// -- individual pointer for each data_buffer_t
        data_buffer_t *tmp_p = (data_buffer_t *) malloc(sizeof(data_buffer_t));
        data_fifo_buff[i] = tmp_p;
    }
    
    meas_fifo.buffer = data_fifo_buff;
	return 1;
}

