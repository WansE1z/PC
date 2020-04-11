#pragma once
#include <asm/types.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>

#include "skel.h"

typedef struct {
    char *ip;
    char *mac;
} parse_arptable;

struct arp_entry {
    uint32_t ip;
    uint8_t mac[6];
};

void read_arptable(struct arp_entry *arpTable);
