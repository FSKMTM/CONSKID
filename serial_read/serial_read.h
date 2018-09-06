#ifndef SERIAL_READ_H_
#define SERIAL_READ_H_

/*################ NA LINUXU MORA BITI IZKLOPLJEN ECHO na tty (stty -F/dev/ttyACM0 -echo), ker ga drugace to zmoti #####*/

/*###### INCLUDES ######*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/*###### DEFINES ######*/

#ifndef DEBUG_PRINTS
#define DEBUG_PRINTS 0
#endif

#define BUF_SIZE 256		//size of buffer for reading from gnss
#define DEVICE "/dev/ttyACM0"	//name of gnss module/device in Linux

#define NMEA_MESSAGE "GNRMC"	//start of valid NMEA message

/*###### STRUCTS ######*/

/* struct for GNSS data
 *  char fix:
 *	A - valid position
 * 	V - notvalid position (as received by GNSS)
 *  char time[9] - time as received by GNSS (hhmmss.ss)
 *  char lat[10] - latitude as reveived by GNSS (ddmm.mmmm)
 *  char ni - northing indicator (N/S)
 *  char lon[11] - longitude as received by GNSS (dddmm.mmmm)
 *  char ei - easting indicator (E/W)
 *  float speed - speed as received by GNSS (knots)
 *  char date[6] - date as received by GNSS (ddmmyy)
 */
struct _gnss_struct {
	char fix;
	char time[10];
	char lat[11];
	char ni;
	char lon[12];
	char ei;
	float speed;
	char date[7];
};
typedef struct _gnss_struct gnss;


/*###### EXTERN VARIABLES ######*/

/*###### FUNCTION DEFINITIONS ######*/

/* int init_gnss_read()
 *  initialize serial reading from gnss
 *  returns 0 if successful, 1 - cannot open device for reading
 */
int init_gnss_read();

/* returns current gnss position
 */
gnss get_curr_gnss_position();

/* int gnss_read(char *read_data, int read_data_size)
 *  reads form gnss module
 *  returns:
 *   1 - read successful -> read data is saved in read_data
 *   0 - error reading
 */
int gnss_read(char *read_data, int read_data_size);

/* int gnss_format_output(char *data)
 *  checks if the data is in the correct form to present a valid gnss output
 *   returns 1, if the output is in valid form, it saves the data in GNSS form (curr) and copies the last info in curr to prev
 *   if message is not valid, it returns 0
 */
int gnss_format_output(char *data);

/* void copy_gnss_struct(gnss *from, gnss *to)
 *  copies content of type gnss from one variable to another
 */
void copy_gnss_struct(gnss *from, gnss *to);

/* void margin_of_error(gnss curr, float *lat_err, float *lon_err, float *speed_err, long int *time_passed)
 *  keeps track of min and max values of latitude, longitude and speed
 *  from that it calculates max error as difference between max and min values (in minutes/knots)
 *  each error is written in the coresponding (###_err) variable
 */
void margin_of_error(gnss curr, float *lat_err, float *lon_err, float *speed_err, long int *time_passed);


#endif //SERIAL_READ_H_
