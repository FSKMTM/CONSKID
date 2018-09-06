#ifndef _REQUESTS_H_
#define _REQUESTS_H_


#include <stdio.h> /* printf, sprintf */
#include <stdlib.h> /* exit */
#include <unistd.h> /* read, write, close */
#include <string.h> /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h> /* struct hostent, gethostbyname */
#include <time.h>
#include "../fifo/fifo.h"


// -- debug
#ifndef DEBUG_PRINTS
#define DEBUG_PRINTS 0
#endif

// -- socket states
#define SOCKET_UNKNOWN_HOST -1
#define SOCKET_CLOSED 0
#define SOCKET_CREATED 1
#define SOCKET_CONNECTED 2
#define SOCKET_WRITE_FINISHED 3
#define SOCKET_READ_FINISHED 4
#define SOCKET_CLOSE_PENDING 4

// -- request buffer
#define REQUEST_FIFO_BUF_SIZE 10	//possible number of kept strings in fifo
#define REQUEST_FIFO_STR_SIZE 128	//size of string to be kept in fifo

// -- socket timing and time intervals
#define MAX_SOCKET_TIME 20    // -- max seconds in individual socket state
#define SOCKET_READ_WRITE_INTERVAL 5  // -- seconds between writing and reading

// -- requset, response and other array lengths
#define REQUEST_LEN 1024
#define RESPONSE_LEN 1024


// -- GLOBALS ------------------------------------------------------------------
extern str_fifo_t request_fifo;

extern int32_t socket_state, request_data_len;

extern char *host;

extern char request_fmt[REQUEST_LEN];
extern char request[REQUEST_LEN], response[RESPONSE_LEN],
            request_data[REQUEST_FIFO_STR_SIZE+1];


// -- FUNS ---------------------------------------------------------------------

/* int32_t setup_socket (void)
 *  validate host address, set up server structure
 *  returns:
 *   1 - setup successful
 *   0 - error setting up
 */
int32_t setup_socket(void);

/* int32_t create_socket(void)
 *  select socket protocol and stream, create socket
 *  reset socket state elapsed time timer
 *  returns:
 *   1 - successfully connected
 *   0 - error connecting
 */
int32_t create_socket(void);

/* void connect_socket(void)
 *  connect socket
 *  reset socket state elapsed time timer
 *  exit if maximum socket state elapsed time is reached (close socket)
 */
void connect_socket(void);

/* void write_socket(void)
 *  write request to socket
 *  enable reentry, if writing process gets interrupted
 *  stop, when everything is written using single function call, or
 *   nothing new was written to the socket (no data left to write)
 *  reset socket state elapsed time timer
 *  exit if maximum socket state elapsed time is reached (close socket)
 */
void write_socket(void);

/* void read_socket(void)
 *  read response from socket
 *  minimum time must pass between writing and reading, to accomodate for
 *   slower connections/servers and prevent non-deterministic behaviour
 *  enable reentry, if reading process gets interrupted
 *  stop, when everything is received using single function call, or
 *   nothing new was received on the socket (no data left to read)
 *  reset socket state elapsed time timer
 *  exit if maximum socket state elapsed time is reached (close socket)
 */
void read_socket(void);

/* void close_socket(void)
 *  close socket
 */
void close_socket(void);

/* int setup_request_buffer(void)
 *  set up request data buffer (only for data, not full request!)
 *  returns:
 *   1 - buffer setup successful
 *   0 - error
 */
int32_t setup_request_buffer(void);

/* void dummy_request(void)
 *  generate dummy request data -> fill buffer
 *  ThingSpeak channel located at: https://thingspeak.com/channels/488607
 *  find fresh IP through the terminal: "nslookup api.thingspeak.com"
 */
void dummy_request(void);

#endif //_REQUESTS_H_
