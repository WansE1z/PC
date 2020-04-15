#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include "lib.h"


#define HOST "127.0.0.1"
#define PORT 10000

int main(int argc, char *argv[])
{
	msg t;
	int res,stanga = 0,dreapta = 4;
	time_t seconds;
	char* bytes;
	printf("[SENDER] Starting.\n");	
	init(HOST, PORT);

		memset(&t, 0, sizeof(msg));
		t.len = MSGSIZE;
		res = send_message(&t);
		time(&seconds);
		printf("sending 0 in %ld seconds\n", seconds);
		bytes = convertToBytes(&t);
		

		memset(&t, 0, sizeof(msg));
		t.len = MSGSIZE;
		res = send_message(&t);
		time(&seconds);
		printf("sending 1 in %ld seconds\n", seconds);

		memset(&t, 0, sizeof(msg));
		t.len = MSGSIZE;
		res = send_message(&t);
		time(&seconds);
		printf("sending 2 in %ld seconds\n", seconds);

		memset(&t, 0, sizeof(msg));
		t.len = MSGSIZE;
		res = send_message(&t);
		time(&seconds);
		printf("sending 3 in %ld seconds\n", seconds);

	/* printf("[SENDER]: BDP=%d\n", atoi(argv[1])); */
	while (stanga < dreapta && stanga <= 97 && dreapta <= 99){
		res = recv_message(&t);
		if (res >= 0){
			stanga++;
			dreapta++;
			memset(&t,0,sizeof(msg));
			t.len = MSGSIZE;
			res = send_message(&t);
			time(&seconds);
			int dr = dreapta -1;
			printf("sending %d in %ld seconds\n", dr, seconds);
		}
	}
	printf("[SENDER] Job done, all sent.\n");
		
	return 0;
}
