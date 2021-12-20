/* RaspberryPi 4 for Automotive IoT Kit
 * TITLE : 
 * File  : can_ex2.c
 * Auth  : wikipedia.org
 * Ment  : Original Soruce - http://en.wikipedia.org/wiki/SocketCAN 
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

#define CAN_ID


int main(void)
{
	int can_fd;
	int nbytes;
	struct sockaddr_can addr;
	struct can_frame can_send, can_recv;
	struct ifreq ifr;
	struct timeval timeout;
	int getCanID=0x00;
	ssize_t recv_bytes=0;

	const char *ifname = "vcan0";

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

	can_send.can_id  = 0x100;
	can_send.can_dlc = 8;
	can_send.data[0] = 0x11;
	can_send.data[1] = 0x22;
	can_send.data[2] = 0x33;
	can_send.data[3] = 0x44;
	can_send.data[4] = 0x55;
	can_send.data[5] = 0x66;
	can_send.data[6] = 0x77;
	can_send.data[7] = 0x88;

	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	while(1) {

		fd_set read_set;
		FD_ZERO(&read_set);
		FD_SET(can_fd, &read_set);

		timeout.tv_sec = 1;
		// Send can frame to target device.
		nbytes = write(can_fd, &can_send, sizeof(struct can_frame));

		if(nbytes>0)
			printf("Send %d data to can bus.\n",nbytes);
		else
			printf("Send Error!\n");

		// Read can frame data when ready
		if(select((can_fd +1), &read_set, NULL, NULL, &timeout) >=0) {
			if(FD_ISSET(can_fd, &read_set)) {
				recv_bytes = read(can_fd, &can_recv, sizeof(struct can_frame));
				if(recv_bytes) {
					int getCanID = (can_recv.can_id & CAN_EFF_FLAG) ? (can_recv.can_id & CAN_EFF_MASK) : (can_recv.can_id & CAN_SFF_MASK);

					switch(getCanID) {
						case 0x10 :
							printf("ID 10 - Data Received\n");
							break;
						case 0x20 :
							printf("ID 20 - Data Received\n");
							break;
						case 0x40 :
							printf("ID 40 - Data Received\n");
							break;
						default :
							printf("Unknow ID : %03x\n",getCanID);
							break;
					}

				}
			}
		}
	}

	printf("Wrote %d bytes\n", nbytes);

	return 0;
}

