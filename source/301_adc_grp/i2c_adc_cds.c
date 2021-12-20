/* Raspberry Pi 4 for Automotive IoT KIT
 * TITLE : CDS control sample
 * FILE : ~/source/301_adc_grp/i2c_adc_cds.c
 * AUTH : mkdev.co.kr x makeutil.tistory.com
 * MENT  : This program code may not be used for commercial purposes. 
 *         The author's information cannot be changed 
 *             at the time of redistribution.  */

#include <stdio.h>
#include <string.h> // strerror()
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h> // errno
#include <stdint.h> // uint8_t
#include <signal.h> 

#include <linux/i2c-dev.h>

#include "../aiot_hardware.h"


volatile int g_loop=1;

void sigint_handler(int sig) {
	printf("Exiting the program. Wait a second...\n");
	g_loop = 0;
}


int main(int argc, char**argv)
{
	int retv=0;
	unsigned char adc_reg=0;
	uint8_t adc_data;

	// Signal Setting
	signal(SIGINT, sigint_handler); 

	int fd = open("/dev/i2c-1", O_RDWR);   // see comments above
	if (fd < 0) {
		printf("Error opening device: %s\n", strerror(errno));
		return 1;
	}

	// Set I2C slave address
	if (ioctl(fd, I2C_SLAVE, I2C_ADC) < 0) {
		printf("ioctl error: %s\n", strerror(errno));
		close(fd);
		return 1;
	}

	// write 0x0: ie. use just channel 0, see data sheet
	/* see the PCF8591 Data Sheet, page 6, Control Byte
		Chanel   	  	: 2 BIT  // 0..3 ex) X = Channel 0, Y = Channel 1
		annelAutoInc 	: 1 BIT  // see data sheet
		served1      	: 1 BIT  // value 0
		ChannelMixing  	: 2 BIT  // see data sheet
		Enable_AOUT    	: 1 BIT  // enabling AOUT, ie. DAC functionality
		Reserved2      	: 1 BIT  // value 0
	 */

	adc_reg &= (0b11111100);
	adc_reg |= ADC_CH0;

	if (write(fd, &adc_reg, sizeof(adc_reg)) != 1) {
		printf("Register setting error!\n");
		return -1;
	}

	while(g_loop) {
		retv = read(fd, &adc_data, sizeof(adc_data));
		if (!retv)
			printf("Read Error!\n");
		else  
			printf("%d\n", adc_data);

		usleep(10 * 1000);    // 10ms delay
	}

	close(fd);

	return 0;
}

