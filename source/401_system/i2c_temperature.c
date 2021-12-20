/* RaspberryPi 4 for Automotive IoT Kit
 * File  : i2c_temperature.c
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

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#define TEMP_ADDR 0x48

volatile int g_loop=1;

void sigint_handle(int sig)
{
	printf("...Terminated!\n");
	g_loop = 0;
}

int main(int argc, char** argv)
{
	int temp_fd;
	int retv;
	char value=0;
	char reg=0;
	char regs[2]={0,};
	char values[2]={0,};

	float temp;

	// Signal Setting
	signal(SIGINT, sigint_handle); 

	// I2C Devce open.
	temp_fd = open("/dev/i2c-1",O_RDWR);
	if(temp_fd<3)
	{
		printf("Device Open Error!\n");
		return -1;
	}

	// Select I2C Address
	retv=ioctl(temp_fd, I2C_SLAVE, TEMP_ADDR);
	if(retv!=0)
	{
		printf("Ioctl Error!\n");
		return -1;
	}

	regs[0]=0x00;
	regs[1]=0x00;

	write(temp_fd,&regs,2);
	usleep(100000);

	while(g_loop)
	{

		read(temp_fd,values,2);
		temp = (((values[0]<<8) | values[1])>>4) * 0.0625;

		printf("Temperature : %4.4f\n",temp);
		usleep(100000);
	}

	close(temp_fd);

	return 0;
}
