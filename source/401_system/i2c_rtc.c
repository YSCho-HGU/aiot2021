/* RaspberryPi 4 for Automotive IoT Kit
 * File  : ~/source/401_system/i2c_rtc.c
 * TITLE : Read data from RTC & Write data to RTC 
 * Auth  : mkdev.co.kr / makeutil.tistory.com
 * Ment  : This program code may not be used for commercial purposes. 
 *         The author's information cannot be changed at the time of redistribution.*/


// Define the header & constants
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include <time.h>

#include <linux/i2c-dev.h>

#include "../aiot_hardware.h"

#define DS1337_REG_SEC    0
#define DS1337_REG_MIN    1
#define DS1337_REG_HOUR   2
#define DS1337_REG_DAY    4
#define DS1337_REG_MONTH  5
#define DS1337_REG_YEAR   6

int bit_check(unsigned int value, int size, char* string, int mod);
int string_parser(char* src, char* dest, int offset, int size, int length);
unsigned char set_reg_ds1337(int _fd, unsigned char addr, unsigned char num);

int main(int argc, char** argv) 
{
	int rtc_fd;
	int retv;
	char value=0;
	unsigned char clock[10];
	unsigned char reg=0, bh, bl;

	int year;
	int month;
	int day;

	int hour;
	int min;
	int sec;

	char str[12]={0,};

	if(argc!=1 && argc!=3) {
		printf("Illegal usage...\n");
		printf("Read Ex) %s\n",argv[0]);
	        printf("Write Ex) %s [YYYYMMDD] [HHMMSS]\n",argv[0]);
		return -1;
	}

	// I2C Devce open.
	rtc_fd = open("/dev/i2c-1",O_RDWR);
	if(rtc_fd<3) {
		printf("Device Open Error!\n");
		return -1;
	}

	// Select I2C Address
	retv=ioctl(rtc_fd, I2C_SLAVE, I2C_RTC);
	if(retv!=0) {
		printf("Ioctl Error!\n");
		return -1;
	}

	if(argc==3) {
		printf("Set-Up DS1337...\n");

//int string_parser(char* src, char* dest, int offset, int size, int length);
		year=string_parser(argv[1], str, 0, 4, sizeof(str));
		month=string_parser(argv[1], str, 4, 2, sizeof(str));
		day=string_parser(argv[1], str, 6, 2, sizeof(str));
		hour=string_parser(argv[2], str, 0, 2, sizeof(str));
		min=string_parser(argv[2], str, 2, 2, sizeof(str));
		sec=string_parser(argv[2], str, 4, 2, sizeof(str));

		printf("%04d-%02d-%02d %02d:%02d:%02d\n",year,month,day,hour,min,sec);
		bit_check(set_reg_ds1337(rtc_fd, DS1337_REG_YEAR, year-2000), sizeof(char), "Year", 4);
		bit_check(set_reg_ds1337(rtc_fd, DS1337_REG_MONTH, month), sizeof(char), "Month", 4);
		bit_check(set_reg_ds1337(rtc_fd, DS1337_REG_DAY, day), sizeof(char), "Day", 4);
		bit_check(set_reg_ds1337(rtc_fd, DS1337_REG_HOUR, hour), sizeof(char), "Hour", 4);
		bit_check(set_reg_ds1337(rtc_fd, DS1337_REG_MIN, min), sizeof(char), "Minute", 4);
		bit_check(set_reg_ds1337(rtc_fd, DS1337_REG_SEC, sec), sizeof(char), "Second", 4);

		return 0;
	} 

	printf("Read from DS1337...\n");
	while(1) {
		value=0x00; // address to access timekeeper registers
		write(rtc_fd,&value,1);	

		// read data from timekeeper registers (0x00~0x06)
		read(rtc_fd,clock,7); 

		printf("%04d-",(((clock[6]&0xF0)>>4)*10+(clock[6]&0xF))+2000); // Year
		printf("%02d-",((clock[5]&0x10)>>4)*10+(clock[5]&0xF)); // Month
		printf("%02d ",((clock[4]&0x30)>>4)*10+(clock[4]&0xF)); // Date

		if(clock[2]&0x40) { // Hour
			printf("%02d:",((clock[2]&0x10)>>4)*12+(clock[2]&0xF)); // 12H
			if (clock[2]&0x20) printf(" PM");
			else printf( " AM"); 
		} else {
			printf("%02d:",((clock[2]&0x30)>>4)*10+(clock[2]&0xF)); // 24H
		}
		printf("%02d:",((clock[1]&0x70)>>4)*10+(clock[1]&0xF)); // Minutes
		printf("%02d ",((clock[0]&0x70)>>4)*10+(clock[0]&0xF)); // Seconds
		printf("\n");
		sleep(1);
	}

	close(rtc_fd);

	return 0;
}

int bit_check(unsigned int value, int size,  char* string, int mod)
{
	int repi = size*8-1;

	for(repi;repi>=0;repi--) {
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


int string_parser(char* src, char* dest, int offset, int size, int length)
{
	int slength;

	slength = strlen(src);
	if(slength<1)
		return -1;

	if(slength<size)
		return -1;

	if(slength<(offset+size))
		return -1;

	memset(dest,0,length);
	strncat(dest, src+offset, size);
	
	return atoi(dest);	
}

unsigned char set_reg_ds1337(int _fd, unsigned char addr, unsigned char num)
{
	unsigned char bl, bh;
	unsigned char regs[2]={0,};

	if(addr>0xf)
		return -1;

	bl = num%10;
	bh = (num-bl)/10;

	regs[0] = addr; // address to access timekeeper registers in DS1337 chip
	regs[1] = (bh<<4)|bl; // BIT[7:4] | BIIT[3:0]

	if(!(_fd<2))
		write(_fd, regs,sizeof(regs));

	return regs[1];
}


