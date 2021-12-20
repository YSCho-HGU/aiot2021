/* RaspberryPi 4 for Automotive IoT Kit
 * File  : ~/source/201_ledsw_grp/i2c_exp_led.c
 * TITLE : GPIO LED control based on I2c Expander 
 * Auth  : mkdev.co.kr / makeutil.tistory.com
 * Ment  : This program code may not be used for commercial purposes. 
 *         The author's information cannot be changed at the time of redistribution.*/


// Define the header & constants
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

//#define I2C_LED_ADDR 0x20
#define I2C_LED_ADDR 0x20
#define I2C_ROTARY_ADDR 0x21
#define I2C_ADC_ADDR 0x4C
#define I2C_RTC_ADDR 0x68
#define I2C_TEMP_ADDR 0x48
#define I2C_EEPR_ADDR 0x50

int main(int argc, char** argv)
{
	int led_fd;
	int retv;
	char value=0;
	char values[2]={0,};

	// Open I2C Device
	led_fd = open("/dev/i2c-1",O_RDWR);
	//if( led_fd < 3 )
	if( led_fd < 3 )
	{
		printf("Device Open Error!\n");
		return -1;
	}
	printf("fd=%d\n", led_fd);

	// I2C Communication
	// 1. send slave address
	retv=ioctl(led_fd, I2C_SLAVE, I2C_LED_ADDR);
	if(retv!=0)
	{
		printf("Ioctl Error!\n");
		return -1;
	}

	// 2. send data to slave device (LED)
	// GPIO Exapnder Port 2 Setting 
	values[0] = 0x07; // GPIO Register - TCA9535 Data Sheet 23p
	values[1] = 0x00; // Setting Value
	write(led_fd,values, 2);

	// LED Initial 
	values[0] = 0x03; // Output Port 2
	values[1] = 0xff; // All Off
	write(led_fd, values, 2);
	value = 0xff;
	while(1)
	{
		if(value<=0)
			break;

		values[1] = value;
		write(led_fd,values,2);
		
		usleep(100000); // 100ms
		value-=1;
	}

	// LED Off
	values[1]=0xff;
	write(led_fd,values,2);

	close(led_fd);

	return 0;
}
