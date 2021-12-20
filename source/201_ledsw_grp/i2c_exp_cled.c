/* Raspberry Pi 4 for Automotive IoT KIT
 * TITLE : I2C Color LED control
 * FILE  : i2c_exp_cled.c
 * AUTH  : mkdev.co.kr / makeutil.tistory.com
 * Ment  : This program code may not be used for commercial purposes. 
 *         The author's information cannot be changed at the time of redistribution. */

// Define the header & constants
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include <signal.h> 

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#define MOD_LED 0x20

volatile int g_loop=1;
void sigint_handle(int sig)
{	printf("...Terminated!\n");
	g_loop = 0;
}

int main(int argc, char** argv)
{	int cled_fd;
	int retv;
	char value;
	char values[2]={0,};
	// Signal Setting
	signal(SIGINT, sigint_handle); 

	cled_fd = open("/dev/i2c-1",O_RDWR);
	if(cled_fd<3)
	{
		printf("Device Open Error!\n");
		return -1;
	}

	retv=ioctl(cled_fd, I2C_SLAVE, MOD_LED);
	if(retv!=0)
	{
		printf("Ioctl Error!\n");
		return -1;
	}

	// GPIO Exapnder Port 1 Setting
	values[0] = 0x06; // GPIO Register - TCA9535 Data Sheet 23p
	values[1] = 0x00; // Setting Value
	write(cled_fd,values, 2);

	// CLED Initial 
	values[0] = 0x02; // Output Port 1
	values[1] = 0xff; // All Off           XBGR0000 = 01110000 ~ 00010000( 70
	write(cled_fd, values, 2);

	printf("Press any key to start...");
	getc(stdin);

	int repi = 0;

	while(g_loop)
	{
		value = 0x00;
		if(repi>7)
		    repi=0;
		values[1] = (value+repi)<< 4;      // 0BGR0000 -> 00000111 -> << 4 -> 00000000 ~ 01110000
		printf("Set Value : 0x%02x...\n",values[1]);
		write(cled_fd,values,2);
		usleep(10000000); // 100ms
		repi+=1;
	}

	// Color LED OFF.
	values[1] = 0xff;

	write(cled_fd,values,2);
	close(cled_fd);
	return 0;

}
