#include "fifo.h"
#include <stdint.h>

/* FIFO FOR STRINGS */
/* str_fifo_read(str_fifo_t *fifo, char *data)
 */
uint32_t str_fifo_read(str_fifo_t *fifo, char *data){
	uint32_t i=0;

	if(fifo->write_idx != fifo->read_idx){
		for(i=0; i < fifo->str_size; i++){
			data[i] = fifo->buffer[fifo->read_idx][i];
		}
		fifo->read_idx = (fifo->read_idx+1)%fifo->buf_size;
		return 1;
	}
	else{
		return 0;
	}
}

/* str_fifo_write(str_fifo_t *fifo, char *data)
 */
uint32_t str_fifo_write(str_fifo_t *fifo, char *data){
	uint32_t i=0;
	uint32_t tmp_write_idx = (fifo->write_idx+1)%fifo->buf_size;

	if(tmp_write_idx != fifo->read_idx){
		for(i=0; i < fifo->str_size; i++){
			fifo->buffer[fifo->write_idx][i] = data[i];
		}
		fifo->write_idx = tmp_write_idx;
		return 1;
	}
	else{
		return 0;
	}
}

/* FIFO FOR MOTION STRUCTS */
/* motion_fifo_read(motion_fifo_t *fifo, motion_bit *data)
 */
uint32_t motion_fifo_read(motion_fifo_t *fifo, motion_bit *data){
	if(fifo->write_idx != fifo->read_idx){
		data->ax = fifo->buffer[fifo->read_idx].ax;
		data->ay = fifo->buffer[fifo->read_idx].ay;
		data->az = fifo->buffer[fifo->read_idx].az;
		data->gx = fifo->buffer[fifo->read_idx].gx;
		data->gy = fifo->buffer[fifo->read_idx].gy;
		data->gz = fifo->buffer[fifo->read_idx].gz;

		fifo->read_idx = (fifo->read_idx+1)%fifo->buf_size;
		return 1;
	}
	else{
		return 0;
	}
}

/* motion_fifo_write(motion_fifo_t *fifo, motion_bit *data)
 */
uint32_t motion_fifo_write(motion_fifo_t *fifo, motion_bit *data){
	uint32_t tmp_write_idx = (fifo->write_idx+1)%fifo->buf_size;

	if(tmp_write_idx != fifo->read_idx){

		(fifo->buffer[fifo->write_idx]).ax = data->ax;
		(fifo->buffer[fifo->write_idx]).ay = data->ay;
		(fifo->buffer[fifo->write_idx]).az = data->az;
		(fifo->buffer[fifo->write_idx]).gx = data->gx;
		(fifo->buffer[fifo->write_idx]).gy = data->gy;
		(fifo->buffer[fifo->write_idx]).gz = data->gz;

		fifo->write_idx = tmp_write_idx;
		return 1;
	}
	else{
		return 0;
	}
}
