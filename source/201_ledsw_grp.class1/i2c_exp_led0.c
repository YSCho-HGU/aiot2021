/* RaspberryPi 4 for Automotive IoT Kit
 * File  : i2c_exp_led.c
 * TITLE : GPIO LED control based on I2c Expander 
 * Auth  : mkdev.co.kr x makeutil.tistory.com
 * Ment  : This program code may not be used for commercial purposes. 
 *         The author's information cannot be changed at the time of redistribution.*/


// Define the header & constants
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#define LEDSW_ADDR 0x20

int main(int argc, char** argv)
{
	int led_fd;
	int retv;
/* RaspberryPi 4 for Automotive IoT Kit
 * File  : i2c_exp_led.c
 * TITLE : GPIO LED control based on I2c Expander 
 * Auth  : mkdev.co.kr x makeutil.tistory.com
 * Ment  : This program code may not be used for commercial purposes. 
 *         The author's information cannot be changed at the time of redistribution.*/


// Define the header & constants
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#define LEDSW_ADDR 0x20

int main(int argc, char** argv)
{
	int led_fd;
	int retv;
    
	char value=0;
	char values[2]={0,};

	// Open I2C Device
	led_fd = open("/dev/i2c-1",O_RDWR);
	if(led_fd<3)
	{
		printf("Device Open Error!\n");
		return -1;
	}

	// Select I2C Address
	retv=ioctl(led_fd, I2C_SLAVE, LEDSW_ADDR);
	if(retv!=0)
	{
		printf("Ioctl Error!\n");
		return -1;
	}

	// GPIO Exapnder 상위 8비트를 출력으로 설정
	values[0] = 0x07;           // GPIO Register - TCA9535 Data Sheet 23p
	values[1] = 0x00;           // 0으로 설정하면 출력으로, 1로 설정하면 입력으로 설정된다.
	write(led_fd,values, 2);       // 설정한 값을 i2c 버스에 보내서 해당장치에 기록한다.

	// LED를 초기화 한다.
	values[0] = 0x03;           // 출력 관련 설정 레지스터 0x03을 지정한다.
	values[1] = 0xff;            // LED 출력핀을 High로 만들어 LED를 소등시킨다.
	write(led_fd, values, 2);      // 설정값을 적용한다.

	value = 0xff;                // 기준값을 설정한다. 11111111 (b) = 255(10)- All Off
	while(1)
	{
		if(value<=0)              // value가 0보다 반복문을 종료
			break;
		values[1] = value;      // value[0]은 앞에서 이미 지정되었으니 값만 입력
		write(led_fd,values,2);   // 입력된 값을 적용한다.
		
		usleep(100000);        // 100ms
		value-=1;              // value를 1 감소시킨다.
	}

	// 종료시 처리할 내용을 지정
	values[1]=0xff;         // LED가 모두 켜져있을 테니 모두 꺼두자.
	write(led_fd,values,2);

	close(led_fd);

	return 0;
}
