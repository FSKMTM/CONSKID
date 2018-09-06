#include <stdio.h> /* printf, sprintf */
#include <stdlib.h> /* exit */
#include <unistd.h> /* read, write, close */
#include <string.h> /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h> /* struct hostent, gethostbyname */
#include <time.h>
#include "requests.h"
#include "../fifo/fifo.h"
#include "../tasks/tasks.h"


// -- GLOBALS ------------------------------------------------------------------
int32_t socket_state, request_data_len;

// -- ThingSpeak
// -- find fresh IP through the terminal: "nslookup api.thingspeak.com"
/*char *host =  "52.54.134.167";
char request_fmt[1024] =  "POST /update HTTP/1.1\r\n"
                          "Host: %s\r\n"
                          "Content-Type: application/x-www-form-urlencoded\r\n"
                          "Content-Length: %d\r\n\r\n"
                          "%s\r\n\r\n";*/
// -- LOCAL environment
char *host = "127.1.0.0";
char request_fmt[REQUEST_LEN] =
            "POST /test_api2/api.php HTTP/1.1\r\n"
            "Host: %s\r\n"
            "Content-Type: application/x-www-form-urlencoded\r\n"
            "Content-Length: %d\r\n\r\n"
            "%s\r\n\r\n";

char  request[REQUEST_LEN],
      response[RESPONSE_LEN],
      request_data[REQUEST_FIFO_STR_SIZE+1];


// -- LOCALS -------------------------------------------------------------------
// -- for measuring time needed in individual socket state
static struct timespec socket_state_start={0,0}, socket_state_end={0,0};
static int32_t socket_elapsed_time = 0;

static int32_t sockfd;
static struct sockaddr_in serv_addr;

static int32_t portno = 80;

// -- requests fifo buffer
static char **request_fifo_buf=NULL;
str_fifo_t request_fifo = {
	0,
	0,
	REQUEST_FIFO_BUF_SIZE+1,
	REQUEST_FIFO_STR_SIZE+1,
	NULL
};


// -- FUNS ---------------------------------------------------------------------

/* int32_t setup_socket (void)
 *  validate host address, set up server structure
 *  returns:
 *   1 - setup successful
 *   0 - error setting up
 */
int32_t setup_socket (void) {
  struct hostent *server;
  // -- lookup host
  server = gethostbyname(host);
  if (server != NULL) {
    socket_state = SOCKET_CLOSED;
  } else {
    socket_state = SOCKET_UNKNOWN_HOST;
    printf("SOCKET: UNKNOWN HOST");
    return 0;
  }

  // -- initialize server
  memset(&serv_addr,0,sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(portno);
  memcpy(&serv_addr.sin_addr.s_addr,server->h_addr,server->h_length);

  return 1;
}


/* int32_t create_socket(void)
 *  select socket protocol and stream, create socket
 *  reset socket state elapsed time timer
 *  returns:
 *   1 - successfully connected
 *   0 - error connecting
 */
int32_t create_socket(void) {
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd >= 0) {
    socket_state = SOCKET_CREATED;
    // -- reset state timer
		clock_gettime(CLOCK_MONOTONIC, &socket_state_start);
    return 1;
  }
return 0;
}


/* void connect_socket(void)
 *  connect socket
 *  reset socket state elapsed time timer
 *  exit if maximum socket state elapsed time is reached (close socket)
 */
void connect_socket(void) {
  // -- try connecting
  if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) >= 0) {
      socket_state = SOCKET_CONNECTED;
      // -- reset state timer
  		clock_gettime(CLOCK_MONOTONIC, &socket_state_start);
  }

  // -- get elapsed time in socket state
  clock_gettime(CLOCK_MONOTONIC, &socket_state_end);
  socket_elapsed_time = socket_state_end.tv_sec - socket_state_start.tv_sec;
  // -- max allowed time in socket state has passed
  if (socket_elapsed_time >= MAX_SOCKET_TIME) {
    printf("MAX ALLOWED SOCKET TIME REACHED IN: %d\n", socket_state);
    socket_state = SOCKET_CLOSE_PENDING;
  }

  return;
}


/* void write_socket(void)
 *  write request to socket
 *  enable reentry, if writing process gets interrupted
 *  stop, when everything is written using single function call, or
 *   nothing new was written to the socket (no data left to write)
 *  reset socket state elapsed time timer
 *  exit if maximum socket state elapsed time is reached (close socket)
 */
