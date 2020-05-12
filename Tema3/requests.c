#include "requests.h"

#include <arpa/inet.h>
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <stdio.h>
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <unistd.h>     /* read, write, close */

#include "helpers.h"

// function that creates a string that will be used by the delete request
char *compute_delete_request(char *host, char *url, char *query_params,
                             char **cookies, int cookies_count, char *jwt) {
  char *message = calloc(BUFLEN, sizeof(char));
  char *line = calloc(LINELEN, sizeof(char));

  // write the method name, URL, request params (if any) and protocol type

  char msg[BUFLEN];
  memset(line, 0, LINELEN);
  if (query_params != NULL) {
    sprintf(line, "DELETE %s?%s HTTP/1.1", url, query_params);
  } else {
    sprintf(line, "DELETE %s HTTP/1.1", url);
  }

  // add the host
  compute_message(message, line);
  sprintf(line, "Host: %s", host);
  compute_message(message, line);

  // add headers and/or cookies, according to the protocol format
  if (cookies != NULL) {
    sprintf(line, "Cookie: ");
    sprintf(msg, "%s ", cookies[0]);
    strcat(line, msg);
    compute_message(message, line);
  }

  // add the jwt
  if (jwt != NULL) {
    sprintf(line, "Authorization: Bearer %s", jwt);
    compute_message(message, line);
  }

  // add newline
  compute_message(message, "");
  return message;
}

// function that creates a string that will be used by the get request
char *compute_get_request(char *host, char *url, char *query_params,
                          char **cookies, int cookies_count, char *jwt) {
  char *message = calloc(BUFLEN, sizeof(char));
  char *line = calloc(LINELEN, sizeof(char));
  char msg[BUFLEN];

  // write the method name, URL, request params (if any) and protocol type

  memset(line, 0, LINELEN);
  if (query_params != NULL) {
    sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    compute_message(message, line);
  } else {
    sprintf(line, "GET %s HTTP/1.1", url);
    compute_message(message, line);
  }

  // add the host
  sprintf(line, "Host: %s", host);
  compute_message(message, line);

  // add headers and/or cookies, according to the protocol format
  if (cookies != NULL) {
    sprintf(line, "Cookie: ");
    sprintf(msg, "%s ", cookies[0]);
    strcat(line, msg);
    compute_message(message, line);
  }

  // add the jwt
  if (jwt != NULL) {
    sprintf(line, "Authorization: Bearer %s", jwt);
    compute_message(message, line);
  }

  // Step 4: add final new line
  strcat(message, "\r\n");
  return message;
}

// function that creates a string that will be used by the post request
char *compute_post_request(char *host, char *url, char *content_type,
                           char **body_data, int body_data_count, char **cookies,
                           int cookies_count, char *jwt) {
  size_t size;
  char *message = calloc(BUFLEN, sizeof(char));
  char *line = calloc(LINELEN, sizeof(char));
  char *bodydata_buf = calloc(LINELEN, sizeof(char));
  char msg[BUFLEN];
  // write the method name, URL and protocol type

  memset(line, 0, LINELEN);
  sprintf(line, "POST %s HTTP/1.1", url);
  compute_message(message, line);

  // add the jwt
  if (jwt != NULL) {
    sprintf(line, "Authorization: Bearer %s", jwt);
    compute_message(message, line);
  }

  // add the host
  sprintf(line, "Host: %s : 8080", host);
  compute_message(message, line);

  /*
   add necessary headers (Content-Type and Content-Length are mandatory)
   in order to write Content-Length you must first compute the message size
  */
  sprintf(line, "Content-Type: %s", content_type);
  compute_message(message, line);

  if (body_data != NULL) {
    memset(bodydata_buf, 0, LINELEN);
    sprintf(msg, "%s", body_data[0]);
    strcat(bodydata_buf, msg);
  }

  size = strlen(bodydata_buf);
  sprintf(line, "Content-Length: %ld", size);
  compute_message(message, line);

  // add cookies
  if (cookies != NULL) {
    compute_message(message, line);
  }

  // add new line at end of header
  strcat(message, "\r\n");

  // add the actual payload data
  if (body_data != NULL) {
    compute_message(message, bodydata_buf);
  }

  free(line);
  return message;
}