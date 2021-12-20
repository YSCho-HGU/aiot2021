/* Automotive IoT Kit
 * Title: Conrol 4 GPIO LEDs using mmap()
 * Filename: gpio_leds.c
 *
 * Hardware connection: LED{22,23,24,25] - GPIO{6,16,17,18}
 * Hardware connection: LED{0,1,2,3] - GPIO{6,16,17,18}
 *
 * Compile: % gcc -o gpio_leds gpio_leds.c
 * Run: % ./gpio_leds
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


	// Open the "/dev/mem" device file to handle the memory area of the Rpi
	fd = open(DEV_NAME, O_RDWR | O_SYNC);

	if( fd < 0 ) {
		perror(DEV_NAME);
		return -1;
	}

	// map GPIO device area onto memory
	gpio_base_addr = (char *) mmap((void *)NULL, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO_BASE);

	if(gpio_base_addr == MAP_FAILED) {
		perror("mmap() error");
		return -1;
	}

	gpio = (volatile unsigned int *) gpio_base_addr;

#define SET_REG32(x)  (1<<x)

	/*
    * set GPIO6 as output with FSEL0[20:18]=001.
    * FSEL0:  20.19.18|17.16.15| ... |5.4.3|2.1.0
    * GPIO#:     6    |   5    | ... |  1  |  0
    * OUTPUT:  0  0  1|......
   */

	// Hardware connection: LED{0,1,2,3] - GPIO{6,16,17,18}
   // reset FSEL to 000 before set to 001
	gpio[GPFSEL0] &= ~(0x07 << (3 * (LED0 % 10))); // GPIO6
	gpio[GPFSEL1] &= ~(0x07 << (3 * (LED1 % 10))); // GPIO16
	gpio[GPFSEL1] &= ~(0x07 << (3 * (LED2 % 10))); // GPIO17
	gpio[GPFSEL1] &= ~(0x07 << (3 * (LED3 % 10))); // GPIO18

	// set 3 bits to 001 to be output
	gpio[GPFSEL0] |= (0x01 << (3 * (LED0 % 10))); // GPIO6
	gpio[GPFSEL1] |= (0x01 << (3 * (LED1 % 10))); // GPIO16
	gpio[GPFSEL1] |= (0x01 << (3 * (LED2 % 10))); // GPIO17
	gpio[GPFSEL1] |= (0x01 << (3 * (LED3 % 10))); // GPIO18

	for(int i = 0; i < 3; i ++) {
		// Turn on LED (active-LOW)
      gpio[GPCLR0/4] |= (1 << (LED0 ));
      gpio[GPCLR0/4] |= (1 << (LED1 ));
      gpio[GPCLR0/4] |= (1 << (LED2 ));
      gpio[GPCLR0/4] |= (1 << (LED3 ));
      sleep(1);

      // Turn off LED (active-LOW)
      gpio[GPSET0/4] |= (1 << (LED0 ));
      gpio[GPSET0/4] |= (1 << (LED1 ));
      gpio[GPSET0/4] |= (1 << (LED2 ));
      gpio[GPSET0/4] |= (1 << (LED3 ));
      sleep(1);
	}

	// set GPIO mode to default value (input)
	gpio[GPFSEL0] &= ~(0x07 << (3 * (LED0 % 10))); // GPIO6
	gpio[GPFSEL1] &= ~(0x07 << (3 * (LED1 % 10))); // GPIO16
	gpio[GPFSEL1] &= ~(0x07 << (3 * (LED2 % 10))); // GPIO17
	gpio[GPFSEL1] &= ~(0x07 << (3 * (LED3 % 10))); // GPIO18

	// Free allocated memory
	if ( munmap(gpio_base_addr, MMAP_SIZE) == -1 ) {
		perror("munmap() error");
		exit(-1);
	}

	close(fd);

	return 0;
}
