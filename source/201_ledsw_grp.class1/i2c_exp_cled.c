/* RaspberryPi 4 for Automotive IoT Kit
 * File  : ~/source/201_ledsw_grp/i2c_exp_cled.c
 * TITLE : GPIO LED control based on I2c Expander 
 * Auth  : mkdev.co.kr x makeutil.tistory.com
 * Ment  : This program code may not be used for commercial purposes. 
 *         The author's information cannot be changed at the time of redistribution.*/


// Define the header & constants
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include <linux/i2c-dev.h>
#include "../aiot_hardware.h"


int main(int argc, char** argv)
{
	int cled_fd;
	int retv;
   char i2c_reg; 
	char cled_data=0;
	char values[2]={0,};

	// Open I2C Device node (adapter)
	cled_fd = open("/dev/i2c-1",O_RDWR);
	if(cled_fd<3)
	{
		printf("Device Open Error!\n");
		return -1;
	}

	// Specify I2C device address you want to communicate (select slave device)
	retv=ioctl(cled_fd, I2C_SLAVE, I2C_LED);
	if(retv!=0)
	{
		printf("Ioctl Error!\n");
		return -1;
	}

	// Send command byte to configure "Configuration Port 0"
	i2c_reg = 0x06;
	values[0] = i2c_reg;
	values[1] = 0x00;
	write(cled_fd, values, 2);

	// command byte to select "Output Port 0" control register
	i2c_reg = 0x02;
	values[0] = i2c_reg;
	values[1] = 0xff; // Active-Low (???)
	write(cled_fd, values, 2);

	cled_data = 0b10001111;

	for ( int i=0; i < 8; i ++ ) {
		values[1] = cled_data + (i<<4); 
		write(cled_fd, values, 2);

		printf("The value of Color LED: 0x%02x\n", values[1]);
		
		usleep(500000);        // 500ms
	}

	// Color LED off
	values[1]=0xff;         // LED가 모두 켜져있을 테니 모두 꺼두자.
	write(cled_fd,values,2);

	close(cled_fd);

	return 0;
}
