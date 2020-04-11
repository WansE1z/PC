#pragma once
#include <asm/types.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include "stdbool.h"

#include "skel.h"

#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))

struct route_table_entry {
    uint32_t prefix;
    uint32_t next_hop;
    uint32_t mask;
    int interface;
} __attribute__((packed));

typedef struct {
    char *prefix;
    char *next_hop;
    char *mask;
    char *interface;
} parse_rtable;

void read_rtable(struct route_table_entry *rtable);

int bitsSet(struct route_table_entry route, __u32 dest_ip);

struct route_table_entry *get_best_route(__u32 dest_ip, int sizeTbl,
                                         struct route_table_entry *rte);

void quickSort(struct route_table_entry *r_table, int low, int high);

int countLines(FILE *fin);

