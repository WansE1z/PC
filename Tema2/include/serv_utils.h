#ifndef SERV_UTILS_H
#define SERV_UTILS_H

using namespace std;
#include "helpers.h"

struct messageUdp {
    char topic[51];
    unsigned int data_type;
    char data[1501];
};

struct messageTcp {
    char topic[51];
    unsigned int data_type;
    char data[1501];
};

struct topic {
    char name[51];
    int sf;
};

struct clientTcp {
    int socket;
    char id[10];
    vector<topic> topics;
    bool status; // online/offline
    char last_message[];
};
/*
    lm -> radu
    topics -> shjdrsgjdgrshdgjhrsdhrsjgdhgrjhsdghjrsradujsdhkrjshdjrshkdhjrsdhkjrksh
    status = offline radu
    cautare binara in topics => radu index 10
    for 10 -> topics.end() -> jsdhkrjshdjrshkdhjrsdhkjrksh <- returnam.
*/

#endif // SERV_UTILS_H