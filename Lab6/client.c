/*
*  	Protocoale de comunicatii: 
*  	Laborator 6: UDP
*	client mini-server de backup fisiere
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
	fprintf(stderr,"Usage: %s ip_server port_server file\n",file);
	exit(0);
}

/*
*	Utilizare: ./client ip_server port_server nume_fisier_trimis
*/
int main(int argc,char**argv)
{
	if (argc!=4)
		usage(argv[0]);
	
	int fd,sock;
	struct sockaddr_in to_station;
	char buf[BUFLEN];
	memset(buf, 0, BUFLEN);
	sock = socket(PF_INET, SOCK_DGRAM, 0);
	if (sock < 0){
		perror("Failed to create socket.");
	}
	to_station.sin_family = AF_INET;
	to_station.sin_port = htons(atoi(argv[2]));
	inet_aton(argv[1], &(to_station.sin_addr));
	fd = open(argv[3], O_RDONLY);
	sprintf(buf, "%s", argv[3]);
	sendto(sock,buf,strlen(buf),0,(struct sockaddr *)&to_station, sizeof(to_station));
	memset(buf,0,BUFLEN);
	int bytesToRead;
	/*Deschidere socket*/
	while ((bytesToRead = read(fd,buf,BUFLEN-1))){
		sendto(sock,buf,strlen(buf),0,(struct sockaddr *)&to_station, sizeof(to_station));
		memset(buf,0,BUFLEN);
	}
	memset(buf,0,BUFLEN);
	sprintf(buf, "It was sent.");
	sendto(sock,buf,strlen(buf),0,(struct sockaddr *)&to_station, sizeof(to_station));
	close(sock);
	close(fd);
	return 0;

	
	/*Setare struct sockaddr_in pentru a specifica unde trimit datele*/
	
	
	
	/*
	*  cat_timp  mai_pot_citi
	*		citeste din fisier
	*		trimite pe socket
	*/	


	/*Inchidere socket*/

	
	/*Inchidere fisier*/

	return 0;
}


