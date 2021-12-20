/* RaspberryPi 4 for Automotive IoT Kit
 * File  : i2c_eerom_write.c
 * TITLE : Write data to EEPROM(AT24C04)
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


int main(int argc, char** argv)
{
	int e2p_fd;
	int retv;
	char value=0;
	char data[12]="Hello World";
	char values[2]={0,};

	// I2C Device open.
	e2p_fd = open("/dev/i2c-1",O_RDWR);
	if(e2p_fd<3) {
		printf("Device Open Error!\n");
		return -1;
	}

	// Select I2C Address
	retv=ioctl(e2p_fd, I2C_SLAVE, I2C_EEPROM);
	if(retv!=0) {
		printf("Ioctl Error!\n");
		return -1;
	}

	
	printf("Data Writing...\n");
	for(int repi=0; repi<strlen(data); repi++) {
		values[0]=repi;
		values[1]=data[repi];
		write(e2p_fd,values,2);
		usleep(100000);
	}

	printf("Finished.\n");

	close(e2p_fd);

	return 0;
}
