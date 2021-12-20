/* Automotive IoT Kit
 * Title: Conrol a GPIO LED (LED0, GPIO6) using mmap()
 * Filename: ~/source/101_gpioled/gpio_led2.c
 *
 * Compile: % gcc -o gpio_led2 gpio_led2.c
 * Run: % ./gpio_led2
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include "../rpi4_bcm2711_gpio_registers.h"
#include "../aiot_hardware.h"

#define DEV_NAME "/dev/mem" // device file to handle the memory
#define MMAP_SIZE 4096


int main(int argc, char **argv)
{
	int fd;

	char *gpio_base_addr;
	volatile unsigned int *gpio;
	volatile unsigned int *gpfsel0;
	volatile unsigned int *gpset0;
	volatile unsigned int *gpclr0;


	// Open the "/dev/mem" device file to handle the memory area of the Rpi
	fd = open(DEV_NAME, O_RDWR | O_SYNC);

	if( fd < 0 ) {
		perror(DEV_NAME);
		return -1;
	}

	// map GPIO device onto memory using mmap()
	gpio_base_addr = (char *) mmap((void *)NULL, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO_BASE);

	if(gpio_base_addr == MAP_FAILED) {
		perror("mmap() error");
		return -1;
	}

	gpio = (volatile unsigned int *) gpio_base_addr;

	// calculate GPIO register address using base_address and offset
	gpfsel0 = gpio + (GPFSEL0/4); 
	gpset0 = gpio + (GPSET0/4);
	gpclr0 = gpio + (GPCLR0/4);

	/*
    * set GPIO6 as output with FSEL0[20:18]=001.
    * FSEL0:  20.19.18|17.16.15| ... |5.4.3|2.1.0
    * GPIO#:     6    |   5    | ... |  1  |  0
    * OUTPUT:  0  0  1|......
    */

	// Added code compared to gpio_led.c
	// reset FSEL0[20:18] to 000 before set to 001
	*gpfsel0 &= (0<<20);
	*gpfsel0 &= (0<<19);
	*gpfsel0 &= (0<<18);

	// set 3 bits to 001 to be output
	*gpfsel0 |= (0<<20);
	*gpfsel0 |= (0<<19);
	*gpfsel0 |= (1<<18);

	for(int i = 0; i < 10; i ++) {
		// Turn on LED (active-LOW)
      *gpclr0 |= (1 << LED0);
      sleep(1);

      // Turn off LED (active-LOW)
      *gpset0 |= (1 << LED0);
      sleep(1);
	}

	// set GPIO mode to default value (input)
	*gpfsel0 &= (0<<20);
	*gpfsel0 &= (0<<19);
	*gpfsel0 &= (0<<18);

	// Free allocated memory
	if ( munmap(gpio_base_addr, MMAP_SIZE) == -1 ) {
		perror("munmap() error");
		exit(-1);
	}

	close(fd);

	return 0;
}
