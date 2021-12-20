/* RaspberryPi 4 for Automotive IoT Kit
 * File  : ~/source/401_system/i2c_temperature.c
 * TITLE : Reading data from Temperature Sensor(TMP112)
 * Auth  : mkdev.co.kr / makeutil.tistory.com
 * Ment  : This program code may not be used for commercial purposes. 
 *         The author's information cannot be changed at the time of redistribution.*/

// Define the header & constants
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include <linux/i2c-dev.h>

#include "../aiot_hardware.h"


volatile int g_loop=1;

void sigint_handler(int sig)
{
	printf("Exiting the Program. Wait a moment...!\n");
	g_loop = 0;
}

int main(int argc, char** argv) {
	int temp_fd;
	int retv;
	char value=0;
	char reg=0;
	char i2c_reg[2]={0,};
	char values[2]={0,};

	float temp_data;

	// Signal Setting
	signal(SIGINT, sigint_handler); 

	// I2C Devce open.
	temp_fd = open("/dev/i2c-1",O_RDWR);
	if ( temp_fd < 3 ) {
		printf("Device Open Error!\n");
		return -1;
	}

	// Select I2C Address
	retv=ioctl(temp_fd, I2C_SLAVE, I2C_TEMP);
	if ( retv != 0 ) {
		printf("Ioctl Error!\n");
		return -1;
	}

	i2c_reg[0]=0x00;
	i2c_reg[1]=0x00;

	write(temp_fd,&i2c_reg,2);
	usleep(100000);

	while(g_loop) {
		read(temp_fd,values,2);
		temp_data = (((values[0]<<8) | values[1])>>4) * 0.0625;

		printf("Temperature : %4.4f\n",temp_data);
		usleep(100000);
	}

	close(temp_fd);

	return 0;
}
