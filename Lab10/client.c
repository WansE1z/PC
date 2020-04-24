#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

int main(int argc, char *argv[])
{
    char *message;
    char *response;
    int sockfd;

        
    // Ex 1.1: GET dummy from main server
    sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM,0);
    message = compute_get_request("3.8.116.10","/api/v1/dummy",NULL,NULL,0);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    printf("%s\n", response);
    close_connection(sockfd);

    // Ex 1.2: POST dummy and print response from main server
    sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM,0);
    char* mesaj[1];
    mesaj[0] = "mesaj=radu";
    message = compute_post_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com","/api/v1/dummy","application/x-www-form-unlencoded",mesaj,1,NULL,0);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    printf("%s\n", response);
    close_connection(sockfd);

    // Ex 2: Login into main server
    sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM,0);
    char* login[2];
    login[0] = "username=student";
    login[1] = "password=student";
    message = compute_post_request("3.8.116.10", "/api/v1/auth/login", "application/x-www-form-urlencoded", login, 2, NULL, 0);
    printf("%s\n", message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    printf("%s\n", response);
    close_connection(sockfd);

    // Ex 3: GET weather key from main server
    sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM,0);
    char* cookies[1];
    cookies[0] = "connect.sid=s%3AeVsWt8yWss7LQK5ey4UFWHZ-ov6VXeIq.96nTIa%2BJ8C16RCxszwgut5EXIEupyPF%2F%2B5aonrC7S5w";
    message = compute_get_request("3.8.116.10", "/api/v1/weather/key", NULL, cookies, 1);
    printf("%s\n", message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    printf("%s\n", response);
    close_connection(sockfd);

    // Ex 4: GET weather data from OpenWeather API
    sockfd = open_connection("37.139.1.159", 80, AF_INET, SOCK_STREAM, 0);
    message = compute_get_request("37.139.1.159", "/data/2.5/weather", "lat=12&lon=12&appid=b912dd495585fbf756dc6d8f415a7649", NULL, 0);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    printf("%s", response);
    close_connection(sockfd);

    // Ex 5: POST weather data for verification to main server
    sockfd = open_connection("37.139.1.159", 80, AF_INET, SOCK_STREAM, 0); 
    char* data[1];
    data[0] = basic_extract_json_response(message);
    message = compute_post_request("3.8.116.10", "/api/v1/weather/12/12", "application/json", data, 1, NULL, 0);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    printf("%s", response);

    // Ex 6: Logout from main server
    message = compute_get_request("3.8.116.10", "/api/v1/auth/logout", NULL, NULL, 0);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    printf("%s", response);
    close_connection(sockfd);
    return 0;
}
