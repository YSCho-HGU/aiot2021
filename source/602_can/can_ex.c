/*
 * Example of the SocketCAN API that sends a packet 
 *		to the virtual CAN device (vcan0) using the raw interface
 * Filename: ~/source/602_can/can_ex.c
 * Original Soruce - http://en.wikipedia.org/wiki/SocketCAN
 *
 * Before running this program, you first install a virtual CAN driver
 * % modprobe vcan
 * % sudo ip link add dev vcan0 type vcan
 * % sudo ip link set up vcan0
 * % ip link show vcan0
 *
 * The packet can be anlayzed on the vcan0 interface using the candump 
 *     utility which is part of the SocketCAN can-utils package
 * % candump vcan0
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

int main(void)
{
	int can_fd;
	int nbytes;
	struct sockaddr_can addr;
	struct can_frame frame;
	struct ifreq ifr;

	const char *ifname = "vcan0";

	if ((can_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW)) == -1 ) {
		perror("Error while opening socket");
		return -1;
	}

	strcpy(ifr.ifr_name, ifname);
	ioctl(can_fd, SIOCGIFINDEX, &ifr);

	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	printf("%s at index %d\n", ifname, ifr.ifr_ifindex);

	if( bind(can_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1 ) {
		perror("Error in socket bind");
		return -2;
	}

	frame.can_id = 0x10;
	frame.can_dlc = 8;
	frame.data[0] = 0x11;
	frame.data[1] = 0x22;
	frame.data[2] = 0x33;
	frame.data[3] = 0x44;
	frame.data[4] = 0x55;
	frame.data[5] = 0x66;
	frame.data[6] = 0x77;
	frame.data[7] = 0x88;

	nbytes = write(can_fd, &frame, sizeof(struct can_frame));

	printf("Wrote %d bytes\n", nbytes);

	return 0;
}

