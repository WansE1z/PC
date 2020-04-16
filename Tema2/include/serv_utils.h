#pragma once
#include "helpers.h"

struct svr_msg {
    char topic[51];
    int sf;
};

struct cli_msg {
    char topic_thread[51];
    unsigned int data_type;
    char message[1501];
};
