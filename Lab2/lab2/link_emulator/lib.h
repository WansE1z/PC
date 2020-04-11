#ifndef LIB
#define LIB

#define MAX_LEN 1400
#define HEADER_NAME 1
#define HEADER_LENGTH 2
#define DATA 3
#define ACK 4
#define EOT 5

typedef struct {
  int type;
  char payload[MAX_LEN - 4];
} content;
typedef struct {
  int len;
  char payload[MAX_LEN];
} msg;

void init(char* remote,int remote_port);
void set_local_port(int port);
void set_remote(char* ip, int port);
int send_message(const msg* m);
int recv_message(msg* r);
#endif
