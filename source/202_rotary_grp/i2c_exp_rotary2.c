/* Raspberry Pi 4 for Automotive IoT KIT
 * TITLE : I2C Rotary Control (wtih LEDs)
 * FILE  : i2c_exp_rotary2.c
 * AUTH  : mkdev.co.kr x makeutil.tistory.com
 * MENT  : This program code may not be used for commercial purposes. 
 *         The author's information cannot be changed at the time of redistribution.  */

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

#define MOD_ROTARY 0x21

volatile int g_loop=1;
void sigint_handle(int sig)
{	printf("...Terminated!\n");
	g_loop = 0;
}

int main(int argc, char** argv)
{
	int fd;
	int retv;
	char reg;
	char regs[2]={0,};
	char value=0;

	// Signal Setting
  	signal(SIGINT, sigint_handle); 

	fd = open("/dev/i2c-1",O_RDWR);
	if(fd<3)
	{
		printf("Device Open Error!\n");
		return -1;
	}

	retv=ioctl(fd, I2C_SLAVE, MOD_ROTARY);
	if(retv!=0)
	{
		printf("Ioctl Error!\n");
		return -1;
	}

	// GPIO Exapnder Port 2 Setting
	regs[0] = 0x07; // GPIO Register - TCA9535 Data Sheet 23p
	regs[1] = 0x00; // Setting Value
	write(fd, regs, 2);

	// LED Initial 
	regs[0] = 0x03; // Output Port 2
	regs[1] = 0xff; // All Off
	write(fd, regs, 2);
	printf("Press any key to start...\n");
	getc(stdin);

	while(g_loop)
	{
		// Set Input (Rotary)
		reg = 0x00;
		write(fd,&reg,1);
		read(fd,&value,1);
		printf("R1 : %d /  R2 : %d\n", 0x0f-(value&0x0F), 0x0f-(value>>4));

		// Set LED state.
		regs[1] = value;
		write(fd,regs,2);
		usleep(500000); // 100ms
	}

	// LED Off
	regs[1]=0xff;
	write(fd,regs,2);

	close(fd);

	return 0;
}

