/* Raspberry Pi 4 for Automotive IoT KIT
 * TITLE : CAN Example 3 (thread)
 * FILE  : ~/source/602_can/can_ex3.c
 * AUTH  : mkdev.co.kr / makeutil.tistory.com
 * Ment  : This program code may not be used for commercial purposes.
 *			The author's information cannot be changed at the time of redistribution. 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include <pthread.h>

void *thread_send(void *arg);
void *thread_recv(void *arg);

int g_is_thread_run = 0;

int main(int argc)
{
	int can_fd;
	int nbytes;
	int thid_send, thid_recv;

	ssize_t recv_bytes=0;
	const char *ifname = "vcan0";

	struct sockaddr_can addr;
	struct can_frame can_send, can_recv;
	struct ifreq ifr;

	pthread_t pth_can_send, pth_can_recv;

	if ((can_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW)) == -1) {
		perror("Error while opening socket");
		return -1;
	}

	strcpy(ifr.ifr_name, ifname);
	ioctl(can_fd, SIOCGIFINDEX, &ifr);

	addr.can_family  = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	printf("%s at index %d\n", ifname, ifr.ifr_ifindex);
	if (bind(can_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		perror("Error in socket bind");
		return -2;
	}

	g_is_thread_run = 1;

	thid_recv = pthread_create(&pth_can_recv, NULL, thread_recv, &can_fd);
	thid_send = pthread_create(&pth_can_send, NULL, thread_send, &can_fd) ;
	if ((thid_send < 0)&&(thid_recv < 0)) {
		printf("Thread Create Error!\n");
		g_is_thread_run = 0;
		exit(0);
	}

	pthread_join(pth_can_recv, NULL);
	pthread_join(pth_can_send, NULL);

	close(can_fd);

	printf("Program Finished!\n");

	return 0;
}

void *thread_send(void *arg)
{
	int repi=0;
	int nbytes;
	int *pcan_fd;
	struct can_frame pcan_send;

	pcan_fd = (int *)arg;

	pcan_send.can_id  = 0x100;
	pcan_send.can_dlc = 8;
	pcan_send.data[0] = 0x11;
	pcan_send.data[1] = 0x22;
	pcan_send.data[2] = 0x33;
	pcan_send.data[3] = 0x44;
	pcan_send.data[4] = 0x55;
	pcan_send.data[5] = 0x66;
	pcan_send.data[6] = 0x77;
	pcan_send.data[7] = 0x88;

	while (g_is_thread_run) {
		nbytes = write(*pcan_fd, &pcan_send, sizeof(struct can_frame));
		printf("Wrote %d bytes\n", nbytes);
		usleep(1000000);
	} 
	printf("Thread Send Finished!\n");
}

void *thread_recv(void *arg)
{
	int repi=0;
	int recv_bytes;
	int *pcan_fd;
	struct can_frame pcan_recv;

	pcan_fd = (int *)arg;

	while (g_is_thread_run) {
		recv_bytes = read(*pcan_fd, &pcan_recv, sizeof(struct can_frame));
		if(recv_bytes>0) {
			printf("Recv IDX : %02x (Data Length : %d) ",pcan_recv.can_id,pcan_recv.can_dlc);
			for(repi=0;repi<pcan_recv.can_dlc;repi++) {
				printf("[%02x] ");
			}
			printf("Receive Size : %d\n",recv_bytes);
		}
		usleep(1000000);
	}

	printf("Thread Read Finished!\n");
}
