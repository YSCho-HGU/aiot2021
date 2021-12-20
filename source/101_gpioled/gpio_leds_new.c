/* Automotive IoT Kit
 * Title: Conrol 4 GPIO LEDs using mmap()
 * Filename: gpio_leds.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include "~/source/rpi4_bcm2711_gpio_registers.h"
#include "~/source/aiot_hardware.h"

#define DEV_NAME "/dev/mem" // device file to handle the memory
#define MMAP_SIZE 4096


int main(int argc, char **argv)
{
	int fd;

	volatile unsigned int *gpio_base_addr;
	volatile unsigned int *gpfsel0;
	volatile unsigned int *gpfsel1;
	volatile unsigned int *gpset0;
	volatile unsigned int *gpclr0;

	int tmp_reg = 0;


	// Open the "/dev/mem" device file to handle the memory area of the Rpi
	fd = open(DEV_NAME, O_RDWR | O_SYNC);

	if( fd < 0 ) {
		perror(DEV_NAME);
		exit(-1);
		// return -1;
	}

	// map GPIO device area onto memory

	// char* gpio_base_addr = (char*)mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO_BASE);

	gpio_base_addr = (volatile unsigned int *) mmap(0, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO_BASE);

	if(gpio_base_addr == MAP_FAILED) {
		perror("mmap() error");
		return -1;
	}

	// Makes each variable point to the specified register.
	//gpio_v_addr = (volatile unsigned int*)gpio_base_addr;

#ifdef OLD_CODE
	// When the integer type pointer is incremented by 1, it moves by 4byte.
	gpfsel0 = gpio_v_addr + (GPFSEL0/4); 
	gpset0 = gpio_v_addr + (GPSET0/4);
	gpclr0 = gpio_v_addr + (GPCLR0/4);
#endif
	gpfsel0 = gpio_base_addr + (GPFSEL0/4); 
	gpfsel1 = gpio_base_addr + (GPFSEL1/4); 
	gpset0 = gpio_base_addr + (GPSET0/4);
	gpclr0 = gpio_base_addr + (GPCLR0/4);

	// Use shift operation to set the bits of the specified register
	/*
    FSEL0:  20.19.18|17.16.15| ... |5.4.3|2.1.0
    GPIO#:     6    |   5    | ... |  1  |  0
    OUTPUT:  0  0  1|......
	*/

#define SET_REG32(x)  (1<<x)

#ifdef OLD_CODE
	// When the shift operation is executed, the specified bit is set to 1. 
	*gpfsel0 |= (0<<20);
	*gpfsel0 |= (0<<19);
	*gpfsel0 |= (1<<18);
	// LED0 - GPIO6
	tmp_reg = *gpfsel0;
	tmp_reg = 0xffe3ffff | SET_REG32(18); //??? 
	*gpfsel0 = tmp_reg;

	// LED1~3 - GPIO 16, 17, 18
	tmp_reg = *gpfsel1;
	tmp_reg = 0xf803ffff | SET_REG32(18) | SET_REG32(21)| SET_REG32(24) ;
	*gpfsel1 = tmp_reg;
#endif

	// reset 3 bits to 000
	*gpfsel0 &= ~(0x07 << (3 * (LED0 % 10)));
	*gpfsel1 &= ~(0x07 << (3 * (LED1 % 10)));
	*gpfsel1 &= ~(0x07 << (3 * (LED2 % 10)));
	*gpfsel1 &= ~(0x07 << (3 * (LED3 % 10)));
	// set 3 bits to 001 to be output
	*gpfsel0 |= (0x01 << (3 * (LED0 % 10)));
	*gpfsel1 |= (0x01 << (3 * (LED1 % 10)));
	*gpfsel1 |= (0x01 << (3 * (LED2 % 10)));
	*gpfsel1 |= (0x01 << (3 * (LED3 % 10)));

	for(int i = 0; i < 10; i ++) {
		// At this time, the correspnding pin outputs High(3.3 volt)
		*gpset0 |= SET_REG32(LED0); // (1 << LED0); 
		*gpset0 |= SET_REG32(LED1); // (1 << LED0); 
		*gpset0 |= SET_REG32(LED2); // (1 << LED0); 
		*gpset0 |= SET_REG32(LED3); // (1 << LED0); 
		sleep(1);

		// Sets the specified bit in the clear register to 1. 
		// At this time, the corresponding pin outputs Low(0 volt).
		*gpclr0 |= SET_REG32(LED0); // (1 << LED0); 
		*gpclr0 |= SET_REG32(LED1); // (1 << LED0); 
		*gpclr0 |= SET_REG32(LED2); // (1 << LED0); 
		*gpclr0 |= SET_REG32(LED3); // (1 << LED0); 
		sleep(1);
	}

	// Free allocated memory
	if ( munmap(gpio_base_addr, MMAP_SIZE) == -1 ) {
		perror("munmap() error");
		exit(-1);
	}

	close(fd);

	return 0;
}
