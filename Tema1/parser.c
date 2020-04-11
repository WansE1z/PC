#include "parser.h"
// A function that swaps to elements
void swap(struct route_table_entry *a, struct route_table_entry *b) {
    struct route_table_entry t = *a;
    *a = *b;
    *b = t;
}

/* This function takes the last elements as the pivot, and it
 places the smaller values to the left of the pivot, and the greater
 values to the right */

int partition(struct route_table_entry *r_table, int low, int high) {
    uint32_t pivot = r_table[high].prefix;
    int i = (low - 1);
    // i -> index of the smallest element
    for (int j = low; j <= high - 1; j++) {
        /* if the element that is iterated is smaller than the pivot
            increment the index and swap the elements
        */
        if (r_table[j].prefix < pivot) {
            i++;
            swap(&r_table[i], &r_table[j]);
        }
    }
    swap(&r_table[i + 1], &r_table[high]);
    return (i + 1);
}
// the main function of the sorting, that sorts the given array
void quickSort(struct route_table_entry *r_table, int low, int high) {
    if (low < high) {
        int part = partition(r_table, low, high);
        quickSort(r_table, low, part - 1);
        quickSort(r_table, part + 1, high);
    }
}
// function that returns the number of lines in a file
int countLines(FILE *fin) {
    fin = fopen("rtable.txt", "r");
    int count = 0;
    for (char c = getc(fin); c != EOF; c = getc(fin)) {
        if (c == '\n') {
            count++;
        }
    }
    if (fseek(fin, 0L, SEEK_SET) != 0) {
        printf("File error");
        /* Handle repositioning error */
    }
    return count;
}

// function that is parsing the rtable.txt
void read_rtable(struct route_table_entry *r_table) {
    parse_rtable *v;
    /* parse_rtable is a structure that i made that stores
    everything that is needed for the parser.
    ----------
    typedef struct {
    char *prefix;
    char *next_hop;
    char *mask;
    char *interface;
    } parse_rtable;
    ----------
    */

    FILE *fin;
    fin = fopen("rtable.txt", "r");
    // if it doesn't open, exit
    if (fin == NULL) {
        printf("Error.");
        return;
    }
    int lines = countLines(fin);
    v = (parse_rtable *)malloc(lines * sizeof(parse_rtable));
    // alocate it
    for (int i = 0; i < lines; i++) {
        v[i].prefix = (char *)malloc(sizeof(char));
        v[i].mask = (char *)malloc(sizeof(char));
        v[i].next_hop = (char *)malloc(sizeof(char));
        v[i].interface = (char *)malloc(sizeof(char));
        // alocate memory for each variable
        fscanf(fin, "%s %s %s %s\n", v[i].prefix, v[i].next_hop, v[i].mask,
               v[i].interface);
        // read from the file each variable
        r_table[i].prefix = inet_addr(v[i].prefix);
        r_table[i].mask = inet_addr(v[i].mask);
        r_table[i].next_hop = inet_addr(v[i].next_hop);
        r_table[i].interface = atoi(v[i].interface);
        // move in the main structure the variables that were read
    }
}

int binarySearch(int left, int right, __u32 dest_ip,
                 struct route_table_entry *r_table_ent) {
    if (left > right) {
        return -1;
        /* if the left side is bigger than the right side
           we have to exit
        */
    } else {
        int mid = (left + right) >> 1;
        if (r_table_ent[mid].prefix == dest_ip) {
            // if the prefix is the one we want, return the position
            return mid;
        } else if (r_table_ent[mid].prefix > dest_ip) {
            binarySearch(left, mid - 1, dest_ip, r_table_ent);
        } else {
            binarySearch(mid + 1, right, dest_ip, r_table_ent);
        }
        /* if not , we go recursively in order to find
           the correct position
        */
    }
    return -1;
}

int bitsSet(struct route_table_entry route, __u32 dest_ip) {
    int nrOfBitsSetted;
    /*
        If a bit is set {mask = mask & (mask + 1)}
        Then increment the returning value
    */
    for (nrOfBitsSetted = 0; route.mask; nrOfBitsSetted++) {
        route.mask &= (route.mask - 1);
    }
    return nrOfBitsSetted;
}

struct route_table_entry *get_best_route(
    __u32 dest_ip, int lines, struct route_table_entry *r_table_ent) {
    struct route_table_entry *bestRoute = NULL;
    int aux = 0, nrBits = 0, oldAux = 0;
    int index = binarySearch(0, lines, dest_ip, r_table_ent);
    for (int i = index; i < lines; i++) {
        // iterate through all the lines
        if ((dest_ip & r_table_ent[i].mask) != r_table_ent[i].prefix) {
            nrBits = -1;
        } else {
            nrBits = bitsSet(r_table_ent[i], dest_ip);
        }
        // count the number of bits
        oldAux = aux;
        /* save in a variable the old variable that we are
           going to compare
        */
        aux = MAX(aux, nrBits);
        // keep in aux the max of the two elements
        if (aux != oldAux) {
            bestRoute = &r_table_ent[i];
            // return the best route
        }
        if (r_table_ent[i].prefix > dest_ip) {
            break;
        }
        /* in order to have not O(n) searching
        we check if the prefix is already bigger
        than ip_hdr->daddr, so we do not have to search
        through all the values, so we stop.
        In this way , it is O(logn)
        */
    }
    return bestRoute;
}
