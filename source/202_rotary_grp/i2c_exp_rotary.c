/* Raspberry Pi 4 for Automotive IoT KIT
* TITLE : I2C Rotary Control
* FILE  : ~/source/202_rotary_grp/i2c_exp_rotary.c
* AUTH  : mkdev.co.kr x makeutil.tistory.com
* MENT  : This program code may not be used for commercial purposes.
*         The author's information cannot be changed 
*             Gat the time of redistribution.  */


// Define the header & constants
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <signal.h>
#include <linux/i2c-dev.h>

#include "../aiot_hardware.h"

volatile int g_loop=1;

void sigint_handler(int sig) {
	printf("Exiting the program. Wait a moment.\n");
	g_loop = 0;
}


int main(int argc, char** argv)
{
	int fd;
	int retv;
	char i2c_reg;
	char regs[2]={0,};
	char rotarysw_value=0;

	// Signal Setting
	signal(SIGINT, sigint_handler);

	fd = open("/dev/i2c-1",O_RDWR);
	if(fd<3) {
		printf("Device Open Error!\n");
		return -1;
	}

	// Set I2C slave address
	retv=ioctl(fd, I2C_SLAVE, I2C_ROTARY);
	if(retv!=0) {
		printf("Ioctl Error!\n");
		return -1;
	}

   // Send command byte to configure "Configuration Port 0"
   i2c_reg = 0x6;
   write(fd, &i2c_reg, sizeof(i2c_reg));

	printf("Press any key to start...\n");
	getc(stdin);

	while ( g_loop ) {
		// Select Input Port 0(Rotary Switch)
		i2c_reg = 0x00; // Input port 0
		write(fd, &i2c_reg, sizeof(i2c_reg));

		read(fd, &rotarysw_value, sizeof(rotarysw_value));
		printf("U6 : %d /  U7 : %d\n", \
		  0x0f-(rotarysw_value&0x0F), 0x0f-(rotarysw_value>>4));

		usleep(500000); // 500ms
	}

	close(fd); 
	return 0;
}

