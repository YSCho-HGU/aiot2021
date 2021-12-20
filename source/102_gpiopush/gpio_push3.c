/* RaspberryPi 4 for Automotive IoT Kit
 * TITLE : Example of GPIO PUSH control using mmap. 
 * File  : ~/source/102_gpiopush/gpio_push.c
 * Auth  : mkdev.co.kr x makeutil.tistory.com
 * Ment  : This program code may not be used for commercial purposes. 
 * The author's information cannot be changed at the time of redistribution.*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include "../rpi4_bcm2711_gpio_registers.h"
#include "../aiot_hardware.h"


int main(int argc, char **argv) 
{
	int fd;

	
	char *gpio_base_addr;
	volatile unsigned int *gpio;

	// Open the /dev/mem device(special file) the handles the memory area of the Rpi.
	fd = open("/dev/mem", O_RDWR | O_SYNC);
	if(fd<0) {
		printf("Device Open Error.\n");
		return -1;
	}

	// Use the mmap() to allocate memory on the Rpi for application access.
	gpio_base_addr = (char*)mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, \
							fd, GPIO_BASE);
	if(gpio_base_addr == MAP_FAILED) {
		printf("Memory Allocation Error\n");
		return -1;
	}

	// pointer to access gpio registers
	gpio = (volatile unsigned int*)gpio_base_addr;

	// reset FSEL before setting the direction
	gpio[GPFSEL0/4] &= ~(0x07 << (3 * (SW6 % 10)));

	// set 3 bits to 000 to be input 
	gpio[GPFSEL0/4] |= (0x00 << (3 * (SW6 % 10)));

	for(int repi=0; repi<10; repi++) {
		if (gpio[GPLEV0/4] & (0x1 << (SW6 % 32)))
			printf("SW6 Released! (Value=HIGH)\n"); // Normally HIGH
				else 
			printf("SW6 is Pressed! (Value=LOW)\n"); // LOW if Switch is pressed
		sleep(1);
	}

	// Free allocated memory
	munmap(gpio_base_addr, 4096);
	close(fd);

	return 0;
}
