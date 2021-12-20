/* RaspberryPi 4 for Automotive IoT Kit
* TITLE : Example of GPIO PUSH control using mmap. 
* File : ~/source/102_gpiopush/gpio_pushes3.c
* Auth : mkdev.co.kr x makeutil.tistory.com
* Ment : This program code may not be used for commercial purposes. 
* The author's information cannot be changed at the time of redistribution.*/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>
#include "../rpi4_bcm2711_gpio_registers.h"
#include "../aiot_hardware.h"

#define SET_REG32(x) (1<<x)

int isQuit = 0;

void signal_handler(int sig){
	isQuit = 1;
}

int bit_check(unsigned int value, char * string, int mod)
{
	int repl;
	for( repl = 31 ; repl >= 0 ; repl--) {
		if(value & ( 1 << repl ))
				printf("1 ");
		else
				printf("0 ");
		if(repl % mod == 0)
				printf("| ");
	}
	printf("%s\n", string);
	return 0;
}

int main(int argc, char **argv) {
	
	int fd;

	char *gpio_base_addr;
	volatile unsigned int * gpio;


 // Open the /dev/mem device(special file) the handles the memory area of the Rpi.
	fd = open("/dev/mem", O_RDWR | O_SYNC);
 	if(fd<0) {
		printf("Device Open Error.\n");
 		return -1;
 	}
 // Use the mmap() to allocate memory on the Rpi for application access.
 	gpio_base_addr = (char*) mmap (0, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO_BASE);
 	if(gpio_base_addr == MAP_FAILED) {
 		printf("Memory Allocation Error\n");
 		return -1;
 	}

   // Pointer to acess gpio registers
 	gpio = (volatile unsigned int*) gpio_base_addr;

	// reset GPFSEL before setting the direction
	gpio[GPFSEL0/4] &= ~(0x07 << (3 * (SW6 % 10))); 
	gpio[GPFSEL0/4] &= ~(0x07 << (3 * (SW7 % 10))); 
	gpio[GPFSEL0/4] &= ~(0x07 << (3 * (SW8 % 10))); 
	gpio[GPFSEL0/4] &= ~(0x07 << (3 * (SW9 % 10))); 
	// set 3 bits to 000 to be input
	gpio[GPFSEL0/4] |= (0x00 << ( 3 * (SW6 % 10)));
	gpio[GPFSEL0/4] |= (0x00 << ( 3 * (SW7 % 10)));
	gpio[GPFSEL0/4] |= (0x00 << ( 3 * (SW8 % 10)));
	gpio[GPFSEL0/4] |= (0x00 << ( 3 * (SW9 % 10)));

	// reset FSEL0[20:18] to 000 before set to 001
	gpio[GPFSEL0/4] &= ~(0x07 << (3 * (LED22 % 10))); 
	gpio[GPFSEL1/4] &= ~(0x07 << (3 * (LED23 % 10))); 
	gpio[GPFSEL1/4] &= ~(0x07 << (3 * (LED24 % 10))); 
	gpio[GPFSEL1/4] &= ~(0x07 << (3 * (LED25 % 10))); 
	// set 3 bits to 001 to be output
	gpio[GPFSEL0/4] |= (0x01 << ( 3 * (LED22 % 10)));
	gpio[GPFSEL1/4] |= (0x01 << ( 3 * (LED23 % 10)));
	gpio[GPFSEL1/4] |= (0x01 << ( 3 * (LED24 % 10)));
	gpio[GPFSEL1/4] |= (0x01 << ( 3 * (LED25 % 10)));

	signal(SIGINT, signal_handler);

	while(!isQuit){

		/*************SW6****************/
		if(gpio[GPLEV0/4] & (0x01 << (SW6 % 32))){ // 1 (Released/ OFF-HIGH)
			gpio[GPSET0/4] |= (0x01 << (LED22 % 32));
			//bit_check(gpio[GPSET0/4], "GPSET0", 1);
			// gpio[GPSET0/4] |= (0x01 << (LED22 % 32));
			printf("Button 0 released ! Valaue : HIGH \n");
		} else{ // 0 (LOW-ON)
			gpio[GPCLR0/4] |= (0x01 << (LED22 % 32));
			//bit_check(gpio[GPCLR0/4], "GPCLR0", 1);
			// gpio[GPCLR0/4] |= (0x01 << (LED22 % 32));
			printf("Button 0 pressed! Value : LOW\n");	
		}

		/*************SW7****************/
		if(gpio[GPLEV0/4] & (0x01 << (SW7 % 32))){ // 1 - OFF (HIGH)
			gpio[GPSET0/4] |= (0x01 << (LED23 % 32));
			printf("Button 1 released ! Valaue : HIGH(LEV0=%x, SET0=%x, CLR0=%x)\n", gpio[GPLEV0/4], gpio[GPSET0/4], gpio[GPCLR0/4]);
		} else{ // 0 - ON (LOW)
			gpio[GPCLR0/4] |= (0x01 << (LED23 % 32));
			printf("Button 1 pressed ! Value : HIGH(LEV0=%x, SET0=%x, CLR0=%x)\n", gpio[GPLEV0/4], gpio[GPSET0/4], gpio[GPCLR0/4]);
		}

		/*************SW8****************/
		if(gpio[GPLEV0/4] & (0x01 << (SW8 % 32))){ // OFF-HIGH
			gpio[GPSET0/4] |= (0x01 << (LED24 % 32)); // 
			printf("Button 2 released ! Valaue : HIGH\n");
		}
		else{ // ON-LOW
			gpio[GPCLR0/4] |= (0x1 << (LED24 % 32));
			printf("Button 2 pressed! Value : LOW\n");	
		}


		/*************SW9****************/
		if(gpio[GPLEV0/4] & (0x1 << (SW9 % 32))){
			gpio[GPSET0/4] |= (0x1 << (LED25 % 32)); // LED22 % 32 = 6 1bit = 1port 
			printf("Button 3 released ! Valaue : HIGH\n");
		}
		else{
			gpio[GPCLR0/4] |= (0x1 << (LED25 % 32));
			printf("Button 3 pressed! Value : LOW\n");
		}
		
		usleep(10000);
 	}
	printf("program is successfully terminated\n"); 
	gpio[GPSET0/4] |= (0x1 << (LED22 % 32));  
	gpio[GPSET0/4] |= (0x1 << (LED23 % 32));  
	gpio[GPSET0/4] |= (0x1 << (LED24 % 32));  
	gpio[GPSET0/4] |= (0x1 << (LED25 % 32));  

	gpio[GPFSEL0/4] &= ~(0x07 << ( 3 * (LED22 % 10)));
	gpio[GPFSEL1/4] &= ~(0x07 << ( 3 * (LED23 % 10)));
	gpio[GPFSEL1/4] &= ~(0x07 << ( 3 * (LED24 % 10)));
	gpio[GPFSEL1/4] &= ~(0x07 << ( 3 * (LED25 % 10)));
	// Free allocated memory
	munmap(gpio_base_addr, 4096);
	close(fd);
	return 0;
}
