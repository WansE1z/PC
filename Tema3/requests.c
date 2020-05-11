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

char *compute_get_request(char *host, char *url, char *query_params,
                          char **cookies, int cookies_count, char *jwt) {
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol
    // type
    memset(line, 0, LINELEN);
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Step 2: add the host
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    // Step 3 (optional): add headers and/or cookies, according to the protocol
    // format
    if (cookies != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Cookie: ");
        for (int i = 0; i < cookies_count - 1; i++) {
            char to_append[100];
            sprintf(to_append, "%s; ", cookies[i]);
            strcat(line, to_append);
        }
        char to_append[100];
        sprintf(to_append, "%s; ", cookies[cookies_count - 1]);
        strcat(line, to_append);
        compute_message(message, line);
    }
    if (jwt != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer %s", jwt);
        compute_message(message, line);
    }
    // Step 4: add final new line
    compute_message(message, "");
    return message;
}

char *compute_post_request(char *host, char *url, char *content_type,
                           char **body_data, int body_data_fields_count,
                           char **cookies, int cookies_count) {
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *body_data_buffer = calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    memset(line, 0, LINELEN);
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    // Step 2: add the host
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    /* Step 3: add necessary headers (Content-Type and Content-Length are
       mandatory) in order to write Content-Length you must first compute the
       message size
    */
    memset(line, 0, LINELEN);
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    if (body_data != NULL) {
        memset(body_data_buffer, 0, LINELEN);
        for (int i = 0; i < body_data_fields_count - 1; i++) {
            char to_append[100];
            sprintf(to_append, "%s&", body_data[i]);
            strcat(body_data_buffer, to_append);
        }
        char to_append[100];
        sprintf(to_append, "%s&", body_data[body_data_fields_count - 1]);
        strcat(body_data_buffer, to_append);
    }

    memset(line, 0, LINELEN);
    sprintf(line, "Content-Length: %ld", strlen(body_data_buffer));
    compute_message(message, line);
    // Step 4 (optional): add cookies
    if (cookies != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Cookie: ");
        for (int i = 0; i < cookies_count - 1; i++) {
            char to_append[100];
            sprintf(to_append, "%s; ", cookies[i]);
            strcat(line, to_append);
        }
        char to_append[100];
        sprintf(to_append, "%s; ", cookies[cookies_count - 1]);
        strcat(line, to_append);
        compute_message(message, line);
    }
    // Step 5: add new line at end of header
    compute_message(message, "");

    // Step 6: add the actual payload data
    if (body_data != NULL) {
        compute_message(message, body_data_buffer);
    }

    free(line);
    return message;
}