void write_socket(void) {
  static int32_t sent = 0;
  int32_t bytes_write, request_len;

  request_len = strlen(request);
  // (socket, buffer pointer + offset, number of characters to send)
  bytes_write = write(sockfd, request + sent,
                      request_len - sent);
  sent+=bytes_write;
  // -- finished writing
  #if(DEBUG_PRINTS==1)
  printf("***REQUEST: \n%s\n", request);
  #endif
  if (request_len == sent || bytes_write == 0) {
    sent = 0;
    socket_state = SOCKET_WRITE_FINISHED;
    // -- reset state timer
		clock_gettime(CLOCK_MONOTONIC, &socket_state_start);
  }

  // -- get elapsed time in socket state
  clock_gettime(CLOCK_MONOTONIC, &socket_state_end);
  socket_elapsed_time = socket_state_end.tv_sec - socket_state_start.tv_sec;
  // -- max allowed time in socket state has passed
  if (socket_elapsed_time >= MAX_SOCKET_TIME) {
    printf("MAX ALLOWED SOCKET TIME REACHED IN: %d\n", socket_state);
    socket_state = SOCKET_CLOSE_PENDING;
  }
  return;
}


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
void read_socket(void) {
  // -- get elapsed time in socket state
  clock_gettime(CLOCK_MONOTONIC, &socket_state_end);
  socket_elapsed_time = socket_state_end.tv_sec - socket_state_start.tv_sec;
  // -- minimal time has passed after writing
  if (socket_elapsed_time >= SOCKET_READ_WRITE_INTERVAL) {
    static int32_t received = 0;
    int32_t bytes_read, response_len;

    // -- set memory only on first call (otherwise response gets overwritten)
    if (received == 0) {
      memset(response,0,sizeof(response));
      response_len = sizeof(response)-1;
    }

    bytes_read = read(sockfd,response+received,response_len-received);
    received+=bytes_read;
    //reached end of response or end of buffer
    if ( response_len == received || bytes_read == 0) {
      #if(DEBUG_PRINTS==1)
      printf("***RESPONSE: \n%s\n", response);
      #endif
      received = 0;
      socket_state = SOCKET_READ_FINISHED;
      // -- reset state timer
  		clock_gettime(CLOCK_MONOTONIC, &socket_state_start);
    }

    // -- max allowed time in socket state has passed
    if (socket_elapsed_time >= MAX_SOCKET_TIME) {
      printf("MAX ALLOWED SOCKET TIME REACHED IN: %d\n", socket_state);
      socket_state = SOCKET_CLOSE_PENDING;
    }
  }

  return;
}


/* void close_socket(void)
 *  close socket
 */
void close_socket(void) {
  close(sockfd);
  socket_state = SOCKET_CLOSED;
  return;
}


/* int setup_request_buffer(void)
 *  set up request data buffer (only for data, not full request!)
 *  returns:
 *   1 - buffer setup successful
 *   0 - error
 */
int32_t setup_request_buffer(void) {

	request_fifo_buf = (char **) malloc(sizeof(char *)*(REQUEST_FIFO_BUF_SIZE+1));
	int i = 0;

	for(i=0; i < REQUEST_FIFO_BUF_SIZE+1; i++){
		char *tmp_p = (char *) malloc(sizeof(char)*(REQUEST_FIFO_STR_SIZE+1));
		request_fifo_buf[i] = tmp_p;
	}

	request_fifo.buffer = request_fifo_buf;
	return 0;
}


/* void dummy_request(void)
 *  generate dummy request data -> fill buffer
 *  ThingSpeak channel located at: https://thingspeak.com/channels/488607
 *  find fresh IP through the terminal: "nslookup api.thingspeak.com"
 */
void dummy_request(void) {
  srand ( time(NULL) );     // -- seed the random function
  float data1;
  int32_t data2;

  // -- fill buffer
	int32_t write_ok = 1;
  if (!setup_request_buffer()) {
    while (write_ok) {
      data1 = rand() % 200 * 0.1;
      data2 = rand() % 20;

      // -- ThingSpeak
      //sprintf(request_data, "api_key=%s&field1=%d", "7HN4EKKQ3ZKMOD92", data2);
      // -- Local
      sprintf(request_data, "w_speed=%.2f&w_direction=%d", data1, data2);

      write_ok=str_fifo_write(&request_fifo, request_data);
      #if(DEBUG_PRINTS==1)
      printf("***REQUEST DATA ADDED: \n%s\n", request_data);
      #endif
    }
  }
  return;
}
