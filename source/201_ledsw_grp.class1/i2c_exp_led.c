/* RaspberryPi 4 for Automotive IoT Kit
 * File  : ~/source/201_ledsw_grp/i2c_exp_led.c
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
	int led_fd;
	int retv;
   char i2c_reg; 
	char led_data=0;
	char values[2]={0,};

	// Open I2C Device node (adapter)
	led_fd = open("/dev/i2c-1",O_RDWR);
	if(led_fd<3)
	{
		printf("Device Open Error!\n");
		return -1;
	}

	// Specify I2C device address you want to communicate (select slave device)
	retv=ioctl(led_fd, I2C_SLAVE, I2C_LED);
	if(retv!=0)
	{
		printf("Ioctl Error!\n");
		return -1;
	}

	// Send command byte to configure "Configuration Port 1"
	i2c_reg = 0x07;
	values[0] = i2c_reg;
	values[1] = 0x00;
	write(led_fd, values, 2);

	// command byte to select "Output Port 1" control register
	i2c_reg = 0x03;
	values[0] = i2c_reg;
	values[1] = 0xff; // Active-Low
	write(led_fd, values, 2);

	led_data = 0xff;

	for ( int i=0; i < 256; i ++ ) {
		values[1] = led_data;
		write(led_fd, values, 2);
		
		usleep(100000);        // 100ms
		led_value --;
	}

	// 종료시 처리할 내용을 지정
	values[1]=0xff;         // LED가 모두 켜져있을 테니 모두 꺼두자.
	write(led_fd,values,2);

	close(led_fd);

	return 0;
}
