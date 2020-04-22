#ifndef SERV_UTILS_H
#define SERV_UTILS_H

using namespace std;
#include "helpers.h"

struct messageUdp {
    char topic[51];
    unsigned int data_type;
    char data[1501];
};

struct clientTcp {
    int socket;
    char id[10];
    vector<string> topics;
    vector<bool> sf;
    bool status; // online/offline
    //char last_message[];
};

#endif // SERV_UTILS_H