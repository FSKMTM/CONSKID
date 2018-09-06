CC = gcc
CFLAGS = -g -Wall -I. -lm
# -- list of dependencies -> header files
DEPS =			supervisor.h								\
				serial_read/serial_read.h					\
				tasks/tasks.h								\
				rtos/rtos.h									\
				fifo/fifo.h									\
				requests/requests.h							\
				i2c/i2c.h									\
				mpu_9250/mpu_9250.h							\
				measurement_flow/measurement_flow.h			\
				gpio/gpio.h									\
				data_save/data_save.h		

# -- list of objet files
OBJ = 			main.o										\
				serial_read/serial_read.o					\
				tasks/tasks.o								\
				rtos/rtos.o									\
				fifo/fifo.o									\
				requests/requests.o							\
				i2c/i2c.o									\
				mpu_9250/mpu_9250.o							\
				measurement_flow/measurement_flow.o			\
				gpio/gpio.o									\
				data_save/data_save.o

# -- list of public libraries
#EXTRA_LIBS = -lwiringPi
EXTRA_LIBS = -lwiringPi -lpthread

# -- list of phony targets
.PHONY: clean

# -- default command
all: main

# -- make main module
main: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(EXTRA_LIBS)

# -- object files assembly rule
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

# -- delete .o files in main directory and sub-directories
clean:
	rm *.o */*.o
