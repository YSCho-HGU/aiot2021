/* RaspberryPi 4 for Automotive IoT Kit
 * File  : i2c_exp_led.c
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

#include <linux/i2c-dev.h>

#include "../aiot_hardware.h"

#define I2C_ADAPTOR "/dev/i2c-1"


int main(int argc, char** argv)
{
	int led_fd;
	int retv;
	char i2c_reg;
	char led_data=0;
	char values[2]={0,};

	// Open I2C device node (adapter)
	led_fd = open(I2C_ADAPTOR,O_RDWR);
	if(led_fd < 3) {
		printf("Device node open error!!!: %s", I2C_ADAPTOR);
		return -1;
	}

	// Specify I2C device address you want to communicate (select slave device)
	retv=ioctl(led_fd, I2C_SLAVE_FORCE, I2C_LED);
	if(retv!=0) {
		printf("Ioctl Error!\n");
		return -1;
	}

	// Send command byte to configure "Configuration Port 1"
	i2c_reg = 0x07;
	//values[0] = 0x07; // GPIO Register - TCA9535 Data Sheet 23p
	values[0] = i2c_reg; // GPIO Register - TCA9535 Data Sheet 23p
	values[1] = 0x00; // Setting Value
	write(led_fd,values, 2);

	// command byte to select "Output Port 1" control register
	i2c_reg = 0x03;
	values[0] = i2c_reg; 
	// Turn off 8 LEDs before starting
	values[1] = 0xff;  // Active-Low
	write(led_fd, values, 2);

#ifdef TTT
	while(1) {
		if(led_data <= 0) break;
		values[1] = led_data;
		write(led_fd,values,2);
		usleep(100000); // 100ms
		led_data -= 1;
	}
#endif

	led_data = 0xff;
	for (int i=0; i<256; i++) {
		values[1] = led_data;
		write(led_fd,values,2);

		usleep(100000); // 100ms
		led_data --;	
	}

	// Turn off 8 LEDs
	values[1]=0xff;
	write(led_fd,values,2);

	close(led_fd);

	return 0;
}
