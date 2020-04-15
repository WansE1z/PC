#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "link_emulator/lib.h"

#define HOST "127.0.0.1"
#define PORT 10000

void transmit(char* file_name) {
  int fd = open(file_name, O_RDONLY);
  if (fd < 0) {
    perror("Missing file!\n");
    exit(1);
  }

  msg t;
  content c;
  c.type = HEADER_NAME;
  memset(c.payload, 0, sizeof(c.payload));
  memcpy(c.payload, file_name, strlen(file_name));
  t.len = 5 + strlen(file_name);
  memcpy(t.payload, &c, t.len);
  send_message(&t);

  if (recv_message(&t) < 0) {
    perror("receive error");
  }

  printf("[./send] Got reply with payload: %s\n",t.payload);

  int len = lseek(fd, 0, SEEK_END);

  c.type = HEADER_LENGTH;
  memset(c.payload, 0, sizeof(c.payload));
  memcpy(c.payload, &len, 4);
  t.len = 8;

  memcpy(t.payload, &c, t.len);
  send_message(&t);

  if (recv_message(&t) < 0) {
    perror("receive error");
  }

  printf("[./send] Got reply with payload: %s\n",t.payload);
  lseek(fd, 0, SEEK_SET);
  memset(c.payload, 0, sizeof(c.payload));
  c.type = DATA;
  while ((len = read(fd, c.payload, sizeof(c.payload)))) {
    t.len = 4 + len;
    memcpy(t.payload, &c, t.len);
    send_message(&t);

    if (recv_message(&t) < 0) {
      perror("receive error");
    }

    printf("[./send] Got reply with payload: %s\n",t.payload);
  }
}

int main(int argc,char** argv){
  init(HOST,PORT);
  msg t;

  sprintf(t.payload,"Hello World of PC");
  t.len = strlen(t.payload)+1;
  send_message(&t);

  if (recv_message(&t)<0){
    perror("receive error");
  }
  else {
    printf("[%s] Got reply with payload: %s\n",argv[0],t.payload);
  }
  if (argc <= 1) {
    printf("Need more args!\n");
  } else {
    int i;
    for (i = 1; i < argc; i++)
      transmit(argv[i]);
    memset(&t, 0, sizeof(msg));
    int type = EOT;
    t.len = 4;
    memcpy(t.payload, &type, 4);
    send_message(&t);

  }
  return 0;
}
