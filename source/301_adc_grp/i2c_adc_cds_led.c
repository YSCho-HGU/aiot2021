/* Raspberry Pi 4 for Automotive IoT KIT
 * TITLE : CDS control sample (with I2C LED)
 * FILE : i2c_adc_cds_led.c
 * AUTH : mkdev.co.kr x makeutil.tistory.com
 * MENT  : This program code may not be used for commercial purposes. 
 *         The author's information cannot be changed at the time of redistribution.  */


#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>

#include <signal.h> 

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

// I2C Address
#define EXP_LED 0x20
#define ADC_CDS 0x4C

#define CH0 0;

volatile int g_loop=1;

void sigint_handle(int sig)
{
	printf("...Terminated!\n");
	g_loop = 0;
}

int main(int argc, char**argv)
{
	int retv=0;
	unsigned char reg=0;
	unsigned char lv=0;
	unsigned char cv=0;
	unsigned char values[2]={0,};

	// Signal Setting
	signal(SIGINT, sigint_handle); 

	// I2C Open - LED
	int led_fd = open("/dev/i2c-1", O_RDWR);   // see comments above
	if (led_fd < 0) {
		printf("Error opening device: %s\n", strerror(errno));
		return 1;
	}

	/* LED Initialization ------------------------ */
	retv=ioctl(led_fd, I2C_SLAVE, EXP_LED);
	if(retv!=0) {
		printf("Ioctl Error!\n");
		return -1;
	}

	// GPIO Exapnder Port 2 Setting 
	values[0] = 0x07; // GPIO Register - TCA9535 Data Sheet 23p
	values[1] = 0x00; // Setting Value
	write(led_fd,values, 2);

	// LED Initial 
	values[0] = 0x03; // Output Port 2
	values[1] = 0xff; // All Off
	write(led_fd, values, 2);
	lv = 0x00;


	/* CDS Initialization ------------------------ */
	int cds_fd = open("/dev/i2c-1", O_RDWR);   // see comments above
	if (cds_fd < 0) {
		printf("Error opening device: %s\n", strerror(errno));
		return 1;
	}

	if (ioctl(cds_fd, I2C_SLAVE, ADC_CDS) < 0) {
		printf("ioctl error: %s\n", strerror(errno));
		close(cds_fd);
		return 1;
	}
	reg |= CH0;

	if (write(cds_fd, &reg, 1) != 1) {
		printf("Register setting error!\n");
		return -1;
	}


	while(g_loop)
	{
		retv = read(cds_fd, &cv, 1);
		if (!retv)
			printf("Read Error!\n");
		else  {
			printf("%d\n", cv);

                        if(cv>32)
			   lv|=0x01;
			if(cv>64)
			   lv|=0x02;
			if(cv>96)
			   lv|=0x04;
			if(cv>128)
			   lv|=0x08;
			if(cv>160)
			   lv|=0x10;
			if(cv>192)
			   lv|=0x20;
			if(cv>=224)
			   lv|=0x40;
			if(cv==255)
			   lv|=0x80;

			values[1] = ~ lv;
			write(led_fd,values,2);

			lv=0;
		}
		usleep(10 * 1000);    
	}

	close(led_fd);
	close(cds_fd);

	return 0;
}
