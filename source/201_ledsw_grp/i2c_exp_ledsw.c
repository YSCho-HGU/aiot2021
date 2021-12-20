/* Raspberry Pi 4 for Automotive IoT KIT 
 * TITLE : I2C LED-Switch Control (0x20)
 * FILE  : i2c_exp_ledsw.c
 * AUTH  : mkdev.co.kr - makeutil.tistory.com
 * Ment  : This program code may not be used for commercial purposes. 
 *         The author's information cannot be changed at the time of redistribution.
 */

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
{
	int fd;
	int retv;
	char reg;
	char getdata=0;
	char i2c_event=0;
	char led_state=0xff;
	char values[2]={0,};
	char btn_state[2]={0,};

	// Signal Setting
	signal(SIGINT, sigint_handle); 

	fd = open("/dev/i2c-1",O_RDWR);
	if(fd<3)
	{
		printf("Device Open Error!\n");
		return -1;
	}

	retv=ioctl(fd, I2C_SLAVE, MOD_LED);
	if(retv!=0)
	{
		printf("Ioctl Error!\n");
		return -1;
	}

	// GPIO Exapnder Port 1 Setting
	values[0] = 0x06; // GPIO Register - TCA9535 Data Sheet 23p
	values[1] = 0x03; // Setting Value     XBGRLLSS = 00000011
	write(fd,values, 2);

	// CLED Initial 
	values[0] = 0x02; // In/Output Port 1
	values[1] = 0xff; // All Off           XBGRLLSS = 11111111
	write(fd, values, 2);

	printf("Press any key to start...");
	getc(stdin);

	while(g_loop)
	{
		// Set Input Address
		reg = 0x00;
		write(fd,&reg,1);
		read(fd,&getdata,1);

		if(!(getdata&0x01))
		{
			if(btn_state[0]==0)
			{
			    printf("LED Key 0 : Pressed\n");
			    btn_state[0] = 1;
			}

		} else {
			if(btn_state[0]==1)
			{
			    printf("LED Key 0 : Released\n");
			    btn_state[0]=0;
			    i2c_event = 1;

			    if(led_state&0x04)
				    led_state&=~(1<<2);
			    else
				    led_state|=(1<<2);  

			}

		}


		if(!(getdata&0x02))
		{
			if(btn_state[1]==0)
			{
			    printf("LED Key 0 : Pressed\n");
			    btn_state[1] = 1;
			}

		} else {
			if(btn_state[1]==1)
			{
			    printf("LED Key 0 : Released\n");
			    btn_state[1]=0;
			    i2c_event = 1;

			    if(led_state&0x08)
				    led_state&=~(1<<3);
			    else
				    led_state|=(1<<3);

			}

		}


		if(i2c_event!=0)
		{
			printf("Event Processing...\n");
			i2c_event = 0;
			
			values[0]=0x02;
			values[1]=led_state;
			printf("LED Data = %x\n",values[1]);
			write(fd, values, 2);
		}

		usleep(100000);
	}

	values[1] = 0xff;

	write(fd,values,2);
	close(fd);
	return 0;

}
