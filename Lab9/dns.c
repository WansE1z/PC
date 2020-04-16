// Protocoale de comunicatii
// Laborator 9 - DNS
// dns.c

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

int usage(char* name)
{
	printf("Usage:\n\t%s -n <NAME>\n\t%s -a <IP>\n", name, name);
	return 1;
}

// Receives a name and prints IP addresses
void get_ip(char* name)
{
	int ret;
	struct addrinfo hints, *result, *p;
	memset(&hints, 0, sizeof(hints));
	char ip[INET_ADDRSTRLEN];
	int sizeIp = sizeof(ip);
	char ipv6[INET6_ADDRSTRLEN];
	int sizeIp6 = sizeof(ipv6);

	// TODO: set hints
	hints.ai_family = AF_UNSPEC;
	hints.ai_protocol = IPPROTO_TCP;

	// TODO: get addresses
	ret = getaddrinfo(name,NULL,&hints,&result);
	if (ret < 0){
		gai_strerror(ret);
	}

	// TODO: iterate through addresses and print them

	for (p = result; p != NULL; p = p->ai_next){
		if (p->ai_family == AF_INET){
			struct sockaddr_in* addr = (struct sockaddr_in*)p->ai_addr;
			if (inet_ntop(p->ai_family,&(addr->sin_addr),ip,sizeIp) != NULL){
				printf("IP is %s\n", ip);
				printf("Port is %d\n", ntohs(addr->sin_port));
			}
		} else if (p->ai_family == AF_INET6){
			struct sockaddr_in6* addr = (struct sockaddr_in6*)p->ai_addr;
			if (inet_ntop(p->ai_family, &(addr->sin6_addr), ipv6, sizeIp6) != NULL){
				printf("IP is %s\n", ipv6);
				printf("Port is %d\n", ntohs(addr->sin6_port));
			}
		}
	}
	// TODO: free allocated data
	freeaddrinfo(result);
}

// Receives an address and prints the associated name and service
void get_name(char* ip)
{
	struct sockaddr_in addr;
	char host[1024];
	char service[20];

	// TODO: fill in address data
	int portno = 8000;
	if (portno < 0){
		printf("Error, port not completed in the right way.");
	}
	addr.sin_family = AF_INET;
	addr.sin_port = htons(portno);
	inet_aton(ip, &addr.sin_addr);
	// TODO: get name and service

	getnameinfo((struct sockaddr*) &addr, sizeof(struct sockaddr_in), host, 1024, service , 20, 0);
	// TODO: print name and service
	printf("Name: %s\n", host);
	printf("Service: %s\n", service);

}

int main(int argc, char **argv)
{
	if (argc < 3) {
		return usage(argv[0]);
	}

	if (strncmp(argv[1], "-n", 2) == 0) {
		get_ip(argv[2]);
	} else if (strncmp(argv[1], "-a", 2) == 0) {
		get_name(argv[2]);
	} else {
		return usage(argv[0]);
	}

	return 0;
}
