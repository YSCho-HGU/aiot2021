/* RaspberryPi 4 for Automotive IoT Kit
 * File  : i2c_eerom_wr.c
 * TITLE : EEPROM R/W Example
 * Auth  : mkdev.co.kr / makeutil.tistory.com
 * Ment  : This program code may not be used for commercial purposes. 
 *         The author's information cannot be changed at the time of redistribution.*/


// Define the header & constants
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include <linux/i2c-dev.h>
#include "../aiot_hardware.h"

#define LEDSW_ADDR 0x50

int main(int argc, char** argv)
{
	int e2p_fd;
	int retv;
	char value=0;
	char values[2]={0,};

	if(argc!=2)
	{
		printf("Illegal Option\n");
		printf("ex) %s String\n",argv[0]);
		return -1;
	}

	printf("* Input String : %s (Length : %d)\n",argv[1],strlen(argv[1]));

	// I2C Devce open.
	e2p_fd = open("/dev/i2c-1",O_RDWR);
	if(e2p_fd<3)
	{
		printf("Device Open Error!\n");
		return -1;
	}

	// Select I2C Address
	retv=ioctl(e2p_fd, I2C_SLAVE, LEDSW_ADDR);
	if(retv!=0)
	{
		printf("Ioctl Error!\n");
		return -1;
	}
	
	printf("+ Data Writing...\n");
	for(int repi=0; repi<strlen(argv[1]); repi++)
	{
		values[0]=repi;
		values[1]=argv[1][repi];
		write(e2p_fd,values,2);
		usleep(100000);
	}

	printf("+ Data Reading...\n");
	value=0x00;
	write(e2p_fd,&value,1);

	printf("* OutPut String : ");
	for(int repi=0; repi<strlen(argv[1]); repi++)
	{
		read(e2p_fd,&value,1);
		printf("%c",value);
		usleep(100000);
	}
	
	printf("\n...Finished!\n");

	close(e2p_fd);

	return 0;
}
