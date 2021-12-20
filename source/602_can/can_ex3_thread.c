/* Raspberry Pi 4 for Automotive IoT KIT
 * TITLE : CAN Example 3 (thread)
 * FILE  : ~/source/502_can/can_ex3_thread.c
 * % gcc can_ex3_thread.c -lpthread -o can_ex3_thread
 *
 * AUTH  : mkdev.co.kr / makeutil.tistory.com
 * Ment  : This program code may not be used for commercial purposes.
 *			The author's information cannot be changed at the time of redistribution. */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>

void *thread_send(void *arg)
{
	 pid_t pid;	// process id
	 pthread_t tid; // thread id

	 pid = getpid();
	 tid = pthread_self();

	 printf("> pID in Thread : %lu\n",(unsigned int)pid);
	 printf("> Thread ID	  : %lu\n",(unsigned int)tid);

	for (int repi=0; repi < 10; repi ++) {	
		 printf("Thread Count : %d!\n",repi);
		 sleep(1); //usleep(1000000);
	 }
	 printf("Thread Finished!\n");
}

int main(int argc)
{
	 int thid_send;
	 int status;

	 pid_t m_pid;
	 pthread_t pth_can_read;

	 m_pid = getpid();
	 printf("main() process ID : %u\n",(unsigned int)m_pid);

	 thid_send = pthread_create(&pth_can_read, NULL, thread_send, NULL);
	 if (thid_send < 0) {
		  printf("Thread Create Error!\n");
		  exit(0);
	 }

	 pthread_join(pth_can_read, NULL);
	 printf("Program Finished!\n");
	 return 0;
}
