#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "link_emulator/lib.h"

#define HOST "127.0.0.1"
#define PORT 10001

int main(int argc,char** argv){
  msg r,t;
  init(HOST,PORT);
  if (recv_message(&r)<0){
    perror("Receive message");
    return -1;
  }

  printf("[%s] Got msg with payload: %s\n",argv[0],r.payload);

  sprintf(t.payload,"ACK(%s)",r.payload);
  t.len = strlen(t.payload+1);
  send_message(&t);
  if (recv_message(&r)<0){
    perror("Receive message");
    return -1;
  }
  
  int type, fd, len = 0;
  char name[200];
  while (0 < 1) {
    memcpy(&type, r.payload, 4);
    if (type == EOT) {
      printf("[%s] Got msg with type: %d\n",argv[0],type);
      break;
    }

    if (type == HEADER_NAME) {
      memset(name, 0, sizeof(name));
      sprintf(name, "recv_%s", r.payload+4);
      fd = open(name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
      printf("[%s] Got msg with type: %d\n",argv[0],type);
    }

    if (type == HEADER_LENGTH) {
      len = 0;
      memcpy(&len, r.payload+4, 4);
      printf("[%s] Got msg with type: %d\n",argv[0],type);
    }

    if (type == DATA) {
      write(fd, r.payload+4, r.len-4);
      len -= (r.len - 4);
      if (len == 0) {
        close(fd);
      }
      printf("[%s] Got msg with type: %d\n",argv[0],type);
    }
    memset(&t, 0, sizeof(msg));
    sprintf(t.payload, "ACK for message with type [%d]", type);
    t.len = 30;

    send_message(&t);
    if (recv_message(&r)<0){
      perror("Receive message");
      return -1;
    }
  }
  printf("[./recv] Transmission over!\n");
  return 0;
}
