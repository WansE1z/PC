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

char *compute_delete_request(char *host, char *url, char *query_params,
                             char **cookies, int cookies_count, char *jwt) {
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    char msg[BUFLEN];
    memset(line, 0, LINELEN);
    if (query_params != NULL) {
        sprintf(line, "DELETE %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "DELETE %s HTTP/1.1", url);
    }

    compute_message(message, line);

    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    if (cookies != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Cookie: ");
        for (int i = 0; i < cookies_count; i++) {
            sprintf(msg, "%s; ", cookies[i]);
            strcat(line, msg);
        }
        compute_message(message, line);
    }
    if (jwt != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer %s", jwt);
        compute_message(message, line);
    }
    compute_message(message, "");
    return message;
}

char *compute_get_request(char *host, char *url, char *query_params,
                          char **cookies, int cookies_count, char *jwt) {
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    char msg[BUFLEN];
    memset(line, 0, LINELEN);
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    if (cookies != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Cookie: ");
        for (int i = 0; i < cookies_count; i++) {
            sprintf(msg, "%s; ", cookies[i]);
            strcat(line, msg);
        }
        compute_message(message, line);
    }
    if (jwt != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer %s", jwt);
        compute_message(message, line);
    }
    compute_message(message, "");
    return message;
}

char *compute_post_request(char *host, char *url, char *content_type,
                           char **body_data, int body_data_count, char **cookies,
                           int cookies_count, char *jwt) {
    size_t size;
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *bodydata_buf = calloc(LINELEN, sizeof(char));
    char msg[BUFLEN];

    memset(line, 0, LINELEN);
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    if (jwt != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer %s", jwt);
        compute_message(message, line);
    }

    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s : 8080", host);
    compute_message(message, line);

    memset(line, 0, LINELEN);
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    if (body_data != NULL) {
        memset(bodydata_buf, 0, LINELEN);
        for (int i = 0; i < body_data_count; i++) {
            sprintf(msg, "%s", body_data[i]);
            strcat(bodydata_buf, msg);
        }
    }

    memset(line, 0, LINELEN);
    size = strlen(bodydata_buf);
    sprintf(line, "Content-Length: %ld", size);
    compute_message(message, line);

    if (cookies != NULL) {
        memset(line, 0, LINELEN);
        for (int i = 0; i < cookies_count; i++) {
            sprintf(msg, "%s; ", cookies[i]);
            strcat(line, msg);
        }
        compute_message(message, line);
    }
    compute_message(message, "");

    if (body_data != NULL) {
        compute_message(message, bodydata_buf);
    }

    free(line);
    return message;
}