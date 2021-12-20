/* RaspberryPi 4 for Automotive IoT Kit
 * TITLE : Example of GPIO PUSH control using mmap. 
 * File  : gpio_push.c
 * Auth  : mkdev.co.kr x makeutil.tistory.com
 * Ment  : This program code may not be used for commercial purposes. 
 * The author's information cannot be changed at the time of redistribution.*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

// See the bcm2711-peripherals.pdf file - page 65.
#define GPIO_BASE 0xfe200000

#define GPFSEL0 0x00 // Defines the offset of the GPIO Function Select Register
#define GPLEV0 0x34  // Defines the offset of the GPIO Pin Level0 Register

// See the AIoT_Development_Kit_Schematics_3rd.pdf file - page 10.
#define PUSH0  0

int main(int argc, char **argv) {
    int fd;

    volatile unsigned int *gpio_v_addr;
    volatile unsigned int *gpfsel0;
    volatile unsigned int *gplev0;

    // Open the /dev/mem device(special file) the handles the memory area of the Rpi.
    fd = open("/dev/mem", O_RDWR | O_SYNC);
    if(fd<0) {
        printf("Device Open Error.\n");
        return -1;
    }

    // Use the mmap() to allocate memory on the Rpi for application access.
    char* gpio_base_addr = (char*)mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, \
                            fd, GPIO_BASE);
    if(gpio_base_addr == MAP_FAILED) {
        printf("Memory Allocation Error\n");
        return -1;
    }

    // Makes each variable point to the specified register.
    gpio_v_addr = (volatile unsigned int*)gpio_base_addr;

    // When the integer type pointer is incremented by 1, it moves by 4byte.
    gpfsel0 = gpio_v_addr + (GPFSEL0/4); 
    gplev0 = gpio_v_addr + (GPLEV0/4);

    // Use shift operation to set the bits of the specified register
    *gpfsel0 |= (0<<2);
    *gpfsel0 |= (0<<1);
    *gpfsel0 |= (0<<0);

    for(int repi=0; repi<10; repi++) {
	    if(!(*gplev0 & (1<<PUSH0)))
	        printf("Button 0 Pressed!\n");
		        else 
	        printf("Button 0 Released!\n");
        sleep(1);
    }

    // Free allocated memory
    munmap(gpio_base_addr, 4096);
    close(fd);

    return 0;
}
