/* RaspberryPi 4 for Automotive IoT Kit
 * File : 101_gpioled.c
 * Auth : mkdev.co.kr
 * ment : Example of GPIO BUZZER control using mmap. */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

// See the bcm2711-peripherals.pdf file - page 65.
#define GPIO_BASE 0xfe200000

#define GPFSEL1 0x04 // Defines the offset of the GPIO Function Select Register0
#define GPSET0 0x1c  // Defines the offset of the GPIO Pin Output Set0 Register
#define GPCLR0 0x28  // Defines the offset of the GPIO Pin Output Clear Register

// See the AIoT_Development_Kit_Schematics_3rd.pdf file - page 10.
#define BUZZER 12

int main(int argc, char **argv)
{
    int fd;

    volatile unsigned int *gpio_v_addr;
    volatile unsigned int *gpfsel1;
    volatile unsigned int *gpset0;
    volatile unsigned int *gpclr0;

    // Open the /dev/mem device(special file) the handles the memory area of the Rpi.
    fd = open("/dev/mem", O_RDWR | O_SYNC);

    if(fd<0) {
        printf("Device Open Error.\n");
        return -1;
    }

    // Use the mmap() to allocate memory on the Rpi for application access.
    char* gpio_base_addr = (char*)mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO_BASE);
    if(gpio_base_addr == MAP_FAILED)
    {
        printf("Memory Allcation Error\n");
        return -1;
    }

    // Makes each variable point to the specified register.
    gpio_v_addr = (volatile unsigned int*)gpio_base_addr;

    // When the integer type pointer is incremented by 1, it moves by 4byte.
    gpfsel1 = gpio_v_addr + (GPFSEL1/4); 
    gpset0 = gpio_v_addr + (GPSET0/4);
    gpclr0 = gpio_v_addr + (GPCLR0/4);

    // Use shift operation to set the bits of the specified register
    *gpfsel1 |= (0<<8);
    *gpfsel1 |= (0<<7);
    *gpfsel1 |= (0<<6);

    //1111111111111111

    for(int repi=0; repi<10; repi++)
    {
	// When the shift operation is executed, the specified bit is set to 1. 
	// At this time, the correspnding pin outputs High(0 volt)
        *gpset0 |= (1<<BUZZER); 
        sleep(2);
    	*gpclr0 |= (1<<BUZZER);
        sleep(2);
    }


    // Free allocated memory
    munmap(gpio_base_addr, 4096);

    close(fd);

    return 0;
}


