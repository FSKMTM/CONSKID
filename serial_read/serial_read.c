#include "serial_read.h"
#include <stdio.h>

/*###### LOCAL VARIABLES ######*/

static int fdsrc;
static int nbytes;
static char buf[BUF_SIZE];

static gnss curr, prev;

/* int init_gnss_read()
 *  initialize serial reading from gnss
 *  returns 0 if successful, 1 - cannot open device for reading
 */
int init_gnss_read(){

        /* Open source file for reading */
        fdsrc = open(DEVICE, O_RDONLY);
	if(fdsrc<0){
	        fprintf(stderr, "Error opening %s for reading: %s\n", DEVICE, strerror(errno));
		return 1;
	}

	return 0;
}

/* returns current gnss position
 */
gnss get_curr_gnss_position(){
    return curr;
}

/* int gnss_read(char *read_data, int read_data_size)
 *  reads from gnss module
 *  1 - read successful, 0 - error
 */
int gnss_read(char *read_data, int read_data_size){
	static int tmp_read = 0;	//for countering problem of double output (each second output is the same)

	/* reads from gnss module until a fix position is given
	 */
	nbytes = read(fdsrc, buf, BUF_SIZE);

	/* validating read output
	 */
	if(nbytes == 0) return 1; //EOF - we are done
	if(nbytes < 0){
		fprintf(stderr, "Error reading file: %s\n", strerror(errno));
		return 0;
	}

	#if(DEBUG_PRINTS==1)
	printf("ALL: %s\n", buf);
	#endif

	if(tmp_read==0){
		tmp_read=1;
	}
	else if(tmp_read==1){
		tmp_read=0;
	}

	/* copying read data from buf to output read_data
	 *  if the read data from output are larger then read_data_size -> error (stop copying)
	 */
	int i=0;
	for(i=0; i<read_data_size-1;i++){
		read_data[i] = buf[i];
		if(buf[i]==0) break;	//we came to the end of the string (0 terminated)
	}
	read_data[read_data_size-1] = 0;

	return 1;
}

/* int gnss_format_output(char *data)
 *  checks if the data is in the correct form to present a valid gnss output
 *   returns 1 if the output is in valid form, it saves the data in GNSS form, and copies the last GNSS data from to previous
 *   if message is not valid, it returns 0
 */
int gnss_format_output(char *data){
	int tmp_sscanf = 0;
	char buf_begin[7];		//buffer containing first 5 letters of NMEA message
	char buf_data[BUF_SIZE];	//buffer containing other parts of the message
	char buf_else[64];

	gnss tmp;

	/* converting read data - validating fix ???
	 */
	tmp_sscanf = sscanf(data,"$%5s,%s", buf_begin, buf_data);

//	printf("BB: %s, %s\n", buf_begin, NMEA_MESSAGE);
//	printf("EOF:%d, %d\n", EOF, tmp_sscanf);

	if(tmp_sscanf != 2) return 0;
	if( strcmp(buf_begin,NMEA_MESSAGE)==0 ){
		#if(DEBUG_PRINTS==1)
		printf("VALID MESSAGE!\n");
		#endif

		tmp_sscanf = sscanf(buf_data,"%9s,%c,%10s,%c,%11s,%c,%f,,%6s,%s", tmp.time, &tmp.fix, tmp.lat, &tmp.ni, tmp.lon, &tmp.ei, &tmp.speed, tmp.date, buf_else);
		if(tmp_sscanf != 9) return 0;
		if(tmp.fix == 'V') return 0;

		#if(DEBUG_PRINTS==1)
		printf("VAILD POSITION\n");
		printf("%s,%c,%s,%c,%s,%c,%.3f,%s\n", tmp.time, tmp.fix, tmp.lat, tmp.ni, tmp.lon, tmp.ei, tmp.speed, tmp.date);
		#endif

		copy_gnss_struct(&curr,&prev);
		copy_gnss_struct(&tmp,&curr);

		//for testing purposes
/*		float lat_err, lon_err, speed_err;
		long int time_passed;
		margin_of_error(curr, &lat_err, &lon_err, &speed_err, &time_passed);
		printf("lat_err: %.4f, lon_err: %.4f, speed_err: %.3f, passed_time: %ld \n", lat_err, lon_err, speed_err, time_passed);
*/
		return 1;
	}
	else return 0;
}

/* void copy_gnss_struct(gnss *from, gnss *to)
 *  copies content of type gnss from one variable to another
 */
void copy_gnss_struct(gnss *from, gnss *to){

	to->fix = from->fix;
	strcpy(to->time,from->time);
	strcpy(to->lat,from->lat);
	(*to).ni = (*from).ni;
	strcpy(to->lon,from->lon);
	to->ei = from->ei;
	(*to).speed = (*from).speed;
	strcpy(to->date,from->date);

}

/* void margin_of_error(gnss curr, float *lat_err, float *lon_err, float *speed_err, long int *time_passed)
 *  keeps track of min and max values of latitude, longitude and speed
 *  from that it calculates max error as difference between max and min values (in minutes/knots)
 */
void margin_of_error(gnss curr, float *lat_err, float *lon_err, float *speed_err, long int *time_passed){

	static long int start_time = 0;
	long int tmp_time = 0;

	static float lat_min = 91*60;
	static float lat_max = -91*60;
	static float lon_min = 181*60;
	static float lon_max = -181*60;
	static float speed_min = 1000;
	static float speed_max = -1;

	float lat_tmp, lon_tmp, speed_tmp;

	char tmp1[4];
	char tmp2[8];
	char tmp3[3];

	char **tmp_endpt = NULL;

	/* for measuring time
	 * at first call, memorise start time
	 */
	sscanf(curr.time,"%2s%2s%2s.**",tmp1,tmp2,tmp3);
	tmp_time = 3600*strtol(tmp1,tmp_endpt, 10) + 60*strtol(tmp2,tmp_endpt, 10) + strtol(tmp3,tmp_endpt, 10);
	if(start_time==0){
		start_time = tmp_time;
	}
	*time_passed = tmp_time - start_time;


	sscanf(curr.lat,"%2s%7s",tmp1,tmp2);

	lat_tmp = strtof(tmp1,tmp_endpt)*60 + strtof(tmp2,tmp_endpt);

	if(lat_tmp < lat_min){
		lat_min = lat_tmp;
	}
	if(lat_tmp > lat_max){
		lat_max = lat_tmp;
	}

	sscanf(curr.lon,"%3s%7s",tmp1,tmp2);

	lon_tmp = strtof(tmp1,tmp_endpt)*60 + strtof(tmp2,tmp_endpt);

	if(lon_tmp < lon_min){
		lon_min = lon_tmp;
	}
	if(lon_tmp > lon_max){
		lon_max = lon_tmp;
	}

	speed_tmp = curr.speed;

	if(speed_tmp < speed_min){
		speed_min = speed_tmp;
	}
	if(speed_tmp > speed_max){
		speed_max = speed_tmp;
	}

	*lat_err = lat_max - lat_min;
	*lon_err = lon_max - lon_min;
	*speed_err = speed_max - speed_min;
}
