/* RaspberryPi 4 for Automotive IoT Kit
 * TITLE : Example of controlling multiple LED+Push-Btn 
 * File  : gpio_ledpushs.c
 * Auth  : mkdev.co.kr x makeutil.tistory.com
 * Ment  : This program code may not be used for commercial purposes. 
           The author's information cannot be changed at the time of redistribution. */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

// See the bcm2711-peripherals.pdf file - page 65.
#define GPIO_BASE 0xfe200000

#define SET_REG32(x) (1<<x)
#define CLR_REG32(x) (0<<x)

#define GPFSEL0 0x00 // Defines the offset of the GPIO Function Select Register
#define GPFSEL1 0x04 // Defines the offset of the GPIO Function Select Register
#define GPSET0 0x1c 
#define GPCLR0 0x28
#define GPLEV0 0x34  // Defines the offset of the GPIO Pin Level0 Register

// See the AIoT_Development_Kit_Schematics_3rd.pdf file - page 10.
#define PUSH0  0
#define PUSH1  1
#define PUSH2  4
#define PUSH3  5

#define LED0  6      
#define LED1 16
#define LED2 17
#define LED3 18


int bit_check(unsigned int value, char* string, int mod);

int main(int argc, char **argv)
{
    int fd;

    volatile unsigned int tmp_reg=0;

    volatile unsigned int *gpio_v_addr;
    volatile unsigned int *gpfsel0;
    volatile unsigned int *gpfsel1;
    volatile unsigned int *gpset0;
    volatile unsigned int *gpclr0;
    volatile unsigned int *gplev0;


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
    gpfsel0 = gpio_v_addr + (GPFSEL0/4); 
    gpfsel1 = gpio_v_addr + (GPFSEL1/4); 
    gpset0 = gpio_v_addr + (GPSET0/4); 
    gpclr0 = gpio_v_addr + (GPCLR0/4); 
    gplev0 = gpio_v_addr + (GPLEV0/4);

    // Use shift operation to set the bits of the specified register
    *gpfsel0&=0xffe3ffff; //11111111 11100011 11111111 11111111
    *gpfsel0|=SET_REG32(18);
    bit_check(*gpfsel0,"GPFSEL0",3);
    *gpfsel1&= 0xf803ffff;//11111000 00000011 11111111 11111111
    *gpfsel1 |= (1<<18) | (1<<21) | (1<<24);
    bit_check(*gpfsel1,"GPFSEL1",3);

    *gpfsel0 &= 0xfffc0fc0; //11111111 11111100 00001111 11000000
    bit_check(*gpfsel0,"GPFSEL0",3);
    printf("----------------------\n");
  
    for(int repi=0; repi<1000; repi++)
    {
	if(!(*gplev0 & SET_REG32(PUSH0)))
    	    *gpclr0 = SET_REG32(LED0);
	 else 
	    *gpset0 = SET_REG32(LED0);

	if(!(*gplev0 & SET_REG32(PUSH1)))
    	    *gpclr0 = SET_REG32(LED1);
	 else 
	    *gpset0 = SET_REG32(LED1);

	if(!(*gplev0 & SET_REG32(PUSH2)))
    	    *gpclr0 = SET_REG32(LED2);
	 else 
	    *gpset0 = SET_REG32(LED2);

	if(!(*gplev0 & SET_REG32(PUSH3)))
    	    *gpclr0 = SET_REG32(LED3);
	else 
	    *gpset0 = SET_REG32(LED3);

	usleep(500000);
    }

    // Free allocated memory
    munmap(gpio_base_addr, 4096);

    close(fd);

    return 0;
}

int bit_check(unsigned int value, char* string, int mod)
{
	int repi;

	for(repi=31;repi>=0;repi--) {
		if(value&(1<<repi))
			printf("1 ");
		else
			printf("0 ");

		if(repi%mod==0)
			printf("| ");
	}

	printf("%s\n",string);

	return 0;
}

