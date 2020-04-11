/*
*  	Protocoale de comunicatii: 
*  	Laborator 6: UDP
*	mini-server de backup fisiere
*/

#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>

#include "helpers.h"


void usage(char*file)
{
	fprintf(stderr,"Usage: %s server_port file\n",file);
	exit(0);
}

/*
*	Utilizare: ./server server_port nume_fisier
*/
int main(int argc,char**argv)
{
	int fd,sock;
	socklen_t length;

	if (argc!=3)
		usage(argv[0]);
	
	struct sockaddr_in from_station ;
	char buf[BUFLEN];
	memset(buf,0,BUFLEN);
	sock = socket(PF_INET, SOCK_DGRAM,0);
	from_station.sin_family = AF_INET;
	from_station.sin_port = htons(atoi(argv[1]));
	from_station.sin_addr.s_addr = INADDR_ANY;
	int r = bind(sock, (struct sockaddr*)&from_station,sizeof(struct sockaddr_in));
	if (r<0){
		perror("Failed to bind the socket!");
	}
	recvfrom(sock,buf,BUFLEN,0,(struct sockaddr*)&from_station, &length);
	printf("%s\n",buf);
	fd = open(buf , O_WRONLY | O_CREAT, 0644);
	memset(buf,0,BUFLEN);
	
	while (1){
		recvfrom(sock,buf,7,0,(struct sockaddr *) &from_station, &length);
		if (strcmp(buf, "It was sent.") == 0){
			break;
		}
		write(fd,buf,strlen(buf));
		memset(buf,0,BUFLEN);
	}
	close(sock);
	close(fd);

	return 0;
}

